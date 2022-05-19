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
* Title       : Print Tray Total key 
* Category    : Print, NCR 2170 US Hospitarity Application 
* Program Name: PrRTrayT.C                                                      
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtTrayTotal()    : prints Tray total key (Total)
*               PrtTrayTotal_TH() : prints Tray total key  (thermal)
*               PrtTrayTotal_EJ() : prints Tray total key  (electric journal)
*               PrtTrayTotal_SP() : prints Tray total key  (slip)
*
*               PrtDflTrayTotal() : displays Tray total key (Total)
*               PrtDflTrayTotalForm() : displays Tray total key (Total)
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-10-93 : 01.00.12 : R.Itoh     : initial                                   
* Jul-13-93 : 01.00.12 : R.itoh     : add PrtDflTrayTotal()                                   
* Oct-29-93 : 02.00.02 : K.You      : del. PrtDflTrayTotal()                                   
* Apr-10-94 : 00.00.04 :  K.You     : add validation slip print feature.(mod. PrtTrayTotal())
* Jan-26-95 : 03.00.00 : M.Ozawa    : add PrtDflTrayTotal()
* Apr-27-95 : 03.00.00 : T.Nakahata : Add Total Mnemonic to Kitchen Printer
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
/* #include <para.h> */
/* #include <csstbpar.h> */
#include <pmg.h>
#include <dfl.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"
#include "para.h"
#include "csstbpar.h"
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
** Synopsis: This function prints tray total key 
*===========================================================================
*/
VOID  PrtTrayTotal(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation print */
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
            return;
        }
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtTrayTotal_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        if ( ! (fsPrtStatus & PRT_TAKETOKIT)) {
            PrtTrayTotal_TH(pTran, pItem);
        }
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric print */
        PrtTrayTotal_EJ(pItem);
    }
}

/*
*===========================================================================
** Format  : VOID PrtTrayTotal_TH(TRANINFORMATION *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tray total key  (thermal)
*===========================================================================
*/
VOID  PrtTrayTotal_TH(TRANINFORMATION *pTran, ITEMTOTAL *pItem)
{
    SHORT sType;

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    PrtTHHead(pTran);                           /* print header if necessary */    

    PrtFeed(PMG_PRT_RECEIPT, 1);                /* 1 line feed(Receipt) */

    PrtTHNumber(pItem->aszNumber);              /* number line */

    PrtTHAmtSym(RflChkTtlAdr(pItem), pItem->lMI, sType);  /* tray total line */

    PrtTHMnemCount(TRN_TRAYCO_ADR, pItem->sTrayCo);       /* tray total co. line */

    PrtFeed(PMG_PRT_RECEIPT, 1);                          /* 1 line feed(Receipt) */
}

/*
*===========================================================================
** Format  : VOID PrtTrayTotal_EJ(ITEMTOTAL *pItem);      
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tray total key  (electric journal)
*===========================================================================
*/
VOID  PrtTrayTotal_EJ(ITEMTOTAL *pItem)
{
    SHORT sType;

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    PrtEJNumber(pItem->aszNumber);              /* number line */

    PrtEJAmtSym(RflChkTtlAdr(pItem), pItem->lMI, sType);   /* tray total line */

    PrtEJMnemCount(TRN_TRAYCO_ADR, pItem->sTrayCo);    /* tray total co. line */

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
    TCHAR   aszSPPrintBuff[3][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usSaveLine;                 /* save slip lines to be added */
    USHORT  i;
    SHORT   sType;                      /* set double wide or single */

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

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
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
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
    TCHAR  aszDflBuff[8][PRT_DFL_LINE + 1]; /* display data save area */
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

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

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
    TCHAR  aszDflBuff[8][PRT_DFL_LINE + 1]; /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    SHORT sType;

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));
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
