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
* Title       : Multiline Display  addcheck                   
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDADD2.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               MldAddChk2() : display addcheck
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

/*
*===========================================================================
** Format  : VOID  MldAddChk2(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First display position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays addcheck line.
*===========================================================================
*/
SHORT  MldAddChk2(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll)
{
    REGDISPMSG      DisplayData;
    PARATRANSMNEMO  TransMnemoRcvBuff;

    if ( uchMldSystem != MLD_PRECHECK_SYSTEM ) {
        return (MLD_NOT_DISPLAY);
    }
    if ( pItem->lAmount == 0L ) {               /* if amount "0", not display */
        return (MLD_NOT_DISPLAY);
    }

    memset(&DisplayData, 0, sizeof(REGDISPMSG));
    DisplayData.uchMajorClass = CLASS_UIFREGTRANSOPEN;
    DisplayData.uchMinorClass = CLASS_UIADDCHECK1;
    DisplayData.lAmount = pItem->lAmount;
 
    TransMnemoRcvBuff.uchMajorClass = CLASS_PARATRANSMNEMO;
    TransMnemoRcvBuff.uchAddress = (UCHAR)(pItem->uchAddNum + TRN_ADCK1_ADR), 
    CliParaRead(&TransMnemoRcvBuff);

    _tcsncpy(DisplayData.aszMnemonic, TransMnemoRcvBuff.aszMnemonics, PARA_TRANSMNEMO_LEN);
    //memcpy(&DisplayData.aszMnemonic, TransMnemoRcvBuff.aszMnemonics, PARA_TRANSMNEMO_LEN);

    if (pItem->fbModifier & VOID_MODIFIER) {
        flDispRegDescrControl |= VOID_CNTRL; 
                                            /* turn on descriptor control flag for condiment */
    } 

    if (pItem->fbModifier & TAX_MOD_MASK) { 
                                            /* tax status is on */
        flDispRegDescrControl |= TAXMOD_CNTRL; 
                                            /* turn on descriptor control flag for condiment */
    }

    /* set scroll symbol */
    DisplayData.uchArrow = MldSetScrollSymbol(usType);

    DisplayData.fbSaveControl = 1;        /* 0: not save data, 1: save display data */

    DispWrite(&DisplayData);                /* display the above data */
    
    /*----- TURN OFF DESCRIPTOR CONTROL FLAG -----*/

    flDispRegDescrControl &=~ (VOID_CNTRL | TAXMOD_CNTRL);

    return(MLD_SUCCESS);

    usScroll = usScroll;
}
#endif
/***** End Of Source *****/
