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
* Title       : Print No Sale                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRNoSlT.c                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtNoSale() : print no sale
*               PrtNoSale_TH() : print no sale (thermal)
*               PrtNoSale_RJ() : print no sale (electric journal)
*               PrtNoSale_SP() : prints no sale (slip)
*
*               PrtDflNoSale() : display no sale
*               PrtDflNoSaleForm() : display no sale
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-10-93 : 01.00.12 : R.Itoh     : initial                                   
* Jul-13-93 : 01.00.12 : R.Itoh     : add PrtDflNoSale()                                   
* Oct-29-93 : 02.00.02 : K.You      : del. PrtDflNoSale()                                   
* Jan-26-95 : 03.00.00 : M.Ozawa    : recover PrtDflNoSale() 
* Dec-15-99 : 00.00.01 : M.Ozawa    : add PrtDflNoSaleForm() 
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
/* #include <para.h> */
#include <pmg.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
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
** Format  : VOID  PrtNoSale(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints  no sale.
*===========================================================================
*/
VOID PrtNoSale(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
{


    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    
    if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
        PrtNoSale_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {  /* thermal print */
        PrtNoSale_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {  /* electric journal */
        PrtNoSale_EJ(pItem);
    }

}

/*
*===========================================================================
** Format  : VOID PrtNoSale_TH(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transactionn Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : SHORT    sSlipLines         -current page & line number
*            
** Synopsis: This function prints no sale .(thermal)
*===========================================================================
*/
VOID  PrtNoSale_TH(TRANINFORMATION *pTran, ITEMMISC *pItem)
{
    PrtTHHead(pTran);                             /* print header if necessary */  

    PrtTHNumber(pItem->aszNumber);                /* number line */

    PrtTHMnem(TRN_NOSALE_ADR, PRT_SINGLE);        /* no sale line */
}

/*
*===========================================================================
** Format  : VOID PrtNoSale_EJ(ITEMMISC *pItem);      
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : SHORT    sSlipLines         -current page & line number
*            
** Synopsis: This function prints no sale .(electric journal)
*===========================================================================
*/
VOID  PrtNoSale_EJ(ITEMMISC *pItem)
{
    PrtEJNumber(pItem->aszNumber);                /* number line */

    PrtEJMnem(TRN_NOSALE_ADR, PRT_SINGLE);        /* no sale line */
}

/*
*===========================================================================
** Format  : VOID PrtNoSale_SP( TRANINFORMATION  *pTran, ITEMMISC *pItem );      
*
*   Input  : ITEMMISC  *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints no sale .(slip)
*===========================================================================
*/
VOID PrtNoSale_SP( TRANINFORMATION  *pTran, ITEMMISC  *pItem )
{
    TCHAR  aszSPPrintBuff[2][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   

    /* -- initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));
                                                
    /* -- set void mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
    /* -- set no sale mnemonic -- */
    usSlipLine += PrtSPTranMnem(aszSPPrintBuff[usSlipLine], TRN_NOSALE_ADR);

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
** Format  : VOID  PrtDflNoSale(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays  no sale.
*===========================================================================
*/
VOID PrtDflNoSale(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
{
    TCHAR  aszDflBuff[7][PRT_DFL_LINE + 1]; /* display data save area */
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
    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflMnem(aszDflBuff[usLineNo], TRN_NOSALE_ADR, PRT_SINGLE); 

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

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
** Format  : VOID  PrtDflNoSale(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays  no sale.
*===========================================================================
*/
USHORT PrtDflNoSaleForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer)
{
    TCHAR  aszDflBuff[7][PRT_DFL_LINE + 1]; /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));
#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflMnem(aszDflBuff[usLineNo], TRN_NOSALE_ADR, PRT_SINGLE); 

    for (i=0; i<usLineNo; i++) {

        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    //memcpy(puchBuffer, aszDflBuff, usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;

}

/***** End Of Source *****/
