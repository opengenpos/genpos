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
* Title       : Multiline Display Transaction Cancel                 
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDCNCL.c                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*      
*      MldCancel()    : display transaction cancel
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
/**------- M S - C -------**/
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
** Format  : VOID MldCancel(ITEMMISC  *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays transaction cancel.
*===========================================================================
*/
SHORT  MldCancel(ITEMMISC  *pItem, USHORT usType, USHORT usScroll)
{
    REGDISPMSG      DispMsg;
    PARATRANSMNEMO  WorkMnem;


/*    memset( &WorkMnem, 0, sizeof( PARATRANSMNEMO ) );                initialize */
    WorkMnem.uchAddress = TRN_CANCEL_ADR;

    WorkMnem.uchMajorClass = CLASS_PARATRANSMNEMO;                  
    CliParaRead( &WorkMnem );                                       /* get target mnemonic */

    memset( &DispMsg, 0, sizeof( REGDISPMSG ) );                   /* initialize "item" */
    DispMsg.uchMajorClass = CLASS_UIFREGMISC;
    DispMsg.uchMinorClass = CLASS_UICANCEL;

    DispMsg.lAmount = pItem->lAmount;                               /* cancel amount */

    _tcsncpy( &DispMsg.aszMnemonic[0], &WorkMnem.aszMnemonics[0], NUM_TRANSMNEM );
    //memcpy( &DispMsg.aszMnemonic[0], &WorkMnem.aszMnemonics[0], NUM_TRANSMNEM );
    
    /* set scroll symbol */
    DispMsg.uchArrow = MldSetScrollSymbol(usType);

    DispMsg.fbSaveControl = 1;        /* 0: not save data, 1: save display data */

    DispWrite(&DispMsg );

    return (MLD_SUCCESS);

    usScroll = usScroll;
}
#endif
/***** End Of Source *****/
