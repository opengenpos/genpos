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
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  Total key (Tray Total)
* Category    : Print, NCR 2170 US Hospitarity Application 
* Program Name: PrRTray_.C                                                      
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtTrayTotal()    : prints Tray total key (Total)
*               PrtTrayTotal_RJ() : prints Tray total key  (receipt & journal)
*               PrtTrayTotal_VL() : prints Tray total key  (validation)
*               PrtTrayTotal_SP() : prints Tray total key  (slip)
*               PrtDflTrayTotal() : displays Tray total key (Total)
*
*               PrtDflTrayTotalForm() : displays Tray total key (Total)
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-21-92 : 00.00.01 : K.Nakajima :                                    
* Jul-13-93 : 01.00.12 : R.itoh     : add PrtDflTrayTotal()                                   
* Oct-28-93 : 02.00.02 : K.You      : del. PrtDflTrayTotal()                                   
* Apr-10-94 : 00.00.04 :  K.You     : add validation slip print feature.(mod. ())
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PrtDflTrayTotal() for display on the fly
* Apr-27-95 : 03.00.00 : T.Nakahata : add Total Mnemonic to Kitchen Feature
* Jul-24-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
* Dec-14-99 : 00.00.01 : M.Ozawa    : add PrtDflTrayTotalForm()
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <dfl.h>
#include <pmg.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"
#include <rfl.h>

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID   PrtTrayTotal(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key (Total)
*===========================================================================
*/
VOID  PrtTrayTotal(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation */

        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
			PmgBeginSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            PrtSPVLHead(pTran);    
            PrtTrayTotal_SP(pTran, pItem);
            PrtSPVLTrail(pTran);
			PmgEndSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
        } else {
            PrtTrayTotal_VL(pTran, pItem);
        }
        return ;
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtTrayTotal_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT)     /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        if ( ! ( fsPrtStatus & PRT_TAKETOKIT )) {

            PrtTrayTotal_RJ(pItem);
        }
    }
}

/*
*===========================================================================
** Format  : VOID PrtTrayTotal_RJ(ITEMTOTAL *pItem);      
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key  (receipt & journal)
*===========================================================================
*/
VOID  PrtTrayTotal_RJ(ITEMTOTAL *pItem)
{
    SHORT sType;

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    PrtRFeed(1);                                /* 1 line feed(Receipt) */

    PrtRJNumber(pItem->aszNumber);              /* number line */

    PrtRJAmtSym(RflChkTtlAdr(pItem), pItem->lMI, sType);   /* tray total line */

    PrtRJMnemCount(TRN_TRAYCO_ADR, pItem->sTrayCo);       /* tray total co. line */

}

/*
*===========================================================================
** Format  : VOID PrtTrayTotal_VL(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key  (validation)
*===========================================================================
*/
VOID  PrtTrayTotal_VL(TRANINFORMATION *pTran, ITEMTOTAL *pItem)
{

    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */

    /* -- in case of 24 char printer -- */
    if ( usPrtVLColumn == PRT_VL_24 ) {
        PrtVLHead(pTran, 0);                    /* send header lines */
                                                /* 0 means not print "VOID" */
        PrtVLNumber(pItem->aszNumber);          /* send number line */
        PrtVLTray(pItem);                       /* send tray total & co. */
        PrtVLTrail(pTran);                      /* send trailer lines */
    }

    /* -- in case of 21 char printer -- */
    if ( usPrtVLColumn == PRT_VL_21 ) {
        PrtVSTotal(pItem, pTran);               /* send tray total line */            
    }

    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */

}

/*
*===========================================================================
** Format  : VOID PrtTrayTotal_SP(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints Tray total  (slip)
*===========================================================================
*/
VOID PrtTrayTotal_SP(TRANINFORMATION *pTran, ITEMTOTAL *pItem)
{
	TCHAR   aszSPPrintBuff[3][PRT_SPCOLUMN + 1] = { 0 }; /* print data save area */
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usSaveLine;                 /* save slip lines to be added */
    USHORT  i;
    SHORT   sType;                      /* set double wide or single */

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    /* -- set mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
    /* -- set total line -- */   
    usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lMI, sType);

    /* -- set counter line -- */   
    usSlipLine += PrtSPTrayCount(aszSPPrintBuff[usSlipLine], TRN_TRAYCO_ADR, pItem->sTrayCo);

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }

    /* -- 1 line feed -- */
    PrtFeed(PMG_PRT_SLIP, 1);   
    usSlipLine ++;

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        

}

/*
*===========================================================================
** Format  : VOID   PrtDflTrayTotal(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays total key (Total)
*===========================================================================
*/
VOID  PrtDflTrayTotal(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
	TCHAR  aszDflBuff[8][PRT_DFL_LINE + 1] = { 0 }; /* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       
    SHORT sType;

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], RflChkTtlAdr(pItem), pItem->lMI, sType);
    usLineNo += PrtDflMnemCount(aszDflBuff[usLineNo], TRN_TRAYCO_ADR, pItem->sTrayCo);

    /* -- set destination CRT -- */
    PrtDflSetTtlNo(pTran);

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_TOTAL); 

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
** Format  : VOID   PrtDflTrayTotal(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays total key (Total)
*===========================================================================
*/
USHORT  PrtDflTrayTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer)
{
	TCHAR  aszDflBuff[8][PRT_DFL_LINE + 1] = { 0 }; /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    SHORT sType;

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], RflChkTtlAdr(pItem), pItem->lMI, sType);
    usLineNo += PrtDflMnemCount(aszDflBuff[usLineNo], TRN_TRAYCO_ADR, pItem->sTrayCo);

    for (i=0; i<usLineNo; i++) {

        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;

}


/***** End Of Source *****/
