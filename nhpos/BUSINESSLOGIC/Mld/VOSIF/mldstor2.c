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
* Program Name: MLDSTOR2.C
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
* Apl-11-95 : 03.00.00 : M.Ozawa    : Initial
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
#include <vos.h>
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

/*==========================================================================
**    Synopsis: SHORT   MldTempStore( VOID *pMldStorage)    
*
*    Input:     VOID    *pMldStorage
*    Output:    none
*    InOut:     none
*
*    Return:    nothing
*
*    Description:   copy current itemization data to temporary buffer.
*
*                   This function does not copy the item data to the temporary buffer.
*                   It assumes that if it is called then item data has been copied to
*                   the temporary buffer and the MLD display data structures need to be
*                   updated.
*
*                   Typically a screen update is done using the following example sequence:
*                       ----- Save Coupon Data to Common Local Data -----
*                       ItemPreviousItem(pItemCoupon, 0);
*                       ----- Display Coupon to LCD -----
*                       MldScrollWrite(pItemCoupon, MLD_NEW_ITEMIZE);
*
*                   In some cases the following sequence is used instead:
*                       MldScrollWrite(&ItemMulti, MLD_NEW_ITEMIZE);
*                       MldScrollFileWrite(&ItemMulti);
*
*                   This function is called within MldScrollWrite() and MldScrollWrite2()
*                   to indicate that the current item buffer has been updated with a new item.
*                   The function MldScrollFileWrite() uses the MLD_CURITEM_TMP flag to determine
*                   if the item should be stored or not and will clear the flag if the store
*                   is successful.
*
*                   See also function MldECScrollWrite() for Error Correct which uses
*                   MldResetItemDiscTmpStore() to back out MldTempStore() changes.
==========================================================================*/
VOID MldTempStore(CONST VOID *pItem)
{
    
    switch ( ITEMSTORAGENONPTR(pItem)->uchMajorClass ) {
    /* --- set special status for noun item with item discount ---- */
    case CLASS_ITEMDISC :                                       /* discount */
        if ( ITEMSTORAGENONPTR(pItem)->uchMinorClass  == CLASS_ITEMDISC1) {
            /* save parent plu major/minor class */
            MldTempStorage.uchNounMajorClass = MldTempStorage.uchMajorClass;
            MldTempStorage.uchNounMinorClass = MldTempStorage.uchMinorClass;
#if DISPTYPE==1
            /* save parent plu display lines */
            if (MldScroll1Buff.uchPreCursor == MLD_OUT_OF_SCROLL_AREA) {
                MldTempStorage.uchPrevLines = 0;
            } else {
                if (MldScroll1Buff.uchPreCursor == uchMldBottomCursor) {
                    MldTempStorage.uchPrevLines = 1;
                } else if ((MldScroll1Buff.uchCurCursor == MLD_OUT_OF_SCROLL_AREA) || (MldScroll1Buff.uchCurCursor == uchMldBottomCursor)) {
                    MldTempStorage.uchPrevLines = uchMldScrollLine - MldScroll1Buff.uchPreCursor;

                } else {
                    MldTempStorage.uchPrevLines = MldScroll1Buff.uchCurCursor - MldScroll1Buff.uchPreCursor;
                }
            }
#endif
            MldScroll1Buff.uchStatus |= MLD_ITEMDISC_STORE;
        }
        /* break */
    case CLASS_ITEMSALES :
    case CLASS_ITEMTENDER :                                     /* tender */
    case CLASS_ITEMCOUPON :                                     /* coupon */
        MldTempStorage.uchMajorClass = ITEMSTORAGENONPTR(pItem)->uchMajorClass;
        MldTempStorage.uchMinorClass = ITEMSTORAGENONPTR(pItem)->uchMinorClass;
        break;

	// we need to process the following item types by setting the MLD_CURITEM_TMP flag for MldScrollFileWrite().
	// function MldTempRestore() is not used for these item class types and will return an error so
	// ignore them. however we still want to set the flag for MldScrollFileWrite().
    case CLASS_ITEMTRANSOPEN :                                  /* transaction open */
    case CLASS_ITEMTOTAL :                                      /* total key */
    case CLASS_ITEMAFFECTION :                                  /* affection */
    case CLASS_ITEMMULTI :                                      /* multi check print */
        break;

    case CLASS_ITEMMISC :                                       /* misc. transaction */
        if (ITEMSTORAGENONPTR(pItem)->uchMinorClass == CLASS_MONEY) {
            MldTempStorage.uchMajorClass = ITEMSTORAGENONPTR(pItem)->uchMajorClass;
            MldTempStorage.uchMinorClass = ITEMSTORAGENONPTR(pItem)->uchMinorClass;
        }
        break;

    default : 
        return;                                    /* illegal major class */

    }

    /* set current itemization exists on temporay buffer */
    MldScroll1Buff.uchStatus |= MLD_CURITEM_TMP;    // indicate MldTempStore() was called.
}

/*==========================================================================
**    Synopsis: SHORT   MldTempRestore( VOID *pMldStorage)    
*
*    Input:     VOID    *pMldStorage
*    Output:    none
*    InOut:     none
*
*    Return:    nothing
*
*    Description:   restore current itemization data from temporary buffer.
*                   not all item data types are stored in the temporary buffer
*                   so only the special few will actually work.
==========================================================================*/
SHORT MldTempRestore(VOID *pMldStorage)
{
    SHORT sStatus = MLD_ERROR;

    switch(MldTempStorage.uchMajorClass ) {
    case CLASS_ITEMSALES :
    case CLASS_ITEMTENDER :                                     /* tender */
    case CLASS_ITEMCOUPON :                                     /* coupon */
        MldTmpSalesRestore( pMldStorage );
        sStatus = MLD_SUCCESS;
        break;

    case CLASS_ITEMDISC :                                       /* discount */
        if (MldTempStorage.uchMinorClass == CLASS_ITEMDISC1) {
            MldTmpDiscRestore( pMldStorage );
        } else {
            MldTmpSalesRestore( pMldStorage );
        }
        sStatus = MLD_SUCCESS;
        break;
    }

    return (sStatus);
}

/*==========================================================================
**    Synopsis: SHORT   MldTmpSalesRestore( ITEMSALES *ItemSales)    
*
*    Input:     ITEMSALES   *pItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    nothing
*
*    Description:   restore sales data from temporary buffer
==========================================================================*/
VOID    MldTmpSalesRestore( ITEMSALES *pItemSales)
{
    ITEMCOMMONLOCAL    *pCommonLocalRcvBuff = ItemCommonGetLocalPointer();

    *pItemSales = pCommonLocalRcvBuff->ItemSales;
}

/*==========================================================================
**    Synopsis: SHORT   MldTmpSalesRestore( ITEMSALES *ItemSales)    
*
*    Input:     ITEMSALES   *pItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    nothing
*
*    Description:   restore sales data from temporary buffer
==========================================================================*/
VOID    MldTmpDiscRestore( ITEMDISC *pItemDisc)
{
    ITEMCOMMONLOCAL    *pCommonLocalRcvBuff = ItemCommonGetLocalPointer();

    *pItemDisc = pCommonLocalRcvBuff->ItemDisc;
}

// MldResetItemDiscTmpStore() - back out the changes made by function MldTempStore()
// in order to handle an Error Correct action.
UCHAR MldResetItemDiscTmpStore(VOID)
{
    UCHAR uchPrevLine;

	// back out the discount class data replacing it with the sales item class data
	// which was put into uchNounMajorClass and uchNounMinorClass by MldTempStore().
    MldTempStorage.uchMajorClass = MldTempStorage.uchNounMajorClass;
    MldTempStorage.uchMinorClass = MldTempStorage.uchNounMinorClass;
    MldTempStorage.uchNounMajorClass = 0;
    MldTempStorage.uchNounMinorClass = 0;
    uchPrevLine = MldTempStorage.uchPrevLines;
    MldTempStorage.uchPrevLines = 0;

    return (uchPrevLine);
}


/*
*===========================================================================
** Format  : SHORT    MldGetItemSalesDiscData(USHORT usScroll
*                                            ITEMSALES *pItemSales,
*                                            ITEMDISC *pItemDisc,
*                                            USHORT usItemNumber)
*               
*    Input : MUSHORT        usScroll       - scroll display
*            ITEMSALES      *pItemSales    - pointer to item sales buffer
*            ITEMDISC       *pItemDisc     - pointer to item discount buffer
*            USHORT         usItemNumber   - item to retrieve, one based
*
*   Output : ITEMSALES      *pItemSales    -item sales data
*            ITEMDISC       *pItemDisc     -item discount data
*
*    InOut : none
*
** Return  : SHORT  
*
** Synopsis: This function provides the indicates item sales and item
*            discount data requested for a particular item.  The item
*            number, 1 through number of items in transaction, indicates
*            which item to retrieve.  If the item requested is in the MLD
*            file, then it is retrieved from the MLD file.  If the item is
*            in the local storage area and has not yet been written to the
*            MLD file then we retrieve the item from local storage.
*
*            WARNING: This function assumes that if the item number specified
*                     is of greater than the item count in the MLD file then
*                     the item desired is in local storage.  This means that
*                     if the item number specified is greater than the count
*                     of items in the MLD file, you will always get the item
*                     in local storage even if you specify a number greater
*                     than the number of items in the transaction.
*===========================================================================
*/
SHORT  MldGetItemSalesDiscData (USHORT usScroll, ITEMSALES *pItemSales, ITEMDISC *pItemDisc, USHORT usItemNumber)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
	PRTIDXHDR  IndexHeader = {0};
    CHAR       achBuff[sizeof(ITEMDATASIZEUNION)];
    USHORT     usReadOffset, usReadLen;
	ULONG      ulActualBytesRead;

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

    //RPH SR# 201
	MldReadFile( pMldCurTrnScrol->usIndexHeadOffset, &IndexHeader, sizeof( PRTIDXHDR ), pMldCurTrnScrol->sIndexHandle, &ulActualBytesRead );
	NHPOS_ASSERT(ulActualBytesRead >= sizeof(PRTIDXHDR));

    memset(&achBuff, 0, sizeof(achBuff));
	if (usItemNumber > IndexHeader.uchNoOfItem) {
        if (MldTempRestore (achBuff) == MLD_SUCCESS) {
			if (ITEMSTORAGENONPTR(achBuff)->uchMajorClass == CLASS_ITEMSALES) {
				memcpy (pItemSales, achBuff, sizeof(ITEMSALES));
				return 0;
			}
			else if (ITEMSTORAGENONPTR(achBuff)->uchMajorClass == CLASS_ITEMDISC) {
				memcpy (pItemDisc, achBuff, sizeof(ITEMDISC));
				return 0;
			}
		}
	}
	else {
		PRTIDX     Index = {0};
		UCHAR      auchTempStoWork[TRN_WORKBUFFER];

		/* read index from index file */
		usReadOffset = pMldCurTrnScrol->usIndexHeadOffset + ( sizeof( PRTIDXHDR ) + ( sizeof( PRTIDX ) * (usItemNumber - 1) ));

		//RPH SR# 201
		MldReadFile( usReadOffset, &Index, sizeof( PRTIDX ), pMldCurTrnScrol->sIndexHandle, &ulActualBytesRead );

		/* read itemize data which directed by index file offset */
		//RPH SR# 201
		MldReadFile(Index.usItemOffset, auchTempStoWork, sizeof(auchTempStoWork), pMldCurTrnScrol->sFileHandle, &ulActualBytesRead);

		/* decompress itemize data */
		memset(pItemSales, 0, sizeof(ITEMSALES));
		usReadLen = RflGetStorageItem (pItemSales, auchTempStoWork, RFL_WITH_MNEM);

        /* ---- set item qty/price by index data ---- */
		if ( pItemSales->uchMajorClass == CLASS_ITEMSALES ) {

            MldSetItemSalesQtyPrice ( pItemSales, &Index);
            MldSetSalesCouponQTY (pItemSales, pMldCurTrnScrol->sFileHandle, usScroll);
            MldResetItemSalesQtyPrice (pItemSales);
        }

        /* ---- display item discount with parent plu ---- */
        if ( pItemDisc && MldIsSalesIDisc( pItemSales )) {
            /* decompress itemize data */
            RflGetStorageItem(achBuff, auchTempStoWork + usReadLen, RFL_WITH_MNEM);

            /* set second item as item discount */
            memcpy(pItemDisc, achBuff, sizeof(ITEMDISC));
        }
	}

	return 0;
}

/***** End Of Source *****/
