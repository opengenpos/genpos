/*
*===========================================================================
* Title       : Multiline Display Item module I/F
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDWRITE.C                                                      
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-15-95 : 03.00.00 : M.Ozawa    : Initial
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Dec-14-15 : 02.02.01 : R.Chambers : allow Cursor Void with Tray Total, fix display issue.
* Sep-19-17 : 02.02.02 : R.Chambers : change use of *(( UCHAR*)pItem) to use ITEMSTORAGENONPTR(pItem)->uchMajorClass
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
#include <csttl.h>
#include <rfl.h>
#include <vos.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"
#include    <prtprty.h>

/*
	This functions handle writing to the physical display and managing
	the physical display scroll buffers.  The format for the physical
	display is as a window into the logical transaction information.  This
	window moves up and down in the transaction as the cursor moves within
	the physical display.

	The physical display contains a number of rows, the number of rows in
	turn depending on the display format (Single Receipt Window or multiple
	receipt windows).

	The last row of the physical display is not used for displaying information
	so this means that if a window has 21 rows, only the first 20 rows are used.
	Rows are numbered beginning with zero so in this example, the rows of
	the physical display are numbered 0 through 19 and row 20 is not used.

	The main entry point used for writing to the physical display is the function
	MldWriteScrollDisplay() which in turn calls one of a set of helper functions
	that perform the actual write.
**/

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
/*
*===========================================================================
** Format  : USHORT   MldScrollWrite(VOID *pItem, USHORT usType);
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
SHORT   MldScrollWrite(VOID *pItem, USHORT usType)
{
    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if (uchMldCurScroll != MLD_SCROLL_1) {
        if (MldLocalSetCursor(MLD_SCROLL_1) != MLD_SUCCESS) {
            /* release semaphore */
            PifReleaseSem(husMldSemaphore);
            return (MLD_ERROR);
        }
    }

    if (ITEMSTORAGENONPTR(pItem)->uchMajorClass == CLASS_ITEMTRANSOPEN) {
        /* not display duplicated item(reorder) */
        if (MldScroll1Buff.uchCurItem) {
            if ((uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL) &&
                (uchMldSystem2 != MLD_SPLIT_CHECK_SYSTEM)) {
                /* display unique transaction no. */
                MldWriteGuestNo((ITEMTRANSOPEN *)pItem);
            }

            /* release semaphore */
            PifReleaseSem(husMldSemaphore);
            return (MLD_SUCCESS);
        }
    }

    /* set temporay buffer for cursor down situation */
    MldTempStore(pItem);

    MldLocalScrollWrite(pItem, usType, MLD_SCROLL_1);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : USHORT   MldScrollWrite2(USHORT usScroll, VOID *pItem, USHORT usType);
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
SHORT   MldScrollWrite2(USHORT usScroll, VOID *pItem, USHORT usType)
{
    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if (usScroll & MLD_SCROLL_1) {
        if (uchMldCurScroll != MLD_SCROLL_1) {
            MldLocalSetCursor(MLD_SCROLL_1);
        }

        if (ITEMSTORAGENONPTR(pItem)->uchMajorClass == CLASS_ITEMTRANSOPEN) {
            /* not display duplicated item(reorder) */
            if (MldScroll1Buff.uchCurItem == 0) {
                /* set temporay buffer for cursor down situation */
                MldTempStore(pItem);
                MldLocalScrollWrite(pItem, usType, MLD_SCROLL_1);

            } else {
                if ((uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL) &&
                    (uchMldSystem2 != MLD_SPLIT_CHECK_SYSTEM)) {
                    /* display unique transaction no. */
                    MldWriteGuestNo((ITEMTRANSOPEN *)pItem);
                }
            }
        } else {
            /* set temporay buffer for cursor down situation */
            MldTempStore(pItem);
            MldLocalScrollWrite(pItem, usType, MLD_SCROLL_1);
        }
    }

    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        if (usScroll & MLD_SCROLL_2) {
            if (ITEMSTORAGENONPTR(pItem)->uchMajorClass == CLASS_ITEMTRANSOPEN) {
                /* not display duplicated item(reorder) */
                if (MldScroll2Buff.uchCurItem == 0) {
                    MldLocalScrollWrite(pItem, MLD_ONLY_DISPLAY, MLD_SCROLL_2);
                    MldScroll2Buff.uchStatus2 |= MLD_UNMATCH_DISPLAY;
                }
            } else {
                MldLocalScrollWrite(pItem, MLD_ONLY_DISPLAY, MLD_SCROLL_2);

                MldScroll2Buff.uchStatus2 |= MLD_UNMATCH_DISPLAY;

                if (ITEMSTORAGENONPTR(pItem)->uchMajorClass == CLASS_ITEMTENDER) {
                    if ( ((ITEMTENDER *)pItem)->lBalanceDue ) {
                        MldScroll2Buff.uchStatus |= MLD_WAIT_STATUS;
                    } else {
                        MldScroll2Buff.uchStatus &= ~MLD_WAIT_STATUS;
                    }
                }
            }
        }

        if (usScroll & MLD_SCROLL_3) {
            if (ITEMSTORAGENONPTR(pItem)->uchMajorClass == CLASS_ITEMTRANSOPEN) {
                /* not display duplicated item(reorder) */
                if (MldScroll3Buff.uchCurItem == 0) {
                    MldLocalScrollWrite(pItem, MLD_ONLY_DISPLAY, MLD_SCROLL_3);
                    MldScroll3Buff.uchStatus2 |= MLD_UNMATCH_DISPLAY;
                }
            } else {
                MldLocalScrollWrite(pItem, MLD_ONLY_DISPLAY, MLD_SCROLL_3);
                MldScroll3Buff.uchStatus2 |= MLD_UNMATCH_DISPLAY;

                if (ITEMSTORAGENONPTR(pItem)->uchMajorClass == CLASS_ITEMTENDER) {
                    if ( ((ITEMTENDER *)pItem)->lBalanceDue ) {
                        MldScroll3Buff.uchStatus |= MLD_WAIT_STATUS;
                    } else {
                        MldScroll3Buff.uchStatus &= ~MLD_WAIT_STATUS;
                    }
                }
            }
        }
    }

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : USHORT   MldECScrollWrite(VOID);
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function is called from application user and clear item
*===========================================================================
*/
SHORT   MldECScrollWrite(VOID)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if (uchMldCurScroll != MLD_SCROLL_1) {
        if (MldLocalSetCursor(MLD_SCROLL_1) != MLD_SUCCESS) {
            /* release semaphore */
            PifReleaseSem(husMldSemaphore);
            return (MLD_ERROR);
        }
    }

    sStatus = MldLocalECScrollWrite(MLD_SCROLL_1); /* ### MOD (2171 for Win32 V1.0) */

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}
/*
*===========================================================================
** Format  : USHORT   MldECScrollWrite2(USHORT usScroll);
*               
*    Input : USHORT usScroll
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function is called from application user and clear item
*===========================================================================
*/
SHORT   MldECScrollWrite2(USHORT usScroll)
{
    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if (usScroll & MLD_SCROLL_1) {
        if (uchMldCurScroll != MLD_SCROLL_1) {
            MldLocalSetCursor(MLD_SCROLL_1);
        }
        MldLocalECScrollWrite(MLD_SCROLL_1);
    }

    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {

        if (usScroll & MLD_SCROLL_2) {
            MldLocalECScrollWrite(MLD_SCROLL_2);
        }

        if (usScroll & MLD_SCROLL_3) {
            MldLocalECScrollWrite(MLD_SCROLL_3);
        }
    }

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (MLD_SUCCESS);

}
/*
*===========================================================================
** Format  : USHORT   MldECScrollWrite(USHORT usScroll);
*               
*    Input : USHORT usScroll
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function is called from application user and clear item
*===========================================================================
*/
SHORT   MldLocalECScrollWrite(USHORT usScrollIn)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
    TCHAR  aszColumn[MLD_PRECOLUMN1 + 1];
    UCHAR  uchAttr, uchPrevLine, uchReadItem;
    USHORT usLine, usRow, usLength;
	SHORT  i;
	USHORT  usScroll = (usScrollIn & MLD_SCROLL_MASK);

    /* check current active scroll */
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

    if (usScroll == MLD_SCROLL_1) {
        if ((MldScroll1Buff.uchStatus & MLD_CURITEM_TMP) == 0) {
            /* not e/c if current itemize is stored in storage file */
            return (MLD_ERROR);
        }
    }

    pMldCurTrnScrol->uchStatus2 &= ~MLD_MOVE_CURSOR;

    /* check if e/c item exist on the scroll display or not */

    if (pMldCurTrnScrol->uchCurCursor == 0) {
        if (usScroll == MLD_SCROLL_1) {
            /* reset data exists in previous buffer status */
            MldScroll1Buff.uchCurItem--;    /* reset current itemization */
            if (MldScroll1Buff.uchStatus & MLD_ITEMDISC_STORE) {
                MldResetItemDiscTmpStore();
                MldScroll1Buff.uchStatus &= ~MLD_ITEMDISC_STORE;
            } else {
                MldScroll1Buff.uchStatus &= ~MLD_CURITEM_TMP;
            }
        }
        return (MLD_SUCCESS);
    }

    if (pMldCurTrnScrol->uchPreCursor == MLD_OUT_OF_SCROLL_AREA) {
        /* reset data exists in previous buffer status */
        if (usScroll == MLD_SCROLL_1) {
            MldScroll1Buff.uchCurItem--;    /* reset current itemization */
            if (MldScroll1Buff.uchStatus & MLD_ITEMDISC_STORE) {
                MldResetItemDiscTmpStore();
                MldScroll1Buff.uchStatus &= ~MLD_ITEMDISC_STORE;
            } else {
                MldScroll1Buff.uchStatus &= ~MLD_CURITEM_TMP;
            }
        }

        /* clear e/c data under bottom item */
        if (pMldCurTrnScrol->uchInvisiLowerLines == 0) {
            uchReadItem = pMldCurTrnScrol->uchItemLine[uchMldBottomCursor];
            if (uchReadItem == MLD_SPECIAL_ITEM) {      /* if bottom item is item discount */
                /* set paraent plu position */
                i = uchMldBottomCursor;
                for (i = i - 1; i >= 0; i--) {
                    uchReadItem = pMldCurTrnScrol->uchItemLine[i];
                    if (uchReadItem != MLD_SPECIAL_ITEM)break;
                }

                if ((uchReadItem == MLD_SPECIAL_ITEM) || (uchReadItem == 0)) {
                    uchReadItem = pMldCurTrnScrol->uchCurItem;
                }
            }

            if (uchReadItem == pMldCurTrnScrol->uchCurItem) {
                pMldCurTrnScrol->uchCurCursor = uchMldBottomCursor;
                pMldCurTrnScrol->uchPreCursor = uchMldBottomCursor;
                pMldCurTrnScrol->uchLowerItemDisc = 0;
                VosSetCurPos(pMldCurTrnScrol->usWinHandle, uchMldBottomCursor,0);
                MldClearContinue(usScroll, MLD_LOWER_ARROW);
            }

        }
        return (MLD_SUCCESS);
    }

    if (pMldCurTrnScrol->uchStatus2 & MLD_ZERO_LINE_DISPLAY) {
        /* reset data exists in previous buffer status */
        if (usScroll == MLD_SCROLL_1) {
            MldScroll1Buff.uchCurItem--;    /* reset current itemization */
            if (MldScroll1Buff.uchStatus & MLD_ITEMDISC_STORE) {
                MldResetItemDiscTmpStore();
                MldScroll1Buff.uchStatus &= ~MLD_ITEMDISC_STORE;
            } else {
                MldScroll1Buff.uchStatus &= ~MLD_CURITEM_TMP;
            }
        }
        pMldCurTrnScrol->uchStatus2 &= ~MLD_ZERO_LINE_DISPLAY;
        return (MLD_SUCCESS);
    }

    usRow = (USHORT)pMldCurTrnScrol->uchPreCursor;

    if ((pMldCurTrnScrol->uchStatus & MLD_SCROLL_UP) ||
            (pMldCurTrnScrol->uchCurCursor == MLD_OUT_OF_SCROLL_AREA)) {   /* if scroll up is occured */
        if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
            usLine = MLD_DRV3_DSP1_ROW_LEN;
        } else {
            usLine = MLD_NOR_DSP1_ROW_LEN;
        }
        usLine -= usRow;                /* clear at the end of bottom */
        pMldCurTrnScrol->uchStatus &= ~MLD_SCROLL_UP; /* cancel scroll up situation */
    } else {
        usLine = (USHORT)(pMldCurTrnScrol->uchCurCursor - pMldCurTrnScrol->uchPreCursor);
    }

    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        usLength = MLD_DR3COLUMN;
    } else {
        usLength = MLD_PRECOLUMN1;
    }

    tcharnset(aszColumn, _T(' '), MLD_PRECOLUMN1);

    /* set cursor to previous position */
    VosSetCurType(pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);
    VosSetCurPos(pMldCurTrnScrol->usWinHandle, usRow, 0);

    uchAttr = (pMldCurTrnScrol->usAttrib ? MLD_C_WHITE : MLD_C_BLACK);

    for (i = 0; i < usLine; i++) {
        /* put space e/c area */        
        VosStringAttr(pMldCurTrnScrol->usWinHandle, aszColumn, usLength, uchAttr);
    }

    /* reset cursor position */
    VosSetCurPos(pMldCurTrnScrol->usWinHandle, usRow, 0);
    pMldCurTrnScrol->uchCurCursor = pMldCurTrnScrol->uchPreCursor;

    for (i = pMldCurTrnScrol->uchPreCursor; i < MLD_NOR_DSP1_ROW_LEN; i++) {
        pMldCurTrnScrol->uchItemLine[i] = 0;
    }

    /* reset data exists in previous buffer status */
    if (usScroll & MLD_SCROLL_1) {
        MldScroll1Buff.uchCurItem--;    /* reset current itemization */
        if (MldScroll1Buff.uchStatus & MLD_ITEMDISC_STORE) {
            uchPrevLine = MldResetItemDiscTmpStore();
            uchPrevLine = MldScroll1Buff.uchPreCursor - uchPrevLine;
            if ((CHAR)(uchPrevLine) < 0) {
                MldScroll1Buff.uchPreCursor = 0;
            } else {
                MldScroll1Buff.uchPreCursor = uchPrevLine;
            }
            MldScroll1Buff.uchStatus &= ~MLD_ITEMDISC_STORE;
        } else {
            MldScroll1Buff.uchStatus &= ~MLD_CURITEM_TMP;
        }
    }

    if (pMldCurTrnScrol->uchItemLine[0] == 0) {
        pMldCurTrnScrol->uchInvisiUpperLines = 0;
    }
    pMldCurTrnScrol->uchInvisiLowerLines = 0;
    pMldCurTrnScrol->uchLowerItemDisc = 0;
    MldClearContinue(usScroll, MLD_LOWER_ARROW);

    VosSetCurType(pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);

    return (MLD_SUCCESS);
}

/* ===========================================================================

	This function will scroll the display area up if a scroll up is needed.
**/

#define MldInsertCheckScrollDownNeeded(pMld) ((pMld)->uchItemLine[(pMld)->usMldBottomCursor] != 0)

static SHORT MldScrollDisplayIncrementCursor (USHORT usScrollIn)
{
	MLDCURTRNSCROL *pMldCurTrnScrol;
	SHORT           sRetStatus = 0;
	USHORT          usScroll = (usScrollIn & MLD_SCROLL_MASK);

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
			return (MLD_ERROR);
    }

	if (pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] > 0) {
		pMldCurTrnScrol->uchCurCursor++;

		// If we are at the bottom of the scrolling region then we will need
		// to scroll up the region, set the indicators that show if lines have
		// been scrolled off the display, and indicate in the return code that
		// a scroll operation has been done.
		if (pMldCurTrnScrol->uchCurCursor > pMldCurTrnScrol->usMldBottomCursor) {
			int i;

			pMldCurTrnScrol->uchInvisiUpperLines++;
			pMldCurTrnScrol->uchInvisiUpperDupItem = (pMldCurTrnScrol->uchItemLine[0] == pMldCurTrnScrol->uchItemLine[1]);

			MldDispContinue (usScroll, MLD_UPPER_ARROW);
			VosScrollUp (pMldCurTrnScrol->usWinHandle, 1);
			sRetStatus = 1;
			pMldCurTrnScrol->uchCurCursor = pMldCurTrnScrol->usMldBottomCursor;

			for (i = 0; i < pMldCurTrnScrol->usMldBottomCursor; i++) {
				pMldCurTrnScrol->uchItemLine [i] = pMldCurTrnScrol->uchItemLine [i+1];
				pMldCurTrnScrol->uchLineStatus [i] = pMldCurTrnScrol->uchLineStatus [i+1];
				pMldCurTrnScrol->usUniqueID [i] = pMldCurTrnScrol->usUniqueID [i+1];
			}

			pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->usMldBottomCursor] = 0;
			pMldCurTrnScrol->uchLineStatus[pMldCurTrnScrol->usMldBottomCursor] = 0;
			pMldCurTrnScrol->usUniqueID[pMldCurTrnScrol->usMldBottomCursor] = 0;

			if (pMldCurTrnScrol->uchInvisiLowerLines > 0) {
				pMldCurTrnScrol->uchInvisiLowerLines--;
				if (pMldCurTrnScrol->uchInvisiLowerLines > 0) {
					MldDispContinue (usScroll, MLD_LOWER_ARROW);
				}
				else {
					MldClearContinue (usScroll, MLD_LOWER_ARROW);
				}
			}
		}
	}
	return sRetStatus;
}


static SHORT MldScrollDisplayDecrementCursor (USHORT usScrollIn)
{
	MLDCURTRNSCROL *pMldCurTrnScrol;
	SHORT           sRetStatus = 0;
	USHORT          usScroll = (usScrollIn & MLD_SCROLL_MASK);

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
			return (MLD_ERROR);
    }

	if (pMldCurTrnScrol->uchCurCursor > 0) {
		pMldCurTrnScrol->uchCurCursor--;
		return 0;
	}

	if (pMldCurTrnScrol->uchInvisiUpperLines > 0) {
		int i;
		int iLast = pMldCurTrnScrol->usMldBottomCursor;

        pMldCurTrnScrol->uchInvisiUpperLines--;

		if (pMldCurTrnScrol->uchInvisiUpperLines > 0) {
			MldDispContinue(usScroll, MLD_UPPER_ARROW);
		}
		else {
			MldClearContinue(usScroll, MLD_UPPER_ARROW);
		}

		if (pMldCurTrnScrol->uchItemLine[iLast] != 0) {
			pMldCurTrnScrol->uchInvisiLowerLines++;
			MldDispContinue(usScroll, MLD_LOWER_ARROW);
		}

		VosScrollDown(pMldCurTrnScrol->usWinHandle, 1);
		sRetStatus = 1;

		// Shuffle down the list of item numbers since we are scrolling
		// down the display.  This will leave the item number unknown for
		// the top row.  Use pMldCurTrnScrol->uchDispItem as the value. 
		for (i = pMldCurTrnScrol->usMldBottomCursor; i > 0 ; i--) {
			pMldCurTrnScrol->uchItemLine[i] = pMldCurTrnScrol->uchItemLine[i - 1];
			pMldCurTrnScrol->uchLineStatus[i] = pMldCurTrnScrol->uchLineStatus[i - 1];
			pMldCurTrnScrol->usUniqueID[i] = pMldCurTrnScrol->usUniqueID[i - 1];
		}
		pMldCurTrnScrol->uchItemLine[0] = pMldCurTrnScrol->uchDispItem;
		pMldCurTrnScrol->uchLineStatus[0] = 0;
		pMldCurTrnScrol->usUniqueID[0] = 0;

		pMldCurTrnScrol->uchCurCursor = 0;
	}
	return sRetStatus;
}


static SHORT MldScrollDisplayDecrementRegion (USHORT usScrollIn, UCHAR uchCurItem)
{
	MLDCURTRNSCROL *pMldCurTrnScrol;
	USHORT          usScroll = (usScrollIn & MLD_SCROLL_MASK);

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
			return (MLD_ERROR);
    }

	if (pMldCurTrnScrol->uchInvisiUpperLines > 0) {
		int i;
		int iLast = pMldCurTrnScrol->usMldBottomCursor;

		pMldCurTrnScrol->uchInvisiUpperLines--;
		if (pMldCurTrnScrol->uchInvisiUpperLines == 0) {
			MldClearContinue (usScroll, MLD_UPPER_ARROW);
		}

		if (pMldCurTrnScrol->uchItemLine [iLast] != 0) {
			pMldCurTrnScrol->uchInvisiLowerLines++;
			MldDispContinue (usScroll, MLD_LOWER_ARROW);
		}

		// Shuffle the item numbers down since the display has also
		// scrolled down.
		for (i = iLast; i > 1 ; i--) {
			pMldCurTrnScrol->uchItemLine [i] = pMldCurTrnScrol->uchItemLine [i-1];
			pMldCurTrnScrol->uchLineStatus[i] = pMldCurTrnScrol->uchLineStatus[i - 1];
			pMldCurTrnScrol->usUniqueID[i] = pMldCurTrnScrol->usUniqueID[i - 1];
		}
		pMldCurTrnScrol->uchItemLine [0] = uchCurItem;
		pMldCurTrnScrol->uchLineStatus[0] = 0;
		pMldCurTrnScrol->usUniqueID[0] = 0;
	}
	return 0;
}

/*
	This function is designed to handle the case involving adding a new
	condiment to an item when the item may have zero or more items after
	it.  In other words, we are going to insert a new condiment into
	the list of condiments displayed.  We do this by adding another row
	to the number of rows occupied by this item and its condiments.

	It is assumed that the cursor, pMldCurTrnScrol->uchCurCursor is
	positioned on the item and we must now insert a new row by moving
	everything that is after the current row down one row.
**/
static SHORT MldScrollDisplayUpdateInsertAtCursor (USHORT usScrollIn)
{
	MLDCURTRNSCROL *pMldCurTrnScrol;
	SHORT           sRetStatus = 0;
	USHORT          usScroll = (usScrollIn & MLD_SCROLL_MASK);

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
			return (MLD_ERROR);
    }

	if (pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] == 0) {
		// If there is no item here then we do not need to do anything
		// since this row is free for use.
	}
	else {
		int i;
		int iLast = pMldCurTrnScrol->usMldBottomCursor;

		if (pMldCurTrnScrol->uchItemLine [iLast] != 0) {
			pMldCurTrnScrol->uchInvisiLowerLines++;
			MldDispContinue (usScroll, MLD_LOWER_ARROW);
		}

		VosScrollDownAtRow (pMldCurTrnScrol->usWinHandle, 1, pMldCurTrnScrol->uchCurCursor);
		sRetStatus = 1;

		// Shuffle the item numbers down since the display has also
		// scrolled down.
		for (i = iLast; i > pMldCurTrnScrol->uchCurCursor; i--) {
			pMldCurTrnScrol->uchItemLine [i] = pMldCurTrnScrol->uchItemLine [i-1];
			pMldCurTrnScrol->uchLineStatus [i] = pMldCurTrnScrol->uchLineStatus [i-1];
			pMldCurTrnScrol->usUniqueID [i] = pMldCurTrnScrol->usUniqueID [i-1];
		}
		pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] = 0;
		pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] = 0;
		pMldCurTrnScrol->usUniqueID [pMldCurTrnScrol->uchCurCursor] = 0;
	}
	return sRetStatus;
}

static SHORT MldScrollDisplayUpdateDeleteAtCursor (USHORT usScrollIn)
{
	MLDCURTRNSCROL *pMldCurTrnScrol;
	SHORT           sRetStatus = 0;
	USHORT          usScroll = (usScrollIn & MLD_SCROLL_MASK);

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
			return (MLD_ERROR);
    }

	if (pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] == 0) {
		// If there is no item here then we do not need to do anything
		// since this row is free for use.
	}
	else {
		int i;
		int iLast = pMldCurTrnScrol->usMldBottomCursor;

		VosScrollUpAtRow (pMldCurTrnScrol->usWinHandle, 1, pMldCurTrnScrol->uchCurCursor);
		sRetStatus = 1;

		// Shuffle the item numbers up since the display has also
		// scrolled down.
		for (i = pMldCurTrnScrol->uchCurCursor; i < iLast; i++) {
			pMldCurTrnScrol->uchItemLine [i] = pMldCurTrnScrol->uchItemLine [i+1];
			pMldCurTrnScrol->uchLineStatus [i] = pMldCurTrnScrol->uchLineStatus [i+1];
			pMldCurTrnScrol->usUniqueID [i] = pMldCurTrnScrol->usUniqueID [i+1];
		}
		pMldCurTrnScrol->uchItemLine [iLast] = 0;
		pMldCurTrnScrol->uchLineStatus [iLast] = 0;
		pMldCurTrnScrol->usUniqueID [iLast] = 0;
	}
	return sRetStatus;
}



/*
*===========================================================================
** Format  : VOID  MldWriteScrollDisplay(UCHAR *pszString, USHORT usScroll,
*                   USHORT usType, USHORT usRow, USHORT usLength);
*
*   Input  : UCHAR   *pszString     -String Data
*            USHORT  usScroll       -Scroll Type(MLD_SCROLL_1
*                                                 MLD_SCROLL_2
*                                                 MLD_SCROLL_3)
*            USHORT  usType         -First Display Position
*            USHORT  usScrollLine   -No. of lines
*            USHORT  usLength       -Length of 1 line.
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays a text string on the first window or the
*            Single Receipt window.  This function is called when a new item
*            is being created and added to the window area.
*===========================================================================
*/
static SHORT MldWriteScrollDisplay1(TCHAR *pszString, USHORT usScroll, USHORT usScrollLine, USHORT usLength,
                                LONG *plAttrib, USHORT usType)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
	USHORT  usType2 = MLD_TYPE_GET_FLAG(usType);
	USHORT  usLastLineItem;
	USHORT  usScrollFirstLine = 0;
	UCHAR   uchCurItem, uchLineStatus = 0;
    USHORT  i;

    switch (usScroll & MLD_SCROLL_MASK) {
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
			return (MLD_ERROR);
	}

    pMldCurTrnScrol->uchStatus2 &= ~MLD_MOVE_CURSOR;

	// Handle the case where the first line of the displayed item which
	// we would expect to contain the item and the price is instead on
	// more than one line because the first line of the item contains
	// other information such as quantity, seat number, mnemonic, and price
	// so that the first line is actually on several lines.  If it is
	// on several lines then the count of those lines is in
	// the top half of the USHORT usScrollLine.
	usScrollFirstLine = MLD_SETLINE_COUNT(usScrollLine);
	usScrollLine &=0xff;

    if (usScrollLine == 0) {
		/*
		 * if no display data exists so nothing to display.
		 * however if item was added to the MLD transaction file then
		 * we need to accomodate this not displayed or invisible item
		 * so that Cursor Void and other cursor select actions will
		 * select the correct item from the MLD transaction data file.
		**/
		if (usScroll & MLD_SCROLL_ADDED_FILE)
			pMldCurTrnScrol->uchCurItem++;    /* update current itemization */
//        pMldCurTrnScrol->uchCurCursor = 0;
//        pMldCurTrnScrol->uchPreCursor = 0;
//        pMldCurTrnScrol->uchInvisiUpperLines = 0;
//        pMldCurTrnScrol->uchInvisiLowerLines = 0;
        return (MLD_NOT_DISPLAY);
    }

    VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);

	// We will make sure that we are going to add this new item onto an empty
	// line of the display area.  So starting with where our cursor is located
	// we move down through the buffer, which echos the displayed information,
	// until we find an empty line.
	pMldCurTrnScrol->uchCurCursor = 0;
	usLastLineItem = pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor];
	while (usLastLineItem != 0) {
		if (pMldCurTrnScrol->uchCurCursor >= pMldCurTrnScrol->usMldBottomCursor) {
			pMldCurTrnScrol->uchCurCursor = pMldCurTrnScrol->usMldBottomCursor - 1;
			usLastLineItem = pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor];
			break;
		}
		pMldCurTrnScrol->uchCurCursor++;
		usLastLineItem = pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor];
	}

	// Okay, now we need to determine if we are also displaying the last of the
	// transaction since we want to add this new item to the end.  If the last
	// non-empty line found is equal to the number of items then the last item
	// is showing so we can just continue.
	// Otherwise, we will need to display the rest of the transaction to get to
	// the bottom of the transaction in order to add this new item.
	// WARNING:  MldDispStoredDataRegion() updates the global variables so
	//           the sequence this is done is important.  We also need to turn
	//           the cursor back to invisible as MldDispStoredDataRegion() will
	//           make it visible.  Failure to do so will cause scrolling issues.
	if (usLastLineItem != 0) {
		if (usLastLineItem < pMldCurTrnScrol->uchCurItem && pMldCurTrnScrol->uchCurCursor > 0) {
			MldDispStoredDataRegion(usScroll, usLastLineItem);
			VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);
		}
	}

	// This is a new item so we will add the item to the display area.
	// Since it is a new item, we will increment our count of items.
	// This is supposed to be synchronized with IdxFileInfo.uchNoOfItem
	// from the header of the MLD file and will be once this item is
	// stored into the MLD file.
	// We also determine whether to use the current item number to identify
	// the item or if it is a special item.
	// Finally we remember the cursor position (display row) we are putting this
	// item on so that we can use this information if adding condiments later.
    pMldCurTrnScrol->uchCurItem++;
    uchCurItem = pMldCurTrnScrol->uchCurItem;
	if (usType2 == MLD_NEW_ITEMIZE2) {
		uchLineStatus |= MLD_LINESTATUS_SPECIAL_ITEM;
    }

	pMldCurTrnScrol->uchDispItem = uchCurItem;

	pMldCurTrnScrol->uchPreCursor = pMldCurTrnScrol->uchCurCursor;
    VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);

	if (usScrollLine > 1)
		uchLineStatus |= MLD_LINESTATUS_MULTILINE;

	pMldCurTrnScrol->uchPreCursor = pMldCurTrnScrol->uchCurCursor;
    for (i = 0; i < usScrollLine; i++) {
        if (*plAttrib) {    /* display with reverse video */
            MldStringAttrSpecialColumn (usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength, plAttrib);
        } else {
            MldString(usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength);
        }
        pszString += (usLength + 1);
		plAttrib += (usLength + 1);

		pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] = uchCurItem;
		pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] = uchLineStatus;
		if (i == 0) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_FIRSTLINE;
		}
		if (usScrollFirstLine) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_CONTLINE;
			usScrollFirstLine--;
		}
		if (MldScrollDisplayIncrementCursor (usScroll)) {
			// If we have scrolled the display region then the first line for this item
			// has also scrolled up so we need to adjust pMldCurTrnScrol->uchPreCursor
			// accordingly.
			if (pMldCurTrnScrol->uchPreCursor > 0)
				pMldCurTrnScrol->uchPreCursor--;
		}
    }

    VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchPreCursor, 0);
    VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : VOID  MldWriteScrollDisplay(UCHAR *pszString, USHORT usScroll,
*                   USHORT usType, USHORT usRow, USHORT usLength);
*
*   Input  : UCHAR   *pszString     -String Data
*            USHORT  usScroll       -Scroll Type(MLD_SCROLL_1
*                                                 MLD_SCROLL_2
*                                                 MLD_SCROLL_3)
*            USHORT  usType         -First Display Position
*            USHORT  usRow          -No. of lines
*            USHORT  usLength       -Length of 1 line.
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays formatted data on the specified scroll display.
*            This function is called when an item's condiments are being updated.
*===========================================================================
*/
static SHORT MldWriteScrollDisplay2(TCHAR *pszString, USHORT usScroll, USHORT usScrollLine, USHORT usLength,
                                LONG *plAttrib, USHORT usType)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
	USHORT  usType2 = MLD_TYPE_GET_FLAG(usType);
	UCHAR   uchCurItem, uchLineStatus = 0;
	USHORT  usScrollFirstLine = 0;
    USHORT   i;

    switch (usScroll & MLD_SCROLL_MASK) {
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
			return (MLD_ERROR);
	}

    pMldCurTrnScrol->uchStatus2 &= ~MLD_MOVE_CURSOR;

	// Handle the case where the first line of the displayed item which
	// we would expect to contain the item and the price is instead on
	// more than one line because the first line of the item contains
	// other information such as quantity, seat number, mnemonic, and price
	// so that the first line is actually on several lines.  If it is
	// on several lines then the count of those lines is in
	// the top half of the USHORT usScrollLine.
	usScrollFirstLine = MLD_SETLINE_COUNT(usScrollLine);
	usScrollLine &=0xff;

    if (usScrollLine == 0) {   /* if no display data exists */
        return (MLD_NOT_DISPLAY);
    }

    /* -- set cursor position & display all data in the buffer -- */
    /* clear display if end of scroll is out of scroll */
    if (pMldCurTrnScrol->uchPreCursor == MLD_OUT_OF_SCROLL_AREA) {
        MldScrollClear (usScroll);
        memset (pMldCurTrnScrol->uchItemLine, 0x00, sizeof(pMldCurTrnScrol->uchItemLine));
        pMldCurTrnScrol->uchCurCursor = 0;
        pMldCurTrnScrol->uchPreCursor = 0;
        pMldCurTrnScrol->uchInvisiUpperLines = 0;
        pMldCurTrnScrol->uchInvisiLowerLines = 0;
        pMldCurTrnScrol->uchStatus &= ~MLD_SCROLL_UP;
        MldDispContinue (usScroll, MLD_UPPER_ARROW);
        MldClearContinue (usScroll, MLD_LOWER_ARROW);
    }

    VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);

	pMldCurTrnScrol->uchCurCursor = pMldCurTrnScrol->uchPreCursor;

    uchCurItem = pMldCurTrnScrol->uchDispItem;
    if (usType2 == MLD_UPDATE_ITEMIZE2) {
		uchLineStatus |= MLD_LINESTATUS_SPECIAL_ITEM;
    }

	if (usScrollLine > 1)
		uchLineStatus |= MLD_LINESTATUS_MULTILINE;

    for (i = 0; i < usScrollLine; i++) {
        if (*plAttrib) {    /* display with reverse video */
            MldStringAttrSpecialColumn (usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength, plAttrib);
        } else {
            MldString (usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength);
        }
        pszString += (usLength + 1);
		plAttrib += (usLength + 1);

		pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] = uchCurItem;
		pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] = uchLineStatus;
		if (i == 0) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_FIRSTLINE;
		}
		if (usScrollFirstLine) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_CONTLINE;
			usScrollFirstLine--;
		}
		if (MldScrollDisplayIncrementCursor (usScroll)) {
			// If we have scrolled the display region then the first line for this item
			// has also scrolled up so we need to adjust pMldCurTrnScrol->uchPreCursor
			// accordingly.
			if (pMldCurTrnScrol->uchPreCursor > 0)
				pMldCurTrnScrol->uchPreCursor--;
		}
    }

    VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchPreCursor, 0);
    VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);

    return (MLD_SUCCESS);
}


/*
*===========================================================================
** Format  : VOID  MldWriteScrollDisplay3(UCHAR *pszString, USHORT usScroll,
*                   USHORT usType, USHORT usRow, USHORT usLength);
*
*   Input  : UCHAR   *pszString     -String Data
*            USHORT  usScroll       -Scroll Type(MLD_SCROLL_1
*                                                 MLD_SCROLL_2
*                                                 MLD_SCROLL_3)
*            USHORT  usType         -First Display Position
*            USHORT  usRow          -No. of lines
*            USHORT  usLength       -Length of 1 line.
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function handles display changes when a cursor up has caused
*            the display to scroll down bringing one or more lines that have
*            been scrolled off of the display at the top back down and making those
*            scrolled lines now visible again.
*            Handles following two message types:
*                MLD_UPPER_CURSOR:
*                MLD_UPPER_CURSOR2:
*===========================================================================
*/
static SHORT MldWriteScrollDisplay3(TCHAR *pszString, USHORT usScroll, USHORT usScrollLine,
                            USHORT usLength, LONG *plAttrib, USHORT usType)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
	USHORT  usType2 = MLD_TYPE_GET_FLAG(usType);
	UCHAR   uchCurItem, uchLineStatus = 0;
    USHORT  uchCursor, usCurRow, usCurCol;
    USHORT  i;
	USHORT  usScrollFirstLine = 0;
	TCHAR   *ptcsBlankLine = _T("                                        ");

	// Handle the case where the first line of the displayed item which
	// we would expect to contain the item and the price is instead on
	// more than one line because the first line of the item contains
	// other information such as quantity, seat number, mnemonic, and price
	// so that the first line is actually on several lines.  If it is
	// on several lines then the count of those lines is in
	// the top half of the USHORT usScrollLine.
	usScrollFirstLine = MLD_SETLINE_COUNT(usScrollLine);
	usScrollLine &=0xff;

    if (usScrollLine == 0) {
		/* nothing to do sine there is no display data  */
        return (MLD_NOT_DISPLAY);
    }

    switch (usScroll & MLD_SCROLL_MASK) {
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
			return (MLD_ERROR);
	}

    /* display first row */
    VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);

    VosGetCurPos (pMldCurTrnScrol->usWinHandle, &usCurRow, &usCurCol);

    uchCurItem = pMldCurTrnScrol->uchDispItem;
    if (usType2 == MLD_UPPER_CURSOR2) {
		uchLineStatus |= MLD_LINESTATUS_SPECIAL_ITEM;
    }

	// The item to be modified may be on the second line of the receipt display
	// (uchCursor == 0) or the item to be modified may be on the first line of the
	// receipt display (uchCursor == 0) so we do this check to determine if we
	// need to roll back one of the decrements to be at the right item.
	for (uchCursor = usCurRow;
	     uchCursor > 0 && pMldCurTrnScrol->uchItemLine[uchCursor] == uchCurItem;
		 uchCursor--) {
	}

	if (pMldCurTrnScrol->uchItemLine[uchCursor] < uchCurItem) {
		uchCursor++;
	}

	for (usCurRow = uchCursor;
	     usCurRow <= pMldCurTrnScrol->usMldBottomCursor && pMldCurTrnScrol->uchItemLine[usCurRow] == uchCurItem;
		 usCurRow++) {
	}

	usCurRow = usCurRow - uchCursor;

    pMldCurTrnScrol->uchPreCursor = 0;
	pMldCurTrnScrol->uchCurCursor = 0;

	// If the number of lines for this item and its condiments is less than
	// the number of lines that we are going to write to the display, then
	// we need to cause the display to scroll down at the line we are going
	// to begin our write so that we do not over write other items and their
	// condiments which may also be displayed.
	if (usCurRow < usScrollLine) {
		for (usCurRow = usScrollLine - usCurRow; usCurRow > 0; usCurRow--) {
			MldScrollDisplayDecrementCursor (usScroll);
		}
	}

	pMldCurTrnScrol->uchCurCursor = 0;
	pMldCurTrnScrol->uchPreCursor = pMldCurTrnScrol->uchCurCursor;

	if (usScrollLine > 1)
		uchLineStatus |= MLD_LINESTATUS_MULTILINE;

    for (i = 0; i < usScrollLine; i++) {
        if (*plAttrib) {    /* display with reverse video */
            MldStringAttrSpecialColumn(usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength, plAttrib);
        } else {
            MldString(usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength);
        }
        pszString += (usLength + 1);
		plAttrib += (usLength + 1);

		pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] = uchCurItem;
		pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] = uchLineStatus;
		if (i == 0) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_FIRSTLINE;
		}
		if (usScrollFirstLine) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_CONTLINE;
			usScrollFirstLine--;
		}
		if (MldScrollDisplayIncrementCursor (usScroll)) {
			// If we have scrolled the display region then the first line for this item
			// has also scrolled up so we need to adjust pMldCurTrnScrol->uchPreCursor
			// accordingly.
			if (pMldCurTrnScrol->uchPreCursor > 0)
				pMldCurTrnScrol->uchPreCursor--;
		}
    }

    MldString(usScroll, pMldCurTrnScrol->usMldBottomCursor, ptcsBlankLine, usLength);

	pMldCurTrnScrol->uchCurCursor = 0;

    VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);

    VosSetCurType(pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : VOID  MldWriteScrollDisplay4(UCHAR *pszString, USHORT usScroll,
*                   USHORT usType, USHORT usRow, USHORT usLength);
*
*   Input  : UCHAR   *pszString     -String Data
*            USHORT  usScroll       -Scroll Type(MLD_SCROLL_1
*                                                 MLD_SCROLL_2
*                                                 MLD_SCROLL_3)
*            USHORT  usType         -First Display Position
*            USHORT  usRow          -No. of lines
*            USHORT  usLength       -Length of 1 line.
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function handles display changes when a cursor down has caused
*            the display to scroll up bringing one or more lines that have
*            been scrolled off of the display at the bottom back up and making those
*            scrolled lines now visible again.
*            Handles following two message types:
*                MLD_LOWER_CURSOR:
*                MLD_LOWER_CURSOR2:
*===========================================================================
*/
static SHORT MldWriteScrollDisplay4(TCHAR *pszString, USHORT usScroll, USHORT usScrollLine,
                            USHORT usLength, LONG *plAttrib, USHORT usType)
{
	USHORT  usType2 = MLD_TYPE_GET_FLAG(usType);
	UCHAR   uchCurItem, uchLineStatus = 0;
    USHORT         usCurRow, usCurCol;
    USHORT         i;
    MLDCURTRNSCROL *pMldCurTrnScrol;
	USHORT  usScrollFirstLine = 0;

	// Handle the case where the first line of the displayed item which
	// we would expect to contain the item and the price is instead on
	// more than one line because the first line of the item contains
	// other information such as quantity, seat number, mnemonic, and price
	// so that the first line is actually on several lines.  If it is
	// on several lines then the count of those lines is in
	// the top half of the USHORT usScrollLine.
	usScrollFirstLine = MLD_SETLINE_COUNT(usScrollLine);
	usScrollLine &=0xff;

    if (usScrollLine == 0) {   /* if no display data exists */
        return (MLD_NOT_DISPLAY);
    }

    switch (usScroll & MLD_SCROLL_MASK) {
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
			return (MLD_ERROR);
    }

    VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);

    VosGetCurPos (pMldCurTrnScrol->usWinHandle, &usCurRow, &usCurCol);

    uchCurItem = pMldCurTrnScrol->uchDispItem;
    if (usType2 == MLD_LOWER_CURSOR2) {
		uchLineStatus |= MLD_LINESTATUS_SPECIAL_ITEM;
    }
	pMldCurTrnScrol->uchCurCursor = usCurRow;

	// If the cursor is on an empty line then we move the cursor up the screen
	// by finding the first non-empty line and then positioning the cursor there
	if (pMldCurTrnScrol->uchItemLine[pMldCurTrnScrol->uchCurCursor] == 0) {
		while (pMldCurTrnScrol->uchCurCursor > 0 && pMldCurTrnScrol->uchItemLine[pMldCurTrnScrol->uchCurCursor] == 0) {
			pMldCurTrnScrol->uchCurCursor--;
		}
		VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);
	}

	// Now lets move the cursor back to the first physical display line that this item
	// is currently displayed.  This will position the cursor so that when we write the
	// item information, we will be overwriting any item information that is already
	// displayed on the screen.
	if (pMldCurTrnScrol->uchItemLine[pMldCurTrnScrol->uchCurCursor] == uchCurItem) {
		while (pMldCurTrnScrol->uchCurCursor > 0 && pMldCurTrnScrol->uchItemLine[pMldCurTrnScrol->uchCurCursor] == uchCurItem) {
			pMldCurTrnScrol->uchCurCursor--;
		}
		pMldCurTrnScrol->uchCurCursor++;
		VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);
	}
	else if (pMldCurTrnScrol->uchItemLine[pMldCurTrnScrol->uchCurCursor] == 0) {
		// nothing to do as the line is empty so we will just use it.
	}
	else {
		MldScrollDisplayIncrementCursor (usScroll);
	}

	pMldCurTrnScrol->uchPreCursor = pMldCurTrnScrol->uchCurCursor;

	if (usScrollLine > 1)
		uchLineStatus |= MLD_LINESTATUS_MULTILINE;

    for (i = 0; i < usScrollLine; i++) {
        if (*plAttrib) {    /* display with reverse video */
            MldStringAttrSpecialColumn(usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength, plAttrib);
        } else {
            MldString(usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength);
        }
        pszString += (usLength + 1);
		plAttrib += (usLength + 1);

		pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] = uchCurItem;
		pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] = uchLineStatus;
		if (i == 0) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_FIRSTLINE;
		}
		if (usScrollFirstLine) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_CONTLINE;
			usScrollFirstLine--;
		}
		if (MldScrollDisplayIncrementCursor (usScroll)) {
			// If we have scrolled the display region then the first line for this item
			// has also scrolled up so we need to adjust pMldCurTrnScrol->uchPreCursor
			// accordingly.
			if (pMldCurTrnScrol->uchPreCursor > 0)
				pMldCurTrnScrol->uchPreCursor--;
		}
    }

    /* display cursor */
    VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchPreCursor, 0);
    VosSetCurType(pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);
        
    return (MLD_SUCCESS);
}

/*===========================================================================
** Format  : VOID  MldWriteScrollDisplay5(UCHAR *pszString, USHORT usScroll,
*                   USHORT usType, USHORT usRow, USHORT usLength);
*
*   Input  : UCHAR   *pszString     -String Data
*            USHORT  usScroll       -Scroll Type(MLD_SCROLL_1
*                                                 MLD_SCROLL_2
*                                                 MLD_SCROLL_3)
*            USHORT  usType         -First Display Position
*            USHORT  usRow          -No. of lines
*            USHORT  usLength       -Length of 1 line.
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays formatted data on the specified scroll display
*            This function handles the following message types:
*                MLD_TRANS_DISPLAY:
*                MLD_TRANS_DISPLAY2:
*===========================================================================
*/
static SHORT MldWriteScrollDisplay5(TCHAR *pszString, USHORT usScroll, USHORT usScrollLine,
                            USHORT usLength, LONG *plAttrib, USHORT usType)
{
	USHORT  usType2 = MLD_TYPE_GET_FLAG(usType);
    USHORT  i;
    MLDCURTRNSCROL *pMldCurTrnScrol;
	UCHAR           uchLineStatus = 0;
	USHORT  usScrollFirstLine = 0;

	// Handle the case where the first line of the displayed item which
	// we would expect to contain the item and the price is instead on
	// more than one line because the first line of the item contains
	// other information such as quantity, seat number, mnemonic, and price
	// so that the first line is actually on several lines.  If it is
	// on several lines then the count of those lines is in
	// the top half of the USHORT usScrollLine.
	usScrollFirstLine = MLD_SETLINE_COUNT(usScrollLine);
	usScrollLine &=0xff;

    if (usScrollLine == 0) {   /* if no display data exists */
        return (MLD_NOT_DISPLAY);
    }

    switch (usScroll & MLD_SCROLL_MASK) {
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
        return (MLD_ERROR);
    }

    if (pMldCurTrnScrol->uchStatus & MLD_SCROLL_UP) {
        pMldCurTrnScrol->uchPreCursor = MLD_OUT_OF_SCROLL_AREA;
        pMldCurTrnScrol->uchCurCursor = MLD_OUT_OF_SCROLL_AREA;
        if (usType2 == MLD_TRANS_DISPLAY2) {
            pMldCurTrnScrol->uchLowerItemDisc = MLD_SPECIAL_ITEM;
        }
        return (MLD_STOP_SCROLL);
    }

	if (usScrollLine > 1)
		uchLineStatus |= MLD_LINESTATUS_MULTILINE;

    /* --- update current itemize position for scroll control --- */
    for (i = 0; i < usScrollLine; i++) {
        if (*plAttrib) {    /* display with reverse video */
            MldStringAttrSpecialColumn (usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength, plAttrib);
        } else {
            MldString (usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength);
        }
        pszString += (usLength + 1);
        plAttrib += (usLength + 1);

        /* special item counter for item discount */
        if (usType2 == MLD_TRANS_DISPLAY2) {
            pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] = MLD_SPECIAL_ITEM;
        } else {
            pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] = pMldCurTrnScrol->uchDispItem;
        }
		pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] = uchLineStatus;
		if (i == 0) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_FIRSTLINE;
		}
		if (usScrollFirstLine) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_CONTLINE;
			usScrollFirstLine--;
		}
		MldScrollDisplayIncrementCursor (usScroll);
    }

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : VOID  MldWriteScrollDisplay6(UCHAR *pszString, USHORT usScroll,
*                   USHORT usType, USHORT usRow, USHORT usLength);
*
*   Input  : UCHAR   *pszString     -String Data
*            USHORT  usScroll       -Scroll Type(MLD_SCROLL_1
*                                                 MLD_SCROLL_2
*                                                 MLD_SCROLL_3)
*            USHORT  usType         -First Display Position
*            USHORT  usRow          -No. of lines
*            USHORT  usLength       -Length of 1 line.
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays formatted data on the specified scroll display
*            This function handles the following message types:
*                MLD_REVERSE_DISPLAY:
*                MLD_REVERSE_DISPLAY2:
*===========================================================================
*/
static SHORT MldWriteScrollDisplay6(TCHAR *pszString, USHORT usScroll, USHORT usScrollLine,
                            USHORT usLength, LONG *plAttrib, USHORT usType)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
	USHORT  usType2 = MLD_TYPE_GET_FLAG(usType);
    UCHAR  uchUpperItem;
    USHORT  i, usDownRow;
	UCHAR   uchLineStatus = 0;
	USHORT  usScrollFirstLine = 0;

	// Handle the case where the first line of the displayed item which
	// we would expect to contain the item and the price is instead on
	// more than one line because the first line of the item contains
	// other information such as quantity, seat number, mnemonic, and price
	// so that the first line is actually on several lines.  If it is
	// on several lines then the count of those lines is in
	// the top half of the USHORT usScrollLine.
	usScrollFirstLine = MLD_SETLINE_COUNT(usScrollLine);
	usScrollLine &=0xff;

    if (usScrollLine == 0) {   /* if no display data exists */
        return (MLD_NOT_DISPLAY);
    }

    switch (usScroll & MLD_SCROLL_MASK) {
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
			return (MLD_ERROR);
    }

    /* stop all itemize display, if last item reached to bottom */
    if (pMldCurTrnScrol->uchItemLine [0]) {
        if (pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->usMldBottomCursor]) {
            return(MLD_STOP_SCROLL);
        }

        if ((usScrollLine + pMldCurTrnScrol->uchCurCursor) > pMldCurTrnScrol->usMldBottomCursor) {
            return(MLD_STOP_SCROLL);
        }
    }

    /* update current itemize position for scroll control */
    uchUpperItem = pMldCurTrnScrol->uchDispItem;
    if (usType2 == MLD_REVERSE_DISPLAY2) {
		uchLineStatus |= MLD_LINESTATUS_SPECIAL_ITEM;
    }

    /* clear display area */
	pMldCurTrnScrol->uchCurCursor = 0;
	VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);
    if (usScrollLine > pMldCurTrnScrol->usMldBottomCursor) {
        usDownRow = pMldCurTrnScrol->usMldBottomCursor;
    } else {
        usDownRow = usScrollLine;
    }

	if (usScrollLine > 1)
		uchLineStatus |= MLD_LINESTATUS_MULTILINE;

    pMldCurTrnScrol->uchCurCursor = 0;
    for (i = 0; i < usScrollLine; i++) {
		MldScrollDisplayUpdateInsertAtCursor (usScroll);
        if (*plAttrib) {    /* display with reverse video */
            MldStringAttrSpecial (usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength, *plAttrib);
        } else {
            MldString (usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength);
        }
        pszString += (usLength + 1);
        plAttrib += (usLength + 1);

		pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] = uchUpperItem;
		pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] = uchLineStatus;
		if (i == 0) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_FIRSTLINE;
		}
		if (usScrollFirstLine) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_CONTLINE;
			usScrollFirstLine--;
		}
		MldScrollDisplayIncrementCursor (usScroll);
    }

    if (usScrollLine >= pMldCurTrnScrol->usMldBottomCursor) {
        /* last item overs scroll line case */
        pMldCurTrnScrol->uchInvisiUpperLines = ((UCHAR)usScrollLine - pMldCurTrnScrol->usMldBottomCursor);
    }

    VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : VOID  MldWriteScrollDisplay(UCHAR *pszString, USHORT usScroll,
*                   USHORT usType, USHORT usRow, USHORT usLength);
*
*   Input  : UCHAR   *pszString     -String Data
*            USHORT  usScroll       -Scroll Type(MLD_SCROLL_1
*                                                 MLD_SCROLL_2
*                                                 MLD_SCROLL_3)
*            USHORT  usType         -First Display Position
*            USHORT  usRow          -No. of lines
*            USHORT  usLength       -Length of 1 line.
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays formatted data on the specified scroll display
*===========================================================================
*/
static SHORT MldWriteScrollDisplay7 (TCHAR *pszString, USHORT usScroll, USHORT usScrollLine,
                                USHORT usLength, LONG *plAttrib, USHORT usType)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
    USHORT  i;
	UCHAR   uchLineStatus = 0;
	USHORT  usScrollFirstLine = 0;

    /* check current active scroll */
    switch (usScroll & MLD_SCROLL_MASK) {
		case MLD_SCROLL_2:
			pMldCurTrnScrol = &MldScroll2Buff;
			break;

		case MLD_SCROLL_3:
			pMldCurTrnScrol = &MldScroll3Buff;
			break;

		default:
			return(MLD_ERROR);
    }

    pMldCurTrnScrol->uchStatus2 &= ~MLD_MOVE_CURSOR;

	// Handle the case where the first line of the displayed item which
	// we would expect to contain the item and the price is instead on
	// more than one line because the first line of the item contains
	// other information such as quantity, seat number, mnemonic, and price
	// so that the first line is actually on several lines.  If it is
	// on several lines then the count of those lines is in
	// the top half of the USHORT usScrollLine.
	usScrollFirstLine = MLD_SETLINE_COUNT(usScrollLine);
	usScrollLine &=0xff;

    if (usScrollLine == 0) {   /* if no display data exists */
        return (MLD_NOT_DISPLAY);
    }

    /* -- set cursor position & display all data in the buffer -- */
    VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);

    /* clear display if end of scroll is out of scroll */
    if (pMldCurTrnScrol->uchCurCursor == MLD_OUT_OF_SCROLL_AREA) {
        MldScrollClear (usScroll);
        memset (pMldCurTrnScrol->uchItemLine, 0x00, MLD_NOR_DSP1_ROW_LEN);
        pMldCurTrnScrol->uchCurCursor = 0;
        pMldCurTrnScrol->uchPreCursor = 0;
        pMldCurTrnScrol->uchInvisiUpperLines = 0;
        pMldCurTrnScrol->uchInvisiLowerLines = 0;
        pMldCurTrnScrol->uchStatus &= ~MLD_SCROLL_UP;
        MldDispContinue (usScroll, MLD_UPPER_ARROW);
        MldClearContinue (usScroll, MLD_LOWER_ARROW);
    }

    /* reset previous cursor if current cursor is displayed */
    if (pMldCurTrnScrol->uchPreCursor == MLD_OUT_OF_SCROLL_AREA) {
        pMldCurTrnScrol->uchPreCursor = 0;
    }

    /* display first row */
    if (pMldCurTrnScrol->uchStatus & MLD_SCROLL_UP) {
        MldDispContinue (usScroll, MLD_UPPER_ARROW);
        VosScrollUp (pMldCurTrnScrol->usWinHandle, 1);
    }

	if (usScrollLine > 1)
		uchLineStatus |= MLD_LINESTATUS_MULTILINE;

    pMldCurTrnScrol->uchCurCursor = 0;
    for (i = 0; i < usScrollLine; i++) {
        if (*plAttrib) {    /* display with reverse video */
            MldStringAttrSpecial (usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength, *plAttrib);
        } else {
            MldString (usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength);
        }
        pszString += (usLength+1);
        plAttrib += (usLength + 1);

		pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] = pMldCurTrnScrol->uchDispItem;
		pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] = uchLineStatus;
		if (i == 0) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_FIRSTLINE;
		}
		if (usScrollFirstLine) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_CONTLINE;
			usScrollFirstLine--;
		}
		MldScrollDisplayIncrementCursor (usScroll);
    }

    VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor,0);
    VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);

    return (MLD_SUCCESS);

    usType = usType;
}

/*
*===========================================================================
** Format  : VOID  MldWriteScrollDisplay(UCHAR *pszString, USHORT usScroll,
*                   USHORT usType, USHORT usRow, USHORT usLength);
*
*   Input  : UCHAR   *pszString     -String Data
*            USHORT  usScroll       -Scroll Type(MLD_SCROLL_1
*                                                 MLD_SCROLL_2
*                                                 MLD_SCROLL_3)
*            USHORT  usType         -First Display Position
*            USHORT  usScrollLine   -No. of lines to repaint
*            USHORT  usLength       -Length of 1 line.
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays formatted data on the specified scroll display
*===========================================================================
*/
static SHORT MldWriteScrollDisplay8 (TCHAR *pszString, USHORT usScroll, USHORT usScrollLine, USHORT usLength,
                                LONG *plAttrib, USHORT usType)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
    USHORT  uchCursor, usCurRow, usCurCol;
	USHORT  usItemNumber;
    SHORT   i;
	UCHAR   uchLineStatus = 0;
	USHORT  usScrollFirstLine = 0;

    switch (usScroll & MLD_SCROLL_MASK) {
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

    pMldCurTrnScrol->uchStatus2 &= ~MLD_MOVE_CURSOR;

	// Handle the case where the first line of the displayed item which
	// we would expect to contain the item and the price is instead on
	// more than one line because the first line of the item contains
	// other information such as quantity, seat number, mnemonic, and price
	// so that the first line is actually on several lines.  If it is
	// on several lines then the count of those lines is in
	// the top half of the USHORT usScrollLine.
	usScrollFirstLine = MLD_SETLINE_COUNT(usScrollLine);
	usScrollLine &=0xff;

    if (usScrollLine == 0) {   /* if no display data exists */
        return (MLD_NOT_DISPLAY);
    }

    /* -- set cursor position & display all data in the buffer -- */
    /* clear display if end of scroll is out of scroll */
    if (pMldCurTrnScrol->uchPreCursor == MLD_OUT_OF_SCROLL_AREA) {
        MldScrollClear (usScroll);
        memset (pMldCurTrnScrol->uchItemLine, 0x00, MLD_NOR_DSP1_ROW_LEN);
        pMldCurTrnScrol->uchCurCursor = 0;
        pMldCurTrnScrol->uchPreCursor = 0;
//        pMldCurTrnScrol->uchInvisiUpperLines = 0;
//        pMldCurTrnScrol->uchInvisiLowerLines = 0;
        pMldCurTrnScrol->uchStatus &= ~MLD_SCROLL_UP;
        MldDispContinue (usScroll, MLD_UPPER_ARROW);
        MldClearContinue (usScroll, MLD_LOWER_ARROW);
    }

    if (pMldCurTrnScrol->uchCurCursor == MLD_OUT_OF_SCROLL_AREA) {
        pMldCurTrnScrol->uchCurCursor = uchMldBottomCursor;
    }

    /* display first row */
    VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);

    VosGetCurPos (pMldCurTrnScrol->usWinHandle, &usCurRow, &usCurCol);
	usItemNumber = pMldCurTrnScrol->uchItemLine[usCurRow];

	// The item to be modified may be on the second line of the receipt display
	// (uchCursor == 0) or the item to be modified may be on the first line of the
	// receipt display (uchCursor == 0) so we do this check to determine if we
	// need to roll back one of the decrements to be at the right item.
	for (uchCursor = usCurRow;
	     uchCursor > 0 && pMldCurTrnScrol->uchItemLine[uchCursor] >= usItemNumber;
		 uchCursor--) {
	}

	if (pMldCurTrnScrol->uchItemLine[uchCursor] != usItemNumber) {
		uchCursor++;
	}

	for (usCurRow = uchCursor;
	     usCurRow <= pMldCurTrnScrol->usMldBottomCursor && pMldCurTrnScrol->uchItemLine[usCurRow] == usItemNumber;
		 usCurRow++) {
	}

	usCurRow = usCurRow - uchCursor;

    pMldCurTrnScrol->uchPreCursor = uchCursor;
	pMldCurTrnScrol->uchCurCursor = uchCursor;

	// If the number of lines for this item and its condiments is less than
	// the number of lines that we are going to write to the display, then
	// we need to cause the display to scroll down at the line we are going
	// to begin our write so that we do not over write other items and their
	// condiments which may also be displayed.
	if (usCurRow < usScrollLine) {
		for (; usCurRow < usScrollLine; usCurRow++) {
			MldScrollDisplayUpdateInsertAtCursor (usScroll);
		}
	} else if (usCurRow > usScrollLine) {
		for (; usCurRow > usScrollLine; usCurRow--) {
			MldScrollDisplayUpdateDeleteAtCursor (usScroll);
		}
	}

	if (usScrollLine > 1)
		uchLineStatus |= MLD_LINESTATUS_MULTILINE;

	pMldCurTrnScrol->uchCurCursor = uchCursor;
    VosSetCurPos(pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);

    for (i = 0; i < usScrollLine; i++) {
		if (*plAttrib) {    /* display with reverse video */
			MldStringAttrSpecialColumn (usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength, plAttrib);
		} else {
			MldString (usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength);
		}
        pszString += (usLength + 1);
		plAttrib += (usLength + 1);

		pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] = usItemNumber;
		pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] = uchLineStatus;
		if (i == 0) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_FIRSTLINE;
		}
		if (usScrollFirstLine) {
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_CONTLINE;
			usScrollFirstLine--;
		}
		if (MldScrollDisplayIncrementCursor (usScroll)) {
			// If we have scrolled the display region then the first line for this item
			// has also scrolled up so we need to adjust pMldCurTrnScrol->uchPreCursor
			// accordingly.
			if (pMldCurTrnScrol->uchPreCursor > 0)
				pMldCurTrnScrol->uchPreCursor--;
		}
    }

    VosSetCurPos(pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchPreCursor, 0);
    VosSetCurType(pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);

    return (MLD_SUCCESS);
}
/*
*===========================================================================
** Format  : VOID  MldWriteScrollDisplay(UCHAR *pszString, USHORT usScroll,
*                   USHORT usType, USHORT usRow, USHORT usLength);
*
*   Input  : UCHAR   *pszString     -String Data
*            USHORT  usScroll       -Scroll Type(MLD_SCROLL_1
*                                                 MLD_SCROLL_2
*                                                 MLD_SCROLL_3)
*            USHORT  usType         -First Display Position
*            USHORT  usRow          -No. of lines
*            USHORT  usLength       -Length of 1 line.
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays formatted data on the specified scroll display,
			 when adding a condiment to an item.  Since the item may have other
			 items after it, we will need to do an insert at the cursor position
			 and move down anything that may be after this item.
*===========================================================================
*/
static SHORT MldWriteScrollDisplay9(TCHAR *pszString, USHORT usScroll, USHORT usScrollLine, USHORT usLength,
                                LONG *plAttrib, USHORT usType)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
    USHORT  usCurRow = 0, usCurCol = 0;
    USHORT  uchCursor = 0;
    USHORT  i=0, j=0, k=0;
	USHORT  usItemNumber=0;
    USHORT usReadOffset=0, usReadLen=0, usReadItem = 0;
	UCHAR   uchLineStatus = 0;
	USHORT  usScrollFirstLine = 0;

    switch (usScroll & MLD_SCROLL_MASK) {
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

    pMldCurTrnScrol->uchStatus2 &= ~MLD_MOVE_CURSOR;

	// Handle the case where the first line of the displayed item which
	// we would expect to contain the item and the price is instead on
	// more than one line because the first line of the item contains
	// other information such as quantity, seat number, mnemonic, and price
	// so that the first line is actually on several lines.  If it is
	// on several lines then the count of those lines is in
	// the top half of the USHORT usScrollLine.
	usScrollFirstLine = MLD_SETLINE_COUNT(usScrollLine);
	usScrollLine &=0xff;

    if (usScrollLine == 0) {   /* if no display data exists */
        return (MLD_NOT_DISPLAY);
    }

    /* display first row */
    VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);

    VosGetCurPos (pMldCurTrnScrol->usWinHandle, &usCurRow, &usCurCol);
	usItemNumber = pMldCurTrnScrol->uchItemLine [usCurRow];
	for (uchCursor = usCurRow;
	     uchCursor > 0 && pMldCurTrnScrol->uchItemLine [uchCursor] == usItemNumber;
		 uchCursor--) {
	}

	// The item to be modified may be on the second line of the receipt display
	// (uchCursor == 0) or the item to be modified may be on the first line of the
	// receipt display (uchCursor == 0) so we do this check to determine if we
	// need to roll back one of the decrements to be at the right item.
	if (pMldCurTrnScrol->uchItemLine [uchCursor] != usItemNumber) {
		uchCursor++;
		usCurRow = 0;
	}
	else {
		// The item begins on the first line, row 0, or has been scrolled off.
		// Now we are going to check to see if this condiment belongs to an item
		// that has been scrolled off of the display.  This can happen if other
		// items have been added, the display has scrolled a few lines but a
		// condiment for an item is still displayed.  We need to ensure that the
		// display is synchronized with the scroll buffer.
		// At this point, uchCursor should be pointing to the item of this condiment
		// being edited so uchCursor + the selected line of the display should be
		// greater than or equal to the number of lines of display to repaint.
		for (usCurRow = uchCursor;
		     usCurRow < pMldCurTrnScrol->usMldBottomCursor && pMldCurTrnScrol->uchItemLine [usCurRow] == usItemNumber;
			 usCurRow++) {
		}
		// usCurRow now contains the number of displayed lines for this item which would
		// be the same as usScrollLine if the display has not been scrolled so that either
		// the line containing the ittem has scrolled up off the display or one or more
		// condiments have either scrolled up off the display or scrolled down off the display.
		// Remember that usCurRow is zero based index into the array so that this loop
		// falls out when usCurRow is one past the last valid item but that in turn is
		// the count of the number of lines displayed.
		usCurRow = usScrollLine - usCurRow;
	}

    pMldCurTrnScrol->uchPreCursor = uchCursor;
	pMldCurTrnScrol->uchCurCursor = uchCursor;

	/*
	This function is designed to handle the case involving adding a new
	condiment to an item when the item may have zero or more items after
	it.  In other words, we are going to insert a new condiment into
	the list of condiments displayed.  We do this by adding another row
	to the number of rows occupied by this item and its condiments.

	It is assumed that the cursor, pMldCurTrnScrol->uchCurCursor is
	positioned on the item and we must now insert a new row by moving
	everything that is after the current row down one row.
	**/

	if (usScrollLine > 1)
		uchLineStatus |= MLD_LINESTATUS_MULTILINE;

	VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);

    for (i = 0; i < usScrollLine; i++) {
		if (i < usCurRow) {
			MldScrollDisplayUpdateInsertAtCursor (usScroll);
		}
		if (*plAttrib) {    /* display with reverse video */
			MldStringAttrSpecialColumn (usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength, plAttrib);
		} else {
			MldString (usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength);
		}
        pszString += (usLength + 1);
		plAttrib += (usLength + 1);

		if (pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] == 0) {
			pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] = usItemNumber;
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] = uchLineStatus;
			if (i == 0) {
				pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_FIRSTLINE;
			}
			if (usScrollFirstLine) {
				pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_CONTLINE;
				usScrollFirstLine--;
			}
		}
		MldScrollDisplayIncrementCursor (usScroll);
    }
	VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);
	VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);
    return (MLD_SUCCESS);
}
/*
*===========================================================================
** Format  : VOID  MldWriteScrollDisplay(UCHAR *pszString, USHORT usScroll,
*                   USHORT usType, USHORT usRow, USHORT usLength);
*
*   Input  : UCHAR   *pszString     -String Data
*            USHORT  usScroll       -Scroll Type(MLD_SCROLL_1
*                                                 MLD_SCROLL_2
*                                                 MLD_SCROLL_3)
*            USHORT  usType         -First Display Position
*            USHORT  usRow          -No. of lines
*            USHORT  usLength       -Length of 1 line.
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays formatted data on the specified scroll display,
			 when adding/editing a condiment that is on the 
*===========================================================================
*/
static SHORT MldWriteScrollDisplay10(TCHAR *pszString, USHORT usScroll, USHORT usScrollLine, USHORT usLength,
                                LONG *plAttrib, USHORT usType)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
    USHORT  uchCursor = 0;
    USHORT  i = 0;
    USHORT  uchReadItem=0;
	UCHAR   uchLineStatus = 0;
	USHORT  usScrollFirstLine = 0;

    switch (usScroll & MLD_SCROLL_MASK) {
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

	pMldCurTrnScrol->uchStatus &= ~MLD_CURITEM_TMP;

    pMldCurTrnScrol->uchStatus2 &= ~MLD_MOVE_CURSOR;

	// Handle the case where the first line of the displayed item which
	// we would expect to contain the item and the price is instead on
	// more than one line because the first line of the item contains
	// other information such as quantity, seat number, mnemonic, and price
	// so that the first line is actually on several lines.  If it is
	// on several lines then the count of those lines is in
	// the top half of the USHORT usScrollLine.
	usScrollFirstLine = MLD_SETLINE_COUNT(usScrollLine);
	usScrollLine &=0xff;

    if (usScrollLine == 0) {   /* if no display data exists */
        return (MLD_NOT_DISPLAY);
    }

    /* display first row */
    VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);

	// This is a new item so we will add the item to the display area.
	// Since it is a new item, we will increment our count of items.
	// This is supposed to be synchronized with IdxFileInfo.uchNoOfItem
	// from the header of the MLD file and will be once this item is
	// stored into the MLD file.

	if ( ! (pMldCurTrnScrol->uchStatus2 & MLD_ORDERDEC_MADE)) {
		pMldCurTrnScrol->uchCurItem++;

		// we now need to renumber any items currently in the list
		// because this new order declaration is being inserted at
		// the beginning of the list.
		for (i = 0; i < sizeof(pMldCurTrnScrol->uchItemLine)/sizeof(pMldCurTrnScrol->uchItemLine[0]); i++) {
			if (pMldCurTrnScrol->uchItemLine [i] != 0) {
				pMldCurTrnScrol->uchItemLine [i]++;
			}
		}
	}

	/*
	This function is designed to handle the case involving adding a new
	condiment to an item when the item may have zero or more items after
	it.  In other words, we are going to insert a new condiment into
	the list of condiments displayed.  We do this by adding another row
	to the number of rows occupied by this item and its condiments.

	It is assumed that the cursor, pMldCurTrnScrol->uchCurCursor is
	positioned on the item and we must now insert a new row by moving
	everything that is after the current row down one row.
	**/

	if (usScrollLine > 1)
		uchLineStatus |= MLD_LINESTATUS_MULTILINE;

	uchCursor = pMldCurTrnScrol->uchCurCursor;
	pMldCurTrnScrol->uchCurCursor = 0;
    for (i = 0; i < usScrollLine; i++) {
		if ( ! (pMldCurTrnScrol->uchStatus2 & MLD_ORDERDEC_MADE)) {
			MldScrollDisplayUpdateInsertAtCursor (usScroll);
		}
		pMldCurTrnScrol->uchStatus2 |= MLD_ORDERDEC_MADE;
		if ( i > 0 && pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] != 1) {
				MldScrollDisplayUpdateInsertAtCursor (usScroll);
		}
		if (*plAttrib) {    /* display with reverse video */
			MldStringAttrSpecialColumn (usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength, plAttrib);
		} else {
			MldString (usScroll, pMldCurTrnScrol->uchCurCursor, pszString, usLength);
		}
        pszString += (usLength + 1);
		plAttrib += (usLength + 1);

		if (pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] == 0) {
			pMldCurTrnScrol->uchItemLine [pMldCurTrnScrol->uchCurCursor] = 1;
			pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] = uchLineStatus;
			if (i == 0) {
				pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_FIRSTLINE;
			}
			if (usScrollFirstLine) {
				pMldCurTrnScrol->uchLineStatus [pMldCurTrnScrol->uchCurCursor] |= MLD_LINESTATUS_CONTLINE;
				usScrollFirstLine--;
			}
		}
		MldScrollDisplayIncrementCursor (usScroll);
    }

	// reposition the cursor to the Order Declare entered.
	pMldCurTrnScrol->uchCurCursor = 0;

	VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);
	VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);

    return (MLD_SUCCESS);
	 
}



/*
*===========================================================================
** Format  : VOID  MldWriteScrollDisplay(UCHAR *pszString, USHORT usScroll,
*                   USHORT usType, USHORT usRow, USHORT usLength);
*
*   Input  : UCHAR   *pszString     -String Data
*            USHORT  usScroll       -Scroll Type(MLD_SCROLL_1
*                                                 MLD_SCROLL_2
*                                                 MLD_SCROLL_3)
*            USHORT  usType         -First Display Position
*            USHORT  usRow          -No. of lines
*            USHORT  usLength       -Length of 1 line.
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays formatted data on the specified scroll display
*===========================================================================
*/
SHORT MldWriteScrollDisplay(TCHAR *pszString, USHORT usScroll, USHORT usType,
                            USHORT usScrollLine, USHORT usLength, LONG *plAttrib)
{
	USHORT  usType2 = MLD_TYPE_GET_FLAG(usType);

    switch (usType2) {
    case MLD_NEW_ITEMIZE:
    case MLD_NEW_ITEMIZE2:
        return (MldWriteScrollDisplay1(pszString, usScroll, usScrollLine, usLength, plAttrib, usType));
        break;

    case MLD_UPDATE_ITEMIZE:
    case MLD_UPDATE_ITEMIZE2:
        return (MldWriteScrollDisplay2(pszString, usScroll, usScrollLine, usLength, plAttrib, usType));
        break;

    case MLD_UPPER_CURSOR:
    case MLD_UPPER_CURSOR2:
        return (MldWriteScrollDisplay3(pszString, usScroll, usScrollLine, usLength, plAttrib, usType));
        break;

    case MLD_LOWER_CURSOR:
    case MLD_LOWER_CURSOR2:
        return (MldWriteScrollDisplay4(pszString, usScroll, usScrollLine, usLength, plAttrib, usType));
        break;

    case MLD_TRANS_DISPLAY:
    case MLD_TRANS_DISPLAY2:
        return (MldWriteScrollDisplay5(pszString, usScroll, usScrollLine, usLength, plAttrib, usType));
        break;

    case MLD_REVERSE_DISPLAY:
    case MLD_REVERSE_DISPLAY2:
        return (MldWriteScrollDisplay6(pszString, usScroll, usScrollLine, usLength, plAttrib, usType));
        break;

    case MLD_ONLY_DISPLAY:
        return (MldWriteScrollDisplay7(pszString, usScroll, usScrollLine, usLength, plAttrib, usType));
        break;

	case MLD_CONDIMENT_UPDATE:
		return (MldWriteScrollDisplay8(pszString, usScroll, usScrollLine, usLength, plAttrib, usType)); //Current Item's Condiment is being displayed
		break;

	case MLD_CONDIMENT_UPDATE_PREVIOUS:
		return (MldWriteScrollDisplay8(pszString, usScroll, usScrollLine, usLength, plAttrib, usType)); //An item that has previously been entered is being					
		break;

	case MLD_ORDER_DECLARATION:
		return (MldWriteScrollDisplay10(pszString, usScroll, usScrollLine, usLength, plAttrib, usType)); //An item that has previously been entered is being					
		break;

	default:
		NHPOS_ASSERT_TEXT(0, "  Bad type in MldWriteScrollDisplay().");
		break;
    }
    return (MLD_ERROR);
}

/***** End Of Source *****/



