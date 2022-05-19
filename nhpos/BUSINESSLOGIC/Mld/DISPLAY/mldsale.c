/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*
*  NeighborhoodPOS Software Development Project
*  Department of Information Systems
*  College of Information Technology
*  Georgia Southern University
*  Statesboro, Georgia
*
*  Copyright 2014 Georgia Southern University Research and Services Foundation
* 
*===========================================================================
* Title       : Display on Scroll Display Item  Dept PLU                 
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDSALE.C                                                      
* --------------------------------------------------------------------------
* Abstract:
*      MldItemSales() : Display items specified class "CLASS_ITEMSALES"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Feb-27-95 : 03.00.03 :  M.Ozawa   : Initial
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
/**------- 2170 local------**/
#include	<tchar.h>
#include    <memory.h>
#include<ecr.h>
#include<regstrct.h>
#include "paraequ.h"
#include<transact.h>
#include<mld.h>
#include"mldlocal.h"
#include"mldrin.h"
#include "termcfg.h"
#include <rfl.h>

/*
*===========================================================================
** Format  : VOID   MldItemSales(ITEMSALES *pItem, USHORT usType);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays dept/plu/set sales and modified discount
*
*   WARNING:  condiment prices for condiments that are PPI PLUs are updated by using
*             function ItemSalesCalcCondimentPPIReplace() so the condiment prices
*			  in ITEMSALES pItem are updated if there are PPI PLUs specified in the
*			  item data.
*===========================================================================
*/
SHORT   MldItemSales(ITEMSALES  *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;
#if 0
    if ((usType == MLD_NEW_ITEMIZE) ||
        (usType == MLD_UPDATE_ITEMIZE) ||
        (usType == MLD_ONLY_DISPLAY)) {

//        pItem->usOffset = 0;        /* clear offset code for void display */
    }
#endif
    if (MldChkDisplayPluNo(pItem) == FALSE) {
        /* not display if plu no is over 9950 */
        return(MldDummyScrollDisplay(usType, usScroll));
    }

    switch (pItem->uchMinorClass) {

    case CLASS_DEPT:                    /* dept */
    case CLASS_DEPTITEMDISC:
    case CLASS_PLU:                     /* PLU */
    case CLASS_PLUITEMDISC:
        sStatus = MldDeptPLU(pItem, usType, usScroll);
        break;

	case CLASS_ITEMORDERDEC:            // display order declare
		sStatus = MldOrderDeclaration(pItem, usType, usScroll);
        break;

    case CLASS_SETMENU:                 /* setmenu */
    case CLASS_SETITEMDISC:
        sStatus = MldSET(pItem, usType, usScroll);
        break;

    case CLASS_DEPTMODDISC:             /* dept modifiered disc. */
    case CLASS_PLUMODDISC:              /* plu modifiered disc. */
        sStatus = MldModDisc(pItem, usType, usScroll);
        break;

    case CLASS_SETMODDISC:              /* setmenu modifiered disc. */
        sStatus = MldModSET(pItem, usType, usScroll);
        break;

    default:
        sStatus = MldDummyScrollDisplay(usType, usScroll);
        break;

    }

    return (sStatus);
}
#include "termcfg.h"

/*
*===========================================================================
** Format  : VOID   MldItemSales(ITEMSALES *pItem, USHORT usType);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays dept/plu/set sales and modified discount
*===========================================================================
*/
SHORT   MldOrderDeclaration(ITEMSALES  *pItem, USHORT usType, USHORT usScroll)
{
	LONG   alAttrib[24][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
	TCHAR  aszScrollBuff[4][MLD_PRECOLUMN1 + 1]; /* scroll data save area */	
	USHORT	usWriteLength;
    USHORT usScrollLine = 0;            /* number of lines to be printed */

	tcharnset(aszScrollBuff[0],0x20, TCHARSIZEOF(aszScrollBuff));
	memcpy(aszScrollBuff, pItem->aszMnemonic, _tcslen(pItem->aszMnemonic) * sizeof(TCHAR));
	usScrollLine++;

    if ((usType == MLD_NEW_ITEMIZE) ||
        (usType == MLD_UPDATE_ITEMIZE) ||
        (usType == MLD_ONLY_DISPLAY)) {

        pItem->usItemOffset = 0;        /* clear offset code for void display */
    }
    if ((pItem->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
		if(pItem->uchColorPaletteCode)
		{
			memset(&alAttrib[0][0], pItem->uchColorPaletteCode, sizeof(alAttrib));
		}else
		{
			memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
		}
    } 

#if 1
	usWriteLength = MLD_PRECOLUMN1;
#else
	if(CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT3) && (uchMldSystem != MLD_DRIVE_THROUGH_3SCROLL))
	{
		usWriteLength = MLD_PRECOLUMN1;
	} else
	{
		usWriteLength = MLD_DR3COLUMN;
	}
#endif

	usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[0], MLD_PRECHECK_SYSTEM);

    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, usWriteLength, &alAttrib[0][0]));
}
/***** End Of Source *****/
