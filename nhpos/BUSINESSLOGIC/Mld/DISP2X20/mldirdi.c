/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display Item  & Regular Discount                 
* Category    : Multiline, NCR 2170 US Hospitarity Application
* Program Name: MLDIRDI.c                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               MldItemRegDisc() : print item / regular discount
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Nov-16-95 : 03.01.00 : M.Ozawa    : Initial
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

#if defined(POSSIBLE_DEAD_CODE)

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
#include <pif.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h> 
#include <para.h>
#include <csstbpar.h>
#include <uie.h>
#include <fsc.h>
#include <rfl.h>
#include <display.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s                      +
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID MldItemRegDisc(ITEMDISC *pItem, USHORT usType, USHORT usScroll);
*               
*    Input : ITEMDISC         *pItem     -Item Data address   
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function displays discount item.
*===========================================================================
*/
SHORT MldItemRegDisc(ITEMDISC  *pItem, USHORT usType, USHORT usScroll)
{
    REGDISPMSG      DisplayData;
    PARATRANSMNEMO  TransMnemoRcvBuff;
    TRANMODEQUAL    WorkMode;
    TRANCURQUAL     WorkQual;

    memset(&DisplayData, 0, sizeof(REGDISPMSG));
    DisplayData.uchMajorClass = CLASS_UIFREGDISC;
    DisplayData.uchMinorClass = CLASS_UIITEMDISC1;
    DisplayData.lAmount = pItem->lAmount;
    DisplayData.uchFsc = pItem->uchSeatNo;
 
    TransMnemoRcvBuff.uchMajorClass = CLASS_PARATRANSMNEMO;
	TransMnemoRcvBuff.uchAddress = RflChkDiscAdr(pItem);
    CliParaRead(&TransMnemoRcvBuff);

    _tcsncpy(DisplayData.aszMnemonic, TransMnemoRcvBuff.aszMnemonics, PARA_TRANSMNEMO_LEN);

    flDispRegDescrControl |= ITEMDISC_CNTRL;

    if (pItem->fbDiscModifier & VOID_MODIFIER) {
        flDispRegDescrControl |= VOID_CNTRL;       /* turn on descriptor control flag for condiment */
    } 

    if (pItem->fbDiscModifier & TAX_MOD_MASK) {        /* tax status is on */
        flDispRegDescrControl |= TAXMOD_CNTRL;     /* turn on descriptor control flag for condiment */
    }

    /* check system */
    TrnGetModeQual( &WorkMode );                                /* get mode qualifier */
    TrnGetCurQual( &WorkQual );                                 /* get current qualifier */
    if ( (WorkQual.fsCurStatus & CURQUAL_OPEMASK ) != CURQUAL_CASHIER ) {

        flDispRegKeepControl &=~ (VOID_CNTRL | ITEMDISC_CNTRL | TAXMOD_CNTRL  | TOTAL_CNTRL | SUBTTL_CNTRL);

    } else {
        flDispRegKeepControl &=~ (VOID_CNTRL | ITEMDISC_CNTRL | TAXMOD_CNTRL  | TAXEXMPT_CNTRL | TOTAL_CNTRL | SUBTTL_CNTRL);
    }
                              
    /* set scroll symbol */
    DisplayData.uchArrow = MldSetScrollSymbol(usType);

    DisplayData.fbSaveControl = 1;        /* 0: not save data, 1: save display data */

    DispWrite(&DisplayData);                /* display the above data */
    
    /*----- TURN OFF DESCRIPTOR CONTROL FLAG -----*/
    if ( (WorkQual.fsCurStatus & CURQUAL_OPEMASK ) != CURQUAL_CASHIER ) {
        flDispRegDescrControl &=~ (CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL | TAXMOD_CNTRL | TOTAL_CNTRL | SUBTTL_CNTRL);
    } else {
        flDispRegDescrControl &=~ (CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL | TOTAL_CNTRL | SUBTTL_CNTRL);
    }

    return(MLD_SUCCESS);

    usScroll = usScroll;
}
#endif
/***** End Of Source *****/
