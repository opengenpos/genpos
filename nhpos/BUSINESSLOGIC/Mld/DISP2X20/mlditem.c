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
* Title       : Multiline Display Item  main                 
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDITEM.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
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
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <display.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/*
============================================================================
+                  S T A T I C   R A M s
============================================================================
*/

//Now using MLD_NO_DISP_PLU_LOW and MLD_NO_DISP_PLU_HIGH - (ecr.h)
//CONST TCHAR auchMldNoDispPLULow[]  = _T("00000000099500"); /* 2172 */
//CONST TCHAR auchMldNoDispPLUHigh[] = _T("00000000099999"); /* 2172 */

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s                      +
;========================================================================
**/
/*
*===========================================================================
** Format  : USHORT   MldLocalScrollWrite(VOID *pItem, USHORT usType, USHORT usScroll);
*               
*    Input : VOID             *pItem     -Item Data address
*            USHORT           usType     -First display position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function distribute data structur according to data class
*===========================================================================
*/
SHORT   MldLocalScrollWrite(VOID *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;

    switch ( ((MLDITEMDATA *)pItem)->uchMajorClass ) {

/*-------------------*\
  R E G    M O D E 
\*-------------------*/
    case CLASS_ITEMSALES:               /* plu, dept, setmenu, mod.discount */
        sStatus = MldItemSales((ITEMSALES *)pItem, usType, usScroll);
        break;

    case CLASS_ITEMDISC:                /* discount */
        sStatus = MldItemDisc((ITEMDISC *)pItem, usType, usScroll);
        break;

    case CLASS_ITEMCOUPON:              /* coupon */
        sStatus = MldItemCoupon((ITEMCOUPON *)pItem, usType, usScroll);
        break;

    case CLASS_ITEMMISC:                /* nosale, ROA, paid out... */
        sStatus = MldItemMisc((ITEMMISC *)pItem, usType, usScroll);
        break;

    case CLASS_ITEMTENDER:              /* tender */
        sStatus = MldItemTender((ITEMTENDER *)pItem, usType, usScroll);
        break;

    case CLASS_ITEMAFFECTION:           /* tax */
        sStatus = MldItemAffection((ITEMAFFECTION *)pItem, usType, usScroll);
        break;

    case CLASS_ITEMTRANSOPEN:           /* newcheck , reorder */
        sStatus = MldItemTransOpen((ITEMTRANSOPEN *)pItem, usType, usScroll);
        break;

    case CLASS_ITEMTOTAL:               /* total */
        sStatus = MldItemTotal((ITEMTOTAL *)pItem, usType, usScroll);
        break;

    default:
        sStatus = MLD_NOT_DISPLAY;
        break;

    }

    return (sStatus);
}

/*
*===========================================================================
** Format  : USHORT   MldSetScrollSymbol(USHORT usType);
*               
*    Input : USHORT           usType     -cursor type
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function sets scroll symbol according to cursor type
*===========================================================================
*/
UCHAR  MldSetScrollSymbol(USHORT usType)
{
    if (usType == MLD_UPPER_CURSOR) {
        return (0x5E);  /*  */
    } else {
        return (0x76);  /*  */
    }
}

/*
*===========================================================================
** Format  : USHORT   MldDispEndMessage(USHORT usType);
*               
*    Input : USHORT           usType     -cursor type
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays end message according to cursor type
*===========================================================================
*/
VOID  MldDispEndMessage(USHORT usType)
{
    REGDISPMSG      DisplayData;
    PARALEADTHRU    ParaBuffLead;

    memset(&DisplayData, 0, sizeof(REGDISPMSG));
    DisplayData.uchMajorClass = CLASS_UIFREGOTHER;
    DisplayData.lAmount = 0L;

    ParaBuffLead.uchMajorClass = CLASS_PARALEADTHRU;

    if (usType == MLD_UPPER_CURSOR) {

        DisplayData.uchMinorClass = CLASS_UISCROLLUP;
        ParaBuffLead.uchAddress = LDT_UPPER_ADR;
        DisplayData.uchArrow  = 0;

    } else {

        DisplayData.uchMinorClass = CLASS_UISCROLLDOWN;
        ParaBuffLead.uchAddress = LDT_LOWER_ADR;
        DisplayData.uchArrow = 0;
    }
    CliParaRead(&ParaBuffLead);

    _tcsncpy(DisplayData.aszMnemonic, ParaBuffLead.aszMnemonics, PARA_LEADTHRU_LEN);
    //memcpy(&DisplayData.aszMnemonic, ParaBuffLead.aszMnemonics, PARA_LEADTHRU_LEN);

    DisplayData.fbSaveControl = 1;        /* 0: not save data, 1: save display data */

    flDispRegDescrControl &=~ (CRED_CNTRL |
                               VOID_CNTRL | 
                               ITEMDISC_CNTRL |
                               TAXMOD_CNTRL |  
                               TAXEXMPT_CNTRL |
                               TOTAL_CNTRL | 
                               SUBTTL_CNTRL);

    DispWrite(&DisplayData);                /* display the above data */
    
}
#endif
/***** End Of Source *****/