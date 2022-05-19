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
* Title       : Multiline Display  Normal tender key                
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDNTND.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               MldTender() : displays tender operation
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Nov-16-95 : 03.01.00 : M.Ozawa    : Initial
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
** Format  : VOID  MldTender(ITEMTENDER *pItem, USHORT usType):
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
SHORT MldTender(ITEMTENDER *pItem, USHORT usType, USHORT usScroll)
{
    REGDISPMSG      DispMsg;
    PARATRANSMNEMO  WorkMnem;

    memset( &WorkMnem, 0, sizeof( PARATRANSMNEMO ) );             //   initialize
    if ((pItem->lChange) && !(MldCheckSpecialStatus(MLD_SCROLL_1, MLD_CHANGE_STATUS))) {
        WorkMnem.uchAddress = TRN_REGCHG_ADR;
    } else {
        if ( pItem->uchMinorClass <= CLASS_TENDER8 ) {
            WorkMnem.uchAddress = ( UCHAR )( TRN_TEND1_ADR + pItem->uchMinorClass - 1 );
        } else if ((pItem->uchMinorClass >= CLASS_TENDER9) && (pItem->uchMinorClass <= CLASS_TENDER11)) {
            WorkMnem.uchAddress = ( UCHAR )( TRN_TEND9_ADR - CLASS_TENDER8 + pItem->uchMinorClass - 1 );
        } else if ((pItem->uchMinorClass >= CLASS_TENDER12) && (pItem->uchMinorClass <= CLASS_TENDER20)) {
			WorkMnem.uchAddress = ( UCHAR )( TRN_TENDER12_ADR + pItem->uchMinorClass - CLASS_TENDER12 );
		} else {
			NHPOS_ASSERT(pItem->uchMinorClass <= CLASS_TENDER20);
			return (MLD_SUCCESS);
		}
    }
    WorkMnem.uchMajorClass = CLASS_PARATRANSMNEMO;                  
    CliParaRead( &WorkMnem );                                       /* get target mnemonic */

    memset( &DispMsg, 0, sizeof( REGDISPMSG ) );                   /* initialize "item" */
    DispMsg.uchMajorClass = CLASS_UIFREGTENDER;
    DispMsg.uchMinorClass = CLASS_UITENDER1;

    if ((pItem->lChange) && !(MldCheckSpecialStatus(MLD_SCROLL_1, MLD_CHANGE_STATUS))) {
        DispMsg.lAmount = pItem->lChange;                     /* change amount */
        flDispRegDescrControl |= CHANGE_CNTRL;                /* descriptor */
    } else {
        DispMsg.lAmount = pItem->lTenderAmount;               /* tender amount */
        flDispRegDescrControl |= TENDER_CNTRL;                /* descriptor */
    }

    _tcsncpy( &DispMsg.aszMnemonic[0], &WorkMnem.aszMnemonics[0], NUM_TRANSMNEM );
    
    /* set scroll symbol */
    DispMsg.uchArrow = MldSetScrollSymbol(usType);

    DispMsg.fbSaveControl = 1;        /* 0: not save data, 1: save display data */

    DispWrite(&DispMsg );                       /* display check total */

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

    usScroll = usScroll;   // unused argument
    return(MLD_SUCCESS);
}
#endif
/***** End Of Source *****/
