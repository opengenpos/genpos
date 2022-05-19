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
* Title       : Print Transaction Cancel                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRCncl_.c                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*      
*      PrtCancel()    : print transaction cancel
*      PrtCancel_RJ() : transaction cancel ( receipt & journal )
*      PrtCancel_SP() : transaction cancel ( slip )
*      PrtDflCancel() : display transaction cancel
*
*      PrtDflCancelForm() : display transaction cancel
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-04-92 : 00.00.01 : K.Nakajima :                                    
* Jul-13-93 : 01.00.12 : R.Itoh     : add PrtDflCancel()                                   
* Oct-28-93 : 02.00.02 : K.You      : del. PrtDflCancel()                                   
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PrtDflCancel() for display on the fly
* Jul-21-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
* Jan-26-95 : 03.00.00 : M.Ozawa    : recover PrtDflCancel()
* Dec-15-99 : 00.00.01 : M.Ozawa    : add PrtDflCancelForm()
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
/**------- M S - C -------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <paraequ.h>
/* #include <para.h> */
#include <regstrct.h>
#include <transact.h>
#include <pmg.h>
#include <dfl.h>
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
** Format  : VOID  PrtCancel(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
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
VOID PrtCancel(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
{


    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    
    if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
        PrtCancel_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT)  /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        PrtCancel_RJ(pItem);
    }


}

/*
*===========================================================================
** Format  : VOID PrtCancel_RJ(ITEMMISC *pItem);      
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints transaction cancel .(receipt & journal)
*===========================================================================
*/
VOID  PrtCancel_RJ(ITEMMISC *pItem)
{
    PrtRJMnem(TRN_TRACAN_ADR, PRT_DOUBLE);     /* cancel with double wide */

    PrtRJNumber(pItem->aszNumber);             /* number line */

    PrtRJAmtMnem(TRN_CANCEL_ADR, pItem->lAmount);/* cancel total */

}

/*
*===========================================================================
** Format  : VOID PrtCancel_SP(TRANINFORMATION  *pTran, ITEMMISC  *pItem);      
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints transaction cancel. (slip)
*===========================================================================
*/
VOID  PrtCancel_SP(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
{
    TCHAR  aszSPPrintBuff[PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */

    /* initialize the buffer */
    memset(aszSPPrintBuff, '\0', sizeof(aszSPPrintBuff));

    /* -- set cancel mnemonic -- */
    usSlipLine += PrtSPCancel(aszSPPrintBuff, TRN_TRACAN_ADR, TRN_CANCEL_ADR, pItem->lAmount);

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);   

    /* -- print all data in the buffer -- */ 
    PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff, PRT_SPCOLUMN);       

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
}

/*
*===========================================================================
** Format  : VOID  PrtDflCancel(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
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
VOID PrtDflCancel(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
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
    usLineNo += PrtDflMnem(aszDflBuff[usLineNo], TRN_TRACAN_ADR, PRT_DOUBLE);    

    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], TRN_CANCEL_ADR, pItem->lAmount); 

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_CANCEL); 

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
** Format  : VOID  PrtDflCancel(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
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
USHORT PrtDflCancelForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer)
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
    usLineNo += PrtDflMnem(aszDflBuff[usLineNo], TRN_TRACAN_ADR, PRT_DOUBLE);     /* cancel with double wide */

    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], TRN_CANCEL_ADR, pItem->lAmount); 

    for (i=0; i<usLineNo; i++) {

        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
	_tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
	//memcpy(puchBuffer, aszDflBuff, usLineNo*(PRT_DFL_LINE+1));
	
    return usLineNo;

}

/***** End Of Source *****/
