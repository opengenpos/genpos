
#if defined(POSSIBLE_DEAD_CODE)
	// This file is a candidate for removal with the next release of source code 
	// These MLD routines are no longer supported with Rel 2.1.0 and
	// later since they apply to the 2x20 display rather than the standard
	// LCD display for new hardware.
	//
	// This code originally used with the early NCR 7448 terminal that did
	// not have an LCD but instead used a two line display with indicator
	// lights.


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
* Title       : Multiline Display Popup Display for 2x20
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDPOP2.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*        MldCreatePopUp(): create popup window
*        MldDeletePopUp(): delete popup window
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Nov-17-95 : 03.01.00 :  M.Ozawa   : Initial
* Jul-23-98 : 03.03.00 :  M.Ozawa   : Add more than 20 items scroll
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
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbstb.h>
#include <rfl.h>
#include <display.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

VOID MldDispOEPLeadthrough(REGDISPMSG *pDisplayData, TCHAR *pLeadThrough);
VOID MldDispOEPChildPlu(REGDISPMSG *pDisplayData, ORDERPMT *pOrderPrompt);

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
MLDPOPORDER FAR MldPopOrder;    /* order entry prompt save area */

/*
*===========================================================================
** Format  : SHORT MldCreatePopUp(VOID *pData)
*
*   Input  : VOID   *pData      -popup display data
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: Create popup window on the scroll#1 display
============================================================================
*/
SHORT MldCreatePopUp(VOID *pData)
{
    if (((MLDITEMDATA *)pData)->uchMajorClass != CLASS_POPUP) {

        return(MLD_ERROR);
    }

    switch (((MLDITEMDATA *)pData)->uchMinorClass) {
    case CLASS_POPORDERPMT:

        MldScroll1Buff.uchDispCond = 0; /* reset scroll counter */
        MldDispOrderEntryPrompt2(pData, MLD_NEW_ITEMIZE); /* order entryp prompt */

        /* save order entry prompt data to mld buffer */
        _RflFMemCpy(&MldPopOrder, pData, sizeof(MLDPOPORDER)); /* ### Mod (2171 for Win32 V1.0) */

        usMldPopupHandle = CLASS_POPORDERPMT;
        break;

    default:
        break;

    }

    return(MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : SHORT MldUpdatePopUp(VOID *pData)
*
*   Input  : VOID   *pData      -popup display data
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: Update popup window on the scroll#1 display
============================================================================
*/
SHORT MldUpdatePopUp(VOID *pData)
{

    if (((MLDITEMDATA *)pData)->uchMajorClass != CLASS_POPUP) {

        return(MLD_ERROR);
    }

    if (!usMldPopupHandle) return (MLD_ERROR);

/*
    if (((MLDITEMDATA *)pData)->uchMinorClass != (UCHAR)usMldPopupHandle) {
        return(MLD_ERROR);
    }
*/
    switch (((MLDITEMDATA *)pData)->uchMinorClass) {
    case CLASS_POPORDERPMT:

        MldScroll1Buff.uchDispCond = 0; /* reset scroll counter */
        MldDispOrderEntryPrompt2(pData, MLD_NEW_ITEMIZE); /* order entryp prompt */

        /* save order entry prompt data to mld buffer */
        _RflFMemCpy(&MldPopOrder, pData, sizeof(MLDPOPORDER));

        break;

    case CLASS_POPORDERPMT_DOWN:  /* V3.3 */

        MldScroll1Buff.uchDispCond = 0; /* reset scroll counter */
        MldDispOrderEntryPrompt2(pData, MLD_LOWER_CURSOR); /* order entryp prompt */

        /* save order entry prompt data to mld buffer */
        _RflFMemCpy(&MldPopOrder, pData, sizeof(MLDPOPORDER));

        break;

    case CLASS_POPORDERPMT_UP:  /* V3.3 */

        MldScroll1Buff.uchDispCond = 20+1; /* reset scroll counter */
        MldDispOrderEntryPrompt2(pData, MLD_UPPER_CURSOR); /* order entryp prompt */

        /* save order entry prompt data to mld buffer */
        _RflFMemCpy(&MldPopOrder, pData, sizeof(MLDPOPORDER));

        break;

    default:
        break;

    }

    return(MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : SHORT MldPopupDownCursor(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: scroll down order entry prompt display
============================================================================
*/
SHORT MldPopUpDownCursor(VOID)
{
    MLDPOPORDER PopOrder;

    if (!usMldPopupHandle) return (MLD_ERROR);

    _RflFMemCpy(&PopOrder, &MldPopOrder, sizeof(MLDPOPORDER));

    return (MldDispOrderEntryPrompt2(&PopOrder, MLD_LOWER_CURSOR)); /* order entryp prompt */

}

/*
*===========================================================================
** Format  : SHORT MldPopupUpCursor(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: scroll down order entry prompt display
============================================================================
*/
SHORT MldPopUpUpCursor(VOID)
{
    MLDPOPORDER PopOrder;

    if (!usMldPopupHandle) return (MLD_ERROR);

    _RflFMemCpy(&PopOrder, &MldPopOrder, sizeof(MLDPOPORDER));

    return (MldDispOrderEntryPrompt2(&PopOrder, MLD_UPPER_CURSOR)); /* order entryp prompt */

}

/*
*===========================================================================
** Format  : SHORT MldPopUpRedisplay(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: redisplay current position of oep display
============================================================================
*/
SHORT MldPopUpRedisplay(USHORT usType)
{
    MLDPOPORDER PopOrder;

    if (!usMldPopupHandle) return (MLD_ERROR);

    _RflFMemCpy(&PopOrder, &MldPopOrder, sizeof(MLDPOPORDER));

    MldDispOrderEntryPrompt2(&PopOrder, usType);    /* order entryp prompt */

    return (MLD_SUCCESS);

}

SHORT MldDispOrderEntryPrompt2( MLDPOPORDER *pData, USHORT usType)
{

	REGDISPMSG      DisplayData = { 0 };
    USHORT          usDispOrder;
    SHORT           sReturn = MLD_SUCCESS;

    DisplayData.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    _tcsncpy(DisplayData.aszAdjMnem, pData->aszAdjective, STD_ADJMNEMO_LEN);
    _tcsncpy(DisplayData.aszStringData, pData->aszPluName, PARA_PLU_LEN);
    DisplayData.lAmount = pData->lPrice;
    DisplayData.fbSaveControl = DISP_SAVE_TYPE_4;

    flDispRegDescrControl |= COMPLSRY_CNTRL;
    flDispRegKeepControl  |= COMPLSRY_CNTRL;

    usDispOrder = (USHORT)MldScroll1Buff.uchDispCond;
    if (usType == MLD_NEW_ITEMIZE) {

        /* display leadthrough */
        MldDispOEPLeadthrough(&DisplayData, pData->aszLeadThrough);
        MldScroll1Buff.uchDispCond = 0;

    } else if (usType == MLD_LOWER_CURSOR) {
        if (usDispOrder < 20) {                         /* Jul/24 */
            usDispOrder++;
            if (pData->OrderPmt[usDispOrder-1].uchOrder) {

                /* display next child plu */
                MldDispOEPChildPlu(&DisplayData, &(pData->OrderPmt[usDispOrder-1]));
                MldScroll1Buff.uchDispCond = (UCHAR)usDispOrder;
            }
        } else {
            sReturn = MLD_CURSOR_BOTTOM;   /* V3.3 */
        }
    } else if (usType == MLD_UPPER_CURSOR) {
        if (usDispOrder) usDispOrder--;
        if (usDispOrder) {

            /* display previous child plu */
            MldDispOEPChildPlu(&DisplayData, &(pData->OrderPmt[usDispOrder-1]));

        } else {

            /* display leadthrough */
            if (pData->OrderPmt[0].uchOrder == 11) {    /* display only top, V3.3 */
                MldDispOEPLeadthrough(&DisplayData, pData->aszLeadThrough);
            }
            sReturn = MLD_CURSOR_TOP;   /* V3.3 */
        }
        MldScroll1Buff.uchDispCond = (UCHAR)usDispOrder;
    } else {        /* redisplay current position */

        if (usDispOrder) {

            /* display child plu */
            MldDispOEPChildPlu(&DisplayData, &(pData->OrderPmt[usDispOrder-1]));

        } else {

            /* display leadthrough */
            MldDispOEPLeadthrough(&DisplayData, pData->aszLeadThrough);
        }
    }

    flDispRegDescrControl &= ~COMPLSRY_CNTRL;
    flDispRegKeepControl  &= ~COMPLSRY_CNTRL; 

    return (sReturn);
}

VOID MldDispOEPLeadthrough(REGDISPMSG *pDisplayData, TCHAR *pLeadThrough)
{
    pDisplayData->uchMinorClass = CLASS_REGDISP_OEP_LEAD;
    _tcsncpy(pDisplayData->aszMnemonic, pLeadThrough, PARA_LEADTHRU_LEN);
    //memcpy(pDisplayData->aszMnemonic, pLeadThrough, PARA_LEADTHRU_LEN);

    DispWrite(pDisplayData);        /* display the above data */
}

VOID MldDispOEPChildPlu(REGDISPMSG *pDisplayData, ORDERPMT *pOrderPrompt)
{
    pDisplayData->uchMinorClass = CLASS_REGDISP_OEP_PLU;
    _tcsncpy(pDisplayData->aszAdjMnem2, pOrderPrompt->aszAdjective, STD_ADJMNEMO_LEN);
    _tcsncpy(pDisplayData->aszMnemonic, pOrderPrompt->aszPluName, PARA_PLU_LEN);
#pragma message("UCHAR CAST IS OKAY, THIS CODE ONLY USED WITH 2x20 OP DISPLAYS")
    pDisplayData->uchFsc = (UCHAR)pOrderPrompt->uchOrder;

    DispWrite(pDisplayData);        /* display the above data */
}

/*
*===========================================================================
** Format  : SHORT MldPopUpGetCursorDisplay(VOID *pData)
*
*   Input  : none
*   Output : none
*   InOut  : VOID *pData
** Return  : MLD_SUCCESS
*
** Synopsis: Get current displayed data structer of OEP
============================================================================
*/
SHORT MldPopUpGetCursorDisplay(VOID *pData)
{
    USHORT  usDispOrder;
    USHORT  usReturn;

    if (!usMldPopupHandle) return (MLD_ERROR);

    if (MldScroll1Buff.uchDispCond == 0) {
        return(MLD_ERROR);
    }

    usDispOrder = (USHORT)MldScroll1Buff.uchDispCond;
    _RflFMemCpy(pData, &(MldPopOrder.OrderPmt[usDispOrder-1]), sizeof(ORDERPMT));
    usReturn = MLD_SUCCESS;

/*
    switch(usMldPopupHandle) {
    case CLASS_POPORDERPMT:

        usDispOrder = (USHORT)MldScroll1Buff.uchDispCond;
        _fmemcpy(pData, &(MldPopOrder.OrderPmt[usDispOrder-1]), sizeof(ORDERPMT));
        usReturn = MLD_SUCCESS;
        break;

    default:
        usReturn = MLD_ERROR;
    }
*/
    return(usReturn);
}
/*
*===========================================================================
** Format  : SHORT MldDeletePopUp(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: delete popup window and recover scroll#1 display
*===========================================================================
*/
SHORT MldDeletePopUp(VOID)
{
    usMldPopupHandle = 0;
    MldScroll1Buff.uchDispCond = 0;
    return(MLD_SUCCESS);
}

SHORT MldForceDeletePopUp(VOID)
{
    usMldPopupHandle = 0;
    MldScroll1Buff.uchDispCond = 0;
    return(MLD_SUCCESS);
}

#endif

/**** End of File ****/
