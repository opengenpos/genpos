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
* Title       : Multiline Display  Foreign Tender key
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDFTND.C                           
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*  MldForeignTender() : display foreign tender operation 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-10-95 : 03.00.00 : M.Ozawa    : Initial
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
#include <rfl.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <mld.h>
#include <mldlocal.h>
#include "mldrin.h"


/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID   PrtGetMoneyMnem(UCHAR uchTendMinor, UCHAR *puchTran, UCHAR *puchSym)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays received on acount
*===========================================================================
*/
VOID    MldGetMoneyMnem(UCHAR uchTendMinor, USHORT *pusTran, UCHAR *puchSym)
{
	*puchSym = 0;
	*pusTran = 0;
    if (uchTendMinor >= CLASS_FOREIGN1 && uchTendMinor <= CLASS_FOREIGN2) {
        *pusTran = (uchTendMinor - CLASS_FOREIGN1 + TRN_FT1_ADR);
        *puchSym  = (uchTendMinor - CLASS_FOREIGN1 + SPC_CNSYMFC1_ADR);
    } else if (uchTendMinor >= CLASS_FOREIGN3 && uchTendMinor <= CLASS_FOREIGN8) {
        *pusTran = (uchTendMinor - CLASS_FOREIGN3 + TRN_FT3_ADR);
        *puchSym  = (uchTendMinor - CLASS_FOREIGN3 + SPC_CNSYMFC3_ADR);
    }  else if (uchTendMinor >= CLASS_TENDER1 && uchTendMinor <= CLASS_TENDER8) {
        *pusTran = (uchTendMinor - CLASS_TENDER1 + TRN_TEND1_ADR);
    }  else if (uchTendMinor >= CLASS_TENDER9 && uchTendMinor <= CLASS_TENDER11) {
        *pusTran = (uchTendMinor - CLASS_TENDER9 + TRN_TEND9_ADR);
    }  else if (uchTendMinor >= CLASS_TENDER12 && uchTendMinor <= CLASS_TENDER20) {
        *pusTran = (uchTendMinor - CLASS_TENDER12 + TRN_TENDER12_ADR);
	} else {
		NHPOS_ASSERT(uchTendMinor <= CLASS_TENDER20);
		return;
	}
}

/*
*===========================================================================
** Format  : VOID  MldForeignTender(ITEMTENDER *pItem, USHORT usType);
*
*   Input  : ITEMTENDER       *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays foreign tender operation
*===========================================================================
*/
SHORT MldForeignTender(ITEMTENDER  *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {

        return (MldDriveForeignTender(pItem, usType, usScroll));

    } else {

        return (MldPrechkForeignTender(pItem, usType, usScroll));
    }
}

/*
*===========================================================================
** Format  : VOID  MldForeignTender(ITEMTENDER *pItem, USHORT usType);
*
*   Input  : ITEMTENDER       *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays foreign tender operation
*===========================================================================
*/
SHORT MldPrechkForeignTender(ITEMTENDER  *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[8][MLD_PRECOLUMN1 + 1]; /* print data save area */
    LONG   alAttrib[8][MLD_PRECOLUMN1 + 1];                 /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
	USHORT  usTranAddr;
	UCHAR   uchSymAddr;

    /* get foreign 1 or 2 mnemonic addresses */
	MldGetMoneyMnem(pItem->uchMinorClass, &usTranAddr, &uchSymAddr);

    /* initialize the buffer */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));

    if ((pItem->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

    /* -- set void mnemonic and number -- */
    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_PRECHECK_SYSTEM);

    usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber, MLD_PRECHECK_SYSTEM);

	if (CliParaMDCCheckField(MDC_TAX2_ADR, MDC_PARAM_BIT_A) == 0) {
		/* -- set foreign amount and rate -- */
		usScrollLine += MldForeign(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->lForeignAmount, uchSymAddr, pItem->auchTendStatus[0], pItem->ulFCRate, pItem->auchTendStatus[1], MLD_PRECHECK_SYSTEM);
	                            
		/* -- set foreign tender mnemonic and amount -- */
		usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_FT_EQUIVALENT, pItem->lTenderAmount, MLD_PRECHECK_SYSTEM);
	} else {
		/* -- set foreign amount and rate -- */
		usScrollLine += MldForeign(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->lForeignAmount, usTranAddr, pItem->auchTendStatus[0], pItem->ulFCRate, pItem->auchTendStatus[1], MLD_PRECHECK_SYSTEM);
		if (pItem->fbModifier & RETURNS_ORIGINAL) {
			usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], (TRN_FT_EQUIVALENT | STD_FORMATMASK_INDENT_4), pItem->lTenderAmount, MLD_PRECHECK_SYSTEM);
		}
	}

    /* -- set change mnemonic and amount -- */
    if (pItem->lChange) { 
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_REGCHG_ADR, pItem->lChange, MLD_PRECHECK_SYSTEM);
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : VOID  MldForeignTender(ITEMTENDER *pItem, USHORT usType);
*
*   Input  : ITEMTENDER       *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays foreign tender operation
*===========================================================================
*/
SHORT MldDriveForeignTender(ITEMTENDER  *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[10][MLD_DR3COLUMN + 1]; /* print data save area */
    LONG   alAttrib[10][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
	USHORT  usTranAddr;
	UCHAR   uchSymAddr;

    /* get foreign 1 or 2 mnemonic addresses */
	MldGetMoneyMnem(pItem->uchMinorClass, &usTranAddr, &uchSymAddr);

    /* initialize the buffer */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));

    if ((pItem->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

    /* -- set void mnemonic and number -- */
    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);

    usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set foreign amount and rate -- */
    usScrollLine += MldForeign(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->lForeignAmount, uchSymAddr, pItem->auchTendStatus[0], pItem->ulFCRate, pItem->auchTendStatus[1], MLD_DRIVE_THROUGH_3SCROLL);
                            
    /* -- set foreign tender mnemonic and amount -- */
	if (CliParaMDCCheckField(MDC_TAX2_ADR, MDC_PARAM_BIT_A) == 0) {
		usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_FT_EQUIVALENT, pItem->lTenderAmount, MLD_DRIVE_THROUGH_3SCROLL);
	} else if (pItem->fbModifier & RETURNS_ORIGINAL) {
		usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], (TRN_FT_EQUIVALENT | STD_FORMATMASK_INDENT_4), pItem->lTenderAmount, MLD_PRECHECK_SYSTEM);
	}

    /* -- set change mnemonic and amount -- */
    if (pItem->lChange) { 
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_REGCHG_ADR, pItem->lChange, MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/***** End Of Source *****/
