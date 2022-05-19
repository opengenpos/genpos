/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION MODULE (BODY)                        
:   Category       : TRANSACTION MODULE, NCR 2170 US Hospitality Application
:   Program Name   : TRNBODY.C                                        
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
* ---------------------------------------------------------------------  
:  Abstract:
:# =  modified or added function at release 3.0
:   TrnOpen()           ;   operator, transaction open        
:   TrnOpeOpen()        ;   operator open
:   TrnTransOpen()      ;   transaction open    
:   TrnItem()           ;   itemize       
:   TrnSales()          ;   sales       
:   TrnDiscount()       ;   discount       
:#  TrnCoupon()         ;   combination coupon
:#  TrnCouponSearch     ;   search for target sales item in each storages
:#  TrnCouponVoidSearch ;   search for coupon item to void 
:   TrnSingleCouponSearch;  search for single coupon has been already done
:   TrnTotal()          ;   total key        
:   TrnTender()         ;   tender
:   TrnMisc()           ;   misc. transaction       
:   TrnModifier()       ;   modifier       
:   TrnOther()          ;   other     
:   TrnOtherDemand()    ;   print demand
:   TrnOtherPrint()     ;   other(except for print demand)
:   TrnAffection()      ;   affection
:   TrnPrint()          ;   print
:   TrnMulti()          ;   multi check payment
:   TrnClose()          ;   operator, transaction close
:   TrnOpeClose()       ;   operator close
:   TrnTransClose()     ;   transaction close
:   TrnTransPrint()     ;   print at transaction close   
:   TrnModeIn()         ;   reg mode-in
:   TrnThrough()        ;   through
:   TrnThroughDisp()    ;   through for KDS, R3.1
:
:#  TrnGetGC()          ;   get target GCF
:#  TrnSaveGC()         ;   save target GCF
:#  TrnCancelGC()       ;   cancel target GCF
:#  TrnQueryGC()        ;   query target GCF (no file lock)
:#  TrnInitialize()     ;   initialize target transaction information
:   TrnSendTotal()      ;   send transaction information to TOTAL MODULE
:   TrnPrintType()      ;   buffering/unbuffering print
:
:#  TrnCreateFile()     ;   create item or consolidate storage file
:#  TrnCreatePostRecFile;   create post receipt file
:#  TrnCreateIndexFile();   create item or consolidate index file
:#  TrnChkAndCreFile()  ;   check and create item/cons./post rec. storage file
:#  TrnChkAndDelFile()  ;   check and delete item/cons./post rec. storage file
:   TrnICPOpenFile()    ;   open item, consoli, post rec. storage file
:#  TrnICOpenIndexFile();   open item and consolidate index file
:   TrnICPCloseFile()   ;   close item, consoli, post rec. storage file
:   TrnOpenFile()       ;   open file
:   TrnExpandFile()     ;   expand file size
:   TrnSeekFile()       ;   seek file
:   TrnReadFile()       ;   read file
:   TrnWriteFile()      ;   write file
:   TrnCloseFile()      ;   close file
:   TrnDeleteFile()     ;   delete file
:   TrnConvertError()   ;   convert error code to leadthru number
:#  TrnCalStoSize()     ;   calculate storage file size
:  ---------------------------------------------------------------------  
:  Update Histories
:    Date  : Ver.Rev. :   Name      : Description
: 92/5/13  : 00.00.01 :   hkato     : initial
: 95/06/07 : 03.00.00 : hkato       : flex drive thru (mod TrnGetGC, TrnCancelGC)
: 95/06/20 : 03.00.00 : T.Nakahata  : Add Combination Coupon Feature
: 95/07/05 : 03.00.01 : T.Nakahata  : save total amount at store/recall
: 95/08/18 : 03.00.04 : T.Nakahata  : no consolidate by total key option
: 95/11/27 : 03.01.00 : hkato       : R3.1
: 96/06/24 : 03.01.09 : M.Ozawa     : add TrnThroughDisp(), TrnSingleCouponSearch()
: 99/08/13 : 03.05.00 : K.Iwata     : R3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Dec-02-99 : 01.00.00 : hrkato     : Saratoga
* Nov-21-15 : 02.02.01 : R.Chambers : using new typedefs for variables and source cleanup.
* Nov-21-15 " 02.02.01 : R.Chambers : fix size calculation in TrnCreateIndexFile() to fix LDT_TAKETL_ADR error
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
#include    <math.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    <stddef.h>

#include    "ecr.h"
#include    "pif.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "trans.h"
#include	"plu.h"
#include	"csstbfcc.h"
#include	"paraequ.h" 
#include	"para.h"
#include	"pararam.h"
#include    "prt.h"
#include    "cscas.h"
#include    "csgcs.h"
#include    "csstbgcf.h"
#include    "csttl.h"
#include    "csstbttl.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "appllog.h"
#include    "prtprty.h"
#include    "csstbstb.h"
#include    "maint.h"
#include	"pmg.h"

#include    "appllog.h"
#include	"rfl.h"
#include	"uie.h"
#include	"uireg.h"
#include	"..\Uifreg\uiregloc.h"

#include	"ConnEngineObjectIf.h"
#include	"item.h"

#if defined(TrnReadWriteFile_Memory)
typedef struct {
	SHORT  hsFileHandle;
	ULONG  ulActualBytesRead;
	UCHAR  *pTrnMemoryFile;
} TrnMemoryFileTableRow;

#define TRNMEMORYFILE_SIZE    65800

static UCHAR  TrnMemoryFile_MemoryArea[14 * TRNMEMORYFILE_SIZE];

static TrnMemoryFileTableRow  TrnMemoryFileTable[] = {
	{-1, 0, TrnMemoryFile_MemoryArea +  0},
	{-1, 0, TrnMemoryFile_MemoryArea +  1 * TRNMEMORYFILE_SIZE},
	{-1, 0, TrnMemoryFile_MemoryArea +  2 * TRNMEMORYFILE_SIZE},
	{-1, 0, TrnMemoryFile_MemoryArea +  3 * TRNMEMORYFILE_SIZE},
	{-1, 0, TrnMemoryFile_MemoryArea +  4 * TRNMEMORYFILE_SIZE},
	{-1, 0, TrnMemoryFile_MemoryArea +  5 * TRNMEMORYFILE_SIZE},
	{-1, 0, TrnMemoryFile_MemoryArea +  6 * TRNMEMORYFILE_SIZE},
	{-1, 0, TrnMemoryFile_MemoryArea +  7 * TRNMEMORYFILE_SIZE},
	{-1, 0, TrnMemoryFile_MemoryArea +  8 * TRNMEMORYFILE_SIZE},
	{-1, 0, TrnMemoryFile_MemoryArea +  9 * TRNMEMORYFILE_SIZE},
	{-1, 0, TrnMemoryFile_MemoryArea + 10 * TRNMEMORYFILE_SIZE},
	{-1, 0, TrnMemoryFile_MemoryArea + 11 * TRNMEMORYFILE_SIZE},
	{-1, 0, TrnMemoryFile_MemoryArea + 12 * TRNMEMORYFILE_SIZE},
	{-1, 0, TrnMemoryFile_MemoryArea + 13 * TRNMEMORYFILE_SIZE},
	{-1, 0, 0}
};

#endif

TRANINFORMATION TrnInformation;                             /* transaction information */
TCHAR FARCONST  aszTrnItemStorageFile[]    = _T("ITEMSTOR");    /* file Name of item storage */
TCHAR FARCONST  aszTrnConsStorageFile[]    = _T("CONSSTOR");    /* file Name of cons. storage */
TCHAR FARCONST  aszTrnPostRecStorageFile[] = _T("POSRSTOR");    /* file Name of post rec. storage */
TCHAR FARCONST  aszTrnItemStorageIndex[]   = _T("ITEMIDX");     /* item storage index */
TCHAR FARCONST  aszTrnNoItemStorageIndex[] = _T("NOITIDX");     /* no cons. storage index */
TCHAR FARCONST  aszTrnConsStorageIndex[]   = _T("CONSIDX");     /* cons. storage index */
TCHAR FARCONST  aszTrnNoConsStorageIndex[] = _T("NOCNIDX");     /* no cons. storage index */
TCHAR FARCONST aszTrnTempStorageFile[]	   = _T("TRNTMP");

/*----- Split Check File,  R3.1 -----*/
TRANINFSPLIT    TrnSplitA;
TCHAR FARCONST  aszTrnConsSplitFileA[]     = _T("SPLITA");      /* file Name of split A */
TCHAR FARCONST  aszTrnConsSplitIndexA[]    = _T("IDXSPLA");     /* split A index */
TRANINFSPLIT    TrnSplitB;
TCHAR FARCONST  aszTrnConsSplitFileB[]     = _T("SPLITB");      /* file Name of split B */
TCHAR FARCONST  aszTrnConsSplitIndexB[]    = _T("IDXSPLB");     /* split B index */

/* 05/20/01 */
/* 12/05/01 */
TRNITEMSTOREPLUMNEM	TrnItemStorePluMnem;
USHORT              usItemStoredPluMnem = 0;
PifSemHandle        husTrnSemaphore = PIF_SEM_INVALID_HANDLE;

/*==========================================================================
**    Synopsis: VOID    TrnGetTranInformation(TRANINFORMATION **TranInfo)
*
*    Input:     TRANINFORMATION     **TranInfo
*    Output:    none
*    InOut:     TRANINFORMATION     *TranInfo
*
*    Return:    none
*
*    Description:   get consoli storage
==========================================================================*/
// See https://stackoverflow.com/questions/4592762/difference-between-const-const-volatile
CONST volatile  TRANINFORMATION  * CONST TrnInformationPtr = &TrnInformation;

TRANINFORMATION  * TrnGetTranInformationPointer (VOID)
{
	return &TrnInformation;
}

VOID TrnGetTranInformation(TRANINFORMATION **ppTranInfo)
{
    *ppTranInfo = &TrnInformation;
}

/*==========================================================================
*    Synopsis:  VOID    TrnQualModeQual( TRANMODEQUAL *TranModeQual )
*    Synopsis:  VOID    TrnPutModeQual( TRANMODEQUAL *TranModeQual )
*
*    Input:     TRANMODEQUAL    *TranModeQual
*    Output:    none
*    InOut:     TRANMODEQUAL    *TranModeQual
*
*    Return:    none
*
*    Description:   get mode qualifier 
*    Description:   put mode qualifier 
*
*                   TrnInformation.TranModeQual is a memory resident data structure
*                   containing the terminal mode information (REG, SUP, etc.) and
*                   is intended to be populated once with a Cashier Sign-in or a
*                   Supervisor Sign-in and to then remain static until such time as
*                   the Cashier or Supervisor performs a Sign-out.
*
*                   A copy of this data area is not saved when a guest check is serviced
*                   and stored in the Guest Check File.
*                   A copy of this data area is saved with the totals data for total update.
*
*                   The data area is typically cleared by function TrnInitialize()
*                   using the TRANI_MODEQUAL flag to indicate to clear the area such
*                   as in function TrnQualClsCashier() or function ItemSignOut().
==========================================================================*/
// See https://stackoverflow.com/questions/4592762/difference-between-const-const-volatile
CONST volatile  TRANMODEQUAL  * CONST TranModeQualPtr = &TrnInformation.TranModeQual;

TRANMODEQUAL *TrnGetModeQualPtr(VOID)
{
	return &TrnInformation.TranModeQual;
}

VOID    TrnGetModeQual( TRANMODEQUAL *pTranModeQual )
{
    *pTranModeQual = TrnInformation.TranModeQual;
}

VOID    TrnPutModeQual( CONST TRANMODEQUAL *pTranModeQual )
{
    TrnInformation.TranModeQual = *pTranModeQual;
}


/*==========================================================================
*    Synopsis:  VOID    TrnGetGCFQual( TRANGCFQUAL **TranGCFQual )
*    Synopsis:  VOID    TrnPutGCFQual(TRANGCFQUAL *TranGCFQual)
*
*    Input:     TRANGCFQUAL *TranGCFQual
*    Output:    none
*    InOut:     TRANGCFQUAL *TranGCFQual
*
*    Return:    none
*
*    Description:   get GCF qualifier                             R3.1
*    Description:   put GCF qualifier
*
*                   TrnInformation.TranGCFQual is a memory resident data structure
*                   containing the guest check management information.  It is
*                   modified at various times when ringing up and tendering or servicing
*                   a guest check.
*
*                   A copy of this data area is saved when a guest check is serviced
*                   and stored in the Guest Check File to provide the data needed when
*                   the guest check is retrieved.
*                   See function TrnUpdateGuestCheckDataFile() as well as function TrnGetGC()
*                   to see how System Type affects the data stored in the guest check file.
*
*                   The data area is typically cleared by function TrnInitialize()
*                   using the TRANI_GCFQUAL flag to indicate to clear the area such
*                   as in function ItemCommonTransOpen().
==========================================================================*/
// See https://stackoverflow.com/questions/4592762/difference-between-const-const-volatile
CONST volatile  TRANGCFQUAL  * CONST TranGCFQualPtr = &TrnInformation.TranGCFQual;

TRANGCFQUAL *TrnGetGCFQualPtr(VOID)
{
    return &TrnInformation.TranGCFQual;
}

VOID    TrnGetGCFQual(TRANGCFQUAL **TranGCFQual)
{
    *TranGCFQual =  &TrnInformation.TranGCFQual;
}

VOID    TrnPutGCFQual(CONST TRANGCFQUAL *pTranGCFQual)
{
   TrnInformation.TranGCFQual = *pTranGCFQual;
}

/*==========================================================================
*    Synopsis:  VOID    TrnGetCurQual( TRANCURQUAL *TranCurQual )
*    Synopsis:  VOID    TrnPutCurQual( TRANCURQUAL *TranCurQual )
*
*    Input:     TRANCURQUAL *TranCurQual
*    Output:    none
*    InOut:     TRANCURQUAL *TranCurQual
*
*    Return:    none 
*
*    Description:   get current qualifier
*    Description:   put current qualifier
*
*                   TrnInformation.TranCurQual is a memory resident data structure
*                   containing current transaction modifier information and is
*                   intended to be populated each time a transaction is started
*                   as well as each time a total key or tender key is pressed.
*                   A number of the data fields are used for receipt and/or kitchen
*                   printer status/control.
*
*                   A copy of this data area is not saved when a guest check is serviced
*                   and stored in the Guest Check File.
*                   A copy of this data area is saved with the totals data for total update.
*
*                   The data area is typically cleared by function TrnInitialize()
*                   using the TRANI_CURQUAL flag to indicate to clear the area such
*                   as in function ItemTendInit() or function ItemTotalInit().
==========================================================================*/
// See https://stackoverflow.com/questions/4592762/difference-between-const-const-volatile
CONST volatile TRANCURQUAL  * CONST TranCurQualPtr = &TrnInformation.TranCurQual;

TRANCURQUAL *TrnGetCurQualPtr(VOID)
{
	return &TrnInformation.TranCurQual;
}

VOID    TrnGetCurQual( TRANCURQUAL *pTranCurQual )
{
    *pTranCurQual = TrnInformation.TranCurQual;
}

VOID    TrnPutCurQual( CONST TRANCURQUAL *pTranCurQual )
{
    TrnInformation.TranCurQual = *pTranCurQual;
}

/*
*  Description: Clear the current qualifier's total status area to reset it.
*               This action is done in a number of places where the only write
*               access to the current qualifier needed is to clear the total status
*               so we provide this function to make it easy to do.
*
*               Get the current qualifier's total status area containing the total status
*               data from the last total key press.
*
*               Put or update the current qualifier's total status area with total status
*               data.  Typically used when processing a total key press.
*/
VOID   TranCurQualTotalStatusClear (VOID)
{
	memset (TrnInformation.TranCurQual.auchTotalStatus, 0, sizeof(TrnInformation.TranCurQual.auchTotalStatus));
}

VOID   TranCurQualTotalStatusGet (UCHAR  *auchTotalStatus)
{
	memcpy (auchTotalStatus, TrnInformation.TranCurQual.auchTotalStatus, sizeof(TrnInformation.TranCurQual.auchTotalStatus));
}

VOID   TranCurQualTotalStatusPut (CONST UCHAR  *auchTotalStatus)
{
	memcpy (TrnInformation.TranCurQual.auchTotalStatus, auchTotalStatus, sizeof(TrnInformation.TranCurQual.auchTotalStatus));
}

// check to see if a Total Key has been pressed and there is a Total Key Status to process.
BOOL   TranCurQualTotalStatusExist(VOID)
{
	return (TrnInformation.TranCurQual.auchTotalStatus[0] != 0);
}


/*==========================================================================
*    Synopsis:  VOID    TrnGetGCFQualSpl(TRANGCFQUAL **TranGCFQual, SHORT sType)
*
*    Input:     sType(0:Auto Selection,
*                   TRN_TYPE_SPLITA(4):Split A, TRN_TYPE_SPLITB(5):Split B)
*    Output:    none
*    InOut:     TRANGCFQUAL *TranGCFQual
*
*    Return:    none
*
*    Description:   get GCF qualifier
*    Description:   put GCF qualifier
==========================================================================*/
// See https://stackoverflow.com/questions/4592762/difference-between-const-const-volatile
CONST volatile TRANGCFQUAL * CONST TrnGCFQualSplPtrSplitA = &TrnSplitA.TranGCFQual;
CONST volatile TRANGCFQUAL * CONST TrnGCFQualSplPtrSplitB = &TrnSplitB.TranGCFQual;

TRANGCFQUAL *TrnGetGCFQualSplPointer(CONST SHORT sType)
{
	TRANGCFQUAL *pTranGCFQual = 0;

    /*----- Decide Target Split File Area -----*/
    if (sType == TRN_TYPE_SPLITA) {
        pTranGCFQual = &TrnSplitA.TranGCFQual;
    } else if (sType == TRN_TYPE_SPLITB) {
        pTranGCFQual = &TrnSplitB.TranGCFQual;
	} else if (sType == TRN_TYPE_SPLITNONE) {
		pTranGCFQual = &TrnInformation.TranGCFQual;
    }

	return pTranGCFQual;
}

VOID    TrnGetGCFQualSpl(TRANGCFQUAL **ppTranGCFQual, CONST SHORT sType)
{
	*ppTranGCFQual = TrnGetGCFQualSplPointer (sType);
}


/*==========================================================================
**    Synopsis: VOID    TrnGetTI(TRANITEMIZERS **pItem)
**    Synopsis: VOID    TrnPutTI(TRANITEMIZERS *pItem)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   get transaction itemizers                     R3.1
*    Description:   put transaction itemizers
==========================================================================*/
// See https://stackoverflow.com/questions/4592762/difference-between-const-const-volatile
CONST volatile TRANITEMIZERS * CONST TranItemizersPtr = &TrnInformation.TranItemizers;

TRANITEMIZERS  *TrnGetTIPtr (VOID)
{
	return &TrnInformation.TranItemizers;
}

VOID    TrnGetTI(TRANITEMIZERS **pItem)
{
    *pItem = &TrnInformation.TranItemizers;
}


VOID    TrnPutTI(CONST TRANITEMIZERS *pItem)
{
    TrnInformation.TranItemizers = *pItem;
}


/*==========================================================================
**    Synopsis: VOID    TrnGetTISpl(TRANITEMIZERS **pItem, SHORT sType)
*
*    Input:     sType(0:Auto Selection,
*                   TRN_TYPE_SPLITA(4):Split A, TRN_TYPE_SPLITB(5):Split B)
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   get transaction itemizers
==========================================================================*/
// See https://stackoverflow.com/questions/4592762/difference-between-const-const-volatile
CONST volatile TRANITEMIZERS * CONST TrnTISplPtrSplitA = &TrnSplitA.TranItemizers;
CONST volatile TRANITEMIZERS * CONST TrnTISplPtrSplitB = &TrnSplitB.TranItemizers;

TRANITEMIZERS * TrnGetTISplPointer(CONST SHORT sType)
{
	TRANITEMIZERS *pItem = 0;

    /*----- Decide Target Split File Area -----*/
    if (sType == TRN_TYPE_SPLITA) {
        pItem = &TrnSplitA.TranItemizers;
    } else if (sType == TRN_TYPE_SPLITB) {
        pItem = &TrnSplitB.TranItemizers;
	} else if (sType == TRN_TYPE_SPLITNONE) {
		pItem = &TrnInformation.TranItemizers;
	}

	return pItem;
}

VOID    TrnGetTISpl(TRANITEMIZERS **pItem, CONST SHORT sType)
{
	*pItem = TrnGetTISplPointer (sType);
}

/*==========================================================================
**    Synopsis: VOID    TrnGetInfSpl(TRANINFSPLIT *pSplit, SHORT sType)
*
*    Input:     sType(0:Auto Selection,
*                   TRN_TYPE_SPLITA(4):Split A, TRN_TYPE_SPLITB(5):Split B)
*    Output:    none
*    InOut:     
*    Return:    none    
*
*    Description:   get consoli storage
==========================================================================*/
// See https://stackoverflow.com/questions/4592762/difference-between-const-const-volatile
CONST volatile TRANINFSPLIT * CONST TrnInfSplPtrSplitA = &TrnSplitA;
CONST volatile TRANINFSPLIT * CONST TrnInfSplPtrSplitB = &TrnSplitB;

TRANINFSPLIT *TrnGetInfSpl(TrnStorageType  sStorageType)
{
	TRANINFSPLIT *pSplit = 0;

    /*----- Split A File -----*/
	switch (sStorageType) {
		case TRN_TYPE_SPLITA:
			pSplit = &TrnSplitA;
			break;

		case TRN_TYPE_SPLITB:
			pSplit = &TrnSplitB;
			break;

		default:
			NHPOS_ASSERT_TEXT((sStorageType == TRN_TYPE_SPLITA), "**ERROR: Invalid sType specified for TrnGetInfSpl()");
			break;
	}

	return pSplit;
}



/*==========================================================================
*    Synopsis:  VOID    TrnOtherDemand( VOID )    
*
*    Input:     none
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   print demand
==========================================================================*/

VOID    TrnOtherDemand( VOID )
{                                                           
    PrtPrintDemand( &TrnInformation );                      /* print demand */
}

VOID    TrnOtherSummaryReceipt ( ITEMTENDER *pWorkOther, int iPrintItems )
{
	PrtPrintSummaryReceipt ( &TrnInformation, pWorkOther, iPrintItems );    /* print demand */
}


/*==========================================================================
**  Synopsis:   SHORT   TrnOpen( VOID *ItemOpen )
*
*   Input:      VOID    *ItemOpen     
*   Output:     none
*   InOut:      none
*
*   Return:     TRN_SUCCESS
*               LDT_TAKETL_ADR
*               TRN_ERROR
*
*   Description:    operator,transaction open method
==========================================================================*/
SHORT    TrnOpen( VOID *ItemOpen )
{
    SHORT   sStatus = TRN_SUCCESS;
            
	switch( ITEMSTORAGENONPTR(ItemOpen)->uchMajorClass ) {
    case CLASS_ITEMOPEOPEN :                                        
        sStatus = TrnOpeOpen( ItemOpen );            /* operator open method */
        break;                               
                                                                        
    case CLASS_ITEMTRANSOPEN :
        sStatus = TrnTransOpen( ItemOpen );          /* transaction open method */
        break;                                                  
                                                                                                                
    default : 
        sStatus = ( TRN_ERROR );                     /* illegal major class */
    }                                                    

//	remove this PifLog for now in order to not clutter up the PIFLOG file.
//	PifLog (MODULE_TRANS_BODY, LOG_EVENT_TRN_OPEN_TRANSACTION);
	return( sStatus );                                                  /* return */
}
                                
                                                                    

/*==========================================================================
**   Synopsis:  VOID    TrnOpeOpen( ITEMOPEOPEN *ItemOpeOpen )    
*  
*    Input:     ITEMOPEOPEN     *ItemOpeOpen  
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:  operator open method
==========================================================================*/
SHORT   TrnOpeOpen( ITEMOPEOPEN *ItemOpeOpen )
{
    TrnQualOpn( ItemOpeOpen );                                /* transaction qualifier module */

    /* 12/05/01 */
    PifRequestSem(husTrnSemaphore);
    memset(&TrnItemStorePluMnem, 0, sizeof(TrnItemStorePluMnem));
    usItemStoredPluMnem = 0;
    PifReleaseSem(husTrnSemaphore);

    return( TRN_SUCCESS );
}



/*==========================================================================
**    Synopsis: SHORT   TrnTransOpen( ITEMTRANSOPEN *ItemTransOpen )    
*
*    Input:     ITEMTRANSOPEN   *ItemTransOpen
*    Output:    none
*    InOut:     none
*
*   Return:     TRN_SUCCESS
*               LDT_TAKETL_ADR
*               TRN_ERROR
*
*    Description:   trnsaction open method
==========================================================================*/
SHORT    TrnTransOpen( ITEMTRANSOPEN *ItemTransOpen )
{
    SHORT   sStatus;
    const SYSCONFIG    *pSysConfig = PifSysConfig();                    /* get system config */

	TrnInformation.ulCustomerSettingsFlags = pSysConfig->ulCustomerSettingsFlags;

	TrnQualTrans( ItemTransOpen );                              /* transaction qualifier module i/F */
    TrnItemTrans( ItemTransOpen );                          /* transaction itemizers module i/F */
    if (( sStatus =  TrnStoOpen( ItemTransOpen )) != TRN_SUCCESS ) {
        return( sStatus );                                          /* error */
    }
    TrnPrintType( ItemTransOpen );                         /* print */

	RflRehashEncryptDecrypt (TrnInformation.TranGCFQual.auchCheckHashKey);

    return( TRN_SUCCESS );
}



/*==========================================================================
**   Synopsis:  SHORT   TrnItem( VOID *ItemItem )    
*   
*    Input:     VOID    *ItemItem     
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*               TRN_ERROR
*
*    Description:   itemize method
==========================================================================*/

SHORT    TrnItem( VOID *ItemItem )
{
    SHORT   sStatus = TRN_SUCCESS;
                                                                                            
	switch( ITEMSTORAGENONPTR(ItemItem)->uchMajorClass ) {

    case CLASS_ITEMSALES :                                          
        sStatus = TrnSales( ItemItem );              /* sales method */
        break;                               
    
    case CLASS_ITEMDISC :
        sStatus = TrnDiscount( ItemItem );            /* discount method */
        break;

    case CLASS_ITEMCOUPON:
        sStatus = TrnCoupon( ItemItem );             /* combi. coupon */
        break;

    case CLASS_ITEMTOTAL :
        sStatus = TrnTotal( ItemItem );              /* total key method */
        break;                               
    
    case CLASS_ITEMTENDER :
        sStatus = TrnTender( ItemItem );            /* tender method */
        break;

    case CLASS_ITEMMISC :    
        sStatus = TrnMisc( ItemItem );                /* misc. transaction method */
        break;                               
    
    case CLASS_ITEMMODIFIER :                                   
        sStatus = TrnModifier( ItemItem );        /* modifier method */
        break;

    case CLASS_ITEMOTHER :    
        TrnOther( ItemItem );                        /* other method */
        break;                               
    
    case CLASS_ITEMAFFECTION :
        sStatus = TrnAffection( ItemItem );      /* affection method */
        break;

    case CLASS_ITEMPRINT :    
        sStatus = TrnPrint( ItemItem );              /* print method */
        break;                                                      
    
    case CLASS_ITEMMULTI :
        sStatus = TrnMulti( ItemItem );              /* multi check method */
        break;

    default : 
		NHPOS_ASSERT_TEXT(0, "==ERROR: Unknown uchMajorClass in TrnItem().");
        return( TRN_ERROR );                                        /* illegal major class */
    }                                                    

    return( sStatus );                                              /* return */
}



/*==========================================================================
*    Synopsis:  SHORT   TrnSales( ITEMSALES *ItemSales )    
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   sales method                            R3.1
==========================================================================*/
SHORT    TrnSalesAndDisc ( ITEMSALES *pItemSales, ITEMDISC *pItemDisc )
{
	SHORT  sRetStatus = TRN_SUCCESS;

	if (pItemSales) {
//		TrnQual((VOID *)pItemSales);
//		TrnItemizer((VOID *)pItemSales);
	}
	if (pItemDisc && sRetStatus == TRN_SUCCESS) {
		TrnQualItems(pItemDisc);
		TrnItemDisc(pItemDisc);

		/* --- Base Disc for "0" Seat,  V3.3 --- */
		if (pItemDisc->uchSeatNo == 'B') {
			pItemDisc->uchSeatNo = 0;
		}
	}

	if (sRetStatus == TRN_SUCCESS) TrnStoSalesAndDisc (pItemSales, pItemDisc);
	return sRetStatus;
}

SHORT   TrnSales(ITEMSALES *ItemSales)
{
    SHORT    sStatus;

    TrnQualItems(ItemSales);
    TrnItemSales(ItemSales);
    if ((sStatus = TrnStoSales(ItemSales)) != TRN_SUCCESS) {
        return(sStatus);
    }

    /* do not send reduced item, V3.3 */
    if ((ItemSales->fsPrintStatus & PRT_NO_UNBUFFING_PRINT) == 0) {
        TrnPrintType(ItemSales);
    }
    return(TRN_SUCCESS);
}



/*==========================================================================
*    Synopsis:  SHORT   TrnDiscount( ITEMDISC *ItemDisc )
*
*    Input:     ITEMDISC    *ItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   discount method                         R3.1
==========================================================================*/

SHORT   TrnDiscount(ITEMDISC *ItemDisc)
{
    SHORT    sStatus;

    TrnQualItems(ItemDisc);
    TrnItemDisc(ItemDisc);

    /* --- Base Disc for "0" Seat,  V3.3 --- */
    if (ItemDisc->uchSeatNo == 'B') {
        ItemDisc->uchSeatNo = 0;
    }
    
    if ((sStatus = TrnStoDisc(ItemDisc)) != TRN_SUCCESS) {
        return(sStatus);
    }

	// for charge tips we want to save the last version into our memory resident area.
    switch(ItemDisc->uchMinorClass) {
		case CLASS_CHARGETIP:
		case CLASS_CHARGETIP2:
		case CLASS_CHARGETIP3:
		case CLASS_AUTOCHARGETIP:
		case CLASS_AUTOCHARGETIP2:
		case CLASS_AUTOCHARGETIP3:
			TrnInformation.TranChargeTips = *ItemDisc;
			break;
		default:
			break;
	}
    /* for auto charge tips, V3.3 */
    if ((ItemDisc->fsPrintStatus & PRT_NO_UNBUFFING_PRINT) == 0) {
        TrnPrintType(ItemDisc);
    }
    return(TRN_SUCCESS);
}



/*==========================================================================
*    Synopsis:  SHORT   TrnCoupon( ITEMCOUPON *pItemCoupon )
*
*    Input:     ITEMCOUPON  *pItemCoupon
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   coupon method                           R3.1
==========================================================================*/

SHORT   TrnCoupon(ITEMCOUPON *pItemCoupon)
{
    SHORT    sStatus;

    TrnQualItems(pItemCoupon);

    /* --- update transaction itemizers & item counters --- */
    TrnItemCoupon( pItemCoupon );

    /* --- store coupon item to item & consoli. storage --- */
    if (( sStatus = TrnStoCoupon( pItemCoupon )) != TRN_SUCCESS ) {
        return ( sStatus );
    }

    /* --- print out coupon item --- */
    TrnPrintType( pItemCoupon );

    return ( TRN_SUCCESS );
}



/*==========================================================================
*    Synopsis:  SHORT TrnCouponSearch( ITEMCOUPONSEARCH *pItemCouponSearch,
*                                      SHORT             sStorageType )
*
*    Input:     ITEMCOUPONSEARCH *pItemCouponSearch
*               SHORT             sStorageType
*                       TRN_TYPE_ITEMSTORAGE, TRN_TYPE_CONSSTORAGE
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS   if search is a success
*               TRN_ERROR     if not a coupon, not found, etc.
*
*    Description:   search for target sales item in item/consoli. storage
==========================================================================*/

SHORT   TrnCouponSearch(ITEMCOUPONSEARCH *pItemCouponSearch, SHORT sStorageType)
{
    SHORT   sStatus = TRN_ERROR;    // TrnCouponSearch() if not a coupon or right kind of coupon, error

    if (pItemCouponSearch->uchMajorClass == CLASS_ITEMCOUPON) {
        switch ( pItemCouponSearch->uchMinorClass ) {
        case CLASS_COMCOUPON:
        case CLASS_UPCCOUPON:   /* 2172 saratoga */
            if ( pItemCouponSearch->fbModifier & VOID_MODIFIER ) {
                /* --- delete specified coupon that is already entered --- */
                /* --- if coupon is not found, it returns error --- */
                sStatus = TrnCouponVoidSearch( pItemCouponSearch, sStorageType );
            } else {
                /* --- determine coupon item is able to enter or not --- */
                /* --- if target sales item is not found, return error --- */
                sStatus = TrnStoCouponSearch( pItemCouponSearch, sStorageType );
            }
            break;
        }
    }
    return ( sStatus );
}



/*==========================================================================
*    Synopsis:  SHORT TrnCouponVoidSearch( ITEMCOUPON *pItemCoupon, SHORT sStorageType )
*
*    Input:     ITEMCOUPON  *pItemCoupon
*               SHORT        sStorageType
*                       TRN_TYPE_ITEMSTORAGE, TRN_TYPE_CONSSTORAGE
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:
*       search for target sales item in item/consoli. storage to void item
==========================================================================*/

SHORT TrnCouponVoidSearch( ITEMCOUPONSEARCH *pItemCouponSearch,
                           TrnStorageType    sStorageType )
{
    ULONG       ulTargetOffset;
	ITEMCOUPON  ItemCoupon = {0};
    SHORT       sSearchStatus;

    /* --- search for specified coupon item in transaction storage --- */
    ItemCoupon.uchMajorClass = pItemCouponSearch->uchMajorClass;
    ItemCoupon.uchMinorClass = pItemCouponSearch->uchMinorClass;
    ItemCoupon.uchCouponNo   = pItemCouponSearch->uchCouponNo;
    _tcsncpy(ItemCoupon.auchUPCCouponNo, pItemCouponSearch->auchUPCCouponNo, NUM_PLU_LEN);
    ItemCoupon.fbModifier    = ( pItemCouponSearch->fbModifier & ~( VOID_MODIFIER ));
    _tcsncpy( ItemCoupon.aszNumber, pItemCouponSearch->aszNumber, NUM_NUMBER );

    sSearchStatus = TrnStoCpnVoidSearch( &ItemCoupon, &ulTargetOffset, sStorageType );

    /* saratoga */
    if (pItemCouponSearch->uchMinorClass == CLASS_UPCCOUPON) {
        pItemCouponSearch->uchNoOfItem = ItemCoupon.uchNoOfItem;
        pItemCouponSearch->auchStatus[0] = ItemCoupon.fbStatus[0];
        pItemCouponSearch->auchStatus[1] = ItemCoupon.fbStatus[1];
        pItemCouponSearch->lAmount     = RflLLabs(ItemCoupon.lAmount);
        _tcsncpy(pItemCouponSearch->aSalesItem[0].auchPLUNo, ItemCoupon.auchPLUNo, NUM_PLU_LEN);
        pItemCouponSearch->aSalesItem[0].usDeptNo = ItemCoupon.usDeptNo;
        pItemCouponSearch->aSalesItem[0].uchAdjective = ItemCoupon.uchAdjective;
        pItemCouponSearch->fbModifier |= ItemCoupon.fbModifier;
    }
    return ( sSearchStatus );
}

/*==========================================================================
*    Synopsis:  SHORT TrnSingleCouponSearch( ITEMCOUPONSEARCH *pItemCouponSearch,
*                                            SHORT             sStorageType )
*
*    Input:     ITEMCOUPONSEARCH *pItemCouponSearch
*               SHORT             sStorageType
*                       TRN_TYPE_ITEMSTORAGE, TRN_TYPE_CONSSTORAGE
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS   if search is a success
*               TRN_ERROR     if not a coupon, not found, etc.
*
*    Description:   search for single coupon has been alreay done
==========================================================================*/

SHORT   TrnSingleCouponSearch(ITEMCOUPONSEARCH *pItemCouponSearch, SHORT sStorageType)
{
    SHORT   sStatus;

    if (((TrnInformation.TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL) ||
        ((TrnInformation.TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK)) {

        if ((TrnInformation.TranCurQual.fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_CASHIER) {

            if (((TrnInformation.TranGCFQual.fsGCFStatus & GCFQUAL_PVOID) == 0) &&
                ((TrnInformation.TranCurQual.fsCurStatus & CURQUAL_PVOID))) {

                /* prohibit if GCF status is normal, and Current status is transaction void */
                return (TRN_ERROR);
            }
        }
    }

    sStatus = TRN_ERROR;    // TrnSingleCouponSearch() if not a coupon or right kind of coupon, error
    if (pItemCouponSearch->uchMajorClass == CLASS_ITEMCOUPON) {
        switch ( pItemCouponSearch->uchMinorClass ) {
        case CLASS_COMCOUPON:
            if ( pItemCouponSearch->fbModifier & VOID_MODIFIER ) {
                /* --- delete specified coupon that is already entered --- */
                /* --- if coupon is not found, it returns error --- */
                sStatus = TrnCouponVoidSearch( pItemCouponSearch, sStorageType );
            } else {
                /* --- determine coupon item is able to enter or not --- */
                /* --- if target sales item is not found, return error --- */
                sStatus = TrnStoQueryCpnType( pItemCouponSearch );
            }
            break;
        }
    }
    return ( sStatus );
}



/*==========================================================================
*    Synopsis:  SHORT   TrnTotal(ITEMTOTAL *ItemTotal)
*
*    Input:     ITEMTOTAL   *ItemTotal
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   total key method                           R3.1
==========================================================================*/

SHORT    TrnTotal(ITEMTOTAL *ItemTotal)
{

	// This source code eliminated as part of changes for JIRA AMT-2853.
	// There are a number of places where there was a check on the flags for
	// a CURQUAL_STORE_RECALL type system along with processing a TENDER_PARTIAL in which the
	// total calculation depended on the system type and if we were doing a split or partial tender.
	// We started with changes in functions TrnItemTotal() and ItemTotalCTAmount() in order
	// to reduce the complexity of the source code and dependencies on flags as much as possible
	// as part of making source code changes for AMT-2853.
	//    Richard Chambers for Amtrak Rel 2.2.1 project June 25, 2013
	if (ItemTotal->uchMinorClass != 0) {
		// in some cases this function may be called when the ITEMTOTAL is
		// a placeholder so that the uchMinorClass is zero and the other data
		// is zero or junk.  There is no reason for us to handle and store such
		// a placeholder ITEMTOTAL since it is basically ignored by receipt printing.
		TrnQualTotal(ItemTotal);

		/* --- save current total amount to Main Itemizer --- */
		TrnItemTotal(ItemTotal);

		if (ItemTotal->uchMinorClass != CLASS_SPLIT) {
			SHORT    sStatus;

			if ((sStatus = TrnStoTotal(ItemTotal)) != TRN_SUCCESS) {
				return(sStatus);
			}
		}

		TrnPrintType(ItemTotal);
	}
    return(TRN_SUCCESS );
}

/*==========================================================================
**   Synopsis:  SHORT   TrnTender( ITEMTENDER *ItemTender )    
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

SHORT    TrnTender( ITEMTENDER *ItemTender )
{
    SHORT    sStatus;

    if (( sStatus =  TrnStoTender( ItemTender )) != TRN_SUCCESS ) {    
        return( sStatus );                                          /* error */
    }
    TrnPrintType( ItemTender );                            /* print */
    return( TRN_SUCCESS );
}



/*==========================================================================
**   Synopsis:  SHORT   TrnMisc( ITEMMISC *ItemMisc )    
*
*    Input:     ITEMMISC    *ItemMisc
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   misc. transaction method
==========================================================================*/

SHORT    TrnMisc(ITEMMISC *ItemMisc)
{
    SHORT   sStatus;

    /* --- update transaction itemizers & item counters,    Saratoga --- */
    TrnItemMisc(ItemMisc);
    if ((sStatus = TrnStoMisc(ItemMisc)) != TRN_SUCCESS ) {
        return(sStatus);
    }
    if (ItemMisc->uchMinorClass == CLASS_MONEY) {
        TrnPrintTypeNoDsp(ItemMisc);
    } else {
        TrnPrintType(ItemMisc);
    }
    return(TRN_SUCCESS);
}

/*==========================================================================
**   Synopsis:  SHORT   TrnModifier( ITEMMODIFIER *ItemModifier )    
*
*    Input:     ITEMMODIFIER    *ItemModifier
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   modifier method                         R3.1
==========================================================================*/

SHORT    TrnModifier( ITEMMODIFIER *ItemModifier )
{
    TrnQualModifier(ItemModifier);
    TrnPrintType(ItemModifier);
    return(TRN_SUCCESS);
}

/*==========================================================================
*    Synopsis:  SHORT   TrnOther( ITEMOTHER *ItemOther )    
*
*    Input:     ITEMOTHER   *ItemOther
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   other method                            R3.1
==========================================================================*/

VOID    TrnOther(ITEMOTHER *ItemOther)
{
    switch(ItemOther->uchMinorClass) {
    case CLASS_PRINTDEMAND:
        TrnOtherDemand();
        break;

    case CLASS_POSTRECEIPT:
    case CLASS_PARKING:
        TrnStoOther(ItemOther);
        break;

    case CLASS_PREINQ_EPT:              /* Saratoga */
        TrnQualOther(ItemOther);
        break;

    default:
        TrnOtherPrint(ItemOther);
        break;
    }
}



/*==========================================================================
*    Synopsis:  VOID    TrnOtherPrint( ITEMOTHER *ItemOther )    
*
*    Input:     ITEMOTHER   *ItemOther
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   other method(except for print demand)
==========================================================================*/

VOID    TrnOtherPrint( ITEMOTHER *ItemOther )
{
    if ( ItemOther->uchMinorClass == CLASS_INSIDEFEED ) {             
        if ( ( ItemOther->fsPrintStatus & PRT_SLIP ) != 0 ) {           /* slip feed */
            if ( ItemOther->lAmount != 0L ) {
                TrnInformation.TranGCFQual.uchSlipLine = ( UCHAR )ItemOther->lAmount;
            } else {
                ItemOther->lAmount = ( LONG )TrnInformation.TranGCFQual.uchSlipLine;
            }               
        }
		PrtPrintItem( &TrnInformation, ItemOther ); /* 1 item print     */
    } else if ((ItemOther->uchMinorClass == CLASS_POSTRECEIPT)  ||
              (ItemOther->uchMinorClass == CLASS_POSTRECPREBUF) ||
              (ItemOther->uchMinorClass == CLASS_CP_EPT_DUPLI)  ||
              (ItemOther->uchMinorClass == CLASS_CP_EPT_FULLDUP)){     /* 5/30/96 */
		UCHAR   fbNoPrint;
		ULONG   flPrintStatus;	/* 09/11/01 */

        TrnInformation.TranCurQual.fbNoPrint &= ~CURQUAL_NO_R;   /* reset no receipt status  */
        fbNoPrint = TrnInformation.TranCurQual.fbNoPrint;        /* save no print status     */
        TrnInformation.TranCurQual.fbNoPrint |= CURQUAL_NO_J | CURQUAL_NO_V | CURQUAL_NO_S;    /* set no print status      */
        if ((ItemOther->uchMinorClass == CLASS_POSTRECEIPT) ||
           (ItemOther->uchMinorClass == CLASS_CP_EPT_FULLDUP)) {            /* 5/30/96 */

            /* corrected duplicate receit storage handle at 06/12/96 */
            if ((TrnInformation.TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL
                || ((TrnInformation.TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
                    && (TrnInformation.TranCurQual.fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_CASHIER)) {

                TrnInformation.TranCurQual.uchPrintStorage = PRT_CONSOLSTORAGE;

            } else {

                TrnInformation.TranCurQual.uchPrintStorage = PRT_ITEMSTORAGE; /* precheck duplicate receipt */
            }
        } else if (ItemOther->uchMinorClass == CLASS_CP_EPT_DUPLI) {
            TrnInformation.TranCurQual.uchPrintStorage = PRT_CONSOL_CP_EPT;
        } else {
            TrnInformation.TranCurQual.uchPrintStorage = PRT_ITEMSTORAGE;
        }

		flPrintStatus = TrnInformation.TranCurQual.flPrintStatus;
		TrnInformation.TranCurQual.flPrintStatus |= CURQUAL_POSTRECT;	/* do not print duplicate receipt data on journal, 09/11/01 */
		
        PrtPrintTran(&TrnInformation);                        /* transaction print        */
        TrnInformation.TranCurQual.fbNoPrint = fbNoPrint;     /* restore no print status  */
		TrnInformation.TranCurQual.flPrintStatus = flPrintStatus;;
	} else if (ItemOther->uchMinorClass == CLASS_RESETLOG) {
		TrnInformation.TranCurQual.usConsNo = MaintCurrentSpcCo(SPCO_CONSEC_ADR);
		PrtPrintItem( &TrnInformation, ItemOther ); /* 1 item print     */
	} else {
		PrtPrintItem( &TrnInformation, ItemOther ); /* 1 item print     */
    }
}



/*==========================================================================
*    Synopsis:  SHORT   TrnAffection( ITEMAFFECTION *ItemAffection )
*
*    Input:     ITEMAFFECTION   *ItemAffection
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   affection method
==========================================================================*/

SHORT    TrnAffection( ITEMAFFECTION *ItemAffection )
{
    SHORT    sStatus;

    TrnItemAC( ItemAffection );                          /* transaction itemizers module i/F */
    if (( sStatus =  TrnStoAffect( ItemAffection )) != TRN_SUCCESS ) {
        return( sStatus );                                          /* error */
    }                                                                 
    TrnPrintType( ItemAffection );                         /* print */
    return( TRN_SUCCESS );
}



/*==========================================================================
*    Synopsis:  SHORT   TrnPrint( ITEMPRINT *ItemPrint )    
*
*    Input:     ITEMPRINT   *ItemPrint
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   print method
==========================================================================*/

SHORT    TrnPrint( ITEMPRINT *ItemPrint )
{
    SHORT    sStatus;

    if ( TrnInformation.TranCurQual.fsCurStatus & CURQUAL_TRAY ) {  /* tray transaction */
        if ( ItemPrint->uchMinorClass == CLASS_START ) {            /* print start */
            return( TRN_SUCCESS );
        }
    }

    if (( sStatus =  TrnStoPrint( ItemPrint )) != TRN_SUCCESS ) {
        return( sStatus );                                          /* error */
    }               
    if (ItemPrint->uchMinorClass == CLASS_TRAILER_MONEY) {
        TrnPrintTypeNoDsp(ItemPrint);
    } else {
        TrnPrintType(ItemPrint);
    }

    return( TRN_SUCCESS );
}



/*==========================================================================
*    Synopsis:  SHORT   TrnMulti( ITEMMULTI *ItemMulti )    
*
*    Input:     ITEMMULTI   *ItemMulti
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*
*    Description:   multi check payment print method
==========================================================================*/
         
SHORT    TrnMulti( ITEMMULTI *ItemMulti )
{
    SHORT    sStatus;

    if (( sStatus =  TrnStoMulti( ItemMulti )) != TRN_SUCCESS ) {
        return( sStatus );                                          /* error */
    }                                                               
    TrnPrintType( ItemMulti );                             /* print */
    return( TRN_SUCCESS );
}



/*==========================================================================
*    Synopsis:  SHORT   TrnClose( VOID *ItemClose )
*
*    Input:     VOID    *ItemClose
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   operator, transaction close method
==========================================================================*/

SHORT    TrnClose(VOID *ItemClose)
{
	switch( ITEMSTORAGENONPTR(ItemClose)->uchMajorClass) {
    case CLASS_ITEMOPECLOSE:
        TrnOpeClose(ItemClose);
        break;

    case CLASS_ITEMTRANSCLOSE:
        TrnTransClose(ItemClose);
        break;

    default:
        return(TRN_ERROR);
    }

    return(TRN_SUCCESS);
}
    

                                                                
/*==========================================================================
*    Synopsis:  VOID    TrnOpeClose( ITEMOPECLOSE *ItemOpeClose )
*
*    Input:     ITEMOPECLOSE    *ItemOpeClose
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               XXXX               
*
*    Description:   operator close method
==========================================================================*/

VOID    TrnOpeClose( ITEMOPECLOSE *ItemOpeClose )
{                                                       
    TrnQualCls( ItemOpeClose );                           /* transaction qualifier module */
    TrnStoOpeClose( ItemOpeClose );                        /* transaction storage module */
}

/*==========================================================================
*    Synopsis:  VOID    TrnTransClose( ITEMTRANSCLOSE *ItemTransClose )
*
*    Input:     ITEMTRANSCLOSE  *ItemTransClose
*    Output:    none
*    InOut:     none
*
*    Return:    none    
*
*    Description:   transaction close method                   R3.1
==========================================================================*/
        
VOID    TrnTransClose(ITEMTRANSCLOSE *ItemTransClose)
{ 
    UCHAR       uchKitchen;
    UCHAR       fbCompPrint;
    UCHAR       fbNoPrint;

    switch(ItemTransClose->uchMinorClass) {
    case CLASS_CLSSEATTRANS:
        TrnSplPrintSeat(TrnInformation.TranCurQual.uchSeat, TRN_SPL_UPDATE, 0, 0, TrnInformation.TranCurQual.auchTotalStatus, TRN_PRT_FULL);
        TrnInformation.TranCurQual.uchSeat = 0;
        TrnInformation.TranCurQual.auchTotalStatus[0] = 0;
        TrnInformation.TranCurQual.uchSplit = 0;
        TrnInformation.fsTransStatus &= ~(TRN_STATUS_SEAT_TRANS | TRN_STATUS_MULTI_TRANS | TRN_STATUS_TYPE2_TRANS);
        break;

    case CLASS_CLSSEATTRANSDUP1:         /* 6/9/96 for CP Duplicate Rec. */
    case CLASS_CLSSEATTRANSDUP2:         
        TrnSplPrintSeat(TrnInformation.TranCurQual.uchSeat, TRN_SPL_UPDATE, 0, 0, TrnInformation.TranCurQual.auchTotalStatus, TRN_PRT_FULL);

        fbCompPrint = TrnInformation.TranCurQual.fbCompPrint;
        TrnInformation.TranCurQual.fbCompPrint |= CURQUAL_COMP_R;
        fbNoPrint = TrnInformation.TranCurQual.fbNoPrint;
        TrnInformation.TranCurQual.fbNoPrint |= (CURQUAL_NO_J | CURQUAL_NO_V | CURQUAL_NO_S);
        uchKitchen = TrnInformation.TranCurQual.uchKitchenStorage;
        TrnInformation.TranCurQual.uchKitchenStorage = 0;
        if (ItemTransClose->uchMinorClass == CLASS_CLSSEATTRANSDUP1) {
            TrnSplPrintSeat(TrnInformation.TranCurQual.uchSeat, TRN_SPL_UPDATE, 0, 0, TrnInformation.TranCurQual.auchTotalStatus, TRN_PRT_FULL);
        } else {
            TrnSplPrintSeat(TrnInformation.TranCurQual.uchSeat, TRN_SPL_UPDATE, 0, 0, TrnInformation.TranCurQual.auchTotalStatus, TRN_PRT_TTL);
        }
        TrnInformation.TranCurQual.fbCompPrint = fbCompPrint;
        TrnInformation.TranCurQual.fbNoPrint = fbNoPrint;
        TrnInformation.TranCurQual.uchKitchenStorage = uchKitchen;
        TrnInformation.TranCurQual.uchSeat = 0;
        TrnInformation.TranCurQual.auchTotalStatus[0] = 0;        // clear the total key type
        TrnInformation.TranCurQual.uchSplit = 0;
        TrnInformation.fsTransStatus &= ~(TRN_STATUS_SEAT_TRANS | TRN_STATUS_MULTI_TRANS | TRN_STATUS_TYPE2_TRANS);
        break;

    case CLASS_CLSSPLITSERV:
        PrtPrintSplit(&TrnInformation, &TrnSplitA, 0, 0);
        TrnInformation.TranCurQual.uchSeat = 0;
        TrnInformation.TranCurQual.auchTotalStatus[0] = 0;         // clear the total key type
        TrnInformation.TranCurQual.uchSplit = 0;
        break;

    case CLASS_CLSPRINT:
        PrtPrintTran(&TrnInformation);      /* for split check kitchen print */
        break;

    case CLASS_CLSMONEY:                    /* Saratoga */
        TrnTransCloseMoney();
        TrnTransPrint();
        break;

    case CLASS_CLS_LOAN:                    /* Saratoga */
    case CLASS_CLS_PICK:
        TrnTransCloseLoanPickup(ItemTransClose);    /* 09/08/00 */
        break;

	case CLASS_CLS_GIFT_RECEIPT:   //SR 525, Gift Receipt functionality JHHJ 9-01-05
		TrnInformation.TranCurQual.fsCurStatus2 |= PRT_GIFT_RECEIPT;
		TrnTransPrint();
		break;

	case CLASS_CLSPOSTRECPT:
		TrnInformation.TranCurQual.fsCurStatus2 |= PRT_POST_RECEIPT;
		TrnTransPrint();
		break;

	case CLASS_CLS_PREAUTH_RECEIPT:
		TrnInformation.TranCurQual.fsCurStatus2 |= PRT_PREAUTH_RECEIPT;
		TrnTransPrint();
		break;

    default:                                                       
        TrnTransPrint();
        break;
    }

    /* --- WIC not stay down,   Saratoga --- */
    if ((TrnInformation.TranModeQual.fsModeStatus & MODEQUAL_WIC) == 0) {
        TrnInformation.TranCurQual.fsCurStatus &= ~CURQUAL_WIC;
    }

	ItemCasAutoOut();

}

/*==========================================================================
*    Synopsis:  VOID    TrnTransPrint( ITEMTRANSCLOSE *ItemTransClose )    
*
*    Input:     ITEMTRANSCLOSE  *ItemTransClose
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   print at transaction close
==========================================================================*/

VOID    TrnTransPrint()
{
    SHORT   sLine = PrtPrintTran(&TrnInformation);

    if (sLine != 0) {
        TrnInformation.TranGCFQual.uchPageNo = (UCHAR)(sLine / PRT_SLIP_MAX_LINES);
        TrnInformation.TranGCFQual.uchSlipLine = (UCHAR)(sLine % PRT_SLIP_MAX_LINES);
    }
}

/*
*==========================================================================
*    Synopsis:  VOID    TrnTransCloseMoney(VOID)
*
*    Input:     none
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   Generate Money Affection Data.              Saratoga
*==========================================================================
*/
VOID    TrnTransCloseMoney(VOID)
{
    USHORT      i;
	ITEMMISC    TrnMisc = {0};

    TrnMisc.uchMajorClass = CLASS_ITEMMISC;
    TrnMisc.uchMinorClass = CLASS_MONEY;

    for (i = 0; i < STD_TENDER_MAX; i++) {
        TrnMisc.uchTendMinor = CLASS_TENDER1 + i;
        TrnMisc.lAmount = TrnInformation.TranItemizers.lTenderMoney[i];
        TrnStoMisc(&TrnMisc);
    }
    for (i = 0; i < STD_NO_FOREIGN_TTL; i++) {
        TrnMisc.uchTendMinor = CLASS_FOREIGN1 + i;
        TrnMisc.lAmount = TrnInformation.TranItemizers.lFCMoney[i];
        TrnStoMisc(&TrnMisc);
    }
}

/*==========================================================================
*    Synopsis:  VOID    TrnModeIn( ITEMREGMODEIN *ItemRegModeIn )
*
*    Input:     ITEMREGMODEIN   *ItemRegModeIn
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   mode-in method                          R3.1
==========================================================================*/

VOID    TrnModeIn( ITEMREGMODEIN *ItemRegModeIn)
{
    if ( ItemRegModeIn->uchMajorClass == CLASS_ITEMMODEIN ) {
		TrnQualClsCashier();         // clear working data
        PrtPrintItem( &TrnInformation, ItemRegModeIn );
    }
}



/*==========================================================================
*    Synopsis:  VOID    TrnThrough( VOID *TrnItem )    
*
*    Input:     VOID    *TrnItem
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   through method for print(validation, ticket)
==========================================================================*/

VOID    TrnThrough( VOID *TrnItem )
{                                                           
    PrtPrintItem( &TrnInformation, TrnItem );             /* 1 item print */
}                                                                   

VOID    TrnSlipRelease (VOID)
{
	ITEMOTHER TrnOther = {0};

    /* release slip to prevent the optional listing slip */
	TrnOther.uchMajorClass = CLASS_ITEMOTHER;
    TrnOther.uchMinorClass = CLASS_SLIPRELEASE;
	TrnOther.lAmount = 0;
	TrnOther.fsPrintStatus = PRT_SLIP; /* set slip bit on other data */
    TrnThrough( &TrnOther );
}


/*==========================================================================
*    Synopsis:  VOID    TrnThroughDisp( VOID *pItem )
*    Input:     VOID    *pItem
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   through method for display on the flay R3.1
==========================================================================*/

VOID    TrnThroughDisp( VOID *pItem )
{
    /* check display on the fly system */
    if (CliParaMDCCheck(MDC_DSP_ON_FLY3, ODD_MDC_BIT0) == 0) { /* RS232C system, 2172 */
		PARAMDC ParaMDCInfo = {0};
        
        ParaMDCInfo.uchMajorClass = CLASS_PARAMDC;
        ParaMDCInfo.usAddress = MDC_DSP_ON_FLY;
        CliParaRead(&ParaMDCInfo);
        if (ParaMDCInfo.uchMDCData ) {                  /* R3.1 (for 8 K/P) */
            PrtDispItem(&TrnInformation, pItem);    /* call display on the fly manager */
        }
    }
}

/*
*===========================================================================
** Synopsis:    SHORT TrnLoanPickup(VOID *pData)
*     Input:    pData - pointer of loan/pick up data class
*
** Return:      TRN_SUCCESS - successful
*
** Description: This function is called from the supervisor mode.
*               This function calls the following function to do in the
*               loan/pick-up data classes.
*                   - memory affection
*                   - print module
*===========================================================================
*/
SHORT   TrnLoanPickup(VOID *pData)
{
    SHORT           sStatus, sLine;

    switch ( ITEMSTORAGENONPTR(pData)->uchMajorClass ) {
    case CLASS_MAINTLOAN:
    case CLASS_MAINTPICKUP:
        if ((sLine = PrtPrintItem(&TrnInformation, pData)) != 0) {
            TrnInformation.TranGCFQual.uchPageNo = (UCHAR)(sLine / PRT_SLIP_MAX_LINES);    /* page No */
            TrnInformation.TranGCFQual.uchSlipLine = (UCHAR)(sLine % PRT_SLIP_MAX_LINES);  /* line No */
        }
        /* check display on the fly system */
        TrnThroughDisp (pData);
        break;

    case CLASS_ITEMAFFECTION:
        if ((sStatus = TrnStoAffect(pData)) != TRN_SUCCESS) {
            return(sStatus);
        }
        break;
    }

    return(TRN_SUCCESS);
}


/*==========================================================================
*    Synopsis:  SHORT   TrnGetGC( USHORT usGCFNo, SHORT sType, USHORT usType )
*
*    Input:     ULONG   ulGCFNo
*               SHORT   sType
*               USHORT  usType
*    Output:    none
*    InOut:     none
*
*    Return:    xxxx
*
*    Description:   get target GCF.  We do a retrieve of the guest check
*                   information and then depending on the type of system
*                   this terminal is provisioned as, we will process
*                   the guest check file that was transferred.
*
*                   The file may contain just the guest check information
*                   as a consolidated check.  If so, then the guest check
*                   file we have received will contain only the guest check
*                   information of the TRANGCFQUAL and TRANITEMIZERS structs.
*
*                   However, if we are a FLEX_STORE_RECALL or FLEX_POST_CHK type
*                   of system then the actual transaction file index is also available.
*                   So we also read that information and reconstruct the index
*                   file for this transaction as well.
==========================================================================*/

SHORT TrnGetGC(USHORT usGCFNo, SHORT sType, USHORT usType)
{
    ULONG       ulTtlIdxReadSize;
    ULONG       ulCurIdxReadSize;
    ULONG       ulReadOffset;
	ULONG		ulActualBytesRead = 0;
    SHORT       sStatus;
    UCHAR       auchIdxWork[ sizeof( PRTIDX ) * 80 ];
    PRTIDXHDR   IdxFileInfo;

    sStatus = CliGusReadLockFH(usGCFNo,                 /* get target GCF */
                               TrnInformation.hsTranConsStorage,
                               sizeof(USHORT) + sizeof(USHORT),
                               TrnInformation.usTranConsStoFSize,
                               usType, &ulActualBytesRead);

	// if there was an error reading the check then we
	// will just return an error status.  If the check
	// exists then we will process the temporary transaction file.
	if (sStatus == 0) {
		// if the guest check file size is zero then the guest check
		// exists but it is empty.  Reasons for this can basically be
		// combined into a single reason, the terminal from which a
		// request to create and open a new guest check experienced some
		// kind of failure before the guest check was update.  This resulted
		// in a guest check that is locked and is empty.
		// We will fix up this empty guest check so that it can be retrieved
		// and reused in order to prevent empty guest checks filling up the
		// guest check file and causing a file full condition.
		memset (&TrnInformation.TranItemizers, 0, sizeof(TrnInformation.TranItemizers));
		memset (&TrnInformation.TranChargeTips, 0, sizeof(TrnInformation.TranChargeTips));
		TrnInformation.usGuestNo[0] = usGCFNo;
		TrnInformation.TranGCFQual.usGuestNo = usGCFNo;
	} else if (sStatus > 0) {
		ULONG   ulSize = 0;
        /* set GCF qual to TrnInformation */

		ulSize = sizeof(USHORT) + sizeof(USHORT);
		//11-1103- SR 201 JHHJ
		TrnReadFile_MemoryForce (ulSize, &(TrnInformation.TranGCFQual), sizeof(TRANGCFQUAL), TrnInformation.hsTranConsStorage, &ulActualBytesRead);
#if 0
        TrnReadFile(ulSize, &(TrnInformation.TranGCFQual), sizeof(TRANGCFQUAL), TrnInformation.hsTranConsStorage, &ulActualBytesRead);
#endif

		// We need to update the Hourly Total Block Offset for the Guest Check so that additional items purchased will
		// be in the current Hourly Total Block and not the previous one when the Guest Check was first created.
		TrnInformation.TranGCFQual.uchHourlyOffset = (UCHAR)ItemTendGetOffset();       /* set the hourly total block offset for the guest check */

		ulSize += sizeof(TRANGCFQUAL);

        /* set tranitemizers to TrnInformation */

		//11-1103- SR 201 JHHJ
		TrnReadFile_MemoryForce (ulSize, &(TrnInformation.TranItemizers), sizeof(TRANITEMIZERS), TrnInformation.hsTranConsStorage, &ulActualBytesRead);
#if 0
        TrnReadFile(ulSize, &(TrnInformation.TranItemizers), sizeof(TRANITEMIZERS), TrnInformation.hsTranConsStorage, &ulActualBytesRead);
#endif
		ulSize += sizeof(TRANITEMIZERS);

        if (sType == 1) {
            /* set consoli. vli to TrnInformation */

			//11-1103- SR 201 JHHJ
			TrnReadFile_MemoryForce (ulSize, &(TrnInformation.usTranConsStoVli), sizeof(TrnInformation.usTranConsStoVli),
                        TrnInformation.hsTranConsStorage, &ulActualBytesRead);
#if 0
            TrnReadFile(ulSize, &(TrnInformation.usTranConsStoVli), sizeof(TrnInformation.usTranConsStoVli),
                        TrnInformation.hsTranConsStorage, &ulActualBytesRead);
#endif
			ulSize += sizeof(TrnInformation.usTranConsStoVli);
			ulSize += TrnInformation.usTranConsStoVli;

			// If there is index information (TrnInformation.usTranConsStoVli is non-zero)
			// then lets read it in as well.
            if ( 0 < TrnInformation.usTranConsStoVli ) {

                /* --- get item index file information, and save it to
                    index file --- */

				//11-1103- SR 201 JHHJ
				TrnReadFile_MemoryForce (ulSize, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranConsStorage, &ulActualBytesRead); /* ### Mod (2171 for Win32) V1.0 */
#if 0
                TrnReadFile(ulSize, &IdxFileInfo, sizeof( PRTIDXHDR ),
                             TrnInformation.hsTranConsStorage, &ulActualBytesRead); /* ### Mod (2171 for Win32) V1.0 */
#endif
				ulSize += sizeof(PRTIDXHDR);

                TrnWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranConsIndex );
                TrnWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranNoConsIndex );

                // Now lets read the index file information from the guest check data and
				// put it into the index files.
                ulReadOffset = ulSize;
                ulTtlIdxReadSize = 0;
                ulCurIdxReadSize = sizeof( auchIdxWork );

                while ( ulTtlIdxReadSize < IdxFileInfo.usIndexVli ) {

					//11-1103- SR 201 JHHJ
					TrnReadFile_MemoryForce ( (ulReadOffset + ulTtlIdxReadSize), auchIdxWork, sizeof( auchIdxWork ),
                                                    TrnInformation.hsTranConsStorage, &ulCurIdxReadSize ); /* ### Mod (2171 for Win32) V1.0 */
#if 0
                    TrnReadFile( (ulReadOffset + ulTtlIdxReadSize), auchIdxWork, sizeof( auchIdxWork ),
                                                    TrnInformation.hsTranConsStorage, &ulCurIdxReadSize ); /* ### Mod (2171 for Win32) V1.0 */
#endif
					if (ulCurIdxReadSize < 1)
						break;

                    if ( IdxFileInfo.usIndexVli < ( ulTtlIdxReadSize + ulCurIdxReadSize )) {
                        ulCurIdxReadSize = IdxFileInfo.usIndexVli - ulTtlIdxReadSize;
                    }
					{
						USHORT  iLoop, iLoopEnd = ulCurIdxReadSize/sizeof(PRTIDX);
						PRTIDX  *pIndex = (PRTIDX *)auchIdxWork;

						for (iLoop = 0; iLoop < iLoopEnd; iLoop++) {
							if (pIndex[iLoop].uchMajorClass == CLASS_ITEMPRINT)
								if ((pIndex[iLoop].uchMinorClass >= CLASS_TRAILER1 && pIndex[iLoop].uchMinorClass <= CLASS_TRAILER_MONEY) ||
									pIndex[iLoop].uchMinorClass == CLASS_EPT_TRAILER) {
								pIndex[iLoop].usItemOffset = pIndex[iLoop].uchMajorClass = pIndex[iLoop].uchMinorClass = 0;
							}
						}
					}

                    TrnWriteFile( (sizeof( PRTIDXHDR ) + ulTtlIdxReadSize), auchIdxWork, ulCurIdxReadSize, TrnInformation.hsTranConsIndex ); /* ### Mod (2171 for Win32) V1.0 */
                    TrnWriteFile( (sizeof( PRTIDXHDR ) + ulTtlIdxReadSize), auchIdxWork, ulCurIdxReadSize, TrnInformation.hsTranNoConsIndex ); /* ### Mod (2171 for Win32) V1.0 */

                    ulTtlIdxReadSize += ulCurIdxReadSize;
                }
            }
        }
    }

	return(sStatus);                                            /* success */
}



/*==========================================================================
*    Synopsis:  SHORT   TrnSaveGC( USHORT usType, USHORT usGCFNo )
*
*    Input:     USHORT  usType
*               ULONG   ulGCFNo
*    Output:    none
*    InOut:     none
*
*    Return:    xxxx
*
*    Description:   save guest check                                   R3.1
*
*                 See also function TrnGetGC() which is used to get the
*                 guest check information we save in TrnSaveGC().
==========================================================================*/
static ULONG TrnUpdateGuestCheckDataFile (USHORT usType)
{
    ULONG   ulTtlIdxReadSize;
    ULONG   ulCurIdxReadSize;
    ULONG   ulWriteOffset;
	ULONG	ulActualBytesRead;
    ULONG   ulSize;
    USHORT  usInitVli = 0;
    USHORT  husIndexFile;
    UCHAR   auchIdxWork[ sizeof( PRTIDX ) * 80 ];
    PRTIDXHDR IdxFileInfo;

    /*
		set GCF qual and tranitemizers to cons. storage file.

		The transaction file has a structure that contains three
		major sections: TRANCONSSTORAGEHEADER is the first part
		that contains known sized information, the next section
		following contains the transaction items, and the third
		section contains the index data for each item.
	**/
	ulSize = sizeof(USHORT) + sizeof(USHORT); // usFSize and usActLen

	// save the current tender id value to be recovered later if GC read back in.
	TrnInformation.TranGCFQual.usCheckTenderId = ItemTenderLocalCheckTenderid (3, 0);
    TrnWriteFile(ulSize, &(TrnInformation.TranGCFQual), sizeof(TRANGCFQUAL), TrnInformation.hsTranConsStorage);
	ulSize += sizeof(TRANGCFQUAL);

    TrnWriteFile(ulSize, &(TrnInformation.TranItemizers), sizeof(TRANITEMIZERS), TrnInformation.hsTranConsStorage);
	ulSize += sizeof(TRANITEMIZERS);

    if ( ( TrnInformation.TranCurQual.flPrintStatus & CURQUAL_POSTCHECK ) && usType != GCF_COUNTER_NOCONSOLI ) { /* not save option */

        TrnWriteFile(ulSize, &(TrnInformation.usTranConsStoVli), sizeof(TrnInformation.usTranConsStoVli), TrnInformation.hsTranConsStorage);
        ulSize += sizeof(TrnInformation.usTranConsStoVli);

		if (0 < TrnInformation.usTranConsStoVli) {
			// there is index data for the guest check so lets save it into the guest check as well.
			ulSize += TrnInformation.usTranConsStoVli;

			/* --- save index file information to GCF --- */

			husIndexFile = ( TrnInformation.TranCurQual.auchTotalStatus[ 4 ] & CURQUAL_TOTAL_NOCONSOLIDATE_PRINT) ?
							TrnInformation.hsTranNoConsIndex :
							TrnInformation.hsTranConsIndex ;

			/* --- save consolidated index file, if void consolidation option 05/30/96 --- */

			if (CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT2)) {
				husIndexFile = TrnInformation.hsTranConsIndex;
			}

			/* --- save consolidated index file, if split check option, V3.3 ICI --- */

			if ((TrnInformation.TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
				&& !CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT3)
				&& !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {

				/*----- Not Use "Not Consoli Print" Option -----*/
				husIndexFile = TrnInformation.hsTranConsIndex;
			}

			// Read the index file header and put it into the guest check information
			TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), husIndexFile, &ulActualBytesRead );

			TrnWriteFile(ulSize, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranConsStorage );
			ulSize += sizeof( PRTIDXHDR );

			// Now lets read through the index file transferring the index information to the
			// guest check file.
			ulWriteOffset = ulSize;
			ulTtlIdxReadSize = 0;
			ulCurIdxReadSize = sizeof( auchIdxWork );

			while ( ulTtlIdxReadSize < IdxFileInfo.usIndexVli ) {

				//11-1103- SR 201 JHHJ
				TrnReadFile((sizeof( PRTIDXHDR ) + ulTtlIdxReadSize), auchIdxWork, sizeof( auchIdxWork ), husIndexFile, &ulCurIdxReadSize ); /* ### Mod (2171 for Win32) V1.0 */

				if ( IdxFileInfo.usIndexVli < ( ulTtlIdxReadSize + ulCurIdxReadSize )) {
					ulCurIdxReadSize = IdxFileInfo.usIndexVli - ulTtlIdxReadSize;
				}

				TrnWriteFile( (ulWriteOffset + ulTtlIdxReadSize), auchIdxWork, ulCurIdxReadSize, TrnInformation.hsTranConsStorage );

				ulTtlIdxReadSize += ulCurIdxReadSize;
			}

			ulSize += IdxFileInfo.usIndexVli;
		}
    } else {

		// Indicate that there is no index file data in this guest check.
		usInitVli = 0;
        TrnWriteFile(ulSize, &( usInitVli ), sizeof( usInitVli ), TrnInformation.hsTranConsStorage);
        ulSize += sizeof( usInitVli );
    }

	NHPOS_ASSERT(ulSize <= 0xFFFF);//MAXWORD
	return ulSize;
}

SHORT TrnConnEngineSendGC(USHORT usGCFNo)
{
    ULONG   ulSize;
	ULONG   flPrintStatus = (TrnInformation.TranCurQual.flPrintStatus & CURQUAL_POSTCHECK);

	// force the guest check file update to do the index information.
	// Sigh, too many flags and globals.
	TrnInformation.TranCurQual.flPrintStatus |= CURQUAL_POSTCHECK;
	ulSize = TrnUpdateGuestCheckDataFile (GCF_COUNTER_TYPE);
	TrnInformation.TranCurQual.flPrintStatus = flPrintStatus;

	ConnEngineSendTransactionFH (1, 0, usGCFNo, TrnInformation.hsTranConsStorage, sizeof(USHORT) + sizeof(USHORT), ulSize);

	return 0;
}

SHORT TrnSaveGC(USHORT usType, USHORT usGCFNo)
{
    SHORT   sStatus;
    ULONG   ulSize;
    USHORT  usInitVli = 0;

	ulSize = TrnUpdateGuestCheckDataFile (usType);
	ConnEngineSendTransactionFH (1, 0, usGCFNo, TrnInformation.hsTranConsStorage, sizeof(USHORT) + sizeof(USHORT), ulSize);

	NHPOS_ASSERT(ulSize <= 0xFFFF);//MAXWORD
	{
		int  iLoop;
		sStatus = GCF_COMERROR;
		for (iLoop = 0; iLoop < 2 && 0 > sStatus; iLoop++) {
			sStatus = CliGusStoreFileFH(usType, usGCFNo, TrnInformation.hsTranConsStorage, sizeof(USHORT) + sizeof(USHORT), ulSize);        /* send GCF */
			if (0 > sStatus) {
				GusConvertError( sStatus );
				PifSleep (50);
			}
		}
	}

    return(sStatus);
}


/*==========================================================================
*    Synopsis:  SHORT   TrnCancelGC( USHORT usGCFNo, USHORT usType )
*
*    Input:     USHORT  usGCFNo
*               USHORT  usType
*    Output:    none
*    InOut:     none
*
*    Return:    xxxx
*
*    Description:   cancel GCF 
==========================================================================*/
SHORT   TrnCancelGC( USHORT usGCFNo, USHORT usType )
{       
    TrnInitialize( TRANI_GCFQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI );
    return( CliGusResetReadFlag( usGCFNo, usType ) );   /* send GCF */
}



/*==========================================================================
*    Synopsis:  SHORT   TrnQueryGC( USHORT usGCFNo )
*
*    Input:     USHORT  usGCFNo - Guest Check File No. to Query
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS -   Function is successful
*               GCF_NOT_IN  -   GC number is not in file
*               other       -   other error
*
*    Description:
*       This function queries target guest check file and store retrieved
*   qualifier and itemizer to Transaction Information, and consolidated
*   item data to storage file.
*       This function is no file lock version of TrnGetGC for flexible
*   drive through.
==========================================================================*/
SHORT TrnQueryGC( USHORT usGCFNo )
{
    ULONG       ulTtlIdxReadSize;
    ULONG       ulCurIdxReadSize;
	ULONG		ulActualBytesRead;
    ULONG       ulReadOffset;
    USHORT      usStoreOffset;
    SHORT       sActualRead;
    PRTIDXHDR   IdxFileInfo;
    UCHAR       auchIdxWork[ sizeof( PRTIDX ) * 80 ];

    /* --- retrieve specified GCF into consolidate storage file --- */
    usStoreOffset = sizeof( USHORT ) + sizeof( USHORT );

    sActualRead = CliGusIndReadFH( usGCFNo, TrnInformation.hsTranConsStorage, usStoreOffset, TrnInformation.usTranConsStoFSize, &ulActualBytesRead );
	//RPH SR#201
    if ( sActualRead < 0 ) {            /* an error occured */
        return ( sActualRead );
    } else if ( sActualRead == 0 ) {    /* target GCF not exist */
        return ( GCF_NOT_IN );
    }

    /* --- set retrieved qualifier/itemizer to Transaction Information --- */
    ulReadOffset = usStoreOffset;
    TrnReadFile_MemoryForce( ulReadOffset, &TrnInformation.TranGCFQual, sizeof( TRANGCFQUAL ), TrnInformation.hsTranConsStorage, &ulActualBytesRead );
#if 0
    TrnReadFile( ulReadOffset, &TrnInformation.TranGCFQual, sizeof( TRANGCFQUAL ), TrnInformation.hsTranConsStorage, &ulActualBytesRead );
#endif

    ulReadOffset += sizeof( TRANGCFQUAL );
    TrnReadFile_MemoryForce( ulReadOffset, &TrnInformation.TranItemizers, sizeof( TRANITEMIZERS ), TrnInformation.hsTranConsStorage, &ulActualBytesRead );
#if 0
    TrnReadFile( ulReadOffset, &TrnInformation.TranItemizers, sizeof( TRANITEMIZERS ), TrnInformation.hsTranConsStorage, &ulActualBytesRead );
#endif

    ulReadOffset += sizeof( TRANITEMIZERS );
    TrnReadFile_MemoryForce( ulReadOffset, &TrnInformation.usTranConsStoVli, sizeof( TrnInformation.usTranConsStoVli ), TrnInformation.hsTranConsStorage, &ulActualBytesRead );
#if 0
    TrnReadFile( ulReadOffset, &TrnInformation.usTranConsStoVli, sizeof( TrnInformation.usTranConsStoVli ), TrnInformation.hsTranConsStorage, &ulActualBytesRead );
#endif
    ulReadOffset += sizeof( TrnInformation.usTranConsStoVli );

    if ( 0 < TrnInformation.usTranConsStoVli ) {
        /* --- get item index information, and save it to index file --- */
        ulReadOffset += TrnInformation.usTranConsStoVli;
		//11-1103- SR 201 JHHJ
        TrnReadFile_MemoryForce( ulReadOffset, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranConsStorage, &ulActualBytesRead );
#if 0
        TrnReadFile( ulReadOffset, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranConsStorage, &ulActualBytesRead );
#endif
        TrnWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranConsIndex );
        TrnWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranNoConsIndex );

        /* --- get print priority index, and save it to index file --- */

        ulReadOffset += sizeof( PRTIDXHDR );
        ulTtlIdxReadSize = 0;
        ulCurIdxReadSize = sizeof( auchIdxWork );

        while ( ulTtlIdxReadSize < IdxFileInfo.usIndexVli ) {

			//11-1103- SR 201 JHHJ
            TrnReadFile_MemoryForce( (ulReadOffset + ulTtlIdxReadSize), auchIdxWork, sizeof( auchIdxWork ), TrnInformation.hsTranConsStorage, &ulCurIdxReadSize ); /* ### Mod (2171 for Win32) V1.0 */
#if 0
            TrnReadFile( (ulReadOffset + ulTtlIdxReadSize), auchIdxWork, sizeof( auchIdxWork ), TrnInformation.hsTranConsStorage, &ulCurIdxReadSize ); /* ### Mod (2171 for Win32) V1.0 */
#endif
            if ( IdxFileInfo.usIndexVli < ( ulTtlIdxReadSize + ulCurIdxReadSize )) {
                ulCurIdxReadSize = IdxFileInfo.usIndexVli - ulTtlIdxReadSize;
            }

            TrnWriteFile( (sizeof( PRTIDXHDR ) + ulTtlIdxReadSize), auchIdxWork, ulCurIdxReadSize, TrnInformation.hsTranConsIndex ); /* ### Mod (2171 for Win32) V1.0 */
            TrnWriteFile( (sizeof( PRTIDXHDR ) + ulTtlIdxReadSize), auchIdxWork, ulCurIdxReadSize, TrnInformation.hsTranNoConsIndex ); /* ### Mod (2171 for Win32) V1.0 */

            ulTtlIdxReadSize += ulCurIdxReadSize;
        }
    }
    return ( TRN_SUCCESS );
}



/*==========================================================================
*    Synopsis:  VOID    TrnInit( VOID )    
*
*    Input:     USHORT  usType
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   initialize transaction file handle, V3.3
==========================================================================*/
VOID TrnInit( VOID )
{
    /* --- set minus value to file handle indicating file closed --- */
    TrnInformation.hsTranItemStorage = -1;
    TrnInformation.hsTranConsStorage = -1;
    TrnInformation.hsTranPostRecStorage = -1;

    TrnInformation.hsTranItemIndex  = -1;
    TrnInformation.hsTranNoItemIndex = -1;
    TrnInformation.hsTranConsIndex = -1;
    TrnInformation.hsTranNoConsIndex = -1;
	TrnInformation.hsTranTmpStorage = -1;

    /* create semaphore for multi-thread use */
    husTrnSemaphore = PifCreateSem(1);
}

/*==========================================================================
*    Synopsis:  VOID    TrnInitialize( USHORT usTraniType )    
*
*    Input:     USHORT  usTraniType - bit map of TRANI_xxx options
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   initialize specified memory resident state data which
*                   includes information about current operator/cashier,
*                   current transaction, etc.
*
*                   See also function TrnInitializeSpl().
==========================================================================*/
VOID TrnInitialize( USHORT usTraniType )
{
    USHORT      usInitData = 0;
	ULONG		ulActualBytesRead;

	if ( usTraniType & TRANI_SIGNINOUT ) {                        /* initialize sign-in key to zero. */
		TrnInformation.TranModeQual.sCasSignInKey = 0;
	}

    if ( usTraniType & TRANI_MODEQUAL ) {                        /* initialize mode qual. */
		// The mode qualifier storage contains the data about the operator currently Signed-in.
		// Normally this data is only cleared at Sign-out or similar type of event and not between transactions.
		memset( &TrnInformation.usGuestNo[0], 0, sizeof(TrnInformation.usGuestNo) );
        memset( &TrnInformation.TranModeQual , 0, sizeof( TRANMODEQUAL ) );  
		memset (&TrnInformation.TranChargeTips, 0, sizeof(TrnInformation.TranChargeTips));
    }
    if ( usTraniType & TRANI_GCFQUAL ) {                         /* initialize GCF qual. */
		// The Guest Check File (GCF) qualifier storage contains the data about the current guest check.
		// Normally this data is cleared at the beginning of a transaction. This data is stored in the
		// Guest Check File. The actual transaction data may also be stored if the System Type requires it.
        memset( &TrnInformation.TranGCFQual , 0, sizeof( TRANGCFQUAL ) ); 
		memset (&TrnInformation.TranChargeTips, 0, sizeof(TrnInformation.TranChargeTips));
    }
    if ( usTraniType & TRANI_CURQUAL ) {                         /* initialize current qual. */
        memset( &TrnInformation.TranCurQual , 0, sizeof( TRANCURQUAL ) ); 
		memset (&TrnInformation.TranChargeTips, 0, sizeof(TrnInformation.TranChargeTips));
    }
    if ( usTraniType & TRANI_ITEMIZERS ) {                       /* initialize TI */
        memset( &TrnInformation.TranItemizers, 0, sizeof( TRANITEMIZERS ) );
		memset (&TrnInformation.TranChargeTips, 0, sizeof(TrnInformation.TranChargeTips));
    }

    if ( usTraniType & TRANI_ITEM ) {                            /* initialize item storage */
		CONST ULONG   ulVliOffset = offsetof(TRANITEMSTORAGEHEADER, usVli);
		PRTIDXHDR     IdxFileInfo = {0};

		if ((TrnInformation.TranCurQual.fsCurStatus & (CURQUAL_TRETURN | CURQUAL_PRETURN)) == 0) {
			TrnInformation.TranModeQual.usReturnType = 0;  // TRANI_ITEM, initialize the returns memory resident data
			TrnInformation.TranModeQual.usReasonCode = 0;  // TRANI_ITEM, initialize the returns memory resident data
		}

        TrnInformation.usTranItemStoVli = 0;
        TrnWriteFile(ulVliOffset, &TrnInformation.usTranItemStoVli, sizeof(TrnInformation.usTranItemStoVli), TrnInformation.hsTranItemStorage);

        /* --- initialize print priority index file --- */
		//11-1103- SR 201 JHHJ
        TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranItemIndex , &ulActualBytesRead);

        IdxFileInfo.usIndexVli     = 0;
        IdxFileInfo.uchNoOfItem    = 0;
        IdxFileInfo.uchNoOfSales   = 0;
        IdxFileInfo.usSalesHighest = 0;
        IdxFileInfo.usSalesLowest  = 0;

        TrnWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranItemIndex );
        TrnWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranNoItemIndex );
        
    	/* 12/05/01 */
    	/* clear plu mnemonics cashe, except store/recall */
        if (RflGetSystemType() != FLEX_STORE_RECALL) {
		    PifRequestSem(husTrnSemaphore);
		    memset(&TrnItemStorePluMnem, 0, sizeof(TrnItemStorePluMnem));
	    	usItemStoredPluMnem = 0;
		    PifReleaseSem(husTrnSemaphore);
		}
    }

    if ( usTraniType & TRANI_CONSOLI ) {
		CONST ULONG   ulVliOffset = offsetof(TRANCONSSTORAGEHEADER, usVli);
		PRTIDXHDR     IdxFileInfo = {0};

		/* initialize consolidated storage */
        TrnInformation.usTranConsStoVli = 0;
        TrnWriteFile(ulVliOffset, &TrnInformation.usTranConsStoVli, sizeof(TrnInformation.usTranConsStoVli), TrnInformation.hsTranConsStorage);

        /* --- initialize print priority index file --- */
		//11-1103- SR 201 JHHJ
        TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranConsIndex, &ulActualBytesRead );

        IdxFileInfo.usIndexVli     = 0;
        IdxFileInfo.uchNoOfItem    = 0;
        IdxFileInfo.uchNoOfSales   = 0;
        IdxFileInfo.usSalesHighest = 0;
        IdxFileInfo.usSalesLowest  = 0;

        TrnWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranConsIndex );
        TrnWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranNoConsIndex );
    }
}




/*==========================================================================
*    Synopsis:  SHORT   TrnSendTotal( VOID )
*
*    Input:     none
*    Output:    none
*    InOut:     none
*
*    Return:    xxxx
*
*    Description: send memory affection data to TOTAL MODULE      R3.1
==========================================================================*/

SHORT TrnSendTotal( VOID )
{
	UCHAR   uchWork[sizeof(TTLTUMTRANQUAL) + sizeof(TrnInformation.usTranItemStoVli)] = {0};
	TTLTUMTRANQUAL  *pTtlTumTranQual;
    USHORT  usSize;
	SHORT   sRetStatus = 0;

	pTtlTumTranQual = (VOID *)uchWork;

	if(UifRegData.regopeclose.uchMajorClass != CLASS_UIFREGOPECLOSE)
	{
		pTtlTumTranQual->TtlTranDBStatus |= uchMaintDelayBalanceFlag;
	}

    TrnInformation.TranModeQual.uchTerm = CliReadUAddr();           /* R3.1 */

	TrnGetModeQual(&pTtlTumTranQual->TtlTranModeQual);    /* mode qualifier */
	TrnGetCurQual(&pTtlTumTranQual->TtlTranCurQual);        /* current qualifier */

    usSize = sizeof(TTLTUMTRANQUAL);
	*((USHORT *)(uchWork + usSize)) = TrnInformation.usTranItemStoVli;
    usSize += sizeof(TrnInformation.usTranItemStoVli) + TrnInformation.usTranItemStoVli;  /* item storage size */

	pTtlTumTranQual->TtlTranVli = usSize;

    /* set CUR qual and MODE qual to item storage file */
    TrnWriteFile(sizeof(USHORT), uchWork, sizeof(uchWork), TrnInformation.hsTranItemStorage);

    sRetStatus = CliTtlUpdateFileFH(sizeof(USHORT), TrnInformation.hsTranItemStorage, usSize);  /* TOTAL MODULE i/F */
	if (sRetStatus != 0) {
		char  xBuff[128];
		sprintf (xBuff, "**WARNING: TrnSendTotal() CliTtlUpdateFileFH() sRetStatus = %d, usConsNo %d %d, Guest No. %d", sRetStatus, TrnInformation.TranCurQual.usConsNo, TrnInformation.TranGCFQual.usConsNo, TrnInformation.TranGCFQual.usGuestNo);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return(sRetStatus);  /* TOTAL MODULE i/F */
}



/*==========================================================================
*    Synopsis:  VOID    TrnPrintType( VOID *pItem )
*    Input:     VOID    *pItem
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   buffering/unbuffering print
==========================================================================*/

VOID    TrnPrintType( VOID *pItem )
{
    /* check display on the fly system */
    TrnPrintTypeNoDsp (pItem);
    TrnThroughDisp (pItem);
}

/*
*==========================================================================
*    Synopsis:  VOID    TrnPrintTypeNoDsp(VOID *pItem)
*    Input:     VOID    *pItem
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   buffering/unbuffering print                 Saratoga
*==========================================================================
*/
VOID    TrnPrintTypeNoDsp(VOID *pItem)
{
    if (((TrnInformation.TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER)
        || (TrnInformation.TranCurQual.fsCurStatus & CURQUAL_TRANSFER))
	{
		SHORT   sLine = PrtPrintItem(&TrnInformation, pItem);

        if (sLine != 0) {
            TrnInformation.TranGCFQual.uchPageNo = (UCHAR )(sLine / PRT_SLIP_MAX_LINES);
            TrnInformation.TranGCFQual.uchSlipLine = (UCHAR)(sLine % PRT_SLIP_MAX_LINES);
        }
    }
}

/*
*==========================================================================
**  Synopsis:   SHORT   TrnCreateFile( USHORT usNoOfItem,
*                                      UCHAR  uchStorageType )
*
*       Input:  USHORT  usNoOfItem      - number of items in storage file
*            :  UCHAR   uchStorageType  - type of storage file to create
*                   FLEX_ITEMSTORAGE_ADR, FLEX_CONSSTORAGE_ADR
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return:
*               TRN_SUCCESS -   function is successful
*               TRN_ERROR   -   function is failed
**  Description:
*           Create item or consolidate storage file. If consolidate file
            is specified, it creates post receipt file.
*==========================================================================
*/
SHORT TrnCreateFile( USHORT usNoOfItem, UCHAR uchStorageType )
{
    TCHAR         *lpszFileName;
    TCHAR         *lpszIndexName;
    ULONG         ulInquirySize;
    ULONG         ulVliOffset = 0;
    PifFileHandle hsStorageFile;
    TrnVliOffset  usInitialVli = 0;

    /* close each file, if each file still opened, V3.3 */
    TrnICPCloseFile();

    /* --- function always create specified file, even if it exists --- */
    switch ( uchStorageType ) {
    case FLEX_ITEMSTORAGE_ADR:
        lpszFileName  = aszTrnItemStorageFile;
        lpszIndexName = aszTrnItemStorageIndex;
        ulVliOffset   = offsetof(TRANITEMSTORAGEHEADER, usVli);
        break;

    case FLEX_CONSSTORAGE_ADR:
        lpszFileName  = aszTrnConsStorageFile;
        lpszIndexName = aszTrnConsStorageIndex;
        ulVliOffset   = offsetof(TRANCONSSTORAGEHEADER, usVli);
        break;

	case FLEX_TMP_FILE_ID:
		lpszFileName = aszTrnTempStorageFile;
		break;

    default:
        return ( TRN_ERROR );
    }

    TrnDeleteFile( lpszFileName );

    hsStorageFile = TrnOpenFile( lpszFileName, auchTEMP_NEW_FILE_READ_WRITE );  /* saratoga */

    if ( hsStorageFile < 0 ) {
		PifLog (MODULE_TRANS_BODY, LOG_EVENT_TRN_CREATE_OPENFILE);
        return ( TRN_ERROR );
    }

	if(uchStorageType == FLEX_TMP_FILE_ID)
	{
        TrnCloseFile( hsStorageFile );
		return TRN_SUCCESS;
	}
    /* --- allocate file size to store specified no. of items --- */

    ulInquirySize = TrnCalStoSize( usNoOfItem, uchStorageType );
    if ( TrnExpandFile( hsStorageFile, ulInquirySize ) != TRN_SUCCESS ) {
		PifLog (MODULE_TRANS_BODY, LOG_EVENT_TRN_CREATE_EXPANDFILE);
        TrnCloseFile( hsStorageFile );
        TrnDeleteFile( lpszFileName );

        return(TRN_ERROR);
    }

    /* --- store size of actual data --- */
    TrnWriteFile( ulVliOffset, &usInitialVli, sizeof(TrnVliOffset), hsStorageFile );

    TrnCloseFile( hsStorageFile );

    /* --- create index file for print priority --- */
    if ( TrnCreateIndexFile( usNoOfItem, lpszIndexName, NULL ) != TRN_SUCCESS ) {
		PifLog (MODULE_TRANS_BODY, LOG_EVENT_TRN_CREATE_INDEXFILE_01);
        TrnDeleteFile( lpszFileName );
        return ( TRN_ERROR );
    }

    if ( uchStorageType == FLEX_ITEMSTORAGE_ADR ) {
        /* --- create no-consoli file, if item storage is specified --- */
        if ( TrnCreateIndexFile( usNoOfItem, aszTrnNoItemStorageIndex, NULL ) != TRN_SUCCESS ) {
			PifLog (MODULE_TRANS_BODY, LOG_EVENT_TRN_CREATE_INDEXFILE_02);
            TrnDeleteFile( lpszFileName );
            TrnDeleteFile( lpszIndexName );
            return ( TRN_ERROR );
        }
    }

    if ( uchStorageType == FLEX_CONSSTORAGE_ADR ) {
        /* --- create post receipt file, if cons. storage is specified --- */
        if ( TrnCreatePostRecFile( ulInquirySize ) != TRN_SUCCESS ) {
			PifLog (MODULE_TRANS_BODY, LOG_EVENT_TRN_CREATE_POSTFILE);
            TrnDeleteFile( lpszFileName );
            TrnDeleteFile( lpszIndexName );
            return ( TRN_ERROR );
        }

        /* --- create no-consoli file, if cons. storage is specified --- */
        if ( TrnCreateIndexFile( usNoOfItem, aszTrnNoConsStorageIndex, NULL ) != TRN_SUCCESS ) {
			PifLog (MODULE_TRANS_BODY, LOG_EVENT_TRN_CREATE_INDEXFILE_03);
            TrnDeleteFile( lpszFileName );
            TrnDeleteFile( lpszIndexName );
            TrnDeleteFile( aszTrnPostRecStorageFile );
            return ( TRN_ERROR );
        }
    }
    return ( TRN_SUCCESS );
}


/*
*==========================================================================
**  Synopsis:   SHORT   TrnCreatePostRecFile( ULONG ulConsFileSize )
*
*       Input:  ULONG   ulConsFileSize - size of consolidate storage file
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return:
*               TRN_SUCCESS -   function is successful
*               TRN_ERROR   -   function is failed
**  Description:
*           it creates post receipt file.
*==========================================================================
*/
SHORT TrnCreatePostRecFile( ULONG ulConsFileSize )
{
    ULONG           ulInquirySize;
    PifFileHandle   hsPostRecFile;
    SHORT           sReturn;

    /* --- function always create specified file, even if it exists --- */
    TrnDeleteFile( aszTrnPostRecStorageFile );

    hsPostRecFile = TrnOpenFile( aszTrnPostRecStorageFile, auchTEMP_NEW_FILE_READ_WRITE );    /* saratoga */
    if ( hsPostRecFile < 0 ) {
		PifLog (MODULE_TRANS_BODY, LOG_EVENT_TRN_CREATEPOST_OPEN);
        return ( TRN_ERROR );
    }

    /* --- allocate file size to store current qualifier data --- */
    ulInquirySize = ulConsFileSize + sizeof( TRANCURQUAL );
    sReturn = TrnExpandFile( hsPostRecFile, ulInquirySize );
    if ( sReturn != TRN_SUCCESS ) {
		PifLog (MODULE_TRANS_BODY, LOG_EVENT_TRN_CREATEPOST_EXPAND);
        TrnCloseFile( hsPostRecFile );
        TrnDeleteFile( aszTrnPostRecStorageFile );
        return( TRN_ERROR );
    }

    TrnCloseFile( hsPostRecFile );

    return ( TRN_SUCCESS );
}

/*
*==========================================================================
**  Synopsis :  SHORT TrnCreateIndexFile( USHORT     usNoOfItem,
*                                         UCHAR FAR *pszIndexName,
*                                         ULONG     *pulCreatedSize )
*
*       Input:  USHORT     usNoOfItem       - number of items in storage file
*            :  UCHAR FAR *pszIndexName     - address of index file name
*               ULONG     *pulCreatedSize   - size of created index file
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*               TRN_SUCCESS -   file creation is successful
*               TRN_ERROR   -   file creation is failed
*
**  Description:    Create index file of specified transaction storage.
*
*                   See also function TrnChkStorageSizeWithOrWithoutReserve() which
*                   is used as items are entered to check if we are near the
*                   transaction data file size limit.
*==========================================================================
*/
SHORT TrnCreateIndexFile( USHORT     usNoOfItem,
                          TCHAR     *lpszIndexName,
                          ULONG     *pulCreatedSize )
{
    PifFileHandle   hsIndexFile;
    ULONG           ulInquirySize;
    PRTIDXHDR       IdxFileInfo;

    /* --- delete current existing index file before new file creation --- */
    TrnDeleteFile( lpszIndexName );
 
    /* --- allocate index file with specified size --- */
    hsIndexFile = TrnOpenFile( lpszIndexName, auchTEMP_NEW_FILE_READ_WRITE );   /* saratoga */
	if ( hsIndexFile < 0 ) {
		PifLog (MODULE_TRANS_BODY, LOG_EVENT_TRN_CREATEINDEX_OPEN);
        return ( TRN_ERROR );
    }

    ulInquirySize = sizeof( PRTIDXHDR ) + ( sizeof( PRTIDX ) * ( usNoOfItem + PRTIDX_RESERVE ));

    if ( TrnExpandFile( hsIndexFile, ulInquirySize ) != TRN_SUCCESS ) {
		PifLog (MODULE_TRANS_BODY, LOG_EVENT_TRN_CREATEINDEX_EXPAND);
        TrnCloseFile( hsIndexFile );
        TrnDeleteFile( lpszIndexName );
        return ( TRN_ERROR );
    }

    /* --- store current created file size to formal parameter --- */
    if ( pulCreatedSize != NULL ) {
        *pulCreatedSize = ulInquirySize;
    }    

    /* --- initialize file header and save it to index file --- */
	NHPOS_ASSERT(ulInquirySize <= 0xFFFF);

    IdxFileInfo.usIndexFSize   = ulInquirySize;
    IdxFileInfo.usIndexVli     = 0;
    IdxFileInfo.uchNoOfItem    = 0;
    IdxFileInfo.uchNoOfSales   = 0;
    IdxFileInfo.usSalesHighest = 0;
    IdxFileInfo.usSalesLowest  = 0;

    TrnWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile );

    TrnCloseFile( hsIndexFile );

    return ( TRN_SUCCESS );
}

/*
*==========================================================================
**  Synopsis:   SHORT   TrnChkAndCreFile( USHORT usNoOfItem,
*                                         UCHAR  uchStorageType )
*
*       Input:  USHORT  usNoOfItem      - number of items in storage file
*            :  UCHAR   uchStorageType  - type of storage file to create
*                   FLEX_ITEMSTORAGE_ADR, FLEX_CONSSTORAGE_ADR
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return:
*               TRN_SUCCESS -   function is successful
*               TRN_ERROR   -   function is failed
*
**  Description:    create specified file, if the file does not exist.
*
*                   See also function TrnChkStorageSizeWithOrWithoutReserve() which
*                   is used as items are entered to check if we are near the
*                   transaction data file size limit.
*==========================================================================
*/
SHORT TrnChkAndCreFile( USHORT usNoOfItem, UCHAR uchStorageType )
{
    TCHAR           *lpszFileName;
    PifFileHandle   hsStorageFile;
    SHORT           sReturn;    

    /* close each file, if each file still opened, V3.3 */
    TrnICPCloseFile();

    /* --- open current existing file --- */
    switch ( uchStorageType ) {
    case FLEX_ITEMSTORAGE_ADR:
        lpszFileName = aszTrnItemStorageFile;
        break;    

    case FLEX_CONSSTORAGE_ADR:
        lpszFileName = aszTrnConsStorageFile;
        break;
	
	case FLEX_TMP_FILE_ID:
		lpszFileName = aszTrnTempStorageFile;
        break;

    default:
        return ( TRN_ERROR );
    }

    hsStorageFile = TrnOpenFile( lpszFileName, auchTEMP_OLD_FILE_READ );    /* saratoga */
    if ( 0 <= hsStorageFile ) { /* file is already existing */
        TrnCloseFile( hsStorageFile );
        sReturn = TRN_SUCCESS;
    } else {                    /* file is not existing */
        sReturn = TrnCreateFile( usNoOfItem, uchStorageType );
    }
    return ( sReturn );
}    

/*
*==========================================================================
**  Synopsis:   VOID    TrnChkAndDelFile( USHORT usNoOfItem,
*                                         UCHAR  uchStorageType )
*
*       Input:  USHORT  usNoOfItem      - number of items in storage file
*            :  UCHAR   uchStorageType  - type of storage file to delete
*                   FLEX_ITEMSTORAGE_ADR, FLEX_CONSSTORAGE_ADR
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return:     Nothing
*
**  Description:
*       delete the specified file, if the file is applicable to one of the
*       following cases.
*         - specified file size is different from existing file size.
*         - specified file has not the index file.
*         - if consolidate file is specified, it has not the post receipt file.
*==========================================================================
*/
VOID TrnChkAndDelFile( USHORT usNoOfItem, UCHAR uchStorageType )
{
    TCHAR       *lpszStorageName;
    TCHAR       *lpszIndexName;
    ULONG       ulInquirySize;
	ULONG		ulActualBytesRead;
    SHORT       hsStorageFile;
    SHORT       hsIndexFile;
    SHORT       hsPostRecFile;
    SHORT       hsNoConsIndex;
    BOOL        fSameSize;
    BOOL        fIndexExist;
    BOOL        fPostRecExist;
    BOOL        fNoConsExist;
    TrnFileSize usActualSize;

    /* close each file, if each file still opened, V3.3 */
    TrnICPCloseFile();

    switch ( uchStorageType ) {
    case FLEX_ITEMSTORAGE_ADR:  /* item storage file */
        lpszStorageName = aszTrnItemStorageFile;
        lpszIndexName   = aszTrnItemStorageIndex;

        fPostRecExist = TRUE;

        /* --- determine no-consolidate file is existing --- */
        hsNoConsIndex  = TrnOpenFile( aszTrnNoItemStorageIndex, auchTEMP_OLD_FILE_READ ); /* open FLEX_ITEMSTORAGE_ADR */
        /* hsNoConsIndex  = TrnOpenFile( aszTrnNoItemStorageIndex, auchOLD_FILE_READ ); */
        if ( hsNoConsIndex < 0 ) {
            fNoConsExist = FALSE;
        } else {
            TrnCloseFile( hsNoConsIndex );
            fNoConsExist = TRUE;
        }
        break;

    case FLEX_CONSSTORAGE_ADR:  /* consolidate storage file */
        lpszStorageName = aszTrnConsStorageFile;
        lpszIndexName   = aszTrnConsStorageIndex;

        /* --- determine post receipt file is existing --- */
        hsPostRecFile   = TrnOpenFile( aszTrnPostRecStorageFile, auchTEMP_OLD_FILE_READ );    /* open FLEX_CONSSTORAGE_ADR */
        /* hsPostRecFile   = TrnOpenFile( aszTrnPostRecStorageFile, auchOLD_FILE_READ ); */
        if ( hsPostRecFile < 0 ) {
            fPostRecExist = FALSE;
        } else {
            TrnCloseFile( hsPostRecFile );
            fPostRecExist = TRUE;
        }

        /* --- determine no-consolidate file is existing --- */
        hsNoConsIndex  = TrnOpenFile( aszTrnNoConsStorageIndex, auchTEMP_OLD_FILE_READ ); /* saratoga */
        /* hsNoConsIndex  = TrnOpenFile( aszTrnNoConsStorageIndex, auchOLD_FILE_READ ); */
        if ( hsNoConsIndex < 0 ) {
            fNoConsExist = FALSE;
        } else {
            TrnCloseFile( hsNoConsIndex );
            fNoConsExist = TRUE;
        }
        break;

    default:
        return;
    }

    /* --- if inquiry size is different from actual size, delete
        current existing storage file --- */
    hsStorageFile = TrnOpenFile( lpszStorageName, auchTEMP_OLD_FILE_READ ); /* saratoga */
    /* hsStorageFile = TrnOpenFile( lpszStorageName, auchOLD_FILE_READ ); */
    if ( hsStorageFile < 0 ) {  /* storage file is not existing */
        return;
    }
	//11-1103- SR 201 JHHJ
    TrnReadFile( 0, &usActualSize, sizeof( TrnFileSize ), hsStorageFile, &ulActualBytesRead );
    TrnCloseFile( hsStorageFile );

    ulInquirySize = TrnCalStoSize( usNoOfItem, uchStorageType );

    fSameSize = ( ulInquirySize == usActualSize ) ? TRUE : FALSE;

    /* --- determine index file is existing --- */
    hsIndexFile = TrnOpenFile( lpszIndexName, auchTEMP_OLD_FILE_READ ); /* saratoga */
    /* hsIndexFile = TrnOpenFile( lpszIndexName, auchOLD_FILE_READ ); */
    fIndexExist = ( 0 <= hsIndexFile ) ? TRUE : FALSE;
    TrnCloseFile( hsIndexFile );

    if ( ! ( fSameSize && fIndexExist && fPostRecExist && fNoConsExist )) {
        TrnDeleteFile( lpszStorageName );
    }
}    

/*
*==========================================================================
**    Synopsis: SHORT   TrnICPOpenFile( VOID )
*                                                     
*       Input:  Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  
*           Normal End: TRN_SUCCESS 
*           Error End:  LDT_PROHBT_ADR                                                       
*                                                                  
**  Description:
*           Open item, consoli, post rec. each files and save file handle.
*==========================================================================
*/
SHORT TrnICPOpenFile( VOID )
{
	ULONG	ulActualBytesRead;
    SHORT   hsFileHandle;
    SHORT   sReturn;

    /* check file already have been opened, V3.3 */
    if (TrnInformation.hsTranItemStorage < 0) {
        if ((hsFileHandle = TrnOpenFile(aszTrnItemStorageFile, auchTEMP_OLD_FILE_READ_WRITE)) < 0) {    /* saratoga */
            return(LDT_PROHBT_ADR);
        }
        TrnInformation.hsTranItemStorage = hsFileHandle;
    
		//11-1103- SR 201 JHHJ
        TrnReadFile(0, &TrnInformation.usTranItemStoFSize, sizeof(TrnInformation.usTranItemStoFSize), hsFileHandle, &ulActualBytesRead);
    }

    if (TrnInformation.hsTranConsStorage < 0) {
        if ((hsFileHandle = TrnOpenFile(aszTrnConsStorageFile, auchTEMP_OLD_FILE_READ_WRITE)) < 0) {    /* saratoga */
            TrnCloseFile(TrnInformation.hsTranItemStorage); TrnInformation.hsTranItemStorage = -1;
            return(LDT_PROHBT_ADR);
        }
        TrnInformation.hsTranConsStorage = hsFileHandle;

		//11-1103- SR 201 JHHJ
        TrnReadFile(0, &TrnInformation.usTranConsStoFSize, sizeof(TrnInformation.usTranConsStoFSize), hsFileHandle, &ulActualBytesRead);
    }

    if (TrnInformation.hsTranPostRecStorage < 0) {
        if ((hsFileHandle = TrnOpenFile(aszTrnPostRecStorageFile, auchTEMP_OLD_FILE_READ_WRITE)) < 0) { /* saratoga */
            TrnCloseFile(TrnInformation.hsTranItemStorage); TrnInformation.hsTranItemStorage = -1;

            TrnCloseFile(TrnInformation.hsTranConsStorage); TrnInformation.hsTranConsStorage = -1;
            return(LDT_PROHBT_ADR);
        }
        TrnInformation.hsTranPostRecStorage = hsFileHandle;
    }
	
	if (TrnInformation.hsTranTmpStorage < 0) {
		if((hsFileHandle = TrnOpenFile(aszTrnTempStorageFile, auchTEMP_OLD_FILE_READ_WRITE)) < 0){
            TrnCloseFile(TrnInformation.hsTranItemStorage); TrnInformation.hsTranItemStorage = -1;
            TrnCloseFile(TrnInformation.hsTranConsStorage); TrnInformation.hsTranConsStorage = -1;
            TrnCloseFile(TrnInformation.hsTranPostRecStorage); TrnInformation.hsTranPostRecStorage = -1;
			return(LDT_PROHBT_ADR);
		}
		TrnInformation.hsTranTmpStorage = hsFileHandle;
	}

    /* --- open index files of item/consolidate storage --- */
    sReturn = TrnICOpenIndexFile();

    if ( sReturn != TRN_SUCCESS ) {
		NHPOS_ASSERT_TEXT((sReturn == TRN_SUCCESS), "**ERROR: TrnICPOpenFile() error opening files.");
        TrnCloseFile(TrnInformation.hsTranItemStorage); TrnInformation.hsTranItemStorage = -1;
        TrnCloseFile(TrnInformation.hsTranConsStorage); TrnInformation.hsTranConsStorage = -1;
        TrnCloseFile(TrnInformation.hsTranPostRecStorage); TrnInformation.hsTranPostRecStorage = -1;
		TrnCloseFile(TrnInformation.hsTranTmpStorage); TrnInformation.hsTranTmpStorage = -1;
    }
    return( sReturn );
}    

/*
*==========================================================================
**  Synopsis :  SHORT TrnICOpenIndexFile( VOID )
*
*       Input:  Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*           TRN_SUCCESS     -   function is successful
*           LDT_PROHBT_ADR  -   function is failed
*
**  Description:
*           Open index files of item and consolidate storage file.
*==========================================================================
*/
SHORT TrnICOpenIndexFile( VOID )
{
    SHORT       hsItemIndex;
    SHORT       hsNoItemIndex;
    SHORT       hsConsIndex;
    SHORT       hsNoConsIndex;
    UCHAR       auchInitIdxInfo[ sizeof( PRTIDXHDR ) - sizeof( USHORT ) ];

    memset( &auchInitIdxInfo[ 0 ], 0x00, sizeof( auchInitIdxInfo ));

    /* --- initialize index file --- */
    if (TrnInformation.hsTranItemIndex < 0) {

        if (( hsItemIndex = TrnOpenFile( aszTrnItemStorageIndex, auchTEMP_OLD_FILE_READ_WRITE )) < 0 ) {    /* saratoga */
            return ( LDT_PROHBT_ADR );
        }
        TrnWriteFile( sizeof( USHORT ), &auchInitIdxInfo[ 0 ], sizeof( auchInitIdxInfo ), hsItemIndex );
        TrnInformation.hsTranItemIndex   = hsItemIndex;
    }

    /* --- open no-cons. item storage index file --- */
    if (TrnInformation.hsTranNoItemIndex < 0) {
        if (( hsNoItemIndex = TrnOpenFile( aszTrnNoItemStorageIndex, auchTEMP_OLD_FILE_READ_WRITE )) < 0 ) {  /* saratoga */
            TrnCloseFile( hsItemIndex );
            TrnInformation.hsTranItemIndex  = -1;
            return ( LDT_PROHBT_ADR );
        }
        TrnWriteFile( sizeof( USHORT ), &auchInitIdxInfo[ 0 ], sizeof( auchInitIdxInfo ), hsNoItemIndex );
        TrnInformation.hsTranNoItemIndex = hsNoItemIndex;
    }

    /* --- open consolidate storage index file --- */
    if (TrnInformation.hsTranConsIndex < 0) {
        if (( hsConsIndex = TrnOpenFile( aszTrnConsStorageIndex, auchTEMP_OLD_FILE_READ_WRITE )) < 0 ) {    /* saratoga */
            TrnCloseFile( hsItemIndex );
            TrnInformation.hsTranItemIndex  = -1;

            TrnCloseFile( hsNoItemIndex );
            TrnInformation.hsTranNoItemIndex = -1;

            return ( LDT_PROHBT_ADR );
        }
        TrnWriteFile( sizeof( USHORT ), &auchInitIdxInfo[ 0 ], sizeof( auchInitIdxInfo ), hsConsIndex );
        TrnInformation.hsTranConsIndex   = hsConsIndex;
    }

    /* --- open no-consolidate storage index file --- */
    if (TrnInformation.hsTranNoConsIndex < 0) {
        if (( hsNoConsIndex = TrnOpenFile( aszTrnNoConsStorageIndex, auchTEMP_OLD_FILE_READ_WRITE )) < 0 ) {  /* saratoga */
            TrnCloseFile( hsItemIndex );
            TrnInformation.hsTranItemIndex  = -1;

            TrnCloseFile( hsNoItemIndex );
            TrnInformation.hsTranNoItemIndex = -1;

            TrnCloseFile( hsConsIndex );
            TrnInformation.hsTranConsIndex = -1;

            return ( LDT_PROHBT_ADR );
        }
        TrnWriteFile( sizeof( USHORT ), &auchInitIdxInfo[ 0 ], sizeof( auchInitIdxInfo ), hsNoConsIndex );
        TrnInformation.hsTranNoConsIndex = hsNoConsIndex;
    }

    return ( TRN_SUCCESS );    
}

/*
*==========================================================================
**    Synopsis: VOID    TrnICPCloseFile( VOID )
*                                                  
*       Input:  Nothing 
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*                                                                  
**  Description:
*           Close item, consoli. post rec. each files.
*==========================================================================
*/
VOID TrnICPCloseFile( VOID )
{
    // check file already have been closed, if not then close it
	// and indicate it is closed by setting file handle to -1
    if (TrnInformation.hsTranItemStorage >= 0) {
        TrnCloseFile(TrnInformation.hsTranItemStorage); TrnInformation.hsTranItemStorage = -1;
    }

    if (TrnInformation.hsTranConsStorage >= 0) {
        TrnCloseFile(TrnInformation.hsTranConsStorage); TrnInformation.hsTranConsStorage = -1;
    }

    if (TrnInformation.hsTranPostRecStorage >= 0) {
        TrnCloseFile(TrnInformation.hsTranPostRecStorage); TrnInformation.hsTranPostRecStorage = -1;
    }

    /* --- close index files of item/consolidate storage --- */
    if (TrnInformation.hsTranItemIndex >= 0) {
        TrnCloseFile( TrnInformation.hsTranItemIndex ); TrnInformation.hsTranItemIndex  = -1;
    }

    if (TrnInformation.hsTranNoItemIndex >= 0) {
        TrnCloseFile( TrnInformation.hsTranNoItemIndex ); TrnInformation.hsTranNoItemIndex = -1;
    }

    if (TrnInformation.hsTranConsIndex >= 0) {
        TrnCloseFile( TrnInformation.hsTranConsIndex ); TrnInformation.hsTranConsIndex = -1;
    }

    if (TrnInformation.hsTranNoConsIndex >= 0) {
        TrnCloseFile( TrnInformation.hsTranNoConsIndex ); TrnInformation.hsTranNoConsIndex = -1;
    }

    if (TrnInformation.hsTranTmpStorage >= 0) {
        TrnCloseFile( TrnInformation.hsTranTmpStorage ); TrnInformation.hsTranTmpStorage = -1;
    }
}

/*
*==========================================================================
**    Synopsis: SHORT   TrnOpenFile(UCHAR FAR *uchFileName, UCHAR FAR *auchType)
*                                                     
*       Input:  UCHAR FAR   *uchFileName - pointer of file name
*               UCHAR FAR   *auchType - pointer of open type def.
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  
*           Normal End: >= 0 : File Handle 
*           Error End:  < 0  : File Open Error                                                       
*                                                                  
**  Description:
*           Open specified files.
*==========================================================================
*/
#if defined(TrnOpenFile)
#pragma message("TrnOpenFile defined")
SHORT TrnOpenFile_Special(CONST TCHAR *uchFileName, CONST UCHAR *auchType);

SHORT TrnOpenFile_Debug(CONST TCHAR  *uchFileName, CONST UCHAR  *auchType, CONST char *aszFilePath, int nLineNo)
{
    SHORT   sReturn;
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

    sReturn = TrnOpenFile_Special(uchFileName, auchType);

	if (sReturn != (TRN_SUCCESS)) {
		sprintf (xBuffer, "TrnOpenFile_Debug(): Error = %d, File %s, lineno = %d", sReturn, aszFilePath + iLen, nLineNo);
		NHPOS_ASSERT_TEXT(0, xBuffer);
	}

	return sReturn;
}

SHORT TrnOpenFile_Special(CONST TCHAR  *uchFileName, CONST UCHAR  *auchType)
#else
SHORT TrnOpenFile(CONST TCHAR *uchFileName, CONST UCHAR *auchType)
#endif
{
    return(PifOpenFile(uchFileName, auchType));
}    

#if defined(TrnReadWriteFile_Memory)

SHORT TrnOpenFile_Memory (TCHAR *uchFileName, UCHAR *auchType)
{
	SHORT  hsFileHandle;
	int    i = 0;
	TrnMemoryFileTableRow  *pMemoryTable = TrnMemoryFileTable;

	for (i = 0; pMemoryTable->pTrnMemoryFile != 0; pMemoryTable++, i++) {
		if (pMemoryTable->hsFileHandle < 0) {
			hsFileHandle = TrnOpenFile_Special(uchFileName, auchType);
			if (hsFileHandle >= 0) {
				pMemoryTable->hsFileHandle = hsFileHandle;
				PifReadFile(hsFileHandle, pMemoryTable->pTrnMemoryFile, TRNMEMORYFILE_SIZE, &(pMemoryTable->ulActualBytesRead));
			}
			return hsFileHandle;
		}
	}
	return TrnOpenFile_Special(uchFileName, auchType);
}

#endif

/*
*==========================================================================
**    Synopsis: SHORT   TrnExpandFile( SHORT  hsFileHandle,
*                                      USHORT usInquirySize )
*                                                        
*       Input:  SHORT   hsFileHandle  - file handle
*               USHORT  usInquirySize - size of file to expand
*      Output:  Nothing
*
**  Return   :  
*           Normal End: TRN_SUCCESS
*           Error End : TRN_ERROR
*                                                                  
**  Description:
*           Expand specified file size
*==========================================================================
*/
#if defined(TrnExpandFile)
#pragma message("TrnExpandFile defined")
SHORT TrnExpandFile_Special( PifFileHandle hsFileHandle, ULONG ulInquirySize);

SHORT TrnExpandFile_Debug( PifFileHandle hsFileHandle, ULONG ulInquirySize, CONST char *aszFilePath, int nLineNo)
{
    SHORT   sReturn;
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

    sReturn = TrnExpandFile_Special(hsFileHandle, ulInquirySize );

	if (sReturn != (TRN_SUCCESS)) {
		sprintf (xBuffer, "TrnExpandFile_Debug(): Error = %d, File %s, lineno = %d", sReturn, aszFilePath + iLen, nLineNo);
		NHPOS_ASSERT_TEXT(0, xBuffer);
	}

	return sReturn;
}

SHORT TrnExpandFile_Special( PifFileHandle hsFileHandle, ULONG ulInquirySize )
#else
SHORT TrnExpandFile( PifFileHandle hsFileHandle, ULONG ulInquirySize )
#endif
{
    ULONG        ulActualSize;
    SHORT        sReturn;
	TrnFileSize  usInquirySize = ulInquirySize;

    sReturn = PifSeekFile( hsFileHandle, ulInquirySize, &ulActualSize );
    if (( sReturn < 0 ) || (ulInquirySize != ulActualSize )) {
        return ( TRN_ERROR );
    }

    /* --- store file size to created file --- */
    sReturn = PifSeekFile( hsFileHandle, 0UL, &ulActualSize );
    if (( sReturn < 0 ) || ( ulActualSize != 0UL )) {
        return ( TRN_ERROR );
    }

    PifWriteFile( hsFileHandle, &usInquirySize, sizeof( TrnFileSize ));

    return ( TRN_SUCCESS );
}

/*
*==========================================================================
**    Synopsis: SHORT   TrnSeekFile(SHORT hsFileHandle,
*                                   ULONG ulActSize,
*                                   ULONG *ulActMove)
*                                                        
*       Input:  SHORT   hsFileHandle - file handle
*               ULONG   ulActSize - seek size
*      Output:  Nothing
*       InOut:  ULONG   *ulActMove - pointer of seeked size save area
*
**  Return   :  
*           Normal End: >= 0 
*           Error End:  < 0                                                       
*                                                                  
**  Description:
*           Seek specified files.
*==========================================================================
*/
#if defined(TrnSeekFile)
#pragma message("TrnSeekFile defined")
SHORT TrnSeekFile_Special(PifFileHandle hsFileHandle, ULONG ulActSize, ULONG *ulActMove);

SHORT TrnSeekFile_Debug(PifFileHandle hsFileHandle, ULONG ulActSize, ULONG *ulActMove, CONST char *aszFilePath, int nLineNo)
{
    SHORT   sReturn;
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

    sReturn = TrnSeekFile_Special(hsFileHandle, ulActSize, ulActMove);

	if (sReturn != (TRN_SUCCESS)) {
		sprintf (xBuffer, "TrnSeekFile_Debug(): Error = %d, File %s, lineno = %d", sReturn, aszFilePath + iLen, nLineNo);
		NHPOS_ASSERT_TEXT(0, xBuffer);
	}

	return sReturn;
}

SHORT TrnSeekFile_Special(PifFileHandle hsFileHandle, ULONG ulActSize, ULONG *ulActMove)
#else
SHORT TrnSeekFile(PifFileHandle hsFileHandle, ULONG ulActSize, ULONG *ulActMove)
#endif
{
    return(PifSeekFile(hsFileHandle, ulActSize, ulActMove));
}

/*
*==========================================================================
**    Synopsis: USHORT  TrnReadFile(USHORT usOffset, VOID *pData,
*                                   USHORT usSize, SHORT hsFileHandle,
*									ULONG *pulActualBytesRead)
*                                                        
*       Input:  USHORT  usOffset - Number of bytes from the biginning of the file.
*               VOID    *pData - Pointer to read data buffer.
*               USHORT  usSize - Number of bytes to be written.
*               SHORT   hsFileHandle - File Handle.
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Success or Error
*
**  Description:
*           Read data from item, consoli., pst rec. file.
*==========================================================================
*/
#if defined(TrnReadFile)
#pragma message("TrnReadFile defined")
SHORT TrnReadFile_Special(ULONG ulOffset, VOID *pData,
                   ULONG ulSize, PifFileHandle hsFileHandle,
				   ULONG *pulActualBytesRead);

SHORT TrnReadFile_Debug(ULONG ulOffset, VOID *pData,
                   ULONG ulSize, PifFileHandle hsFileHandle,
				   ULONG *pulActualBytesRead, CONST char *aszFilePath, int nLineNo)
{
    SHORT  sReturn;
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

    sReturn = TrnReadFile_Special(ulOffset, pData, ulSize, hsFileHandle, pulActualBytesRead);

	if (sReturn != (TRN_SUCCESS)) {
		sprintf (xBuffer, "TrnReadFile_Debug(): Error = %d, File %s, lineno = %d", sReturn, aszFilePath + iLen, nLineNo);
		NHPOS_ASSERT_TEXT(0, xBuffer);
	}

	return sReturn;
}

SHORT TrnReadFile_Special(ULONG ulOffset, VOID *pData,
                   ULONG ulSize, PifFileHandle hsFileHandle,
				   ULONG *pulActualBytesRead)
#else
SHORT TrnReadFile(ULONG ulOffset, VOID *pData,
                   ULONG ulSize, PifFileHandle hsFileHandle,
				   ULONG *pulActualBytesRead)
#endif
{
    ULONG   sReturn;
    ULONG   ulActPos;
//    USHORT  usFSize;

	*pulActualBytesRead = 0;          // initialize number of bytes read to zero
    if (!ulSize) {                    /* check read size */
        return(TRN_NO_READ_SIZE);     /* if read nothing, return */
    }

//    sReturn = TrnSeekFile( hsFileHandle, 0UL, &ulActPos );

	//JHHJ 11-7-03, added pulActualBytesRead for PifReadFile
//    sReturn = PifReadFile( hsFileHandle, &usFSize, sizeof( USHORT ), pulActualBytesRead);
    sReturn = TrnSeekFile( hsFileHandle, ulOffset, &ulActPos );

	PifReadFile(hsFileHandle, pData, ulSize, pulActualBytesRead);
	if( *pulActualBytesRead != ulSize)
	{
		// NOTE: Most callers will just gnore the return value from TrnReadFile() since
		//       if the read is near the end of the transaction file, there may be a final
		//       record which is smaller than the requested buffer size but still a valid record.
		return(TRN_ERROR);
	}

    return (TRN_SUCCESS);
}

#if defined(TrnReadWriteFile_Memory)

SHORT TrnReadFile_MemoryForce (ULONG ulOffset, VOID *pData,
                   ULONG ulSize, SHORT hsFileHandle,
				   ULONG *pulActualBytesRead)
{
	SHORT   sRetStatus = (TRN_ERROR);
	UCHAR  *pTrnMemoryFile;
	TrnMemoryFileTableRow  *pMemoryTable;

	NHPOS_ASSERT_TEXT((hsFileHandle >= 0), "TrnReadFile_MemoryForce(): Bad file handle.");

	for (pMemoryTable = TrnMemoryFileTable; pMemoryTable->pTrnMemoryFile != 0; pMemoryTable++) {
		if (pMemoryTable->hsFileHandle == hsFileHandle) {
			pTrnMemoryFile = pMemoryTable->pTrnMemoryFile;
			break;
		}
	}

	*pulActualBytesRead = 0;

	sRetStatus = TrnReadFile_Special (ulOffset, pData, ulSize, hsFileHandle, pulActualBytesRead);

	if (pMemoryTable->pTrnMemoryFile == 0) {
		return sRetStatus;
	}

	if (ulOffset + ulSize < TRNMEMORYFILE_SIZE) {
		memcpy (pTrnMemoryFile + ulOffset, pData, ulSize);
	}
	return sRetStatus;
}

SHORT TrnReadFile_MemoryForceIfFile (ULONG ulOffset, VOID *pData,
                   ULONG ulSize, SHORT hsFileHandle,
				   ULONG *pulActualBytesRead)
{
	SHORT   sRetStatus = (TRN_ERROR);
	UCHAR  *pTrnMemoryFile;
	TrnMemoryFileTableRow  *pMemoryTable;

	NHPOS_ASSERT_TEXT((hsFileHandle >= 0), "TrnReadFile_MemoryForceIfFile(): Bad file handle.");

	for (pMemoryTable = TrnMemoryFileTable; pMemoryTable->pTrnMemoryFile != 0; pMemoryTable++) {
		if (pMemoryTable->hsFileHandle == hsFileHandle) {
			pTrnMemoryFile = pMemoryTable->pTrnMemoryFile;
			break;
		}
	}

	*pulActualBytesRead = 0;

	if (pMemoryTable->pTrnMemoryFile == 0)
		return (TRN_ERROR);

	sRetStatus = TrnReadFile_Special (ulOffset, pData, ulSize, hsFileHandle, pulActualBytesRead);

	if (pMemoryTable->pTrnMemoryFile == 0) {
		return sRetStatus;
	}

	if (ulOffset + ulSize < TRNMEMORYFILE_SIZE) {
		memcpy (pTrnMemoryFile + ulOffset, pData, ulSize);
	}
	return sRetStatus;
}


SHORT TrnReadFile_Memory (ULONG ulOffset, VOID *pData,
                   ULONG ulSize, SHORT hsFileHandle,
				   ULONG *pulActualBytesRead)
{
	SHORT   sRetStatus;
	UCHAR  *pTrnMemoryFile = 0;
	TrnMemoryFileTableRow  *pMemoryTable;

	NHPOS_ASSERT_TEXT((hsFileHandle >= 0), "TrnReadFile_Memory(): Bad file handle.");

	for (pMemoryTable = TrnMemoryFileTable; pMemoryTable->pTrnMemoryFile != 0; pMemoryTable++) {
		if (pMemoryTable->hsFileHandle == hsFileHandle) {
			pTrnMemoryFile = pMemoryTable->pTrnMemoryFile;
			break;
		}
	}

	*pulActualBytesRead = 0;

	if (pMemoryTable->pTrnMemoryFile != 0 && ulOffset + ulSize < TRNMEMORYFILE_SIZE) {
		memcpy (pData, pTrnMemoryFile + ulOffset, ulSize);
		*pulActualBytesRead = ulSize;
		sRetStatus = (TRN_SUCCESS);
	}
	else {
		sRetStatus = TrnReadFile_Special (ulOffset, pData, ulSize, hsFileHandle, pulActualBytesRead);
	}
	return sRetStatus;
}
#else
SHORT TrnReadFile_MemoryForceIfFile (ULONG ulOffset, VOID *pData,
                   ULONG ulSize, SHORT hsFileHandle,
				   ULONG *pulActualBytesRead)
{
	SHORT   sRetStatus = (TRN_ERROR);

	NHPOS_ASSERT_TEXT((hsFileHandle >= 0), "TrnReadFile_MemoryForceIfFile(): Bad file handle.");

	*pulActualBytesRead = 0;

	sRetStatus = TrnReadFile (ulOffset, pData, ulSize, hsFileHandle, pulActualBytesRead);

	return sRetStatus;
}

#endif

/*
*==========================================================================
**    Synopsis: VOID    TrnWriteFile(USHORT usOffset, VOID *pData,
*                                    USHORT usSize, SHORT hsFileHandle)
*                                                        
*       Input:  USHORT  usOffset - Number of bytes from the biginning of the file.
*               VOID    *pData - Pointer to write data buffer.
*               USHORT  usSize - Number of bytes to be written.
*               SHORT   hsFileHandle - File Handle.
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing 
*
**  Description:
*           Write data to item, consoli. post rec. file.
*==========================================================================
*/
#if defined(TrnWriteFile)
#pragma message("TrnWriteFile defined")
SHORT TrnWriteFile_Special(ULONG ulOffset, VOID *pData,
                   ULONG ulSize, PifFileHandle hsFileHandle);

SHORT TrnWriteFile_Debug(ULONG ulOffset, VOID *pData,
                   ULONG ulSize, PifFileHandle hsFileHandle,
				   CONST char *aszFilePath, int nLineNo)
{
    SHORT   sReturn;
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

    sReturn = TrnWriteFile_Special(ulOffset, pData, ulSize, hsFileHandle);

	if (sReturn != (TRN_SUCCESS)) {
		sprintf (xBuffer, "TrnReadFile_Debug(): Error = %d, File %s, lineno = %d", sReturn, aszFilePath + iLen, nLineNo);
		NHPOS_ASSERT_TEXT(0, xBuffer);
	}

	return sReturn;
}

SHORT TrnWriteFile_Special(ULONG ulOffset, VOID *pData,
                   ULONG ulSize, PifFileHandle hsFileHandle)
#else
SHORT TrnWriteFile(ULONG ulOffset, VOID *pData,
                  ULONG ulSize, PifFileHandle hsFileHandle)
#endif
{
    SHORT   sReturn;
    ULONG   ulActPos;
//    USHORT  usFSize;
//	ULONG	ulActualBytesRead; //For PifReadFile 11-7-3

    if (ulSize == 0) {              /* check writed size */
        return 0;                     /* if nothing, return */
    }

//    sReturn = TrnSeekFile( hsFileHandle, 0UL, &ulActPos );
//    sReturn = PifReadFile( hsFileHandle, &usFSize, sizeof( USHORT ), &ulActualBytesRead);
    sReturn = TrnSeekFile( hsFileHandle, ulOffset, &ulActPos);

    PifWriteFile(hsFileHandle, pData, ulSize);

	return sReturn;
}

#if defined(TrnReadWriteFile_Memory)

SHORT TrnWriteFile_Memory (ULONG ulOffset, VOID *pData,
                  ULONG ulSize, SHORT hsFileHandle)
{
	SHORT   sRetStatus;
	UCHAR  *pTrnMemoryFile;
	TrnMemoryFileTableRow  *pMemoryTable;

	NHPOS_ASSERT_TEXT((hsFileHandle >= 0), "TrnWriteFile_Memory(): Bad file handle.");

	for (pMemoryTable = TrnMemoryFileTable; pMemoryTable->pTrnMemoryFile != 0; pMemoryTable++) {
		if (pMemoryTable->hsFileHandle == hsFileHandle) {
			pTrnMemoryFile = pMemoryTable->pTrnMemoryFile;
			break;
		}
	}

	sRetStatus = TrnWriteFile_Special(ulOffset, pData, ulSize, hsFileHandle);

	if (pMemoryTable->pTrnMemoryFile == 0) {
		return sRetStatus;
	}

	if (ulOffset + ulSize < TRNMEMORYFILE_SIZE) {
		memcpy (pTrnMemoryFile + ulOffset, pData, ulSize);
	}
	return sRetStatus;
}
#endif

/*
*==========================================================================
**    Synopsis: VOID    TrnCloseFile(SHORT hsFileHandle)
*                                                  
*       Input:  SHORT   hsFileHandle - File Handle 
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*                                                                  
**  Description:
*           Close specified files.
*==========================================================================
*/
#if defined(TrnCloseFile)
#pragma message("TrnCloseFile defined")
VOID TrnCloseFile_Special(SHORT hsFileHandle);

VOID TrnCloseFile_Debug(SHORT hsFileHandle, CONST char *aszFilePath, int nLineNo)
{
    SHORT   sReturn = (TRN_SUCCESS);
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

    TrnCloseFile_Special(hsFileHandle);

	if (sReturn != (TRN_SUCCESS)) {
		sprintf (xBuffer, "TrnCloseFile_Debug(): Error = %d, File %s, lineno = %d", sReturn, aszFilePath + iLen, nLineNo);
		NHPOS_ASSERT_TEXT(0, xBuffer);
	}

}

VOID TrnCloseFile_Special(SHORT hsFileHandle)
#else
VOID TrnCloseFile(SHORT hsFileHandle)
#endif
{
    PifCloseFile(hsFileHandle);                           
}    

#if defined(TrnReadWriteFile_Memory)

VOID TrnCloseFile_Memory (SHORT hsFileHandle)
{
	TrnMemoryFileTableRow  *pMemoryTable;

	NHPOS_ASSERT_TEXT((hsFileHandle >= 0), "TrnCloseFile_Memory(): Bad file handle.");

	for (pMemoryTable = TrnMemoryFileTable; pMemoryTable->pTrnMemoryFile != 0; pMemoryTable++) {
		if (pMemoryTable->hsFileHandle == hsFileHandle) {
			TrnCloseFile_Special(pMemoryTable->hsFileHandle);
			pMemoryTable->hsFileHandle = -1;
			break;
		}
	}

	if (pMemoryTable->pTrnMemoryFile == 0) {
		TrnCloseFile_Special(hsFileHandle);
	}
}

#endif

/*
*==========================================================================
**    Synopsis: VOID    TrnDeleteFile(UCHAR FAR *uchFileName)
*                                                                                
*       Input:  UCHAR FAR *uchFileName - pointer of file name 
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*                                                                  
**  Description:
*           Delete specified files.
*==========================================================================
*/
#if defined(TrnDeleteFile)
#pragma message("TrnDeleteFile defined")
VOID TrnDeleteFile_Special(CONST TCHAR *uchFileName);

VOID TrnDeleteFile_Debug(CONST TCHAR *uchFileName, CONST char *aszFilePath, int nLineNo)
{
    SHORT   sReturn = (TRN_SUCCESS);
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

	sprintf (xBuffer, "TrnDeleteFile_Debug(): FileName = %S, File %s, lineno = %d", uchFileName, aszFilePath + iLen, nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);

    TrnDeleteFile_Special(uchFileName);
	return sReturn;
}

VOID TrnDeleteFile_Special(CONST TCHAR *uchFileName)
#else
VOID TrnDeleteFile(CONST TCHAR *uchFileName)
#endif
{
    PifDeleteFile(uchFileName);                           
}

/*
*===========================================================================
** Synopsis:    USHORT  TrnConvertError(SHORT sError)
*                                
*       Input:  SHORT   sError - transaction module internal error
*      Output:  Nothing
*       InOut:  Nothing
*
** Return:      Leadthru No 
*
** Description: convert error code to leadthru number.
*===========================================================================
*/
#if defined(TrnConvertError)
#pragma message("TrnConvertError defined")
USHORT TrnConvertError_Special(SHORT sError);

USHORT TrnConvertError_Debug(SHORT sError, CONST char *aszFilePath, int nLineNo)
{
    SHORT   sReturn = (TRN_SUCCESS);
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

	sprintf (xBuffer, "TrnConvertError_Debug(): sError = %d, File %s, lineno = %d", sError, aszFilePath + iLen, nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);

    return TrnConvertError_Special(sError);
}

USHORT TrnConvertError_Special(SHORT sError)
#else
USHORT TrnConvertError(SHORT sError)
#endif
{
    USHORT  usLeadthruNo;

    switch ( sError ) {
    case TRN_SUCCESS:                       /* Success 0 */
        usLeadthruNo = 0; 
        break;
                                        
    case TRN_ERROR:
        usLeadthruNo = LDT_FLFUL_ADR;       /* 2 * fill full */
        break;
                                        
    default:    
        usLeadthruNo = LDT_ERR_ADR;         /* 21 * Error(Global) */
        break;
    }

	if (sError != TRN_SUCCESS) {
		PifLog (MODULE_TRANS_BODY, LOG_EVENT_TRN_CONVERTERROR);
		PifLog (MODULE_ERROR_NO(MODULE_TRANS_BODY), (USHORT)(sError * (-1)));
		PifLog (MODULE_ERROR_NO(MODULE_TRANS_BODY), usLeadthruNo);
	}

    return(usLeadthruNo);           
}

/*
*===========================================================================
** Synopsis:    USHORT TrnCalStoSize( USHORT usNoOfItem,
*                                     UCHAR  uchStorageType )
*
*   Input:      USHORT usNoOfItem     - number of items in storage file
*               UCHAR  uchStorageType - type of storage file
*                       FLEX_ITEMSTORAGE_ADR, FLEX_CONSSTORAGE_ADR
*   Output:     Nothing
*   InOut:      Nothing
*
** Return:      size of storage file in bytes.
*
** Description: calculate size of storage file with specified no. of items.
*               See also functions MldCalStoSize() and FDTCalStoSize()
*               which perform a similar calculation.
*
*               See also function TrnCheckSize() which checks that a transaction
*               storage file has enough space to hold a new item being added.
*
*               See also function Gcf_GetNumberofBlockAssign() which uses this
*               to determine the number of Guest Check file blocks needed to
*               store a transaction.
*===========================================================================
*/
ULONG TrnCalStoSize( ULONG ulNoOfItem, UCHAR uchStorageType )
{
    ULONG  ulHeaderSize;
    ULONG  ulActualSize;
    ULONG  ulIndexSize;

    switch ( uchStorageType ) {

    case FLEX_ITEMSTORAGE_ADR:
        ulHeaderSize = sizeof( TRANITEMSTORAGEHEADER );
        ulIndexSize  = 0;
        break;

    case FLEX_CONSSTORAGE_ADR:
        ulHeaderSize = sizeof( TRANCONSSTORAGEHEADER );
        ulIndexSize  = sizeof( PRTIDXHDR ) + ( sizeof( PRTIDX ) * ( ulNoOfItem + PRTIDX_RESERVE ));
        break;

    default:
		NHPOS_ASSERT_TEXT(0, "**WARNING: TrnCalStoSize() invalid uchStorageType specified.");
        return ( TRN_ERROR );
    }

	// round up the predicted actual size in case there is some partial block used.
    ulActualSize = ( ulHeaderSize + ulIndexSize + TRN_STOR_BASE_BYTES + ( TRN_1ITEM_BYTES * ulNoOfItem ) - 1 );
    ulActualSize = ( (ulActualSize / TRN_STOSIZE_BLOCK) + 1 ) * TRN_STOSIZE_BLOCK;

	if (ulActualSize > 0xffff) {
		// most places exect the file size to be a USHORT so if the value is larger
		// than a USHORT, log the fact and set the size to the max USHORT.
		// we do the bitwise or so that 
		PifLog (MODULE_TRANS_BODY, LOG_EVENT_TRN_CALSTOSIZE);
		NHPOS_ASSERT_TEXT((ulActualSize <= 0xffff), "**WARNING: TrnCalStoSize() ulActualSize > 0xffff");
		ulActualSize |= 0xffff;
	}

    return ( ulActualSize );
}

/*
*===========================================================================
** Synopsis:    SHORT   TrnTaxSystem(VOID)
*
*   Input:      
*   Output:     Nothing
*   InOut:      Nothing
*
** Return:      
** Description: Return Tax System
*===========================================================================
*/
SHORT   TrnTaxSystem(VOID)
{
    if (TrnInformation.TranModeQual.fsModeStatus & MODEQUAL_INTL) {
        return(TRN_TAX_INTL);
    } else if (TrnInformation.TranModeQual.fsModeStatus & MODEQUAL_CANADIAN) {
        return(TRN_TAX_CANADA);
    } else {
        return(TRN_TAX_US);
    }
}

/*
*==========================================================================
*    Synopsis:  VOID    TrnTransCloseLoanPicup(VOID)
*
*    Input:     none
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   Generate loan pickp Affection Data. 09/08/00
*==========================================================================
*/
VOID    TrnTransCloseLoanPickup(ITEMTRANSCLOSE *ItemTransClose)
{
	ITEMAFFECTION    TrnAffect = {0};

    TrnAffect.uchMajorClass = CLASS_ITEMAFFECTION;
    TrnAffect.uchMinorClass = (ItemTransClose->uchMinorClass == CLASS_CLS_LOAN) ? CLASS_LOANCLOSE : CLASS_PICKCLOSE;
    TrnStoAffect(&TrnAffect);
}


/*
*===========================================================================
** Format  : USHORT   TrnPrtGetSlipPageLine(VOID)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : 
*
** Synopsis: 
*===========================================================================
*/
USHORT   TrnPrtGetSlipPageLine(VOID)
{
    return (PrtGetSlipPageLine());         /* return current slip line & page */
}

/*
*===========================================================================
** Format  : USHORT   TrnPrtSetSlipPageLine(USHORT usLine)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : 
*
** Synopsis: 
*===========================================================================
*/
VOID   TrnPrtSetSlipPageLine(USHORT usLine)
{
    PrtSetSlipPageLine(usLine);
}


/****** End of Source ******/
