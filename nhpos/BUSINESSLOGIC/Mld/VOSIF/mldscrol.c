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
* Title       : Multiline Display Scroll Up/Down Control
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDSCROL.C
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
* Apl-05-95 : 03.00.00 : M.Ozawa    : Initial
* 
*** NCR2171 **
* Aug-26-99 : 01.00.00 : M.Teraki   : initial (for Win32)
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
/**------- MS - C ---------**/
#include	<tchar.h>
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <trans.h>
#include <rfl.h>
#include <prtprty.h>
#include <vos.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/*
============================================================================
+                  S T A T I C   R A M s
============================================================================
*/

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s                      +
;========================================================================
**/

static SHORT MldReadFilePRTIDX (USHORT usScroll, USHORT usItemNumber, PRTIDX *pIndex)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
	USHORT usReadOffset;
	ULONG  ulActualBytesRead;

    switch (usScroll) {
		case MLD_SCROLL_1:
			pMldCurTrnScrol = &MldScroll1Buff;
			break;

		case MLD_SCROLL_2:
			pMldCurTrnScrol = &MldScroll2Buff;
			break;

		case MLD_SCROLL_3:
			pMldCurTrnScrol = &MldScroll3Buff;
			break;

		default:
			return(MLD_ERROR);
    }

    usReadOffset = pMldCurTrnScrol->usIndexHeadOffset +
                    ( sizeof(PRTIDXHDR) + ( sizeof(PRTIDX) * usItemNumber));

    //RPH SR# 201
	MldReadFile (usReadOffset, pIndex, sizeof(PRTIDX),
                pMldCurTrnScrol->sIndexHandle, &ulActualBytesRead);

	return (ulActualBytesRead == sizeof(PRTIDX));
}

/*
*===========================================================================
** Format  : USHORT   MldUpCursor(VOID);
*               
*    Input : VOID             *pItem     -Item Data address
*            USHORT           usType     -First display position
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function is called from application user and display item.
*===========================================================================
*/
static SHORT   MldLocalUpCursor(USHORT usScroll)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
    PRTIDX     Index;
    PRTIDXHDR  IndexHeader;
    UCHAR   auchTempStoWork[sizeof(ITEMSALESDISC)];
    CHAR    achBuff[sizeof(ITEMDATASIZEUNION)];
    UCHAR   uchReadItem;
    USHORT  usReadLen;
    USHORT  usCurRow, usCurCol;
    SHORT   sStatus = MLD_SUCCESS;
    USHORT  i;
    SHORT   sFileHandle;
    SHORT   sIndexHandle;
	ULONG	ulActualBytesRead;//RPH SR# 201
	UCHAR   uchLineStatus = 0;

    /* check current active scroll */
    switch (uchMldCurScroll) {
    case MLD_SCROLL_1:
        pMldCurTrnScrol = &MldScroll1Buff;
        break;

    case MLD_SCROLL_2:
        pMldCurTrnScrol = &MldScroll2Buff;
        break;

    case MLD_SCROLL_3:
        pMldCurTrnScrol = &MldScroll3Buff;
        break;

    default:
        return(MLD_ERROR);
    }

    if ((pMldCurTrnScrol->sFileHandle < 0) || (pMldCurTrnScrol->sIndexHandle < 0)) {
        return(MLD_ERROR);
    }

    /* waiting order displayed */
    if (pMldCurTrnScrol->uchStatus & MLD_WAIT_STATUS) {
        return (MLD_ERROR);
    }

    /* if storage file is overflowed */
    if (pMldCurTrnScrol->uchStatus & MLD_STORAGE_OVERFLOW) {
        return (MLD_ERROR);
    }

    /* if no data is displayed */
    if (pMldCurTrnScrol->uchItemLine[0] == 0) {
        if (pMldCurTrnScrol->uchCurItem == 0) {
            return(MLD_SUCCESS);
        }
    }

    if (uchMldSystem2 == MLD_SPLIT_CHECK_SYSTEM) {
        if (uchMldCurScroll & (MLD_SCROLL_2 | MLD_SCROLL_3)) {
            /* consolidate display data for scroll control */
            if ((pMldCurTrnScrol->uchStatus & MLD_SCROLL_UP) && (pMldCurTrnScrol->uchStatus2 & MLD_UNMATCH_DISPLAY)) {

                sFileHandle = pMldCurTrnScrol->sFileHandle;     /* save for initialize */
                sIndexHandle = pMldCurTrnScrol->sIndexHandle;

                MldLocalDisplayInit((USHORT)uchMldCurScroll, pMldCurTrnScrol->usAttrib);

                pMldCurTrnScrol->sFileHandle = sFileHandle;
                pMldCurTrnScrol->sIndexHandle = sIndexHandle;

                /* display from end of storage */
                MldDispAllReverseData((USHORT)uchMldCurScroll);
            }
        }
    }

    pMldCurTrnScrol->uchStatus2 |= MLD_MOVE_CURSOR;

    /* check current cursor position and move cursor for void search */
    VosGetCurPos(pMldCurTrnScrol->usWinHandle, &usCurRow, &usCurCol);
    if (usCurRow != 0) {
        VosSetCurPos(pMldCurTrnScrol->usWinHandle, (USHORT)(usCurRow-1), 0);
        return (MLD_SUCCESS);
    }

    /* check top of the storage file is displayed or not */
    if (pMldCurTrnScrol->uchItemLine[0] <= 1) {
		pMldCurTrnScrol->uchInvisiUpperLines = 0;
        return(MLD_SUCCESS);
    }
	else {
		// If the number of invisable lines above the current displayed top
		// is less than 1 but there are some other items so force it to
		// be one.
		if (pMldCurTrnScrol->uchInvisiUpperLines < 1)
			pMldCurTrnScrol->uchInvisiUpperLines = 1;
	}

    /* read current top item and get its status */
    uchReadItem = pMldCurTrnScrol->uchItemLine[0];
    uchLineStatus = pMldCurTrnScrol->uchLineStatus[0];
    if (uchReadItem == MLD_SPECIAL_ITEM) {      /* if top item is item discount */
        /* read next item position */
        for (i = 1; i < uchMldBottomCursor; i++) {
            uchReadItem = pMldCurTrnScrol->uchItemLine[i];
            if (uchReadItem != MLD_SPECIAL_ITEM) break;
        }
        if ((uchReadItem == MLD_SPECIAL_ITEM) || (uchReadItem == 0)) {
            uchReadItem = pMldCurTrnScrol->uchCurItem;
        }
        /* set parent plu position */
        uchReadItem--;
    } else if (uchReadItem == 0) {  /* top item is error corrected */
        uchReadItem = pMldCurTrnScrol->uchCurItem;
    } else {
		// If the scrolled up item is a duplicate of the current item displayed on the
		// first row then this means that the display has scrolled so that an item with
		// condiments is only showing some of its condiments.
		// So rather than going to the previous item, we will stay with this item
		// and show the complete item instead.  We turn off this indicator so that
		// the next time through we will go ahead and go back to the previous item
		// in the list.pMldCurTrnScrol->uchInvisiUpperDupItem
        if (uchReadItem > 1 && (uchLineStatus & MLD_LINESTATUS_FIRSTLINE)) {
            uchReadItem--;  /* decrement index offset to read previous item */
        }
		pMldCurTrnScrol->uchInvisiUpperDupItem = 0;
    }
    uchReadItem--;  /* decrement for index offset search */

    pMldCurTrnScrol->uchDispItem = uchReadItem;
    pMldCurTrnScrol->uchDispItem++;

    /* read index file header */
    //RPH SR# 201
	MldReadFile(pMldCurTrnScrol->usIndexHeadOffset, &IndexHeader, sizeof( PRTIDXHDR ), pMldCurTrnScrol->sIndexHandle, &ulActualBytesRead );

    for (; pMldCurTrnScrol->uchDispItem > 0; uchReadItem--, pMldCurTrnScrol->uchDispItem--) {      /* loop routine for zero row display */
        if ((pMldCurTrnScrol->uchDispItem == pMldCurTrnScrol->uchCurItem) && (pMldCurTrnScrol->uchStatus & MLD_CURITEM_TMP)) {
            /* read temporary storage */
            if (MldTempRestore(achBuff) == MLD_ERROR) {
                return (MLD_ERROR);
            }

            /* ---- display check ---- */
            if ( achBuff[ 0 ] == CLASS_ITEMSALES ) {
                if (achBuff [1] != CLASS_ITEMORDERDEC && MldChkDisplayPluNo ((ITEMSALES *)achBuff ) == FALSE) {
                    continue;
                }
            }
        } else if ((pMldCurTrnScrol->uchDispItem == (pMldCurTrnScrol->uchCurItem-(UCHAR)1)) && (pMldCurTrnScrol->uchStatus & MLD_ITEMDISC_STORE)) {
            /* read temporary storage */
            MldTmpSalesRestore((ITEMSALES *)&achBuff);

            /* ---- display check ---- */
            if ( achBuff[ 0 ] == CLASS_ITEMSALES ) {
                if (achBuff [1] != CLASS_ITEMORDERDEC && MldChkDisplayPluNo ((ITEMSALES *)achBuff ) == FALSE) {
                    continue;
                }
            }
        } else {
            if (pMldCurTrnScrol->uchDispItem > IndexHeader.uchNoOfItem) {
                return (MLD_ERROR);
            }

			MldReadFilePRTIDX (usScroll, uchReadItem, &Index);

            /* read itemize data which directed by index file offset */
            //RPH SR# 201
			MldReadFile(Index.usItemOffset, auchTempStoWork, sizeof(auchTempStoWork), pMldCurTrnScrol->sFileHandle, &ulActualBytesRead);

            /* decompress itemize data */
            memset(&achBuff, 0, sizeof(achBuff));
            usReadLen = RflGetStorageItem(achBuff, auchTempStoWork, RFL_WITH_MNEM);

            /* ---- skip item discount without parent plu */
            if (( achBuff[ 0 ] == CLASS_ITEMDISC ) && ( achBuff[ 1 ] == CLASS_ITEMDISC1 )) {
                continue;
            }

            /* ---- display check ---- */
            if ( achBuff[ 0 ] == CLASS_ITEMSALES ) {
                /* ---- set item qty/price by index data ---- */
                MldSetItemSalesQtyPrice( (ITEMSALES *)achBuff, &Index);
                MldSetSalesCouponQTY((ITEMSALES *)achBuff, pMldCurTrnScrol->sFileHandle, (USHORT)uchMldCurScroll);
                if (achBuff [1] != CLASS_ITEMORDERDEC && MldChkDisplayPluNo ((ITEMSALES *)achBuff ) == FALSE) {
                    continue;
                }
            }

            /* ---- display item discount with noun plu ---- */
            if ( MldIsSalesIDisc( achBuff )) {
                if ((pMldCurTrnScrol->uchItemLine[0] == MLD_SPECIAL_ITEM)
                        && (!pMldCurTrnScrol->uchInvisiUpperLines)) {
                    ;   /* if item discount is already displayed */
                        /* parent plu is not displayed */
                } else if (pMldCurTrnScrol->uchItemLine[0] == pMldCurTrnScrol->uchDispItem) {
                    ;   /* if item discount is already displayed */
                        /* parent plu is stripped */
                } else {
                    /* display item discount */
                    /* decompress itemize data */
                    memset(&achBuff, 0, sizeof(achBuff));
                    RflGetStorageItem(achBuff, auchTempStoWork+usReadLen, RFL_WITH_MNEM);

                    /* create format and display on the scroll area */
                    sStatus = MldLocalScrollWrite(achBuff, MLD_UPPER_CURSOR2, (USHORT)uchMldCurScroll);

                    memset(&achBuff, 0, sizeof(achBuff));
                    RflGetStorageItem(achBuff, auchTempStoWork, RFL_WITH_MNEM);

                    /* ---- set item qty/price by index data ---- */
                    MldSetItemSalesQtyPrice( (ITEMSALES *)achBuff, &Index);
                    MldSetSalesCouponQTY((ITEMSALES *)achBuff, pMldCurTrnScrol->sFileHandle, (USHORT)uchMldCurScroll);
                    MldSetItemSalesQtyPrice( (ITEMSALES *)achBuff, &Index);
                }
            }
        }

        if (( achBuff[ 0 ] == CLASS_ITEMDISC ) && ( achBuff[ 1 ] == CLASS_ITEMDISC1 )) {
            sStatus = MldLocalScrollWrite(achBuff, MLD_UPPER_CURSOR2, (USHORT)uchMldCurScroll);
        } else {
            sStatus = MldLocalScrollWrite(achBuff, MLD_UPPER_CURSOR, (USHORT)uchMldCurScroll);
        }

        if (sStatus != MLD_NOT_DISPLAY) break;
    }

    /* reset cursor and continue symbol if reached to top */
    if (pMldCurTrnScrol->uchDispItem == 0) {
        MldClearContinue((USHORT)uchMldCurScroll, MLD_UPPER_ARROW);
        VosSetCurPos(pMldCurTrnScrol->usWinHandle, 0, 0);
    }

    return (MLD_SUCCESS);
}


/*
*===========================================================================
** Format  : USHORT   MldUpCursor(VOID);
*               
*    Input : VOID             *pItem     -Item Data address
*            USHORT           usType     -First display position
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function is called from application user and display item.
*===========================================================================
*/
SHORT   MldUpCursor(VOID)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    sStatus = MldLocalUpCursor(uchMldCurScroll);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}


/*
*===========================================================================
** Format  : USHORT   MldDownCursor(VOID);
*               
*    Input : VOID             *pItem     -Item Data address
*            USHORT           usType     -First display position
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function is called from application user and display item.
*===========================================================================
*/
SHORT   MldLocalDownCursor(USHORT usScroll)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
	USHORT     usDisplayCount = 0;
    PRTIDX     Index;
    PRTIDXHDR  IndexHeader;
    UCHAR   auchTempStoWork[sizeof(ITEMSALESDISC)];
    CHAR    achBuff[sizeof(ITEMDATASIZEUNION)];
    USHORT  uchReadItem, uchReadItemFirst;
    USHORT  usReadLen;
    USHORT  usCurRow, usCurCol;
    SHORT   sStatus = MLD_SUCCESS;
    SHORT   i;
    SHORT   sFileHandle;
    SHORT   sIndexHandle;
	ULONG	ulActualBytesRead;//RPH SR# 201

    switch (usScroll) {
		case MLD_SCROLL_1:
			pMldCurTrnScrol = &MldScroll1Buff;
			break;

		case MLD_SCROLL_2:
			pMldCurTrnScrol = &MldScroll2Buff;
			break;

		case MLD_SCROLL_3:
			pMldCurTrnScrol = &MldScroll3Buff;
			break;

		default:
			return(MLD_ERROR);
    }

    if ((pMldCurTrnScrol->sFileHandle < 0) || (pMldCurTrnScrol->sIndexHandle < 0)) {
        return(MLD_ERROR);
    }

    /* waiting order displayed */
    if (pMldCurTrnScrol->uchStatus & MLD_WAIT_STATUS) {
        return (MLD_ERROR);
    }

    /* if storage file is overflowed */
    if (pMldCurTrnScrol->uchStatus & MLD_STORAGE_OVERFLOW) {
        return (MLD_ERROR);
    }

    if (uchMldSystem2 == MLD_SPLIT_CHECK_SYSTEM) {
        if (uchMldCurScroll & (MLD_SCROLL_2|MLD_SCROLL_3)) {
            /* consolidate display data for scroll control */
            if ((pMldCurTrnScrol->uchStatus & MLD_SCROLL_UP) &&
                (pMldCurTrnScrol->uchStatus2 & MLD_UNMATCH_DISPLAY)) {

                sFileHandle = pMldCurTrnScrol->sFileHandle;     /* save for initialize */
                sIndexHandle = pMldCurTrnScrol->sIndexHandle;

                MldLocalDisplayInit (usScroll, pMldCurTrnScrol->usAttrib);

                pMldCurTrnScrol->sFileHandle = sFileHandle;
                pMldCurTrnScrol->sIndexHandle = sIndexHandle;

                /* display from end of storage */
                MldDispAllReverseData (usScroll);
            }
        }
    }

    pMldCurTrnScrol->uchStatus2 |= MLD_MOVE_CURSOR;

    /*
		check current cursor position and see if we are at the bottom of the
		display area.  If not, then we merely need to move the cursor down.
		With this code we will set our cursor to the correct position on the
		physical screen as well as the logical cursor.
	 */
    VosGetCurPos (pMldCurTrnScrol->usWinHandle, &usCurRow, &usCurCol);
	pMldCurTrnScrol->uchCurCursor = (UCHAR)usCurRow;
    if (pMldCurTrnScrol->uchCurCursor < pMldCurTrnScrol->usMldBottomCursor - 1) {
        if (pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] > 0) {
			pMldCurTrnScrol->uchCurCursor++;
            VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);
        }
        return (MLD_SUCCESS);
    }

    /*
		Let's see if there is any data already on the this line of the display area.  If not, then
		we may be done but we may not since the bottom line may be empty yet there is still
		more data to be displayed.

		We check to see if the currently displayed item is the most recently added item.
		If not then we do have some additional items to display so we need to do so..
	 */
    if (pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] == 0) {
		if (pMldCurTrnScrol->uchDispItem >= pMldCurTrnScrol->uchCurItem) {
			return(MLD_SUCCESS);
		}
		// there is more data to show so we will assume that the bottom line is
		// blank so lets decrement the logical cursor to the previous line which
		// should have an item associated with it.
		pMldCurTrnScrol->uchCurCursor--;
        VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);
    }

    /* check bottom of the storage file is displayed or not */
    if ((pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] == pMldCurTrnScrol->uchCurItem) &&
            (pMldCurTrnScrol->uchInvisiLowerLines == 0)) {

        MldClearContinue (usScroll, MLD_LOWER_ARROW);
        VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);
        return(MLD_SUCCESS);
    }

    /* read current bottom item */
    pMldCurTrnScrol->uchDispItem = pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor];

	// Allow for the redisplay of a partially displayed item.  For instance, if
	// we have an entree that is composed of a main item, say Filet Mignon, with
	// a set of condiments, say Rare (cooking temperature), Italian (dressing on a salad)
	// and Fries, the item with its condiments may be only partially dispalyed.  So
	// lets update the display showing the entire item with its condiments and then
	// we will display the next item, if there should be one.
    uchReadItemFirst = uchReadItem = pMldCurTrnScrol->uchDispItem - 1;

	// Make sure that the cursor is positioned at the line that we want to begin
	// our update on.
	usCurRow = pMldCurTrnScrol->uchCurCursor;
	while (pMldCurTrnScrol->uchDispItem == pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor]) {
		pMldCurTrnScrol->uchCurCursor--;
		if (pMldCurTrnScrol->uchCurCursor == 0)
			break;
	}
	if (pMldCurTrnScrol->uchDispItem != pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor])
		pMldCurTrnScrol->uchCurCursor++;
	VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);
	usDisplayCount = usCurRow - pMldCurTrnScrol->uchCurCursor + 1;

    if (uchReadItem == MLD_SPECIAL_ITEM) {      /* if bottom item is item discount */
        /* set paraent plu position */
        for (i = pMldCurTrnScrol->uchCurCursor - 1; i >= 0; i--) {
            uchReadItem = pMldCurTrnScrol->uchItemLine[i];
            if (uchReadItem != MLD_SPECIAL_ITEM) break;
        }
        if ((uchReadItem == MLD_SPECIAL_ITEM) || (uchReadItem == 0)) {
            uchReadItem = pMldCurTrnScrol->uchCurItem - (UCHAR)1;
        }
        /* check bottom of the storage file is displayed or not */
        if ((uchReadItem == pMldCurTrnScrol->uchCurItem) &&
                (pMldCurTrnScrol->uchInvisiLowerLines == 0)) {
            return(MLD_SUCCESS);
        }
    }
    if (pMldCurTrnScrol->uchLowerItemDisc) {    /* if next display is item discount */
        if (((uchReadItem + (UCHAR)1) == pMldCurTrnScrol->uchCurItem) 
                && (pMldCurTrnScrol->uchStatus & MLD_CURITEM_TMP)) {
            ;       /* not decrement index offset to read previous buffer */
        } else {
            uchReadItem--;  /* decrement index offset to read parent item */
        }
    } else {
//        if (pMldCurTrnScrol->uchInvisiLowerLines ) {
//            uchReadItem--;  /* decrement index offset to read same item */
//        }
    }

    /* set void consolidation display flag R3.1 */
    /* check bottom of the storage file is displayed or not */
    if (uchReadItem >= pMldCurTrnScrol->uchCurItem) {
        if (pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->usMldBottomCursor] == MLD_SPECIAL_ITEM) {
            if ((pMldCurTrnScrol->uchDispItem + 1) > pMldCurTrnScrol->uchCurItem) {
                MldClearContinue (usScroll, MLD_LOWER_ARROW);
                VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->usMldBottomCursor, 0);
                return(MLD_SUCCESS);
            }
        }
    }

    /* read index file header */
    //RPH SR# 201
	MldReadFile (pMldCurTrnScrol->usIndexHeadOffset, &IndexHeader, sizeof( PRTIDXHDR ),
		         pMldCurTrnScrol->sIndexHandle, &ulActualBytesRead );

    for (;
            pMldCurTrnScrol->uchDispItem <= pMldCurTrnScrol->uchCurItem;
            uchReadItem++, pMldCurTrnScrol->uchDispItem++) {      /* loop routine for zero row display */

        if ((pMldCurTrnScrol->uchDispItem == pMldCurTrnScrol->uchCurItem) 
                && (pMldCurTrnScrol->uchStatus & MLD_CURITEM_TMP)) {

            /* read temporary storage */
            if (MldTempRestore (achBuff) == MLD_ERROR) {
                return (MLD_ERROR);
            }

            /* ---- display check ---- */
            if ( achBuff [0] == CLASS_ITEMSALES ) {
                if (achBuff [1] != CLASS_ITEMORDERDEC && MldChkDisplayPluNo ((ITEMSALES *)achBuff ) == FALSE) {
                    continue;
                }
            }

        } else if ((pMldCurTrnScrol->uchDispItem == (pMldCurTrnScrol->uchCurItem - (UCHAR)1))
                && (pMldCurTrnScrol->uchStatus & MLD_ITEMDISC_STORE)) {

            /* read temporary storage */
            MldTmpSalesRestore ((ITEMSALES *)&achBuff);

            /* ---- display check ---- */
            if ( achBuff [0] == CLASS_ITEMSALES ) {
                if (achBuff [1] != CLASS_ITEMORDERDEC && MldChkDisplayPluNo ((ITEMSALES *)achBuff ) == FALSE) {
                    continue;
                }
            }
        } else {
            if (pMldCurTrnScrol->uchDispItem > IndexHeader.uchNoOfItem) {
                return (MLD_ERROR);
            }

			MldReadFilePRTIDX (usScroll, uchReadItem, &Index);

            /* read itemize data which directed by index file offset */
            //RPH SR# 201
			MldReadFile (Index.usItemOffset, auchTempStoWork, sizeof(auchTempStoWork),
                        pMldCurTrnScrol->sFileHandle, &ulActualBytesRead);

            /* decompress itemize data */
            memset (&achBuff, 0, sizeof(achBuff));
            usReadLen = RflGetStorageItem (achBuff, auchTempStoWork, RFL_WITH_MNEM);

            /* ---- skip item discount without parent plu */
            if (( achBuff [0] == CLASS_ITEMDISC ) &&
                ( achBuff [1] == CLASS_ITEMDISC1 )) {
                continue;
            }

            if ( achBuff [0] == CLASS_ITEMSALES ) {
                /* ---- set item qty/price by index data ---- */
                MldSetItemSalesQtyPrice ( (ITEMSALES *)achBuff, &Index);

                MldSetSalesCouponQTY ((ITEMSALES *)achBuff, pMldCurTrnScrol->sFileHandle, usScroll);

                /* ---- display check ---- */
                if (achBuff [1] != CLASS_ITEMORDERDEC && MldChkDisplayPluNo ((ITEMSALES *)achBuff ) == FALSE) {
                    continue;
                }
            }

            /* ---- display item discount with parent plu ---- */
            if ( MldIsSalesIDisc (achBuff)) {
                if ((pMldCurTrnScrol->uchItemLine[uchMldBottomCursor] == MLD_SPECIAL_ITEM) && (pMldCurTrnScrol->uchInvisiLowerLines)) {
                    ;       /* item discount is stripped */
                } else if ((!pMldCurTrnScrol->uchInvisiLowerLines) && (pMldCurTrnScrol->uchLowerItemDisc)) {
                    ;       /* if parent plu is already displayed */
                } else {
                    /* display parent plu */
                    sStatus = MldLocalScrollWrite (achBuff, MLD_LOWER_CURSOR, usScroll);
                }

                /* decompress itemize data */
                memset (&achBuff, 0, sizeof(achBuff));
                RflGetStorageItem (achBuff, auchTempStoWork + usReadLen, RFL_WITH_MNEM);
            }
        }

        /* create format and display on the scroll area */
        if ((achBuff [0]  == CLASS_ITEMDISC) && (achBuff [1] == CLASS_ITEMDISC1)) {
            sStatus = MldLocalScrollWrite (achBuff, MLD_LOWER_CURSOR2, usScroll);
        } else {
            sStatus = MldLocalScrollWrite (achBuff, MLD_LOWER_CURSOR, usScroll);
        }

		// Make sure that the cursor is positioned at the line that we want to begin
		// our next update on.  The MldLocalScroll
		usCurRow = pMldCurTrnScrol->uchCurCursor;
		while (pMldCurTrnScrol->uchDispItem == pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor]) {
			pMldCurTrnScrol->uchCurCursor++;
			if (pMldCurTrnScrol->uchCurCursor >= pMldCurTrnScrol->usMldBottomCursor) {
				pMldCurTrnScrol->uchCurCursor = pMldCurTrnScrol->usMldBottomCursor;
				break;
			}
		}
		VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);
		usDisplayCount = usCurRow - pMldCurTrnScrol->uchCurCursor + 1;

		if (uchReadItemFirst == uchReadItem)
			continue;

        if (sStatus != MLD_NOT_DISPLAY) break;
    }

    /* reset cursor and continue symbol if reached to bottom */
//    if (pMldCurTrnScrol->uchDispItem > pMldCurTrnScrol->uchCurItem) {
//        MldClearContinue (usScroll, MLD_LOWER_ARROW);
//        VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->usMldBottomCursor, 0);
//        pMldCurTrnScrol->uchCurCursor = (UCHAR)pMldCurTrnScrol->usMldBottomCursor;
//    }
    return (MLD_SUCCESS);
}


/*
*===========================================================================
** Format  : USHORT   MldDownCursor(VOID);
*               
*    Input : VOID             *pItem     -Item Data address
*            USHORT           usType     -First display position
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function is called from application user and display item.
*===========================================================================
*/
SHORT   MldDownCursor(VOID)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    sStatus = MldLocalDownCursor(uchMldCurScroll);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}

/*
*===========================================================================
** Format  : USHORT   MldGetCursorDisplay(USHORT usScroll, VOID *pData1, VOID *pData2);
*               
*    Input : USHORT           usScroll     -scroll display
*
*   Output : VOID             *pData1      -data structure of cursor position
*                             *pData2      -data structure of cursor position
*
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function is called from application user and sets data
*            structure of cursor position
*===========================================================================
*/
static SHORT   MldLocalGetCursorDisplay(USHORT usScroll, VOID *pData1, VOID *pData2, USHORT usGetType)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
    CHAR       achBuff[sizeof(ITEMDATASIZEUNION)];
    USHORT     uchReadItem, uchReadItemSave;
    USHORT     usCurRow, usCurCol;
    SHORT      i;
	USHORT     condNum = 0;
	ITEMSALES  Data1Temp = {0};

    switch (usScroll) {
		case MLD_SCROLL_1:
			pMldCurTrnScrol = &MldScroll1Buff;
			break;

		case MLD_SCROLL_2:
			pMldCurTrnScrol = &MldScroll2Buff;
			break;

		case MLD_SCROLL_3:
			pMldCurTrnScrol = &MldScroll3Buff;
			break;

		default:
			return(MLD_ERROR);
    }

	if(pData2 && usGetType == MLD_GET_COND_NUM)
	{
		*((USHORT *)pData2) = 0;  // initialize condNum to zero
	}

    if ((pMldCurTrnScrol->sFileHandle < 0) || (pMldCurTrnScrol->sIndexHandle < 0)) {
        return(MLD_ERROR);
    }

    /* waiting order displayed */
    if (pMldCurTrnScrol->uchStatus & MLD_WAIT_STATUS) {
        return (MLD_ERROR);
    }

    /* if storage file is overflowed */
    if (pMldCurTrnScrol->uchStatus & MLD_STORAGE_OVERFLOW) {
        return (MLD_ERROR);
    }

    /* if displayed data is not reflected to the storage file */
    if (pMldCurTrnScrol->uchStatus2 & MLD_UNMATCH_DISPLAY) {
        return (MLD_ERROR);
    }

	// If the pointer for the data area for the sales data is NULL then we will
	// use our own internal data area and throw away the result.
	// This may be used to obtain Item Discount data for the given cursor location.
	if (pData1 == 0) {
		pData1 = &Data1Temp;
	}

    /* read cursor position item */
    VosGetCurPos(pMldCurTrnScrol->usWinHandle, &usCurRow, &usCurCol);
    uchReadItem = pMldCurTrnScrol->uchItemLine[usCurRow];
    uchReadItemSave = uchReadItem;

    if (uchReadItem == MLD_SPECIAL_ITEM) {      /* if cursor directs item discount */
        /* set paraent plu position */
        i= usCurRow;
        for (i = i - 1; i >= 0; i--) {
            uchReadItem = pMldCurTrnScrol->uchItemLine[i];
            if (uchReadItem != MLD_SPECIAL_ITEM) break;
        }
        if ((uchReadItem == MLD_SPECIAL_ITEM) || (uchReadItem == 0)) {
            for (i= usCurRow + 1; i <= uchMldBottomCursor; i++) {
                uchReadItem = pMldCurTrnScrol->uchItemLine[i];
                if (uchReadItem != MLD_SPECIAL_ITEM) break;
            }
        }
        if (uchReadItem == MLD_SPECIAL_ITEM) {
            return(MLD_ERROR);
        }
    }

    /* if no data is displayed */
    if (uchReadItem == 0) {
        return (MLD_ERROR);
    }

	//end condiment addition
	memset (achBuff, 0, sizeof(achBuff));
	MldGetItemSalesDiscData (usScroll, pData1, (ITEMDISC *)achBuff, uchReadItem);
	if (uchReadItemSave == MLD_SPECIAL_ITEM) {  /* item discount is directed */
        memcpy(pData1, achBuff, sizeof(ITEMDISC));
	}

	if (pData2 && usGetType == MLD_GET_SALES_DISC) {
        *(ITEMDISC *)pData2 = *(ITEMDISC *)achBuff;
	}
	else if (pData2 && usGetType == MLD_GET_COND_NUM)
	{
		// JHHJ Condiment Addition
		// We are now going to determine which condiment this is
		// within the provided item.  As we are processing this
		// we must take into account that the display may not contain
		// all of the item because part of the displayed item information
		// may have scrolled up or down.  Therefore, we will use a combination
		// of the scroll buffer information and the ITEMSALES information
		// to determine which condiment is selected.

		condNum = 0;
		if (pMldCurTrnScrol->uchItemLine[0] == 0 || 
			(pMldCurTrnScrol->uchLineStatus[usCurRow] & (MLD_LINESTATUS_FIRSTLINE)) == (MLD_LINESTATUS_FIRSTLINE))
		{
			// First of all, if this is the first line of an item or a continuation of the first line
			// of an item then we are pointing at the item itself.
			;
		} else if (pMldCurTrnScrol->uchItemLine[0] != 0) {
			USHORT  iS1, iS2, condCountDisplayed;

			// Beginning with the first line of the display, go down the display until we find
			// the first line of the item pointed to by the cursor.
			for(iS1 = 0 ; iS1 < pMldCurTrnScrol->usMldBottomCursor && pMldCurTrnScrol->uchItemLine[iS1] != uchReadItem; iS1++)
			{
			}
			while (iS1 < pMldCurTrnScrol->usMldBottomCursor && 
				(pMldCurTrnScrol->uchLineStatus[iS1+1] & MLD_LINESTATUS_CONTLINE) != 0)
			{
				iS1++;
			}

			// Next we find the last line of the item being displayed.
			for(iS2 = iS1; iS2 < pMldCurTrnScrol->usMldBottomCursor && pMldCurTrnScrol->uchItemLine[iS2] == uchReadItem; iS2++)
			{
			}

			// At this point iS1 is index to the first line for this item that is displayed.
			// and iS2 is index to the last line for this item that is displayed.  Remember
			// that this is what is on the display so now we need to determine the actual condiment
			// by determining what part of the condiment list is being displayed and
			// where in the list is the cursor located.

			if (iS1 < pMldCurTrnScrol->usMldBottomCursor && iS2 > iS1) {
				// Determine number of condiments displayed.  We subtract one because
				// the first line contains the actual item rather than a condiment but it
				// also has the item number.
				condCountDisplayed = iS2 - iS1 - 1;
				if (pMldCurTrnScrol->uchItemLine[0] != uchReadItem) {
					// Since there is nothing of this item on first line of display
					// its simple subtraction to determine the condiment displayed.
					condNum = usCurRow - iS1 + ((ITEMSALES *)pData1)->uchChildNo;
				}
				else {
					// Zero or more condiments have been scrolled off the display.
					// They may have been scrolled up off the display or scrolled
					// down off the display.
					// It may be zero because the first line may contain the first
					// condiment and the line scrolled off was the item itself.
//					NHPOS_ASSERT(condCountDisplayed <= (((ITEMSALES *)pData1)->uchCondNo + ((ITEMSALES *)pData1)->uchChildNo));
					if (condCountDisplayed > (((ITEMSALES *)pData1)->uchCondNo))
						condCountDisplayed = (((ITEMSALES *)pData1)->uchCondNo);

					condNum = (usCurRow - iS1) + ((((ITEMSALES *)pData1)->uchCondNo + ((ITEMSALES *)pData1)->uchChildNo) - condCountDisplayed);
				}
				pMldCurTrnScrol->usCurPressed = iS1;
			}
		}
		// guard agains the arithemtic resulting in a negative number.
		// if too large then it is bogus calculation so just set to zero.
		if (condNum > 0x00ff)
			condNum = 0;
		*((USHORT *)pData2) = condNum;
	}

    if ( pData1 && ITEMSTORAGENONPTR(pData1)->uchMajorClass == CLASS_ITEMSALES ) {
        /* mask previous item data which is not saved to storage item */
        //((ITEMSALES *)pData1)->ControlCode.uchItemType = 0;
        /* ((ITEMSALES *)pData1)->ControlCode.uchDeptNo = 0; */
        //((ITEMSALES *)pData1)->ControlCode.uchReportCode = 0;
        ((ITEMSALES *)pData1)->fbStorageStatus &= NOT_ITEM_CONS;
    }

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : USHORT   MldGetCursorDisplay(USHORT usScroll, VOID *pData1, VOID *pData2);
*               
*    Input : USHORT           usScroll     -scroll display
*
*   Output : VOID             *pData1      -data structure of cursor position
*                             *pData2      -data structure of cursor position
*
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function is called from application user and sets data
*            structure of cursor position
*===========================================================================
*/
SHORT   MldGetCursorDisplay(USHORT usScroll, VOID *pData1, VOID *pData2, USHORT usType)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    sStatus = MldLocalGetCursorDisplay(usScroll, pData1, pData2, usType);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}



/***** End Of Source *****/


