/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-8          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Check Transfer                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRChkTT.c                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*       PrtCheckTran() : print check transfer
*       PrtCheckTran_TH() : prints check transfer ( thermal )
*       PrtCheckTran_EJ() : prints check transfer ( electric journal )
*       PrtCheckTran_SP() : prints check transfer ( slip )
*
*       PrtDflCheckTran() : display check transfer
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-10-93 : 00.00.01 : R.Itoh     : initial                                   
* Jul-13-93 : 01.00.12 : R.Itoh     : add PrtDflCheckTran()                                   
* Oct-29-93 : 02.00.02 : K.You      : del. PrtDflCheckTran()                                   
* Jan-26-95 : 03.00.00 : M.Ozawa    : recover PrtDflCheckTran() 
* Sep-16-98 : 03.03.00 : hrkato     : V3.3(Check Transfer Print)
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
** Format  : VOID  PrtCheckTran(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints check transfer.
*===========================================================================
*/
VOID PrtCheckTran(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
        PrtCheckTran_SP(pTran, pItem);
    }
    if ( fsPrtPrintPort & PRT_RECEIPT ) {  /* thermal print */
        PrtCheckTran_TH(pTran, pItem);
    }
    if ( fsPrtPrintPort & PRT_JOURNAL ) {  /* eleectric journal */
        PrtCheckTran_EJ(pItem);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtCheckTran_TH(TRANINFORMATION *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints check transfer. (thermal)         V3.3
*===========================================================================
*/
VOID  PrtCheckTran_TH(TRANINFORMATION *pTran, ITEMMISC *pItem)
{
    UCHAR   uchAdr;

    PrtTHHead(pTran);                            /* print header if necessary */
    PrtTHNumber(pItem->aszNumber);               /* number line */
    if (pItem->uchMinorClass == CLASS_CHECKTRANS_FROM) {
        uchAdr = TRN_CKFRM_ADR;
    } else if (pItem->uchMinorClass == CLASS_CHECKTRANS_TO) {
        uchAdr = TRN_CKTO_ADR;
    } else {                                     /* old waiter */
        uchAdr = TRN_OLDWT_ADR;
    }
    PrtTHWaiGC(uchAdr, pItem->ulID, pItem->usGuestNo, pItem->uchCdv);
    PrtTHAmtMnem(TRN_TRNSB_ADR, pItem->lAmount);
}

/*
*===========================================================================
** Format  : VOID  PrtCheckTran_EJ(ITEMMISC *pItem);      
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints check transfer. (electric journal)    V3.3
*===========================================================================
*/
VOID  PrtCheckTran_EJ(ITEMMISC *pItem)
{
    UCHAR   uchAdr;

    PrtEJNumber(pItem->aszNumber);               /* number line */
    
    if (pItem->uchMinorClass == CLASS_CHECKTRANS_FROM) {
        uchAdr = TRN_CKFRM_ADR;
    } else if (pItem->uchMinorClass == CLASS_CHECKTRANS_TO) {
        uchAdr = TRN_CKTO_ADR;
    } else {                                     /* old waiter */
        uchAdr = TRN_OLDWT_ADR;
    }
    PrtEJTranNum(uchAdr, pItem->ulID);           /* old waiter */

    PrtEJGuest(pItem->usGuestNo, pItem->uchCdv); /* guest chek number */

    PrtEJAmtMnem(TRN_TRNSB_ADR, pItem->lAmount); /* transfered balance */
}

/*
*===========================================================================
** Format  : VOID  PrtCheckTran_SP(TRANINFORMATION *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*          : ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints check transfer.               V3.3
*===========================================================================
*/
VOID PrtCheckTran_SP(TRANINFORMATION  *pTran, ITEMMISC *pItem)
{
    TCHAR  aszSPPrintBuff[3][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- set void mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);

    /* -- set old waiter and guest check mnemonic and No.   V3.3 -- */
    usSlipLine += PrtSPGCTransfer(aszSPPrintBuff[usSlipLine], pItem);

    /* -- set trailer -- */
    usSlipLine += PrtSPGCTranTrail(aszSPPrintBuff[usSlipLine], pItem);

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- if paper changed, re-set page, line, consecutive No. -- */    
    if (usSaveLine != 0) {
        usSlipLine --;      /* override the data in the last buffer */
        usSlipLine += PrtSPGCTranTrail(aszSPPrintBuff[usSlipLine], pItem);
    }    

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
** Format  : VOID  PrtDflCheckTran(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays check transfer.
*===========================================================================
*/
VOID PrtDflCheckTran(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
{
    TCHAR   aszDflBuff[9][PRT_DFL_LINE + 1]; /* display data save area */
    UCHAR   uchAdr;
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

    if (pItem->uchMinorClass == CLASS_CHECKTRANS_FROM) {
        uchAdr = TRN_CKFRM_ADR;

    } else if (pItem->uchMinorClass == CLASS_CHECKTRANS_TO) {
        uchAdr = TRN_CKTO_ADR;
    } else {                                     /* old waiter */
        uchAdr = TRN_OLDWT_ADR;
    }
    usLineNo += PrtDflTranNum(aszDflBuff[usLineNo], uchAdr, pItem->ulID); 

    usLineNo += PrtDflGuest(aszDflBuff[usLineNo], pItem->usGuestNo, pItem->uchCdv); 

    usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], TRN_TRNSB_ADR, pItem->lAmount);

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

/***** End Of Source *****/
