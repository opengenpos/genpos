/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             AT&T GIS Corporation, E&M OISO   ||
||        * << ECR-90 >>  *                                              ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION STORAGE MODULE
:   Category       : TRANSACTION MODULE, NCR 2170 US Hospitality Application
:   Program Name   : TRNSTSPL.C
:  ---------------------------------------------------------------------
:   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
:   Memory Model       : Medium Model
:   Options            : /c /AM /W4 /G1s /Os /Za /Zp
:  ---------------------------------------------------------------------
:  Abstract:
:
:  ---------------------------------------------------------------------
:  Update Histories
:  Date     : Ver.Rev. :   Name     : Description
: Nov-28-95 : 03.01.00 :  hkato     : R3.1
: Aug-16-99 : 03.05.00 : K.Iwata    : R3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/
#include	<tchar.h>
#include    <stdlib.h>
#include    <string.h>

#include    <ecr.h>
#include    <pif.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <trans.h>
#include    <prt.h>
#include    <rfl.h>
#include    <paraequ.h>
#include    <appllog.h>
#include    <item.h>
#include    <prtprty.h>


/*==========================================================================
**    Synopsis: VOID    TrnStoSalesSplQty(ITEMSALES *pItem, UCHAR *puchBuffer)
*
*    Input:     ITEMSALES   *pItem
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   Decide Sales Qty
==========================================================================*/

static VOID    TrnStoSalesSplQty(ITEMSALES *pItem, UCHAR *puchBuffer)
{
    TRANSTORAGESALESNON     *pSalesNon = TRANSTORAGENONPTR(puchBuffer);

    if (!(pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE)) {
        pItem->sCouponQTY = (SHORT)(pItem->lQTY / 1000L);
    }

    pSalesNon->sCouponQTY = pItem->sCouponQTY;
}


/*==========================================================================
**    Synopsis: SHORT   TrnStoOpenSpl(ITEMTRANSOPEN *pItem, TrnStorageType sType)
*   
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store transaction open data
==========================================================================*/
        
SHORT   TrnStoOpenSpl(ITEMTRANSOPEN *pItem, TrnStorageType sType)
{
    if (!(pItem->fbStorageStatus & NOT_CONSOLI_STORAGE)) {
		UCHAR   auchBuffer[TRN_WORKBUFFER];
		SHORT   sSize;

        sSize = TrnStoOpenComp(pItem, auchBuffer);
        TrnStoConsNormalSpl(auchBuffer, sSize, sType);
    }
    return(TRN_SUCCESS);
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoSalesSpl(ITEMSALES *pItemSales, TrnStorageType sType)
*
*    Input:     ITEMSALES   *pItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store sales data
==========================================================================*/

SHORT   TrnStoSalesSpl(ITEMSALES *pItemSales, TrnStorageType sStoType)
{
    if (!(pItemSales->fbStorageStatus & NOT_CONSOLI_STORAGE)) {
		ITEMSALES          SalesWork = *pItemSales;
		TRANSTOSALESSRCH   CompItem = {0};

		if (( TranCurQualPtr->flPrintStatus & CURQUAL_POSTCHECK ) &&
			( ! ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_PVOID )) &&
			( TranCurQualPtr->fsCurStatus & CURQUAL_REORDER ) &&
			( TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK ) &&
            ( pItemSales->fbModifier & VOID_MODIFIER )) {
            SalesWork.fbModifier &= ~VOID_MODIFIER;
        }

        CompItem.usItemSize = TrnStoSalesComp(&SalesWork, CompItem.auchBuffer);

        if (pItemSales->fbStorageStatus & NOT_ITEM_CONS) {
            TrnStoConsNormalSpl(CompItem.auchBuffer, CompItem.usItemSize, sStoType);
        } else {
			SHORT  sReturn;
			TRANSTOSALESSRCH    NormalItem = {0}, VoidItem = {0};

            sReturn = TrnStoSalesSearch(&SalesWork, CompItem.auchBuffer, &NormalItem, &VoidItem, sStoType, 0);
			if (sReturn == TRN_ERROR) {
				NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoSalesSearch() failed.");
				return TRN_ERROR;
			}

			TrnStoSalesSplQty(&SalesWork, CompItem.auchBuffer);
            TrnStoSalesUpdate(&SalesWork, &CompItem, &NormalItem, &VoidItem, sStoType);
            TrnStoIdxUpdate(&SalesWork, &NormalItem, &VoidItem, sStoType);
        }
    }

    return(TRN_SUCCESS);
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoDiscSpl(ITEMDISC *pItemDisc, TrnStorageType sType)
*
*    Input:     ITEMDISC    *pItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store discount data
==========================================================================*/

SHORT   TrnStoDiscSpl(ITEMDISC *pItemDisc, TrnStorageType sType)
{
	if (!(pItemDisc->fbStorageStatus & NOT_CONSOLI_STORAGE)) {
		UCHAR       auchBuffer[TRN_WORKBUFFER];
		SHORT       sSize;

        sSize = TrnStoDiscComp (pItemDisc, auchBuffer);
        TrnStoConsNormalSpl(auchBuffer, sSize, sType);
    }

    return(TRN_SUCCESS);
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoCouponSpl(ITEMCOUPON *pItem, TrnStorageType sType)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store coupon data
==========================================================================*/

SHORT   TrnStoCouponSpl(ITEMCOUPON *pItem, TrnStorageType sType)
{
    if (!(pItem->fbStorageStatus & NOT_CONSOLI_STORAGE)) {
		UCHAR       auchBuffer[TRN_WORKBUFFER];
		SHORT       sSize;

        sSize = TrnStoCouponComp(pItem, &auchBuffer[0]);
        TrnStoConsNormalSpl(&auchBuffer[0], sSize, sType);
    }

    return (TRN_SUCCESS);
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoTotalSpl(ITEMTOTAL *ItemTotal, TrnStorageType sType)
*
*    Input:     ITEMTOTAL   *ItemTotal
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store total key data
==========================================================================*/

SHORT   TrnStoTotalSpl(ITEMTOTAL *ItemTotal, TrnStorageType sType)
{
    if ((ItemTotal->fbStorageStatus & NOT_CONSOLI_STORAGE) == 0) {
		UCHAR   uchBuffer[TRN_WORKBUFFER];
		SHORT   sSize;

        sSize = TrnStoTotalComp(ItemTotal, &uchBuffer[0]);
        TrnStoConsNormalSpl(&uchBuffer[0], sSize, sType);
    }

    return(TRN_SUCCESS);
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoTenderSpl(ITEMTENDER *ItemTender, TrnStorageType sType)
*
*    Input:     ITEMTENDER  *ItemTender
*    Output:    none      
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store tender data
==========================================================================*/

SHORT   TrnStoTenderSpl(ITEMTENDER *ItemTender, TrnStorageType sType)
{
    if ((ItemTender->fbStorageStatus & NOT_CONSOLI_STORAGE) == 0) {
		UCHAR   uchBuffer[TRN_WORKBUFFER];
		SHORT   sSize;

        sSize = TrnStoTenderComp(ItemTender, &uchBuffer[0]);
        TrnStoConsNormalSpl(&uchBuffer[0], sSize, sType);
    }

    return(TRN_SUCCESS);
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoMiscSpl(ITEMMISC *ItemMisc, TrnStorageType sType)
*
*    Input:     ITEMMISC    *ItemMisc
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store misc. transaction data
==========================================================================*/

SHORT   TrnStoMiscSpl(ITEMMISC *ItemMisc, TrnStorageType sType)
{
    if ((ItemMisc->fbStorageStatus & NOT_CONSOLI_STORAGE) == 0) {
		UCHAR   uchBuffer[TRN_WORKBUFFER];
		SHORT   sSize;

        sSize = TrnStoMiscComp(ItemMisc, &uchBuffer[0]);
        TrnStoConsNormalSpl(&uchBuffer[0], sSize, sType);
    }

    return(TRN_SUCCESS);
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoAffectSpl(ITEMAFFECTION *pItem, TrnStorageType sType)
*
*    Input:     ITEMAFFECTION   *ItemAffection
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store affection data
==========================================================================*/

SHORT   TrnStoAffectSpl(ITEMAFFECTION *pItem, TrnStorageType sType)
{
    if ((pItem->fbStorageStatus & NOT_CONSOLI_STORAGE) == 0) {
		UCHAR   uchBuffer[TRN_WORKBUFFER];
		SHORT   sSize;

        sSize = TrnStoAffectComp(pItem, &uchBuffer[0]);
        TrnStoConsNormalSpl(&uchBuffer[0], sSize, sType);
    }

    return(TRN_SUCCESS);
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoPrintSpl(ITEMPRINT *ItemPrint, TrnStorageType sType)
*
*    Input:     ITEMPRINT   *ItemPrint
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store print data
==========================================================================*/

SHORT   TrnStoPrintSpl(ITEMPRINT *ItemPrint, TrnStorageType sType)
{
    if ((ItemPrint->fbStorageStatus & NOT_CONSOLI_STORAGE) == 0) {
		UCHAR   uchBuffer[TRN_WORKBUFFER];
		SHORT   sSize;

        sSize = TrnStoPrintComp(ItemPrint, &uchBuffer[0]);
        TrnStoConsNormalSpl(&uchBuffer[0], sSize, sType);
    }

    return(TRN_SUCCESS);
}

/*==========================================================================
**  Synopsis:   VOID    TrnStoConsNormalSpl(VOID *pData,
*                               SHORT sSize, TrnStorageType sType)
*
*   Input:      VOID  *pData    - address of item data to write file
*               SHORT sSize     - size of source data in bytes
*               SHORT sType     - TRN_TYPE_SPLITA: Split File A,
*                                 TRN_TYPE_SPLITB: Split File B
*   Output:     none
*   InOut:      none
*
*   Return:     none
*
*   Description:    write "item" to consoli storage
==========================================================================*/

USHORT    TrnStoConsNormalSpl(VOID *pData, SHORT sSize, TrnStorageType sType)
{
    TRANINFSPLIT          *pSpl = TrnGetInfSpl(sType);
    TRANSTORAGESALESNON   *pTranNon = TRANSTORAGENONPTR(pData);
    USHORT                usItemOffset;

    /* --- store item to split storage file --- */
    usItemOffset = (sizeof(TRANCONSSTORAGEHEADER) + pSpl->usTranConsStoVli);
    TrnWriteFile(usItemOffset, pData, sSize, pSpl->hsTranConsStorage);

    /* --- determine current system uses print priority --- */
    if (TrnInformation.TranCurQual.flPrintStatus & CURQUAL_PRINT_PRIORITY) {
        if (pTranNon->uchMajorClass == CLASS_ITEMSALES) {
            /* --- if source item is sales item, analyze it's print priority
                and store it at analyzed position --- */
            TrnStoInsertIndexSpl(pTranNon, usItemOffset, sType);
        } else {
            /* --- if source item is not sales item, store it at the end of
                item storage file --- */
            TrnStoAppendIndexSpl(pTranNon, usItemOffset, sType);
        }
    } else {
        /* --- append print priority index to index file --- */
        TrnStoAppendIndexSpl(pTranNon, usItemOffset, sType);
    }

    /* --- update size of actual data in item storage file --- */
    pSpl->usTranConsStoVli += sSize;
	return usItemOffset;
}

/*==========================================================================
**  Synopsis:   SHORT   TrnStoAppendIndexSpl(TRANSTORAGESALESNON *pTranNon,
*                           USHORT usItemOffset, TrnStorageType sType)
*
*   Input:      TRANSTORAGESALESNON *pTranNon       - address of sales data
*               USHORT               usItemOffset   - offset from top of storage
*               SHORT                sType          - TRN_TYPE_SPLITA: Split File A,
*                                                     TRN_TYPE_SPLITB: Split File B
*   Output:     none
*   InOut:      none
*
*   Return:     TRN_SUCCESS - function is successful.
*               TRN_ERROR   - function is failed.
*
*   Description:    append an item index to index file.
==========================================================================*/

SHORT   TrnStoAppendIndexSpl(TRANSTORAGESALESNON *pTranNon,
            USHORT usItemOffset, TrnStorageType sType)
{
    SHORT       sReturn;
    PRTIDX      IndexNew;
    PRTIDXHDR   IdxFileInfo;
    USHORT      usWriteOffset;
    TRANINFSPLIT    *pSpl = TrnGetInfSpl(sType);
	ULONG		ulActualBytesRead;//RPH 11-11-3 SR# 201

    /* --- prepare print priority index to append storage file --- */
    if ((sReturn = TrnStoSetupPrtIdx(&IndexNew, pTranNon, usItemOffset)) != TRN_SUCCESS) {
        return(sReturn);
    }

    /* --- append print priority index to bottom of index file --- */
    //RPH 11-11-3 SR# 201
	TrnReadFile(0, (VOID *)&IdxFileInfo, (USHORT)sizeof(PRTIDXHDR), (SHORT)pSpl->hsTranConsIndex, &ulActualBytesRead);
    usWriteOffset = sizeof(PRTIDXHDR) + (sizeof(PRTIDX) * IdxFileInfo.uchNoOfItem);
    TrnWriteFile(usWriteOffset, &IndexNew, sizeof(PRTIDX), pSpl->hsTranConsIndex);

    /* --- update information about index file --- */
    IdxFileInfo.uchNoOfItem++;
    IdxFileInfo.usIndexVli += sizeof(PRTIDX);
    TrnWriteFile(0, &IdxFileInfo, sizeof(PRTIDXHDR), pSpl->hsTranConsIndex);

    return(TRN_SUCCESS);
}

/*==========================================================================
**  Synopsis:   SHORT   TrnStoInsertIndexSpl(TRANSTORAGESALESNON *pTranNon,
*                           USHORT usItemOffset, TrnStorageType sType)
*
*   Input:      TRANSTORAGESALESNON *pTranNon       - address of sales data
*               USHORT               usItemOffset   - offset from top of storage
*               SHORT                sType          - TRN_TYPE_SPLITA: Split File A,
*                                                     TRN_TYPE_SPLITB: Split File B
*   Output:     none
*   InOut:      none
*
*   Return:     TRN_SUCCESS - function is successful.
*               TRN_ERROR   - function is failed.
*
*   Description:    insert an item index to index file.
==========================================================================*/

SHORT   TrnStoInsertIndexSpl(TRANSTORAGESALESNON *pTranNon,
            USHORT usItemOffset, TrnStorageType sType)
{
    TRANINFSPLIT    *pSpl = TrnGetInfSpl(sType);

	return TrnStoInsertIndexFileHandle (pSpl->hsTranConsIndex, pTranNon, usItemOffset);
}


/****** End of Source ******/
