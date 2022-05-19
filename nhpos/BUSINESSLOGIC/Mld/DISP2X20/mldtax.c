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
* Title       : Multiline Display Tax                 
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDTAX.C                     
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      MldItemAffection() : display items specified class "CLASS_ITEMAFFECTION"
*      MldTax()           : display tax
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
/**------- 2170 local------**/
#include	<tchar.h>
#include<ecr.h>
#include<pif.h>
#include<regstrct.h>
#include<transact.h>
#include<log.h>
#include<mld.h>
#include"mldlocal.h"
#include"mldrin.h"
#include "termcfg.h"

/*
*===========================================================================
** Format  : VOID   MldItemAffection(ITEMAFFECTION *pItem, USHORT usType,
*                                   USHORT usScroll);
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First display position
*            USHORT           usScroll   -Destination scroll display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tax and addcheck .
*            
*===========================================================================
*/
SHORT  MldItemAffection(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;

    switch (pItem->uchMinorClass) {

    case CLASS_ADDCHECKTOTAL:                /* auto addcheck total */
    case CLASS_MANADDCHECKTOTAL:             /* manual addcheck total */
        sStatus = MldAddChk2(pItem, usType, usScroll);
        break;

    default:
        sStatus = MLD_NOT_DISPLAY;
        break;
    }
    return (sStatus);
}
#endif
/***** End Of Source *****/
