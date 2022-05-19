/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION MODULE (SPLIT BODY)
:   Category       : TRANSACTION MODULE, NCR 2170 US Hospitality Application
:   Program Name   : TRNBDSPL.C
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen)
*       2012  -> GenPOS Rel 2.2.0
*       2014  -> GenPOS Rel 2.2.1
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
:  ---------------------------------------------------------------------  
:  Abstract:
:
:  ---------------------------------------------------------------------  
:  Update Histories
:    Date  : Ver.Rev. :   Name      : Description
: 95/11/27 : 03.01.00 : hkato       : R3.1
: 99/08/13 : 03.05.00 : K.Iwata     : merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* Nov-21-15 : 02.02.01 : R.Chambers : using new typedefs for variables and source cleanup.
-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/
#include	<tchar.h>
#include    <string.h>
#include    <stddef.h>

#include    <ecr.h>
#include    <pif.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <trans.h>
#include    <prt.h>
#include    <rfl.h>
#include    <csgcs.h>
#include    <csstbgcf.h>
#include    <csttl.h>
#include    <csstbttl.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <appllog.h>
#include    <prtprty.h>

                                                    
                                                             
/*==========================================================================
**  Synopsis:   VOID    TrnOpenSpl(VOID *ItemOpen, SHORT sType)
*
*   Input:      VOID    *ItemOpen     
*   Output:     none
*   InOut:      none
*
*   Return:     none
*
*   Description:    operator,transaction open
==========================================================================*/

SHORT   TrnOpenSpl(VOID *ItemOpen, TrnStorageType  sStorageType)
{
	if ( ITEMSTORAGENONPTR(ItemOpen)->uchMajorClass == CLASS_ITEMTRANSOPEN) {
        TrnQualTransSpl(ItemOpen, sStorageType);
        TrnStoOpenSpl(ItemOpen, sStorageType);
    }

    return(TRN_SUCCESS);
}

/*==========================================================================
**   Synopsis:  SHORT   TrnItemSpl(VOID *pItem, SHORT sType)
*   
*    Input:     VOID    *Item
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*               TRN_ERROR
*
*    Description:   itemize
==========================================================================*/

SHORT   TrnItemSpl(VOID *pItem, TrnStorageType  sStorageType)
{
	switch( ITEMSTORAGENONPTR(pItem)->uchMajorClass ) {
    case CLASS_ITEMSALES:
        return(TrnSalesSpl(pItem, sStorageType));

    case CLASS_ITEMDISC:
        return(TrnDiscountSpl(pItem, sStorageType));

    case CLASS_ITEMCOUPON:
        return(TrnCouponSpl(pItem, sStorageType));

    case CLASS_ITEMTOTAL:
        return(TrnTotalSpl(pItem, sStorageType));

    case CLASS_ITEMTENDER:
        return(TrnTenderSpl(pItem, sStorageType));

    case CLASS_ITEMMISC:
        return(TrnMiscSpl(pItem, sStorageType));

    case CLASS_ITEMAFFECTION:
        return(TrnAffectionSpl(pItem, sStorageType));

    case CLASS_ITEMPRINT:
        return(TrnPrintSpl(pItem, sStorageType));

    default:
		NHPOS_ASSERT_TEXT(0, "==ERROR: Unknown uchMajorClass in TrnItemSpl().");
        return(TRN_ERROR);
    }                                                    
}

/*==========================================================================
*    Synopsis:  SHORT   TrnSalesSpl(ITEMSALES *ItemSales, SHORT sType)
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   sales
==========================================================================*/

SHORT   TrnSalesSpl(ITEMSALES *ItemSales, SHORT sType)
{
    if (ItemSales->uchMinorClass == CLASS_DEPTMODDISC
        || ItemSales->uchMinorClass == CLASS_PLUMODDISC
        || ItemSales->uchMinorClass == CLASS_SETMODDISC
        || ItemSales->uchMinorClass == CLASS_OEPMODDISC
        || ItemSales->uchMinorClass == CLASS_DEPTITEMDISC
        || ItemSales->uchMinorClass == CLASS_PLUITEMDISC
        || ItemSales->uchMinorClass == CLASS_SETITEMDISC
        || ItemSales->uchMinorClass == CLASS_OEPITEMDISC
        || (ItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE)) {

        if (ItemSales->fbReduceStatus & REDUCE_ITEM) {
            return(TRN_SUCCESS);
        }
    }

    TrnItemSalesSpl(ItemSales, sType);
    ItemSales->usItemOffset = 0;
    TrnStoSalesSpl(ItemSales, sType);
    return(TRN_SUCCESS);
}

/*==========================================================================
*    Synopsis:  SHORT   TrnDiscountSpl(ITEMDISC *ItemDisc, SHORT sType)
*
*    Input:     ITEMDISC    *ItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   discount method
==========================================================================*/

SHORT   TrnDiscountSpl(ITEMDISC *ItemDisc, SHORT sType)
{
    if (!(ItemDisc->fbReduceStatus & REDUCE_ITEM)) {
        TrnQualDiscSpl(ItemDisc, sType);
        TrnItemDiscSpl(ItemDisc, sType);
        TrnStoDiscSpl(ItemDisc, sType);
    }
    return(TRN_SUCCESS);
}

/*==========================================================================
*    Synopsis:  SHORT   TrnCouponSpl(ITEMCOUPON *pItem, SHORT sType)
*
*    Input:     ITEMCOUPON  *pItemCoupon
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   coupon
==========================================================================*/

SHORT   TrnCouponSpl(ITEMCOUPON *pItem, SHORT sType)
{
    if (!(pItem->fbReduceStatus & REDUCE_ITEM)) {
        TrnItemCouponSpl(pItem, sType);
        TrnStoCouponSpl(pItem, sType);
    }
    return(TRN_SUCCESS);
}

/*==========================================================================
*    Synopsis:  SHORT   TrnTotalSpl(ITEMTOTAL *ItemTotal, SHORT sType)
*
*    Input:     ITEMTOTAL   *ItemTotal
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   total key
==========================================================================*/

SHORT   TrnTotalSpl(ITEMTOTAL *ItemTotal, SHORT sType)
{
    return(TrnStoTotalSpl(ItemTotal, sType));
}

/*==========================================================================
**   Synopsis:  SHORT   TrnTenderSpl(ITEMTENDER *ItemTender, SHORT sType)
*
*    Input:     ITEMTENDER  *ItemTender
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   tender method
==========================================================================*/

SHORT   TrnTenderSpl(ITEMTENDER *ItemTender, SHORT sType)
{
    SHORT   i, sFin = 0;

    /*----- Check Finalize Seat# Transaction -----*/
    for (i = 0; i < NUM_SEAT; i++) {
        if (TrnSplitA.TranGCFQual.auchFinSeat[i] != 0) {
            sFin++;
        }
    }

    TrnInformation.fsTransStatus |= TRN_STATUS_SEAT_TRANS;
    if (sFin > 1) {
        TrnInformation.fsTransStatus &= ~TRN_STATUS_SEAT_TRANS;
        TrnInformation.fsTransStatus |= TRN_STATUS_MULTI_TRANS;
    }

    return(TrnStoTenderSpl(ItemTender, sType));
}

/*==========================================================================
**   Synopsis:  SHORT   TrnMiscSpl(ITEMMISC *ItemMisc, SHORT sType)
*
*    Input:     ITEMMISC    *ItemMisc
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   misc. transaction
==========================================================================*/

SHORT   TrnMiscSpl(ITEMMISC *ItemMisc, SHORT sType)
{
    return(TrnStoMiscSpl(ItemMisc, sType));
}                                                                       

/*==========================================================================
*    Synopsis:  SHORT   TrnAffectionSpl(ITEMAFFECTION *pItem, SHORT sType)
*
*    Input:     ITEMAFFECTION   *ItemAffection
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   affection
==========================================================================*/

SHORT   TrnAffectionSpl(ITEMAFFECTION *pItem, SHORT sType)
{
    TrnItemAffectionSpl(pItem, sType);
    return(TrnStoAffectSpl(pItem, sType));
}

/*==========================================================================
*    Synopsis:  SHORT   TrnPrintSpl(ITEMPRINT *ItemPrint, SHORT sType)
*
*    Input:     ITEMPRINT   *ItemPrint
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   print
==========================================================================*/

SHORT   TrnPrintSpl(ITEMPRINT *ItemPrint, SHORT sType)
{
    return(TrnStoPrintSpl(ItemPrint, sType));
}

/*==========================================================================
*    Synopsis:  VOID    TrnThroughSpl(VOID *TrnItem)
*
*    Input:     VOID    *TrnItem
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   through
==========================================================================*/

VOID    TrnThroughSpl(VOID *TrnItem)
{                                                           
    PrtPrintItem(&TrnInformation, TrnItem);
}

/*==========================================================================
*    Synopsis:  VOID    TrnInitializeSpl(USHORT usType)
*
*    Input:     USHORT  usType
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   initialize specified split
==========================================================================*/

VOID    TrnInitializeSpl(USHORT usTraniType)
{
    PRTIDXHDR     IdxFileInfo;
	ULONG		  ulActualBytesRead;
	//11-11-03 JHHJ

    if (usTraniType & TRANI_GCFQUAL_SPLA) {
        memset(&TrnSplitA.TranGCFQual, 0, sizeof(TRANGCFQUAL));
    }
    if (usTraniType & TRANI_ITEMIZERS_SPLA) {
        memset(&TrnSplitA.TranItemizers, 0, sizeof(TRANITEMIZERS));
    }
    if (usTraniType & TRANI_CONSOLI_SPLA) {
		CONST ULONG  ulVliOffset = offsetof(TRANCONSSTORAGEHEADER, usVli);

        TrnSplitA.usTranConsStoVli = 0;
        TrnWriteFile(ulVliOffset, &TrnSplitA.usTranConsStoVli, sizeof(TrnSplitA.usTranConsStoVli), TrnSplitA.hsTranConsStorage);

        /* --- initialize print priority index file --- */
		//11-1103- SR 201 JHHJ
        TrnReadFile(0, &IdxFileInfo, sizeof(PRTIDXHDR), TrnSplitA.hsTranConsIndex, &ulActualBytesRead);
        IdxFileInfo.usIndexVli     = 0;
        IdxFileInfo.uchNoOfItem    = 0;
        IdxFileInfo.uchNoOfSales   = 0;
        IdxFileInfo.usSalesHighest = 0;
        IdxFileInfo.usSalesLowest  = 0;
        TrnWriteFile(0, &IdxFileInfo, sizeof(PRTIDXHDR), TrnSplitA.hsTranConsIndex);
    }

    if (usTraniType & TRANI_GCFQUAL_SPLB) {
        memset(&TrnSplitB.TranGCFQual, 0, sizeof(TRANGCFQUAL));
    }
    if (usTraniType & TRANI_ITEMIZERS_SPLB) {
        memset(&TrnSplitB.TranItemizers, 0, sizeof(TRANITEMIZERS));
    }
    if (usTraniType & TRANI_CONSOLI_SPLB) {
		CONST ULONG  ulVliOffset = offsetof(TRANCONSSTORAGEHEADER, usVli);

        TrnSplitB.usTranConsStoVli = 0;
        TrnWriteFile(ulVliOffset, &TrnSplitB.usTranConsStoVli, sizeof(TrnSplitB.usTranConsStoVli), TrnSplitB.hsTranConsStorage);

        /* --- initialize print priority index file --- */
		//11-1103- SR 201 JHHJ
        TrnReadFile(0, &IdxFileInfo, sizeof(PRTIDXHDR), TrnSplitB.hsTranConsIndex, &ulActualBytesRead);
        IdxFileInfo.usIndexVli     = 0;
        IdxFileInfo.uchNoOfItem    = 0;
        IdxFileInfo.uchNoOfSales   = 0;
        IdxFileInfo.usSalesHighest = 0;
        IdxFileInfo.usSalesLowest  = 0;
        TrnWriteFile(0, &IdxFileInfo, sizeof(PRTIDXHDR), TrnSplitB.hsTranConsIndex);
    }
}

/*
*==========================================================================
**  Synopsis:   SHORT   TrnCreateFileSpl(USHORT usNoOfItem)
*
*       Input:  USHORT  usNoOfItem      - number of items in storage file
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return:     TRN_SUCCESS -   function is successful
*               TRN_ERROR   -   function is failed
*
**  Description:    Create split file.
*==========================================================================
*/
SHORT   TrnCreateFileSpl(USHORT usNoOfItem)
{
	CONST ULONG    ulVliOffset = offsetof(TRANCONSSTORAGEHEADER, usVli);
    PifFileHandle  hsStorageFile;
    ULONG          ulInquirySize;

    /* --- function always create specified file, even if it exists --- */
    TrnDeleteFile(aszTrnConsSplitFileA);
    TrnDeleteFile(aszTrnConsSplitFileB);

    hsStorageFile = TrnOpenFile(aszTrnConsSplitFileA, auchTEMP_NEW_FILE_READ_WRITE);   /* saratoga */
    if (hsStorageFile < 0) {
        return (TRN_ERROR);
    }
    /* --- allocate file size to store specified no. of items --- */
    ulInquirySize = TrnCalStoSize(usNoOfItem, FLEX_CONSSTORAGE_ADR);
    if (TrnExpandFile(hsStorageFile, ulInquirySize) != TRN_SUCCESS) {
        TrnCloseFile(hsStorageFile);    /* add, bug fix */
        TrnDeleteFile(aszTrnConsSplitIndexA);
        return(TRN_ERROR);
    }

    /* --- store size of actual data --- */
    TrnSplitA.usTranConsStoVli = 0;
    TrnWriteFile(ulVliOffset, &TrnSplitA.usTranConsStoVli, sizeof(TrnSplitA.usTranConsStoVli), hsStorageFile);
    TrnCloseFile(hsStorageFile);

    /* --- create index file for print priority --- */
    if (TrnCreateIndexFile(usNoOfItem, aszTrnConsSplitIndexA, NULL) != TRN_SUCCESS) {
        TrnCloseFile(hsStorageFile);
        TrnDeleteFile(aszTrnConsSplitIndexA);
        return(TRN_ERROR);
    }

    hsStorageFile = TrnOpenFile(aszTrnConsSplitFileB, auchTEMP_NEW_FILE_READ_WRITE);   /* saratoga */
    if (hsStorageFile < 0) {
        return (TRN_ERROR);
    }
    /* --- allocate file size to store specified no. of items --- */
    if (TrnExpandFile(hsStorageFile, ulInquirySize) != TRN_SUCCESS) {
        TrnDeleteFile(aszTrnConsSplitFileA);
        TrnDeleteFile(aszTrnConsSplitIndexA);
        TrnCloseFile(hsStorageFile);    /* add, bug fix */
        TrnDeleteFile(aszTrnConsSplitFileB);
        return(TRN_ERROR);
    }

    /* --- store size of actual data --- */
    TrnSplitB.usTranConsStoVli = 0;
    TrnWriteFile(ulVliOffset, &TrnSplitB.usTranConsStoVli, sizeof(TrnSplitB.usTranConsStoVli), hsStorageFile);
    TrnCloseFile(hsStorageFile);

    /* --- create index file for print priority --- */
    if (TrnCreateIndexFile(usNoOfItem, aszTrnConsSplitIndexB, NULL) != TRN_SUCCESS) {
        TrnDeleteFile(aszTrnConsSplitFileA);
        TrnDeleteFile(aszTrnConsSplitIndexA);
        TrnDeleteFile(aszTrnConsSplitFileB);
        return(TRN_ERROR);
    }

    return (TRN_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   SHORT   TrnChkAndCreFileSpl(USHORT usNoOfItem)
*
*       Input:  USHORT  usNoOfItem      - number of items in storage file
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return:     TRN_SUCCESS -   function is successful
*               TRN_ERROR   -   function is failed
*
**  Description:    create specified file, if the file does not exist.
*==========================================================================
*/
SHORT   TrnChkAndCreFileSpl(USHORT usNoOfItem)
{
    PifFileHandle    hsStorageFileA;
    PifFileHandle    hsStorageFileB;

    /* --- try to open current existing file to determine if already exists --- */
    hsStorageFileA = TrnOpenFile(aszTrnConsSplitFileA, auchTEMP_OLD_FILE_READ);    /* saratoga */
    if (0 <= hsStorageFileA) {                  /* file is already existing */
        TrnCloseFile(hsStorageFileA);
    }

    hsStorageFileB = TrnOpenFile(aszTrnConsSplitFileB, auchTEMP_OLD_FILE_READ);    /* saratoga */
    if (0 <= hsStorageFileB) {                  /* file is already existing */
        TrnCloseFile(hsStorageFileB);
    }

    if (hsStorageFileA >= 0 || hsStorageFileB >= 0) {
        return(TRN_SUCCESS);
    }

    return(TrnCreateFileSpl(usNoOfItem));
}    

/*
*==========================================================================
**  Synopsis:   VOID    TrnChkAndDelFileSpl(USHORT usNoOfItem)
*
*       Input:  USHORT  usNoOfItem      - number of items in storage file
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return:     TRN_SUCCESS -   function is successful
*               TRN_ERROR   -   function is failed
**  Description:
*       delete the specified file, if the file is applicable to one of the
*   following cases.
*       * specified file size is different from existing file size.
*       * specified file has not the index file.
*       * if consolidate file is specified, it has not the post receipt file.
*==========================================================================
*/
VOID    TrnChkAndDelFileSpl(USHORT usNoOfItem)
{
    TCHAR       *lpszStorageNameA = aszTrnConsSplitFileA;
    TCHAR       *lpszIndexNameA = aszTrnConsSplitIndexA;
    TCHAR       *lpszStorageNameB = aszTrnConsSplitFileB;
    TCHAR       *lpszIndexNameB = aszTrnConsSplitIndexB;
    SHORT       hsStorageFile;
    SHORT       hsIndexFileA;
    SHORT       hsIndexFileB;
    BOOL        fSameSize;
    BOOL        fIndexExist;
    USHORT      usActualSize = 0;
    ULONG       ulInquirySize;
	ULONG		ulActualBytesRead;

    /* --- if inquiry size is different from actual size, delete
        current existing storage file --- */
    hsStorageFile = TrnOpenFile(lpszStorageNameA, auchTEMP_OLD_FILE_READ);  /* saratoga */
    if (hsStorageFile < 0) {                /* storage file is not existing */
        return;
    }
	//11-1103- SR 201 JHHJ
    TrnReadFile(0, &usActualSize, sizeof(USHORT), hsStorageFile, &ulActualBytesRead);
    TrnCloseFile(hsStorageFile);

    ulInquirySize = TrnCalStoSize(usNoOfItem, FLEX_CONSSTORAGE_ADR);
    fSameSize = (ulInquirySize == usActualSize) ? TRUE : FALSE;

    /* --- determine index file is existing --- */
    hsIndexFileA = TrnOpenFile(lpszIndexNameA, auchTEMP_OLD_FILE_READ); /* saratoga */
    fIndexExist = (0 <= hsIndexFileA) ? TRUE : FALSE;
    TrnCloseFile(hsIndexFileA);

    if (!(fSameSize && fIndexExist)) {
        TrnDeleteFile(lpszStorageNameA);
    }

    hsStorageFile = TrnOpenFile(lpszStorageNameB, auchTEMP_OLD_FILE_READ);  /* saratoga */
    if (hsStorageFile < 0) {                /* storage file is not existing */
        TrnDeleteFile(lpszStorageNameA);
        TrnDeleteFile(lpszIndexNameA);
        return;
    }
	//11-1103- SR 201 JHHJ
    TrnReadFile(0, &usActualSize, sizeof(USHORT), hsStorageFile, &ulActualBytesRead);
    TrnCloseFile(hsStorageFile);

    fSameSize = (ulInquirySize == usActualSize) ? TRUE : FALSE;

    /* --- determine index file is existing --- */
    hsIndexFileB = TrnOpenFile(lpszIndexNameB, auchTEMP_OLD_FILE_READ); /* saratoga */
    fIndexExist = (0 <= hsIndexFileB) ? TRUE : FALSE;
    TrnCloseFile(hsIndexFileB);

    if (!(fSameSize && fIndexExist)) {
        TrnDeleteFile(lpszStorageNameA);
        TrnDeleteFile(lpszIndexNameA);
        TrnDeleteFile(lpszStorageNameB);
        TrnDeleteFile(lpszIndexNameB);  /* add, bug fix */
    }
}    

/*
*==========================================================================
**  Synopsis :  SHORT TrnOpenIndexFileSpl(VOID)
*
*       Input:  Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return  TRN_SUCCESS     -   function is successful
*           LDT_PROHBT_ADR  -   function is failed
*
**  Description:    Open index files of item and consolidate storage file.
*==========================================================================
*/
static SHORT   TrnOpenIndexFileSpl(VOID)
{
	PRTIDXHDR    auchInitIdxInfo = {0};
	int          initSize = sizeof(auchInitIdxInfo.usIndexVli) - sizeof (auchInitIdxInfo.usIndexFSize);

    /* --- initialize index file --- */
    if ((TrnSplitA.hsTranConsIndex = TrnOpenFile(aszTrnConsSplitIndexA, auchTEMP_OLD_FILE_READ_WRITE)) < 0) {    /* saratoga */
        return (LDT_PROHBT_ADR);
    }
    TrnWriteFile(sizeof (auchInitIdxInfo.usIndexFSize), &auchInitIdxInfo.usIndexVli, initSize, TrnSplitA.hsTranConsIndex);

    if ((TrnSplitB.hsTranConsIndex = TrnOpenFile(aszTrnConsSplitIndexB, auchTEMP_OLD_FILE_READ_WRITE)) < 0) {    /* saratoga */
        TrnCloseFile(TrnSplitA.hsTranConsIndex);  TrnSplitA.hsTranConsIndex = -1;
        return(LDT_PROHBT_ADR);
    }
    TrnWriteFile(sizeof (auchInitIdxInfo.usIndexFSize), &auchInitIdxInfo.usIndexVli, initSize, TrnSplitB.hsTranConsIndex);

    return (TRN_SUCCESS);
}

/*
*==========================================================================
**    Synopsis: SHORT   TrnOpenFileSpl(VOID)
*                                                     
*       Input:  Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return  Normal End: TRN_SUCCESS 
*           Error End:  LDT_PROHBT_ADR                                                       
*                                                                  
**  Description:
*           Open item, consoli, post rec. each files and save file handle.
*==========================================================================
*/
SHORT   TrnOpenFileSpl(VOID)
{
    SHORT   sReturn;
	ULONG	ulActualBytesRead;
	ULONG   ulInquirySize = TrnCalStoSize(RflGetMaxRecordNumberByType (FLEX_CONSSTORAGE_ADR), FLEX_CONSSTORAGE_ADR);

    if ((TrnSplitA.hsTranConsStorage = TrnOpenFile(aszTrnConsSplitFileA, auchTEMP_OLD_FILE_READ_WRITE)) < 0) { /* saratoga */
        return(LDT_PROHBT_ADR);
    }
    
	//11-1103- SR 201 JHHJ
    TrnReadFile(0, &TrnSplitA.usTranConsStoFSize, sizeof(TrnSplitA.usTranConsStoFSize), TrnSplitA.hsTranConsStorage, &ulActualBytesRead);
	NHPOS_ASSERT(ulInquirySize <= TrnSplitA.usTranConsStoFSize);

    if ((TrnSplitB.hsTranConsStorage = TrnOpenFile(aszTrnConsSplitFileB, auchTEMP_OLD_FILE_READ_WRITE)) < 0) { /* saratoga */
        TrnCloseFile(TrnSplitA.hsTranConsStorage); TrnSplitA.hsTranConsStorage = -1;
        return(LDT_PROHBT_ADR);
    }

	//11-1103- SR 201 JHHJ
    TrnReadFile(0, &TrnSplitB.usTranConsStoFSize, sizeof(TrnSplitB.usTranConsStoFSize), TrnSplitB.hsTranConsStorage, &ulActualBytesRead);
	NHPOS_ASSERT(ulInquirySize <= TrnSplitB.usTranConsStoFSize);

    /* --- open index files of item/consolidate storage --- */
    sReturn = TrnOpenIndexFileSpl();
    if (sReturn != TRN_SUCCESS) {
        TrnCloseFile(TrnSplitA.hsTranConsStorage); TrnSplitA.hsTranConsStorage = -1;
        TrnCloseFile(TrnSplitB.hsTranConsStorage); TrnSplitB.hsTranConsStorage = -1;
    }
    return(sReturn);
}    

/*
*==========================================================================
**    Synopsis: VOID    TrnCloseFileSpl(VOID)
*                                                  
*       Input:  Nothing 
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*                                                                  
**  Description:    Close split files.
*==========================================================================
*/
VOID    TrnCloseFileSpl(VOID)
{
    TrnCloseFile(TrnSplitA.hsTranConsStorage); TrnSplitA.hsTranConsStorage = -1;
    TrnCloseFile(TrnSplitB.hsTranConsStorage); TrnSplitB.hsTranConsStorage = -1;

    TrnCloseFile(TrnSplitA.hsTranConsIndex); TrnSplitA.hsTranConsIndex = -1;
    TrnCloseFile(TrnSplitB.hsTranConsIndex); TrnSplitB.hsTranConsIndex = -1;
}


/****** End of Source ******/
