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
* Title       : Multiline Display Item module I/F (2x20 Scroll)
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDWRITE.C                                                      
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
* Nov-10-95 : 03.01.00 : M.Ozawa    : Initial
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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
#include <uie.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <trans.h>
#include <rfl.h>
#include <prtprty.h>
#include <mld.h>
#include <csop.h>
#include <csetk.h>
#include <csstbetk.h>
#include <cpm.h>
#include <eept.h>
#include "..\..\item\include\itmlocal.h"
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

static SHORT MldReadFilePRTIDX (USHORT usItemNumber, PRTIDX *pIndex)
{
	USHORT usReadOffset;
	ULONG  ulActualBytesRead;

    usReadOffset = MldScroll1Buff.usIndexHeadOffset +
                    ( sizeof( PRTIDXHDR ) +
                    ( sizeof( PRTIDX ) * usItemNumber));

    //RPH SR# 201
	MldReadFile( usReadOffset, pIndex, sizeof( PRTIDX ),
                MldScroll1Buff.sIndexHandle, &ulActualBytesRead);

	return (ulActualBytesRead == sizeof(PRTIDX));
}


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
    ITEMTENDER  *pItemTender;

    switch ( ((MLDITEMDATA *)pItem)->uchMajorClass ) {
    case CLASS_ITEMOPEOPEN:
        return (MLD_SUCCESS);

    case CLASS_ITEMTRANSOPEN:
        switch ( ((MLDITEMDATA *)pItem)->uchMinorClass ) {
        case CLASS_OPENNOSALE:              /* no sales */
        case CLASS_OPENPO:                  /* paid out */
        case CLASS_OPENRA:                  /* received on account */
        case CLASS_OPENTIPSPO:              /* tips paid out */
        case CLASS_OPENTIPSDECLARED:        /* declared tips */
        case CLASS_OPENCHECKTRANSFER:       /* check transfer */
            return (MLD_SUCCESS);
        }
    }

    if (*(( UCHAR*)pItem) == CLASS_ITEMTRANSOPEN) {
        /* not display duplicated item(reorder) */

        if (MldScroll1Buff.uchCurItem) {

            return (MLD_SUCCESS);
        }
    }

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    /* set temporay buffer for cursor down situation */
    MldTempStore(pItem);

    if (*(( UCHAR*)pItem) == CLASS_ITEMMODIFIER) {

        MldScroll1Buff.uchDispItem = MldScroll1Buff.uchCurItem;

        MldScroll1Buff.uchSetCond = 0x00;
        MldScroll1Buff.uchDispCond = 0x00;
        MldScroll1Buff.uchItemDisc = 0;

    } else {

        if (usType == MLD_NEW_ITEMIZE) {

            MldScroll1Buff.uchCurItem++;    /* update current itemization */
            MldScroll1Buff.uchDispItem = MldScroll1Buff.uchCurItem;

            MldScroll1Buff.uchSetCond = 0x00;
            MldScroll1Buff.uchDispCond = 0x00;
            MldScroll1Buff.uchItemDisc = 0;

            if (*(( UCHAR*)pItem) == CLASS_ITEMTENDER) {

                pItemTender = pItem;
                if (pItemTender->lChange) {
                    MldScroll1Buff.uchDispItem++;   /* for  tender/change display */
                }
            }

        } else if (usType == MLD_UPDATE_ITEMIZE) {

            MldScroll1Buff.uchDispItem = MldScroll1Buff.uchCurItem;

            MldScroll1Buff.uchSetCond++;
            MldScroll1Buff.uchDispCond = MldScroll1Buff.uchSetCond;
            MldScroll1Buff.uchItemDisc = 0;

            /* ---- check mdc of print modifier & condiment plu priority --- */
            if ( CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT0 ) &&
                 CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT2 )) {

                MldScroll1Buff.uchDispCond++;   /* for last item display */
            }
        }
    }

    if (MldScroll1Buff.uchStatus & MLD_EXEC_CONSOL) {
        if ( *( UCHAR *)pItem != CLASS_ITEMAFFECTION) {     /* ignor add check at post/recall */
            MldScroll1Buff.uchStatus &= ~MLD_EXEC_CONSOL;   /* reset item consolidation flag */
        }
    }
    MldScroll1Buff.uchStatus2 &= ~MLD_MOVE_CURSOR;
    MldScroll1Buff.uchSpecialStatus &= ~MLD_CHANGE_STATUS;

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
    if (usScroll & MLD_SCROLL_1) {

        MldScrollWrite(pItem, usType);

    }

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
    /* SHORT sStatus; ### V1.0 DEL */

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    /* reset data exists in previous buffer status */
    MldScroll1Buff.uchCurItem--;    /* reset current itemization */
    if (MldScroll1Buff.uchStatus & MLD_ITEMDISC_STORE) {
        MldResetItemDiscTmpStore();
        MldScroll1Buff.uchStatus &= ~MLD_ITEMDISC_STORE;
    } else {
        MldScroll1Buff.uchStatus &= ~MLD_CURITEM_TMP;
    }
    MldScroll1Buff.uchDispCond = 0;
    MldScroll1Buff.uchSetCond = 0;
    MldScroll1Buff.uchItemDisc = 0;

    MldScroll1Buff.uchStatus2 &= ~MLD_MOVE_CURSOR;

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return 0;	/* ### V1.0 Mod */
}

/*
*===========================================================================
** Format  : USHORT   MldECScrollWrite2(USHORT usScroll);
*               
*    Input : USHORT           usScroll
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function is called from application user and clear item.
*===========================================================================
*/
SHORT   MldECScrollWrite2(USHORT usScroll)
{
    if (usScroll & MLD_SCROLL_1) {

        MldECScrollWrite();

    }

    return (MLD_SUCCESS);

    usScroll = usScroll;
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
SHORT   MldLocalUpCursor(USHORT usScroll)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
    PRTIDX Index;
    PRTIDXHDR  IndexHeader;
    UCHAR  auchTempStoWork[sizeof(ITEMSALES) +sizeof(ITEMDISC)+ 10];
    CHAR   achBuff[sizeof(ITEMSALES) + 10];
    USHORT uchReadItem;
    USHORT usReadLen;
    SHORT  sStatus = MLD_SUCCESS, i, sConsStatus = 0;
	ULONG	ulActualBytesRead;//RPH SR# 201

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
    if (pMldCurTrnScrol->uchCurItem == 0) {
        return(MLD_SUCCESS);
    }

    /* item consolidation by cursor key */
    if (!(pMldCurTrnScrol->uchStatus2 & MLD_MOVE_CURSOR) && (pMldCurTrnScrol->uchStatus & MLD_CURITEM_TMP)) {

        MldTempRestore(achBuff);
        if (MldCheckConsolidation((ITEMSALES *)achBuff) == MLD_SUCCESS) {

            for (i = 0; i<(pMldCurTrnScrol->uchCurItem-1); i++) {

				MldReadFilePRTIDX (usScroll, i, &Index);

                if (PRT_GET_CLASS(Index.uchPrintClass) == PRT_DEPT) {
                    if ((UCHAR)Index.usDeptNo != ((ITEMSALES *)achBuff)->usDeptNo) {
                    /* if ((UCHAR)Index.usPLUDeptNo != ((ITEMSALES *)achBuff)->uchDeptNo) { */
                        continue;   /* not consolidate */
                    }
                } else
                if ((PRT_GET_CLASS(Index.uchPrintClass) == PRT_PLU) ||
                    (PRT_GET_CLASS(Index.uchPrintClass) == PRT_SETMENU)) {
                    if (_tcsncmp(Index.auchPLUNo,((ITEMSALES *)achBuff)->auchPLUNo, NUM_PLU_LEN) != 0) {
                    /* if (Index.usPLUDeptNo != ((ITEMSALES *)achBuff)->usPLUNo) { */
                        continue;   /* not consolidate */
                    }
                } else {
                    continue;   /* not consolidate */
                }

                /* read itemize data which directed by index file offset */
                //RPH SR# 201
				MldReadFile(Index.usItemOffset,
                            auchTempStoWork,
                            sizeof(auchTempStoWork),
                            pMldCurTrnScrol->sFileHandle, &ulActualBytesRead);

                /* decompress itemize data */
                memset(&achBuff, 0, sizeof(achBuff));
                usReadLen = RflGetStorageItem(achBuff, auchTempStoWork, RFL_WITH_MNEM);

                /* verify previous item is able to consolidate or not */
                MldTempRestore(auchTempStoWork);

                if (MldSetSalesConsolidation( (ITEMSALES *)achBuff, (ITEMSALES *)auchTempStoWork)
                                            == MLD_SUCCESS) {

                    pMldCurTrnScrol->uchCurItem--;    /* not display previous item */
                    pMldCurTrnScrol->uchStatus &= ~MLD_CURITEM_TMP;

                    sConsStatus = 1;
                    break;
                }
            }
        }
    }

    pMldCurTrnScrol->uchStatus2 |= MLD_MOVE_CURSOR;

    if (pMldCurTrnScrol->uchStatus & MLD_EXEC_CONSOL) {           /* after item consolidation */
        pMldCurTrnScrol->uchDispItem = pMldCurTrnScrol->uchCurItem; /* display from last item */
        pMldCurTrnScrol->uchDispItem++;                           /* for last item display */
        pMldCurTrnScrol->uchStatus &= ~MLD_EXEC_CONSOL;
    } else if (pMldCurTrnScrol->uchDispItem <= 1) {
        /* display upper message */
        MldDispEndMessage(MLD_UPPER_CURSOR);
        pMldCurTrnScrol->uchItemDisc = 0;
        pMldCurTrnScrol->uchSetCond = 0;
        pMldCurTrnScrol->uchDispCond = 0;
        return(MLD_SUCCESS);
    }

    /* read current top item */
    if ((pMldCurTrnScrol->uchDispCond == 0) ||    /* if condiment is displayed, display noun by up key */
        (sConsStatus)) {                        /* previous item is consolidated */
        pMldCurTrnScrol->uchDispItem--;
    }
    uchReadItem = pMldCurTrnScrol->uchDispItem;
    uchReadItem--;

    /* read index file header */
    //RPH SR# 201
	MldReadFile(pMldCurTrnScrol->usIndexHeadOffset,
                &IndexHeader, sizeof( PRTIDXHDR ),
                pMldCurTrnScrol->sIndexHandle, &ulActualBytesRead );

    memset(&achBuff, 0x00, sizeof(achBuff));

    for (;
            pMldCurTrnScrol->uchDispItem > 0;
            pMldCurTrnScrol->uchDispItem--, uchReadItem--) {      /* loop routine for zero row display */
        if ((pMldCurTrnScrol->uchDispItem == pMldCurTrnScrol->uchCurItem) 
                && (pMldCurTrnScrol->uchStatus & MLD_CURITEM_TMP)) {
            /* read temporary storage */
            if (MldTempRestore(achBuff) == MLD_ERROR) {
                sStatus = MLD_NOT_DISPLAY;
                continue;
            }

            /* ---- display check ---- */
            if ( achBuff[ 0 ] == CLASS_ITEMSALES ) {
                if (MldChkDisplayPluNo((ITEMSALES *)achBuff ) == FALSE) {
                    sStatus = MLD_NOT_DISPLAY;
                    continue;
                }
            }
        } else if ((pMldCurTrnScrol->uchDispItem== (pMldCurTrnScrol->uchCurItem-(UCHAR)1)) 
                && (pMldCurTrnScrol->uchStatus & MLD_ITEMDISC_STORE)) {
            /* read temporary storage */
            MldTmpSalesRestore((ITEMSALES *)&achBuff);

            /* ---- display check ---- */
            if ( achBuff[ 0 ] == CLASS_ITEMSALES ) {
                if (MldChkDisplayPluNo((ITEMSALES *)achBuff ) == FALSE) {
                    sStatus = MLD_NOT_DISPLAY;
                    continue;
                }
            }
        } else {
            if (pMldCurTrnScrol->uchDispItem > IndexHeader.uchNoOfItem) {
                return (MLD_ERROR);
                break;
            }

			MldReadFilePRTIDX (usScroll, uchReadItem, &Index);

            /* read itemize data which directed by index file offset */
            //RPH SR# 201
			MldReadFile(Index.usItemOffset,
                        auchTempStoWork,
                        sizeof(auchTempStoWork),
                        pMldCurTrnScrol->sFileHandle, &ulActualBytesRead);

            /* decompress itemize data */
            memset(&achBuff, 0, sizeof(achBuff));
            usReadLen = RflGetStorageItem(achBuff, auchTempStoWork, RFL_WITH_MNEM);

            /* ---- skip item discount without parent plu */
            if (( achBuff[ 0 ] == CLASS_ITEMDISC ) &&
                ( achBuff[ 1 ] == CLASS_ITEMDISC1 )) {
                sStatus = MLD_NOT_DISPLAY;
                continue;
            }

            /* ---- display check ---- */
            if ( achBuff[ 0 ] == CLASS_ITEMSALES ) {
                /* ---- set item qty/price by index data ---- */
                MldSetItemSalesQtyPrice( (ITEMSALES *)achBuff, &Index);

                MldSetSalesCouponQTY((ITEMSALES *)achBuff, pMldCurTrnScrol->sFileHandle, MLD_SCROLL_1);

                if (MldChkDisplayPluNo((ITEMSALES *)achBuff ) == FALSE) {
                    sStatus = MLD_NOT_DISPLAY;
                    continue;
                }
            }

            if ( MldIsSalesIDisc( achBuff )) {
                pMldCurTrnScrol->uchItemDisc = 1;
            } else {
                pMldCurTrnScrol->uchItemDisc = 0;
            }

            /* ---- set item consolidation with previous item ---- */
            if ( achBuff[ 0 ] == CLASS_ITEMSALES ) {
                /* read temporary storage */
                MldTmpSalesRestore((ITEMSALES *)&auchTempStoWork);
                
                MldSetSalesConsolidation( (ITEMSALES *)achBuff, (ITEMSALES *)auchTempStoWork);
            }
        }

        pMldCurTrnScrol->uchSetCond = 0;
        pMldCurTrnScrol->uchDispCond = 0;
        sStatus = MldLocalScrollWrite(achBuff, MLD_UPPER_CURSOR, MLD_SCROLL_1);

        if (sStatus != MLD_NOT_DISPLAY) break;
    }

    if (sStatus == MLD_DISPLAY_CHANGE) {  /* retry if change existed */
        pMldCurTrnScrol->uchDispItem++;
    }
    if (pMldCurTrnScrol->uchDispItem == 0) {   /* reached to upper end */
        if (sStatus == MLD_NOT_DISPLAY) {
            /* display upper message */
            MldDispEndMessage(MLD_UPPER_CURSOR);
            pMldCurTrnScrol->uchItemDisc = 0;
            pMldCurTrnScrol->uchSetCond = 0;
            pMldCurTrnScrol->uchDispCond = 0;
        }
    }

    pMldCurTrnScrol->uchDispCond = 0;     /* reset condiment counter */

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
    PRTIDX Index;
    PRTIDXHDR  IndexHeader;
    UCHAR  auchTempStoWork[sizeof(ITEMSALES) +sizeof(ITEMDISC)+ 10];
    CHAR   achBuff[sizeof(ITEMSALES) + 10];
    USHORT uchReadItem;
    USHORT usReadLen;
    SHORT  sStatus = MLD_SUCCESS, i;
	ULONG	ulActualBytesRead;//RPH SR# 201

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
    if (pMldCurTrnScrol->uchCurItem == 0) {
        return(MLD_SUCCESS);
    }
        
    /* item consolidation by cursor key */
    if (!(pMldCurTrnScrol->uchStatus2 & MLD_MOVE_CURSOR) &&
            (pMldCurTrnScrol->uchStatus & MLD_CURITEM_TMP)) {

        MldTempRestore(achBuff);
        if (MldCheckConsolidation((ITEMSALES *)achBuff) == MLD_SUCCESS) {

            for (i = 0; i < (pMldCurTrnScrol->uchCurItem-1); i++) {
				MldReadFilePRTIDX (usScroll, i, &Index);

                if (PRT_GET_CLASS(Index.uchPrintClass) == PRT_DEPT) {
                    if (Index.usDeptNo != ((ITEMSALES *)achBuff)->usDeptNo) {
                    /* if ((UCHAR)Index.usPLUDeptNo != ((ITEMSALES *)achBuff)->uchDeptNo) { */
                        continue;   /* not consolidate */
                    }
                } else
                if ((PRT_GET_CLASS(Index.uchPrintClass) == PRT_PLU) ||
                    (PRT_GET_CLASS(Index.uchPrintClass) == PRT_SETMENU)) {
                    if (_tcsncmp(Index.auchPLUNo,((ITEMSALES *)achBuff)->auchPLUNo, NUM_PLU_LEN) != 0) {
                    /* if (Index.usPLUDeptNo != ((ITEMSALES *)achBuff)->usPLUNo) { */
                        continue;   /* not consolidate */
                    }
                } else {
                    continue;   /* not consolidate */
                }

                /* read itemize data which directed by index file offset */
                //RPH SR# 201
				MldReadFile(Index.usItemOffset, auchTempStoWork, sizeof(auchTempStoWork),
                            pMldCurTrnScrol->sFileHandle, &ulActualBytesRead);

                /* decompress itemize data */
                memset(&achBuff, 0, sizeof(achBuff));
                usReadLen = RflGetStorageItem(achBuff, auchTempStoWork, RFL_WITH_MNEM);

                /* verify previous item is able to consolidate or not */
                MldTempRestore(auchTempStoWork);

                if (MldSetSalesConsolidation( (ITEMSALES *)achBuff, (ITEMSALES *)auchTempStoWork)
                                            == MLD_SUCCESS) {
                    pMldCurTrnScrol->uchCurItem--;    /* not display previous item */
                    pMldCurTrnScrol->uchStatus &= ~MLD_CURITEM_TMP;
                    break;
                }
            }
        }
    }

    pMldCurTrnScrol->uchStatus2 |= MLD_MOVE_CURSOR;

    if (pMldCurTrnScrol->uchStatus & MLD_EXEC_CONSOL) {
        pMldCurTrnScrol->uchDispItem = 0; /* display first item */
        pMldCurTrnScrol->uchStatus &= ~MLD_EXEC_CONSOL;   /* reset item consolidation flag */
    }

    if (pMldCurTrnScrol->uchDispItem >= pMldCurTrnScrol->uchCurItem) {
        if (pMldCurTrnScrol->uchSpecialStatus & MLD_CHANGE_STATUS) {
            /* if cursor is down at tender display */
            pMldCurTrnScrol->uchDispItem = pMldCurTrnScrol->uchCurItem;
            if (pMldCurTrnScrol->uchDispItem) {
                pMldCurTrnScrol->uchDispItem--;
            }
            pMldCurTrnScrol->uchSpecialStatus &= ~MLD_CHANGE_STATUS;
            /* display change mnemonic and amount */
        } else {
            /* display lower message */
            MldDispEndMessage(MLD_LOWER_CURSOR);
            pMldCurTrnScrol->uchDispItem = (UCHAR)(pMldCurTrnScrol->uchCurItem + 1);
            pMldCurTrnScrol->uchItemDisc = 0;
            pMldCurTrnScrol->uchSetCond = 0;
            pMldCurTrnScrol->uchDispCond = 0;
            return(MLD_SUCCESS);
        }
    }

    /* read current bottom item */
    if (pMldCurTrnScrol->uchItemDisc) {       /* retry if item discount */
        if(pMldCurTrnScrol->uchDispItem)
			pMldCurTrnScrol->uchDispItem--;
    }
    uchReadItem = pMldCurTrnScrol->uchDispItem;
    pMldCurTrnScrol->uchDispItem++;

    /* read index file header */
    //RPH SR# 201
	MldReadFile(pMldCurTrnScrol->usIndexHeadOffset, &IndexHeader, sizeof( PRTIDXHDR ),
                pMldCurTrnScrol->sIndexHandle, &ulActualBytesRead );

    memset(&achBuff, 0x00, sizeof(achBuff));

    for ( ; pMldCurTrnScrol->uchDispItem <= pMldCurTrnScrol->uchCurItem;
            uchReadItem++, pMldCurTrnScrol->uchDispItem++) {      /* loop routine for zero row display */

        if ((pMldCurTrnScrol->uchDispItem == pMldCurTrnScrol->uchCurItem) 
                && (pMldCurTrnScrol->uchStatus & MLD_CURITEM_TMP)) {
            /* read temporary storage */
            if (MldTempRestore(achBuff) == MLD_ERROR) {
                sStatus = MLD_NOT_DISPLAY;
                break;
            }

            /* ---- display check ---- */
            if ( achBuff[ 0 ] == CLASS_ITEMSALES ) {
                if (MldChkDisplayPluNo((ITEMSALES *)achBuff ) == FALSE) {
                    sStatus = MLD_NOT_DISPLAY;
                    continue;
                }
            }
        } else if ((pMldCurTrnScrol->uchDispItem == (pMldCurTrnScrol->uchCurItem-(UCHAR)1))
                && (pMldCurTrnScrol->uchStatus & MLD_ITEMDISC_STORE)) {
            /* read temporary storage */
            MldTmpSalesRestore((ITEMSALES *)&achBuff);

            /* ---- display check ---- */
            if ( achBuff[ 0 ] == CLASS_ITEMSALES ) {
                if (MldChkDisplayPluNo((ITEMSALES *)achBuff ) == FALSE) {
                    sStatus = MLD_NOT_DISPLAY;
                    continue;
                }
            }
        } else {
            if (pMldCurTrnScrol->uchDispItem > IndexHeader.uchNoOfItem) {
                return(MLD_ERROR);
                break;
            }

			MldReadFilePRTIDX (usScroll, uchReadItem, &Index);

            /* read itemize data which directed by index file offset */
            //RPH SR# 201
			MldReadFile(Index.usItemOffset,
                        auchTempStoWork,
                        sizeof(auchTempStoWork),
                        pMldCurTrnScrol->sFileHandle, &ulActualBytesRead);

            /* decompress itemize data */
            memset(&achBuff, 0, sizeof(achBuff));
            usReadLen = RflGetStorageItem(achBuff, auchTempStoWork, RFL_WITH_MNEM);

            /* ---- skip item discount without parent plu */
            if (( achBuff[ 0 ] == CLASS_ITEMDISC ) &&
                ( achBuff[ 1 ] == CLASS_ITEMDISC1 )) {
                sStatus = MLD_NOT_DISPLAY;
                continue;
            }

            /* ---- display check ---- */
            if ( achBuff[ 0 ] == CLASS_ITEMSALES ) {
                /* ---- set item qty/price by index data ---- */
                MldSetItemSalesQtyPrice( (ITEMSALES *)achBuff, &Index);

                MldSetSalesCouponQTY((ITEMSALES *)achBuff, pMldCurTrnScrol->sFileHandle, MLD_SCROLL_1);

                if (MldChkDisplayPluNo((ITEMSALES *)achBuff ) == FALSE) {
                    sStatus = MLD_NOT_DISPLAY;
                    continue;
                }
            }

            /* ---- display item discount with noun plu ---- */
            if ( MldIsSalesIDisc( achBuff )) {
                if (pMldCurTrnScrol->uchItemDisc) {
                    /* display item discount */
                    /* decompress itemize data */
                    memset(&achBuff, 0, sizeof(achBuff));
                    RflGetStorageItem(achBuff, auchTempStoWork+usReadLen, RFL_WITH_MNEM);

                    pMldCurTrnScrol->uchItemDisc = 0;
                } else {
                    /* display noun plu */
                    pMldCurTrnScrol->uchItemDisc = 1;
                }
            } else {
                pMldCurTrnScrol->uchItemDisc = 0;
            }

            /* ---- set item consolidation with previous item ---- */
            if ( achBuff[ 0 ] == CLASS_ITEMSALES ) {
                /* read temporary storage */
                MldTmpSalesRestore((ITEMSALES *)&auchTempStoWork);
                
                MldSetSalesConsolidation( (ITEMSALES *)achBuff, (ITEMSALES *)auchTempStoWork);
            }
        }

        pMldCurTrnScrol->uchSetCond = 0;
        pMldCurTrnScrol->uchDispCond = 0;
        sStatus = MldLocalScrollWrite(achBuff, MLD_LOWER_CURSOR, MLD_SCROLL_1);

        if (sStatus != MLD_NOT_DISPLAY) break;
    }

    if (sStatus == MLD_DISPLAY_CHANGE) {    /* retry if change existed */
        if (pMldCurTrnScrol->uchDispItem) {
            pMldCurTrnScrol->uchDispItem--;
        }
    }
    if (pMldCurTrnScrol->uchDispItem >= pMldCurTrnScrol->uchCurItem) {
        if (sStatus == MLD_NOT_DISPLAY) {
            MldDispEndMessage(MLD_LOWER_CURSOR);
            pMldCurTrnScrol->uchDispItem = (UCHAR)(pMldCurTrnScrol->uchCurItem + 1);
            pMldCurTrnScrol->uchItemDisc = 0;
            pMldCurTrnScrol->uchSetCond = 0;
            pMldCurTrnScrol->uchDispCond = 0;
        }
    }

    pMldCurTrnScrol->uchDispCond = 0;     /* reset condiment counter */

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : USHORT   MldMoveCursor(USHORT usType);
*               
*    Input : USHORT     usType      -type of cursor movement
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays condiment.
*===========================================================================
*/
SHORT MldMoveCursor(USHORT usType)
{
    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if (usType == 0) {  /* right key */
        MldDispCondiment(MLD_NEXT_CONDIMENT);
    } else {            /* left key */
        MldDispCondiment(MLD_BEFORE_CONDIMENT);
    }

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);
    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : USHORT   MldDispCondiment(VOID);
*               
*    Input : VOID             *pItem     -Item Data address
*            USHORT           usType     -First display position
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays condiment.
*===========================================================================
*/
SHORT   MldDispCondiment(USHORT usType)
{
    PRTIDX Index;
    PRTIDXHDR  IndexHeader;
    UCHAR  auchTempStoWork[sizeof(ITEMSALES) +sizeof(ITEMDISC)+ 10];
    CHAR   achBuff[sizeof(ITEMSALES) + 10];
    USHORT uchReadItem;
    USHORT usReadLen;
    SHORT  sStatus = MLD_SUCCESS;
	ULONG	ulActualBytesRead;//RPH SR# 201

    if ((MldScroll1Buff.sFileHandle < 0) || (MldScroll1Buff.sIndexHandle < 0)) {
        return(MLD_ERROR);
    }

    /* waiting order displayed */
    if (MldScroll1Buff.uchStatus & MLD_WAIT_STATUS) {
        return (MLD_ERROR);
    }

    /* if storage file is overflowed */
    if (MldScroll1Buff.uchStatus & MLD_STORAGE_OVERFLOW) {
        return (MLD_ERROR);
    }

    /* if no data is displayed */
    if (MldScroll1Buff.uchCurItem == 0) {
            return(MLD_SUCCESS);
    }
    if (MldScroll1Buff.uchSetCond == 0) {
            return(MLD_SUCCESS);
    }

    /* read current top item */
    uchReadItem = MldScroll1Buff.uchDispItem;
    uchReadItem--;

    /* read index file header */
    //RPH SR# 201
	MldReadFile(MldScroll1Buff.usIndexHeadOffset,
                &IndexHeader, sizeof( PRTIDXHDR ),
                MldScroll1Buff.sIndexHandle, &ulActualBytesRead );

    if ((MldScroll1Buff.uchDispItem == MldScroll1Buff.uchCurItem) 
            && (MldScroll1Buff.uchStatus & MLD_CURITEM_TMP)) {

        /* read temporary storage */
        if (MldTempRestore(achBuff) == MLD_ERROR) {
            return (MLD_ERROR);
        }

    } else if ((MldScroll1Buff.uchDispItem== (MldScroll1Buff.uchCurItem-(UCHAR)1)) 
            && (MldScroll1Buff.uchStatus & MLD_ITEMDISC_STORE)) {

        /* read temporary storage */
        MldTmpSalesRestore((ITEMSALES *)&achBuff);

    } else {

		MldReadFilePRTIDX (usScroll, uchReadItem, &Index);

        /* read itemize data which directed by index file offset */
        //RPH SR# 201
		MldReadFile(Index.usItemOffset,
                    auchTempStoWork,
                    sizeof(auchTempStoWork),
                    MldScroll1Buff.sFileHandle, &ulActualBytesRead);

        /* decompress itemize data */
        memset(&achBuff, 0, sizeof(achBuff));
        usReadLen = RflGetStorageItem(achBuff, auchTempStoWork, RFL_WITH_MNEM);

        /* ---- set item qty/price by index data ---- */
        if ( achBuff[ 0 ] == CLASS_ITEMSALES ) {

            MldSetItemSalesQtyPrice( (ITEMSALES *)achBuff, &Index);

            MldSetSalesCouponQTY((ITEMSALES *)achBuff, MldScroll1Buff.sFileHandle, MLD_SCROLL_1);

            /* ---- set item consolidation with previous item ---- */
            /* read temporary storage */
            MldTmpSalesRestore((ITEMSALES *)&auchTempStoWork);
            
            MldSetSalesConsolidation( (ITEMSALES *)achBuff, (ITEMSALES *)auchTempStoWork);
        }
    }

    /* display condiment */
    sStatus = MldLocalScrollWrite(achBuff, usType, MLD_SCROLL_1);

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : USHORT   MldPutTransToScroll(USHORT usScroll, USHORT usVliSize,
*                       SHORT sFileHandle, SHORT sIndexHandle);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            USHORT           usVliSize  -vli size of storage file
*            SHORT            sFileHandle -storage file handle
*            SHORT            sIndexHandle -file handle of index
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldPutTransToScroll(MLDTRANSDATA *pData1)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    sStatus = MldLocalPutTransToScroll(pData1->usVliSize,
                                    pData1->sFileHandle,
                                    pData1->sIndexHandle,
                                    MLD_TRANS_DISPLAY);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}

/*
*===========================================================================
** Format  : USHORT   MldPutTransToScroll2(USHORT usScroll, USHORT usVliSize,
*                       SHORT sFileHandle, SHORT sIndexHandle);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            USHORT           usVliSize  -vli size of storage file
*            SHORT            sFileHandle -storage file handle
*            SHORT            sIndexHandle -file handle of index
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*            (check wait key status)
*===========================================================================
*/
SHORT   MldPutTransToScroll2(MLDTRANSDATA *pData1)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if (MldScroll1Buff.uchStatus & MLD_WAIT_STATUS) {
        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return (MLD_ALREADY_DISPLAYED);
    }

    sStatus = MldLocalPutTransToScroll(pData1->usVliSize,
                                    pData1->sFileHandle,
                                    pData1->sIndexHandle,
                                    MLD_TRANS_DISPLAY);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);
    return (sStatus);
}

/*
*===========================================================================
** Format  : USHORT   MldPutTrnToScrollReverse(USHORT usScroll, USHORT usVliSize,
*                       SHORT sFileHandle, SHORT sIndexHandle);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            USHORT           usVliSize  -vli size of storage file
*            SHORT            sFileHandle -storage file handle
*            SHORT            sIndexHandle -file handle of index
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*            from last itemize data
*===========================================================================
*/
SHORT   MldPutTrnToScrollReverse(MLDTRANSDATA *pData1)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    sStatus = MldLocalPutTransToScroll(pData1->usVliSize,
                                    pData1->sFileHandle,
                                    pData1->sIndexHandle,
                                    MLD_REVERSE_DISPLAY);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}

/*
*===========================================================================
** Format  : USHORT   MldPutTrnToScrollReverse2(USHORT usScroll, USHORT usVliSize,
*                       SHORT sFileHandle, SHORT sIndexHandle);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            USHORT           usVliSize  -vli size of storage file
*            SHORT            sFileHandle -storage file handle
*            SHORT            sIndexHandle -file handle of index
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*            from last itemize data(check wait key status)
*===========================================================================
*/
SHORT   MldPutTrnToScrollReverse2(MLDTRANSDATA *pData1)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if (MldScroll1Buff.uchStatus & MLD_WAIT_STATUS) {
        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return (MLD_ALREADY_DISPLAYED);
    }

    sStatus = MldLocalPutTransToScroll(pData1->usVliSize,
                                    pData1->sFileHandle,
                                    pData1->sIndexHandle,
                                    MLD_REVERSE_DISPLAY);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);
    return (sStatus);
}

/*
*===========================================================================
** Format  : USHORT   MldPutSplitTransToScroll(USHORT usScroll, MLDTRANSDATA *pData1);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            USHORT           usVliSize  -vli size of storage file
*            SHORT            sFileHandle -storage file handle
*            SHORT            sIndexHandle -file handle of index
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldPutSplitTransToScroll(USHORT usScroll, MLDTRANSDATA *pData1)
{
    SHORT sStatus = MLD_SUCCESS;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    switch(usScroll) {
    case MLD_SCROLL_1:

        /* copy transaction storage file to mld storage file and display */
        sStatus = MldLocalPutTransToScroll(pData1->usVliSize,
                                        pData1->sFileHandle,
                                        pData1->sIndexHandle,
                                        MLD_TRANS_DISPLAY);
        break;
    }

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}

/*
*===========================================================================
** Format  : USHORT   MldPutSplitTransToScroll2(USHORT usScroll, MLDTRANSDATA *pData1);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            USHORT           usVliSize  -vli size of storage file
*            SHORT            sFileHandle -storage file handle
*            SHORT            sIndexHandle -file handle of index
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldPutSplitTransToScroll2(USHORT usScroll, MLDTRANSDATA *pData1)
{
    SHORT sStatus = MLD_SUCCESS;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    switch(usScroll) {
    case MLD_SCROLL_1:

        if (MldScroll1Buff.uchStatus & MLD_WAIT_STATUS) {

            sStatus = MLD_ALREADY_DISPLAYED;

        } else {

            /* copy transaction storage file to mld storage file and display */
            sStatus = MldLocalPutTransToScroll(pData1->usVliSize,
                                            pData1->sFileHandle,
                                            pData1->sIndexHandle,
                                            MLD_TRANS_DISPLAY);
        }
        break;
    }

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}

/*
*===========================================================================
** Format  : USHORT   MldPutGcfToScroll(USHORT usScroll, SHORT sFileHandle);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            SHORT            sFileHandle -guest check file handle
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldPutGcfToScroll(USHORT usScroll, SHORT sFileHandle)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    sStatus = MldLocalPutGcfToScroll(usScroll, sFileHandle);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}

/*
*===========================================================================
** Format  : USHORT   MldPutGcfToScroll2(USHORT usScroll, SHORT sFileHandle);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            SHORT            sFileHandle -guest check file handle
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldPutGcfToScroll2(USHORT usScroll, SHORT sFileHandle)
{
    SHORT sStatus;

    if (usScroll != MLD_SCROLL_1) {
        return(MLD_ERROR);
    }

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if (MldScroll1Buff.uchStatus & MLD_WAIT_STATUS) {
        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return (MLD_ALREADY_DISPLAYED);
    }

    sStatus = MldLocalPutGcfToScroll(usScroll, sFileHandle);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);
    return (sStatus);
}

/*
*===========================================================================
** Format  : USHORT   MldPutAllGcfToScroll(USHORT usScroll,
*                                SHORT sFileHandle1);
*                                SHORT sFileHandle2,
*                                SHORT sFileHandle3)
*               
*    Input : USHORT           usScroll   -destination scroll display
*            SHORT            sFileHandle1 -guest check file handle
*            SHORT            sFileHandle2 -guest check file handle
*            SHORT            sFileHandle3 -guest check file handle
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldPutAllGcfToScroll(SHORT sFileHandle1, 
                                SHORT sFileHandle2,
                                SHORT sFileHandle3)
{
    SHORT sStatus;

    /* --- display scroll #1 --- */
    sStatus = MldPutGcfToScroll(MLD_SCROLL_1, sFileHandle1);

    return (MLD_SUCCESS);

    sFileHandle2 = sFileHandle3;
    sFileHandle2 = sFileHandle3;
}


/*
*===========================================================================
** Format  : USHORT   MldPutAllGcfToScroll2(USHORT usScroll,
*                                SHORT sFileHandle1);
*                                SHORT sFileHandle2,
*                                SHORT sFileHandle3)
*               
*    Input : USHORT           usScroll   -destination scroll display
*            SHORT            sFileHandle1 -guest check file handle
*            SHORT            sFileHandle2 -guest check file handle
*            SHORT            sFileHandle3 -guest check file handle
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldPutAllGcfToScroll2(SHORT sFileHandle1, 
                                SHORT sFileHandle2,
                                SHORT sFileHandle3)
{
    SHORT sStatus;

    /* --- display scroll #1 --- */
    sStatus = MldPutGcfToScroll2(MLD_SCROLL_1, sFileHandle1);

    return (MLD_SUCCESS);

    sFileHandle2 = sFileHandle3;
    sFileHandle2 = sFileHandle3;
}

/*
*===========================================================================
** Format  : USHORT   MldLocalPutTransToScroll(USHORT usScroll, USHORT usVliSize,
*                       SHORT sFileHandle, SHORT sIndexHandle, USHORT usType);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            USHORT           usVliSize  -vli size of storage file
*            SHORT            sFileHandle -storage file handle
*            SHORT            sIndexHandle -file handle of index
*            USHORT           usType      -reverse or not
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldLocalPutTransToScroll(USHORT usVliSize, SHORT sFileHandle,
                                    SHORT sIndexHandle, USHORT usType)
{
    PRTIDXHDR  IndexHeader;
    UCHAR uchSpecialStatus;
	ULONG	ulActualBytesRead;

    /* save special status contorl bit */
    uchSpecialStatus = MldScroll1Buff.uchSpecialStatus;

    if ((sFileHandle < 0) || (sIndexHandle < 0)) {
        MldScroll1Buff.sFileHandle = -1;
        MldScroll1Buff.sIndexHandle = -1;
        return(MLD_INVALID_HANDLE);
    }

    /* initialize scorll #1 storage file */
    MldStorageInit(&MldScroll1Buff, uchMldSystem);

    /* copy transaction storage to mld storage */
    MldScroll1Buff.usStoVli = usVliSize;
    if (MldGetTrnStoToMldSto(usVliSize, sFileHandle, sIndexHandle,
                    MldScroll1Buff.sFileHandle, MldScroll1Buff.sIndexHandle)
                != MLD_SUCCESS) {
        return (MLD_ERROR);
    }

    /* read index file header */
    //RPH SR# 201
	MldReadFile(MldScroll1Buff.usIndexHeadOffset,
                        &IndexHeader, sizeof( PRTIDXHDR ),
                        MldScroll1Buff.sIndexHandle, &ulActualBytesRead );
    /* set no of item */
    MldScroll1Buff.uchCurItem = IndexHeader.uchNoOfItem;
    MldScroll1Buff.uchDispItem = 0;

    /* restore special status contorl bit */
    MldScroll1Buff.uchSpecialStatus = uchSpecialStatus;

    /* set item consolidation flag */
    MldScroll1Buff.uchStatus |= MLD_EXEC_CONSOL;

    return (MLD_SUCCESS);

    usType = usType;
}

/*
*===========================================================================
** Format  : USHORT   MldLocalPutGcfToScroll(USHORT usScroll, SHORT sFileHandle);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            SHORT            sFileHandle -guest check file handle
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldLocalPutGcfToScroll(USHORT usScroll, SHORT sFileHandle)
{
    PRTIDXHDR  IndexHeader;
	ULONG		ulActualBytesRead;//RPH SR# 201

    if (usScroll != MLD_SCROLL_1) {
        return(MLD_ERROR);
    }

    if (sFileHandle < 0) {
        MldScroll1Buff.sFileHandle = -1;
        MldScroll1Buff.sIndexHandle = -1;
        return(MLD_INVALID_HANDLE);
    }

    /* initialize scorll #1 storage file */
    MldStorageInit(&MldScroll1Buff, uchMldSystem);

    if (MldCopyGcfToStorage(usScroll, sFileHandle) != MLD_SUCCESS) {
        return (MLD_ERROR);
    }

    /* read index file header */
    //RPH SR# 201
	MldReadFile(MldScroll1Buff.usIndexHeadOffset,
                        &IndexHeader, sizeof( PRTIDXHDR ),
                        MldScroll1Buff.sIndexHandle, &ulActualBytesRead );
    /* set no of item */
    MldScroll1Buff.uchCurItem = IndexHeader.uchNoOfItem;
    MldScroll1Buff.uchDispItem = 0;

    /* set item consolidation flag */
    MldScroll1Buff.uchStatus |= MLD_EXEC_CONSOL;

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
SHORT   MldLocalGetCursorDisplay(USHORT usScroll, VOID *pData1, VOID *pData2, USHORT usType)
{
    PRTIDX Index;
    UCHAR  auchTempStoWork[sizeof(ITEMSALES) +sizeof(ITEMDISC)+ 10];
    CHAR   achBuff[sizeof(ITEMSALES) + 10];
    USHORT uchReadItem, uchDispItem;
    USHORT usReadLen;
	ULONG	ulActualBytesRead;//RPH SR# 201
	ITEMSALES Data1Temp;

    if (usScroll != MLD_SCROLL_1) {
        return(MLD_ERROR);
    }

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

	// If the pointer for the data area for the sales data is NULL then we will
	// use our own internal data area and throw away the result.
	// This may be used to obtain Item Discount data for the given cursor location.
	if (pData1 == 0) {
		pData1 = &Data1Temp;
	}

    /* read cursor position item */
    uchReadItem = pMldCurTrnScrol->uchDispItem;

    /* if no data is displayed */
    if (uchReadItem == 0) {
        return (MLD_ERROR);
    } else if (uchReadItem > pMldCurTrnScrol->uchCurItem) {
        return (MLD_ERROR);
    }

    uchDispItem = uchReadItem;
    uchReadItem--;  /* decrement to read the offset of storage */

    if ((uchDispItem == pMldCurTrnScrol->uchCurItem) && (pMldCurTrnScrol->uchStatus & MLD_CURITEM_TMP)) {

        /* read temporary storage */
        if (MldTempRestore(pData1) == MLD_ERROR) {
            return (MLD_ERROR);
        }

    } else if ((uchDispItem == (pMldCurTrnScrol->uchCurItem-(UCHAR)1)) && (pMldCurTrnScrol->uchStatus & MLD_ITEMDISC_STORE)) {
        /* read temporary storage */
        MldTmpSalesRestore((ITEMSALES *)pData1);

        /* read item discount */
        if (MldTempRestore(pData2) == MLD_ERROR) {
            return (MLD_ERROR);
        }
    } else {
        /* read index from index file */
		MldReadFilePRTIDX (usScroll, uchReadItem, &Index);

        /* read itemize data which directed by index file offset */
        //RPH SR# 201
		MldReadFile (Index.usItemOffset, auchTempStoWork, sizeof(auchTempStoWork), pMldCurTrnScrol->sFileHandle, &ulActualBytesRead);

        /* decompress itemize data */
        memset(pData1, 0, sizeof(ITEMSALES));
        usReadLen = RflGetStorageItem (pData1, auchTempStoWork, RFL_WITH_MNEM);

        /* ---- set item qty/price by index data ---- */
        if ( *(UCHAR *)pData1 == CLASS_ITEMSALES ) {
            MldSetItemSalesQtyPrice ( (ITEMSALES *)pData1, &Index);

            MldSetSalesCouponQTY ((ITEMSALES *)pData1, pMldCurTrnScrol->sFileHandle, MLD_SCROLL_1);

            MldResetItemSalesQtyPrice ( (ITEMSALES *)pData1);
        }

        /* ---- display item discount with parent plu ---- */
        if ( MldIsSalesIDisc ( pData1 )) {
            /* decompress itemize data */
            memset (&achBuff, 0, sizeof(achBuff));
            RflGetStorageItem (achBuff, auchTempStoWork+usReadLen, RFL_WITH_MNEM);

            if (pMldCurTrnScrol->uchItemDisc == 0) {  /* item discount is directed */
                /* set first item as item discount */
                memset (pData1, 0, sizeof(ITEMSALES));
                memcpy (pData1, achBuff, sizeof(ITEMDISC));
            } else {
                /* set second item as item discount */
                memcpy (pData2, achBuff, sizeof(ITEMDISC));
            }
        }

        /* ---- set item consolidation with previous item ---- */
        if ( ((ITEMSALES *)pData1)->uchMajorClass == CLASS_ITEMSALES ) {
            /* read temporary storage */
            MldTmpSalesRestore ((ITEMSALES *)&auchTempStoWork);
            MldSetSalesConsolidation ( (ITEMSALES *)pData1, (ITEMSALES *)auchTempStoWork);
        }
    }

    if ( ((ITEMSALES *)pData1)->uchMajorClass == CLASS_ITEMSALES ) {
        /* mask previous item data which is not saved to storage item */
        ((ITEMSALES *)pData1)->ControlCode.uchItemType = 0;
        /* ((ITEMSALES *)pData1)->ControlCode.uchDeptNo = 0; */
        ((ITEMSALES *)pData1)->ControlCode.uchReportCode = 0;
        ((ITEMSALES *)pData1)->fbStorageStatus &= NOT_ITEM_CONS;
    }

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : VOID   MldCheckConsolidation(ITEMSALES *pItem)
*               
*    Input : ITEMSALES        *pItem   -data structure of previous item
*
*   Output : none
*
*    InOut : none
*
** Return  : none
*
** Synopsis: 
*===========================================================================
*/
SHORT   MldCheckConsolidation(ITEMSALES *pItem)
{
    /* verify storage item is able to consolidate */
    if (pItem->uchMajorClass != CLASS_ITEMSALES) {
        return(MLD_ERROR);
    }

    switch (pItem->uchMinorClass) {
    case CLASS_DEPTITEMDISC:
    case CLASS_PLUITEMDISC:
    case CLASS_SETITEMDISC:
    case CLASS_DEPTMODDISC:
    case CLASS_PLUMODDISC:
    case CLASS_SETMODDISC:
        return(MLD_ERROR);
    }

    if (pItem->ControlCode.auchStatus[2] & PLU_SCALABLE) {
        return(MLD_ERROR);
    }

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : VOID   MldSetSalesConsolidation(ITEMSALES *pItem, ITEMSALES *pTempItem)
*               
*    Input : ITEMSALES        *pTempItem   -data structure of previous item
*
*   Output : none
*
*    InOut : ITEMSALES        *pItem       -data structure of storage item 
*
** Return  : none
*
** Synopsis: 
*===========================================================================
*/
SHORT   MldSetSalesConsolidation(ITEMSALES *pItem, ITEMSALES *pTempItem)
{
    LONG            lQTY;                       /* quantity */
    LONG            lProduct;                   /* product */
    SHORT           sCouponQTY;                 /* quantity for coupon */
    SHORT           sCouponCo;                  /* coupon counter */
    USHORT          usOffset;                   /* Offset of Partner, R3.1 */

    /* verify storage item is able to consolidate */
    if (MldCheckConsolidation(pItem) != MLD_SUCCESS) {
        return(MLD_ERROR);
    }

    /* verify previous item is able to consolidate */
    if (MldCheckConsolidation(pTempItem) != MLD_SUCCESS) {
        return(MLD_ERROR);
    }

    /* save storage item data */
    lQTY = pItem->lQTY;
    lProduct = pItem->lProduct;
    sCouponQTY = pItem->sCouponQTY;
    sCouponCo = pItem->sCouponCo;
    usOffset = pItem->usOffset;

    /* override storage item data by previous item data */
    pItem->lQTY = pTempItem->lQTY;
    pItem->lProduct = pTempItem->lProduct;
    pItem->sCouponQTY = pTempItem->sCouponQTY;
    pItem->sCouponCo = pTempItem->sCouponCo;
    pItem->usOffset = pTempItem->usOffset;

    /* mask previous item data which is not saved to storage item */
    pTempItem->ControlCode.uchItemType = 0;
    /* pTempItem->ControlCode.uchDeptNo = 0; */
    pTempItem->ControlCode.uchReportCode = 0;
    pTempItem->fbStorageStatus &= NOT_ITEM_CONS;

    /* void consolidation */
    if (CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT2)) {
        pTempItem->fbModifier &= ~VOID_MODIFIER;
    }

    if (memcmp(pItem, pTempItem, sizeof(ITEMSALES))) {

        /* not consolidate */
        pItem->lQTY = lQTY;
        pItem->lProduct = lProduct;
        pItem->sCouponQTY = sCouponQTY;
        pItem->sCouponCo = sCouponCo;
        pItem->usOffset = usOffset;

        return(MLD_ERROR);

    } else {

        /* consolidate */
        pItem->lQTY = lQTY + pTempItem->lQTY;
        pItem->lProduct = lProduct + pTempItem->lProduct;
        pItem->sCouponQTY = sCouponQTY + pTempItem->sCouponQTY;
        pItem->sCouponCo = sCouponCo + pTempItem->sCouponCo;
        pItem->usOffset = usOffset + pTempItem->usOffset;

        return(MLD_SUCCESS);
    }
}

#endif
/***** End Of Source *****/

