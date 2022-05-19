/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display  Foreign tender key                
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDFTND.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               MldForiegnTender() : displays foreign tender operation
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Nov-16-95 : 03.01.00 : M.Ozawa    : Initial
* Nov-30-99 : 01.00.00 : hrkato     : Saratoga
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

#if defined(POSSIBLE_DEAD_CODE)

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
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID  MldForeignTender(ITEMTENDER *pItem, USHORT usType):
*
*   Input  : ITEMTENDER       *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function displays tender operation
*===========================================================================
*/
SHORT MldForeignTender(ITEMTENDER *pItem, USHORT usType, USHORT usScroll)
{
    REGDISPMSG      DispMsg;
    PARATRANSMNEMO  WorkMnem;

    memset( &WorkMnem, 0, sizeof( PARATRANSMNEMO ) );           //     initialize
    if ((pItem->lChange) && !(MldCheckSpecialStatus(MLD_SCROLL_1, MLD_CHANGE_STATUS))) {
        WorkMnem.uchAddress = TRN_REGCHG_ADR;
    } else {
		USHORT  usTranAddr;
		UCHAR   uchSymAddr;

		MldGetMoneyMnem(pItem->uchMinorClass, &usTranAddr, &uchSymAddr);
        WorkMnem.uchAddress = usTranAddr;
    }
    WorkMnem.uchMajorClass = CLASS_PARATRANSMNEMO;                  
    CliParaRead( &WorkMnem );                                       /* get target mnemonic */

    memset( &DispMsg, 0, sizeof( REGDISPMSG ) );                   /* initialize "item" */
    DispMsg.uchMajorClass = CLASS_UIFREGTENDER;
    DispMsg.uchMinorClass = CLASS_UIFOREIGN1;

    if ((pItem->lChange) && !(MldCheckSpecialStatus(MLD_SCROLL_1, MLD_CHANGE_STATUS))) {
        DispMsg.lAmount = pItem->lChange;                     /* change amount */
        flDispRegDescrControl |= CHANGE_CNTRL;                /* descriptor */
    } else {
        DispMsg.lAmount = pItem->lForeignAmount;               /* tender amount */
        flDispRegDescrControl |= TENDER_CNTRL;                /* descriptor */
    }

    _tcsncpy( &DispMsg.aszMnemonic[0], &WorkMnem.aszMnemonics[0], NUM_TRANSMNEM );
    
    /* set scroll symbol */
    DispMsg.uchArrow = MldSetScrollSymbol(usType);

    DispMsg.fbSaveControl = 1;        /* 0: not save data, 1: save display data */

	// Check the MDC settings that have been retrieved for this foreign currency tender key
	// and set the bits for the display correctly.  Default is 2 decimal places.
	// if both bits set then no decimal, if one bit set then 3 decimal places.
    if ( pItem->auchTendStatus[0] & ITEMTENDER_STATUS_0_DECIMAL_3 ) {    /* set decimal point data */
        DispMsg.fbSaveControl |= DISP_SAVE_DECIMAL_3;
        if ( pItem->auchTendStatus[0] & ITEMTENDER_STATUS_0_DECIMAL_0 ) {
            DispMsg.fbSaveControl |= DISP_SAVE_DECIMAL_0;
        }
    }

    DispWrite(&DispMsg );

    flDispRegDescrControl &= ~(TENDER_CNTRL|CHANGE_CNTRL);

    if (usType == MLD_UPPER_CURSOR) {
        if ((pItem->lChange) && !(MldCheckSpecialStatus(MLD_SCROLL_1, MLD_CHANGE_STATUS))) {
            MldSetSpecialStatus(MLD_SCROLL_1, MLD_CHANGE_STATUS);
            return(MLD_DISPLAY_CHANGE); /* display tender after change display */
        }
    } else {
        if ((pItem->lChange) && (MldCheckSpecialStatus(MLD_SCROLL_1, MLD_CHANGE_STATUS))) {
            MldResetSpecialStatus(MLD_SCROLL_1, MLD_CHANGE_STATUS);
            return(MLD_DISPLAY_CHANGE); /* display change after tender display */
        }
    }

    usScroll = usScroll;    // unused argument
    return(MLD_SUCCESS);
}
#endif
/***** End Of Source *****/
