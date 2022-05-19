/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Decompress storage data and get 1 item
* Category    : Reentrant Finctions Library, NCR 2170 US Hospitality
* Program Name: RflGetIt.c
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-18-92 : 00.00.01 : K.Nakajima :                                    
* Jun-30-93 : 01.00.12 : K.You      : mod. RflGetStorageItem for US enhance.                                  
* May-20-94 : 02.05.00 : K.You      : bug fixed S-23 (mod. RflGetStorageItem)
* May-25-94 : 02.05.00 : K.You      : stop to fixed S-23
* Apr-04-95 : 03.00.00 : T.Nakahata : Add Coupon Feature
* Apr-14-95 : 03.00.00 : T.Nakahata : bug fixed (CHAR* -> UCHAR*)
* Apr-01-96 : 03.01.02 : M.Ozawa    : bug fixed (add print priority of condiment)
* Apr-26-96 : 03.01.04 : M.Ozawa    : bug fixed (not plu search if plu no is directed)
* 
*** NCR2171 **
* Aug-30-99 : 01.00.00 : M.Teraki   : initial (for Win32)
* Jan-28-15 : 02.02.01 : R.Chambers : new functions, source cleanup, use Major Class for RflUnZip() buff size.
* Sep-25-17 : 02.02.02 : R.Chambers : use CONST in function interfaces, modify table ItemSizeTable, updated comments.
* Sep-25-17 : 02.02.02 : R.Chambers : added new function RflGetItemStorageClassLen() for use later.
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
/** -- M S -  C -- **/
#include  <tchar.h>
#include  <string.h>
#include  <stdio.h>

/** ---- 2170 local ---- **/
#include <ecr.h>
#include <pif.h>
#include <regstrct.h>
#include <transact.h>
#include <trans.h> 
#include <csop.h>
#include <rfl.h>
#include <csstbopr.h>
#include "paraequ.h"
#include "para.h"
#include "csstbpar.h"
#include "rflin.h"

/**
;========================================================================
;+                      S T A T I C    R A M s
;========================================================================
**/
/* CONST UCHAR FARCONST  aszRflPLUNo[] = "PLU%04d";  / trans.mnem and amount */

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/

/*
*===========================================================================
** Format  : int  RflIsSpecialPlu((TCHAR *auchPLUNo, TCHAR *auchMldSpecialPlu);
*
*   Output : 
*    Input : TCHAR    auchPLUNo          - 14 digit PLU number
*            TCHAR    auchMldSpecialPlu  - 14 digit PLU number
*    InOut : none
** Return  : int       commparison result against auchMldSpecialPlu
*                      < 0 - auchPLUNo less than auchMldSpecialPlu
*                      = 0 - auchPLUNo equals auchMldSpecialPlu
*                      > 0 - auchPLUNo greater than auchMldSpecialPlu
*
** Synopsis: There are a fair number of cases in which a 14 digit PLU number auchPLUNo[]
*            is compared against a dummmy value of all binary zeros to determine if a PLU
*            number actually exists or not. This function is being provided in order to
*            replace all those instances of source like the following with a simple call
*            to this function instead.
*                   TCHAR    auchDummy[NUM_PLU_LEN] = {0};   // dummy PLU for comparison
*                   
*                   for (i = 0; i < usMaxChild; i++) {
*                       // check condiment PLU exist or not
*                       if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
*                           continue;
*                       }
*
*           will change to the following:
*                   for (i = 0; i < usMaxChild; i++) {
*                       // check condiment PLU exist or not
*                       if (RflIsSpecialPlu(pItem->Condiment[i].auchPLUNo, MLD_NO_DISP_PLU_DUMMY) == 0) {
*                           continue;
*                       }
*           See also comments for special PLU values of:
*             - MLD_NO_DISP_PLU_LOW
*             - MLD_NO_DISP_PLU_HIGH
*             - MLD_NO_DISP_PLU_DUMMY
*             - MLD_NONSALESITEM_PLU
*===========================================================================
*/
int RflIsSpecialPlu (CONST TCHAR *auchPLUNo, CONST TCHAR *auchMldSpecialPlu)
{
	if (! auchMldSpecialPlu)
		auchMldSpecialPlu = MLD_NO_DISP_PLU_DUMMY;

	return _tcsncmp(auchPLUNo, auchMldSpecialPlu, NUM_PLU_LEN);
}

TCHAR * RflCopyPlu (TCHAR *auchPluNoDest, CONST TCHAR *auchPluNoSrc)
{
	return _tcsncpy (auchPluNoDest, auchPluNoSrc, NUM_PLU_LEN);
}

/*
*===========================================================================
** Format  : int  RflChkSpecialPluRange (TCHAR *auchPLUNo, TCHAR *auchMldSpecialPluLow, TCHAR *auchMldSpecialPluHigh);
*
*   Output : 
*    Input : TCHAR    auchPLUNo              - 14 digit PLU number
*            TCHAR    auchMldSpecialPluLow   - 14 digit PLU number
*            TCHAR    auchMldSpecialPluHigh  - 14 digit PLU number
*    InOut : none
** Return  : int       commparison result against auchMldSpecialPluLow and auchMldSpecialPluHigh
*                      = 0 - auchPLUNo is not in the range specified
*                      = 1 - auchPLUNo is in the range specified
*
** Synopsis: There are a fair number of cases in which a 14 digit PLU number auchPLUNo[]
*            is compared against a range of special PLU numbers to determine if the PLU number
*            should be treated in a manner other than a standard PLU number. This function is being
*            provided in order to replace all those instances of source like the following with a simple call
*            to this function instead.*                   
*                   for (i = 0; i < usMaxChild; i++) {
*                       // check condiment PLU exist or not
*                       if (_tcsncmp(pItem->Condiment[i].auchPLUNo, MLD_NO_DISP_PLU_LOW, NUM_PLU_LEN) >= 0 &&
*                           _tcsncmp(pItem->Condiment[i].auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) <= 0) {
*                           continue;
*                       }
*
*           will change to the following:
*                   for (i = 0; i < usMaxChild; i++) {
*                       // check condiment PLU exist or not
*                       if (RflChkSpecialPluRange(pItem->Condiment[i].auchPLUNo, MLD_NO_DISP_PLU_LOW, MLD_NO_DISP_PLU_HIGH)) {
*                           continue;
*                       }
*           See also comments for special PLU values of:
*             - MLD_NO_DISP_PLU_LOW
*             - MLD_NO_DISP_PLU_HIGH
*             - MLD_NO_DISP_PLU_DUMMY
*             - MLD_NONSALESITEM_PLU
*===========================================================================
*/
int RflChkSpecialPluRange (CONST TCHAR *auchPLUNo, CONST TCHAR *auchMldSpecialPluLow, CONST TCHAR *auchMldSpecialPluHigh)
{
	int iResult = 0;  // default returned is false.

	if (auchMldSpecialPluLow && auchMldSpecialPluHigh) {
		iResult = (_tcsncmp(auchPLUNo, auchMldSpecialPluLow, NUM_PLU_LEN) >= 0 &&
					_tcsncmp(auchPLUNo, auchMldSpecialPluHigh, NUM_PLU_LEN) <= 0);
	} else if (auchMldSpecialPluLow) {
		iResult = (_tcsncmp(auchPLUNo, auchMldSpecialPluLow, NUM_PLU_LEN) >= 0);
	} else if (auchMldSpecialPluHigh) {
		iResult = (_tcsncmp(auchPLUNo, auchMldSpecialPluHigh, NUM_PLU_LEN) <= 0);
	}

	return iResult;
}

/*
*===========================================================================
** Format  : int  RflChkNoDisplayRange (TCHAR *auchPLUNo;
*
*   Output : 
*    Input : TCHAR    auchPLUNo              - 14 digit PLU number
*    InOut : none
** Return  : int       commparison result against MLD_NO_DISP_PLU_LOW and MLD_NO_DISP_PLU_HIGH
*                      = 0 - auchPLUNo is not in the range specified
*                      = 1 - auchPLUNo is in the range specified
*
** Synopsis: Check that a PLU number is in the Do Not Display PLU range of PLU numbers.
*            These are special PLU numbers that are used for underlying logic and are
*            not intended to be printed on a receipt or displayed on the terminal.
*            PLU numbers in this range are used to change the behavior of GenPOS.
*===========================================================================
*/
int RflChkIfNoDisplayRange (CONST TCHAR *auchPLUNo)
{
	return RflChkSpecialPluRange (auchPLUNo, MLD_NO_DISP_PLU_LOW, MLD_NO_DISP_PLU_HIGH);
}

/*
*===========================================================================
** Format  : VOID  RflGetPLUMnemo(UCHAR  *pszPLUMnem, USHORT  usPLUNo );
*
*   Output : UCHAR     *pszPLUMnem  -plu mnemonics
*    Input : USHORT    usPLUNo      -plu number
*   Output : UCHAR     *pszPLUMnem  -plu mnemonics
*    InOut : none
** Return  : none  
*
** Synopsis: This function provides the PLU mnemonic.
*            The function first tries to satisfy the PLU mnemonic request from the
*            PLU mnemonic cache in order to reduce the amount of overhead and delays
*            involved in talking to the Master Terminal, if we are a Satellite Terminal,
*            or accessing the PLU data base file.
*            If the PLU number is not in the cache then we do the fetching of the PLU
*            data from the PLU database file.
*===========================================================================
*/
static VOID RflGetPLUMnemo(TCHAR  *pszPLUMnem, TCHAR  *pszPLUMnemAlt, TCHAR  *auchPLUNo)
{
    PLUIF     Plif = {0};         /* for getting plu mnemonics */
	TCHAR     auchDummy[NUM_PLU_LEN + 1] = {0};

    if (_tcsncmp(auchPLUNo, auchDummy, NUM_PLU_LEN) != 0) {      /* to avoid storage search error 04/26/96 */
		/* 05/20/01 */

		// perform a lookup to see if we can retrieve the PLU mnemonic from the cache or not.
		if (TrnItemGetPLUMnemo(pszPLUMnem, pszPLUMnemAlt, auchPLUNo) == TRUE) return;

		// the PLU number is not in the cache so we will do the actual retrieve of the PLU information.
		// if the PLU retrieve works then we will update our cache for the future.

        _tcsncpy(Plif.auchPluNo, auchPLUNo, NUM_PLU_LEN);
        Plif.uchPluAdj  = 1;
        if (OP_PERFORM != CliOpPluIndRead(&Plif, 0) ) {
            /* set the PLU mnemonic as the PLU number if PLU record is not found */
            RflConvertPLU(Plif.ParaPlu.auchPluName, auchPLUNo);	/* not print c/d of velocity no, 09/11/01 */
            RflConvertPLU(Plif.ParaPlu.auchAltPluName, auchPLUNo);	/* not print c/d of velocity no, 09/11/01 */
        } else {
			TCHAR     auchPLUMnemo[STD_PLU_MNEMONIC_LEN + 1] = {0};
			TCHAR     auchPLUMnemoAlt[STD_PLU_MNEMONIC_LEN + 1] = {0};
			/* 05/20/01 */
			/* TAR 180320 */
			_tcsncpy(auchPLUMnemo, Plif.ParaPlu.auchPluName, STD_PLU_MNEMONIC_LEN);
			_tcsncpy(auchPLUMnemoAlt, Plif.ParaPlu.auchAltPluName, STD_PLU_MNEMONIC_LEN);
			TrnItemStoPLUMnemo(auchPLUMnemo, auchPLUMnemoAlt, auchPLUNo);    // update the cache with the mnemonic.
		}
    }

    if (pszPLUMnem) _tcsncpy( pszPLUMnem, Plif.ParaPlu.auchPluName, STD_PLU_MNEMONIC_LEN);
    if (pszPLUMnemAlt) _tcsncpy( pszPLUMnemAlt, Plif.ParaPlu.auchAltPluName, STD_PLU_MNEMONIC_LEN);
}

/*
*===========================================================================
** Format  : VOID  RflGetDeptMnemo(UCHAR  *pszDeptMnem, UCHAR  uchDeptNo);
*
*    Input : UCHAR     uchDeptNo      -Dpet number
*   Output : UCHAR     *pszDeptMnem   -Dept mnemonics
*    InOut : none
** Return  : none  
*
** Synopsis: This function read Dept mnemonic. 2172
*===========================================================================
*/
static VOID RflGetDeptMnemo(TCHAR  *pszDeptMnem, USHORT  usDeptNo)
{
	DEPTIF    Dpif = {0};

    Dpif.usDeptNo    = usDeptNo;
    CliOpDeptIndRead(&Dpif, 0);

    _tcsncpy(pszDeptMnem, Dpif.ParaDept.auchMnemonic, OP_DEPT_NAME_SIZE);
}

static struct {
	UCHAR   uchMajorClass;      // major class which identifies this item type
	USHORT  usItemSize;         // uncompressed data struct for the major class, size in bytes
	USHORT  usItemNoCmpSize;    // non-compressed portion of transaction record, size in bytes
	USHORT  usItemCmpSize;      // compressed portion of transaction record, size in bytes
} ItemSizeTable[] = {
	{CLASS_ITEMOPEOPEN, sizeof(ITEMOPEOPEN), 0, 0},
	{CLASS_ITEMTRANSOPEN, sizeof(ITEMTRANSOPEN), sizeof(TRANSTORAGETRANSOPENNON), sizeof(TRANSTORAGETRANSOPEN)},
	{CLASS_ITEMSALES, sizeof(ITEMSALES), sizeof(TRANSTORAGESALESNON), sizeof(TRANSTORAGESALES)},
	{CLASS_ITEMDISC, sizeof(ITEMDISC), sizeof(TRANSTORAGEDISCNON), sizeof(TRANSTORAGEDISC)},
	{CLASS_ITEMCOUPON, sizeof(ITEMCOUPON), sizeof(TRANSTORAGECOUPONNON), sizeof(TRANSTORAGECOUPON)},
	{CLASS_ITEMTOTAL, sizeof(ITEMTOTAL), sizeof(TRANSTORAGETRANSMININON), sizeof(TRANSTORAGETOTAL)},
	{CLASS_ITEMTENDER, sizeof(ITEMTENDER), sizeof(TRANSTORAGETENDERNON), sizeof(TRANSTORAGETENDER)},
	{CLASS_ITEMMISC, sizeof(ITEMMISC), sizeof(TRANSTORAGETRANSMININON), sizeof(TRANSTORAGEMISC)},
	{CLASS_ITEMMODIFIER, sizeof(ITEMMODIFIER), 0, 0},
	{CLASS_ITEMOTHER, sizeof(ITEMOTHER), sizeof(TRANSTORAGETRANSMININON), sizeof(TRANSTORAGEOTHER)},
	{CLASS_ITEMAFFECTION, sizeof(ITEMAFFECTION), sizeof(TRANSTORAGETRANSMININON), sizeof(TRANSTORAGEAFFECTION)},
	{CLASS_ITEMPRINT, sizeof(ITEMPRINT), sizeof(TRANSTORAGETRANSMININON), sizeof(TRANSTORAGEPRINT)},
	{CLASS_ITEMOPECLOSE, sizeof(ITEMOPECLOSE), sizeof(TRANSTORAGETRANSMININON), sizeof(TRANSTORAGEOPECLOSE)},
	{CLASS_ITEMTRANSCLOSE, sizeof(ITEMTRANSCLOSE), 0, 0},
	{CLASS_ITEMMODEIN, sizeof(ITEMREGMODEIN), 0, 0},
	{CLASS_ITEMMULTI, sizeof(ITEMMULTI), sizeof(TRANSTORAGETRANSMININON), sizeof(TRANSTORAGEMULTICHECK)},
	{CLASS_ITEMTENDEREC, sizeof(ITEMTENDER), sizeof(TRANSTORAGETENDERNON), sizeof(TRANSTORAGETENDER)},
	{CLASS_ITEMPREVCOND, sizeof(ITEMSALES), sizeof(TRANSTORAGESALESNON), sizeof(TRANSTORAGESALES)}
};


/*
 *
 *  Returns:     sizeof() the item data struct for the specific uchMajorClass or
 *               zero (0) if there is no match for the uchMajorClass.
 *
 *  Description: The following couple of functions are used to determine from
 *               the data provided the item class of a data item and the size of
 *               the data buffer used to contain the uncompressed item data.
 *
 *               RflGetItemClassLen() accepts the Major Class and returns the size
 *               of the struct used for that item data type when the item is uncompressed.
 *
 *               RflGetStorageItemClassLen() accepts a pointer to a data buffer that may
 *               be either uncompressed item data or compressed item data and then uses
 *               the correct offset to the uchMajorClass in the non-compressed portion of
 *               a transaction record which corresponds to the first two elements of every
 *               ITEM struct type to determine the ITEM struct size.
 *               By ITEM struct size is meant the size of a struct such as ITEMSALES, ITEMDISC, etc.
 *               and not the transaction record storage size.
*/
static USHORT  RflGetItemClassLen (UCHAR  uchMajorClass)
{
	USHORT  usBufferSize = 0;
	int     i;

	for (i = 0; i < ARRAYSIZEOF(ItemSizeTable); i++) {
		if (uchMajorClass == ItemSizeTable[i].uchMajorClass) {
			usBufferSize = ItemSizeTable[i].usItemSize;
			break;
		}
	}

#if  defined(_DEBUG) || defined(DEBUG)
	// this NHPOS_ASERT() is also triggered by a CANCEL key so lets don't clutter up the ASSRTLOG
	// in the field when the Operator does CANCEL.
	NHPOS_ASSERT(usBufferSize > 0);
#endif
	return usBufferSize;
}

// determine item data struct size for uncompressed data by specifying
// a pointer to a buffer containing data with a pointer starting to the
// first byte of the item data. For instance this may be either a buffer
// of ITEMSALES or TRANSTORAGESALESNON since the first part of either of
// these structs are non-compressed data with the uchMajorClass, etc.
USHORT  RflGetStorageItemClassLen (CONST VOID *pItemData)
{
	return RflGetItemClassLen (ITEMSTORAGENONPTR(pItemData)->uchMajorClass);
}

// determine item data struct size for a transaction record that is read
// from the transaction file. offset of one (1) is to step over non-compressed
// data length to get to the uchMajorClass element of the non-compressed data.
USHORT  RflGetStorageTranRecordItemClassLen (CONST VOID *pTranRecData)
{
	return RflGetItemClassLen (TRANSTORAGEDATAPTR(pTranRecData)->uchMajorClass);
}

// determine the length of a transaction record fetched from a transaction data file.
// these records contain both the non-compressed and compressed data segments.
USHORT  RflGetStorageTranRecordLen (CONST VOID *pTranRecData)
{
	CONST UCHAR *puchWork = pTranRecData;

    /*----- Increment Read Pointer -----*/
    return (*puchWork + *(USHORT *)(puchWork + *puchWork));
}

// determine item data struct size for raw data as read from a transaction data file.
// the raw data record contains in the first byte the length of the uncompressed data
// so we must step over the uncompressed data length to get to the first byte of the
// uncompressed data which is the uchMajorClass specifier indicate the type of data
// contained in the record.
typedef struct {
	USHORT  usItemLen;      // length when uncompressed or size of ITEM type struct needed for this data such as sizeof(ITEMSALES)
	USHORT  usNonCmpLen;    // length of struct of uncompressed segment of a compressed item record or size of trans type struct such as sizeof(TRANSTORAGESALESNON)
	USHORT  usCmpLen;       // length of struct of compressed segment of a compressed item record (the data when uncompressed is of TRANS type struct such as sizeof(TRANSTORAGESALES))
	USHORT  usOffsetToCmp;  // offset in bytes to beginning of compressed segment of compressed item record
	USHORT  usTotalLen;     // used for offset of next record. total length of the compressed item record adding both non-compressed and compressed lengths together.
} RflRawStorageClassLen;

#if 0

static RflRawStorageClassLen  RflGetItemStorageClassLen (UCHAR  uchMajorClass, UCHAR *uchBuffer)
{
	RflRawStorageClassLen  len = {0};
	int     i;

	for (i = 0; i < ARRAYSIZEOF(ItemSizeTable); i++) {
		if (uchMajorClass == ItemSizeTable[i].uchMajorClass) {
			len.usItemLen = ItemSizeTable[i].usItemSize;           // size of the struct for this Major Class
			len.usNonCmpLen = ItemSizeTable[i].usItemNoCmpSize;    // size of the struct for non-compressed part of transaction record
			len.usCmpLen = ItemSizeTable[i].usItemCmpSize;         // size of the struct for compressed part of transaction record
			len.usOffsetToCmp = len.usNonCmpLen + sizeof(USHORT);  // non-compressed struct length plus size of USHORT value of compressed length
			len.usTotalLen = 0;   // this value must be calculated after compressing a particular item record
			break;
		}
	}

#if  defined(_DEBUG) || defined(DEBUG)
	// this NHPOS_ASERT() is also triggered by a CANCEL key so lets don't clutter up the ASSRTLOG
	// in the field when the Operator does CANCEL.
	NHPOS_ASSERT(len.usNonCmpLen > 0);
#endif
	return len;
}
#endif

RflRawStorageClassLen  RflGetRawStorageClassLen (CONST VOID *pRawData)
{
	CONST UCHAR  *puchRead = pRawData;
	RflRawStorageClassLen  len = {0};

	// calculate or retrieve the various useful lengths for a compressed data record.
	// a compressed data record is the data record as stored in a transaction data file.
	// it is composed of a non-compressed segment and a compressed segment. the length
	// stored for the compressed segment includes the size of the part of the record
	// containing the compressed segment length which is a USHORT so we need to adjust for that.
	len.usNonCmpLen = puchRead[0];                               // offset in bytes to start of length of the non-compressed data part of the record
	len.usCmpLen = *( (USHORT *)( &puchRead[puchRead [0]] ) );   // number of bytes of the compressed data part of the record
	len.usTotalLen = len.usNonCmpLen + len.usCmpLen;             // total record length in bytes, offset to following record. See TrnSplRetLength()
	len.usOffsetToCmp = len.usNonCmpLen + sizeof( USHORT );      // offset in bytes to where compressed data starts after the length field
	if (len.usCmpLen > sizeof(USHORT)) {
		// adjust our returned compressed length to remove size of the area containing the length.
		len.usCmpLen -= sizeof( USHORT );
	}
	if (len.usNonCmpLen > sizeof(UCHAR)) {
		// adjust our returned non-compressed length to remove size of the area containing the length.
		len.usNonCmpLen -= sizeof( UCHAR );
	}
	// we use a default of ITEMSALES because so much of the source just allocates an ITEMSALES
	// struct to receive the uncompressed item data by default. ITEMSALES was once the largest of
	// the various item data types.
	len.usItemLen = RflGetItemClassLen (puchRead[1]);
	if (len.usItemLen == 0) len.usItemLen = sizeof(ITEMSALES);

	return len;
}

static VOID RflGetSalesItem( VOID *puchDest, CONST UCHAR *puchRead, SHORT sType, RflRawStorageClassLen *precLen )
{
	ITEMSALES          *pItemSales = puchDest;
	TRANSTORAGESALES    stsale = {0};     /* buffer for decompress */ 
    USHORT              i;                /* */ 

	/* decompress data  */                                          
	RflUnZip(&stsale, sizeof(stsale), puchRead + precLen->usOffsetToCmp, precLen->usCmpLen);

	/* --  formats structure ITEMSALES -- */
	pItemSales->lUnitPrice   = stsale.lUnitPrice;
	pItemSales->uchAdjective = stsale.uchAdjective;  
	pItemSales->fbModifier   = stsale.fbModifier;    
	pItemSales->fbModifier2   = stsale.fbModifier2;    
	memcpy( pItemSales->aszNumber, stsale.aszNumber, sizeof(stsale.aszNumber));
                                           
	pItemSales->ControlCode = stsale.ControlCode;
	pItemSales->uchPM = stsale.uchPM;                 /* price multiple, 2172 */
	pItemSales->usLinkNo = stsale.usLinkNo;           /* link no. 2172 */
	pItemSales->usFamilyCode = stsale.usFamilyCode;   /* family code 2172 */
	pItemSales->uchTableNumber = stsale.uchTableNumber;
	pItemSales->uchDiscountSignifier = stsale.uchDiscountSignifier;
	pItemSales->usFor = stsale.usFor;
	pItemSales->usForQty = stsale.usForQty;

	pItemSales->uchColorPaletteCode = stsale.uchColorPalette;	//colorpalette-changes
	pItemSales->uchHourlyOffset = stsale.uchHourlyOffset;	//Hourly total block offset
	pItemSales->usTareInformation = stsale.usTareInformation;	//tare information for specific PLU.
	pItemSales->blPrecedence = stsale.blPrecedence;	        //Precedence allowed?
	pItemSales->usReasonCode = stsale.usReasonCode;	//a reason code associated with this operator action.

	memset (pItemSales->aszMnemonic, 0, sizeof (pItemSales->aszMnemonic));          // NUM_DEPTPLU
	memset (pItemSales->aszMnemonicAlt, 0, sizeof (pItemSales->aszMnemonicAlt));    // NUM_DEPTPLU

	/* -- get mnemonics -- */
	if ( pItemSales->uchMinorClass == CLASS_DEPT
		 || pItemSales->uchMinorClass == CLASS_DEPTITEMDISC
		 || pItemSales->uchMinorClass == CLASS_DEPTMODDISC ) {

		if (sType == RFL_WITH_MNEM) {
			RflGetDeptMnemo(pItemSales->aszMnemonic, pItemSales->usDeptNo);
		}
	} else if ( pItemSales->uchMinorClass == CLASS_ITEMORDERDEC) {
		// if this is an Order Declare then we are going to return the Order Declare mnemonic as the mnemonic
		// we do this regardless of whether sType is RFL_WITH_MNEM or RFL_WITHOUT_MNEM since the data is readily
		// available and does not require any kind of a lookup.
		memcpy( pItemSales->aszMnemonic, &(stsale.aszNumber[STD_OD_ASZNUMBER_INDEX]), sizeof(pItemSales->aszMnemonic));
		pItemSales->aszNumber[STD_OD_ASZNUMBER_INDEX][0] = 0;

		// In order to allow edit of an Order Declare by only a change of the uncompressed portion of the ITEMSALES
		// record, we are storing the Order Declare number in the usDeptNo field, which is part of the
		// uncompressed portion of the record, TRANSTORAGESALESNON.
		// On an umcompress of ITEMSALES data we must set uchAdjective with the current Order Declare number.
		//    Richard Chambers, Dec-21-2018
		pItemSales->uchAdjective = pItemSales->usDeptNo;
	} else {
		if (sType == RFL_WITH_MNEM) {
			RflGetPLUMnemo(pItemSales->aszMnemonic, pItemSales->aszMnemonicAlt, pItemSales->auchPLUNo);
		}
	}

	memcpy(pItemSales->auchPrintModifier, stsale.auchPrintModifier, sizeof(stsale.auchPrintModifier));
	pItemSales->fsPrintStatus           = stsale.fsPrintStatus;        
   
	if(stsale.uchCondNo > STD_MAX_COND_NUM)
	{
		char    aszErrorBuffer[128];
		sprintf(aszErrorBuffer,"uchCondNo above STD_NUM_COND_MAX, uchCondNo = %d",stsale.uchCondNo);
		NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
		stsale.uchCondNo = 0;
	}

	if(stsale.uchPrintModNo > STD_MAX_COND_NUM)
	{
		char    aszErrorBuffer[128];
		sprintf(aszErrorBuffer,"uchPrintModNo above STD_NUM_COND_MAX, uchCondNo = %d",stsale.uchPrintModNo);
		NHPOS_ASSERT_TEXT(0,aszErrorBuffer);
		stsale.uchPrintModNo = 0;
	}
	
	pItemSales->uchPrintModNo           = stsale.uchPrintModNo;        
	pItemSales->uchCondNo               = stsale.uchCondNo;            
	pItemSales->uchChildNo              = stsale.uchChildNo;

	/* -- get condiment -- */
	for (i = 0; i < (stsale.uchChildNo + stsale.uchCondNo + stsale.uchPrintModNo); i++) {
		_tcsncpy(pItemSales->Condiment[i].auchPLUNo, stsale.Condiment[i].auchPLUNo, NUM_PLU_LEN);
		pItemSales->Condiment[i].usDeptNo = stsale.Condiment[i].usDeptNo; /* 2172 */
		pItemSales->Condiment[i].uchPrintPriority = stsale.Condiment[i].uchPrintPriority; /* add */
		pItemSales->Condiment[i].lUnitPrice = stsale.Condiment[i].lUnitPrice;
		pItemSales->Condiment[i].uchAdjective = stsale.Condiment[i].uchAdjective;
		pItemSales->Condiment[i].uchCondColorPaletteCode = stsale.Condiment[i].uchCondColorPaletteCode;

		pItemSales->Condiment[i].ControlCode = stsale.Condiment[i].ControlCode;

		pItemSales->Condiment[i].aszMnemonic[0] = '\0';
		pItemSales->Condiment[i].aszMnemonicAlt[0] = '\0';
	}

	if (sType == RFL_WITH_MNEM) {
		for (i = 0; i < (stsale.uchChildNo + stsale.uchCondNo + stsale.uchPrintModNo); i++) {
			RflGetPLUMnemo(pItemSales->Condiment[i].aszMnemonic, pItemSales->Condiment[i].aszMnemonicAlt, pItemSales->Condiment[i].auchPLUNo);
		}
	}

	pItemSales->uchRate         = stsale.uchRate;
	pItemSales->lDiscountAmount = stsale.lDiscountAmount;
	pItemSales->fbStorageStatus = stsale.fbStorageStatus;
}


/*
*===========================================================================
** Format  : VOID  RflGetCouponItem( UCHAR *puchDest,
*                                    UCHAR *puchSource,
*                                    SHORT sType )
*
*    Input : UCHAR  *puchDest   :   address of destination buffer
*            UCHAR  *puchSource :   address of source item
*            SHORT  sType       :   RFL_WITH_MNEM
*                               :   RFL_WITHOUT_MNEM
*   Output : UCHAR  *puchDest   :   address of destination buffer
*    InOut : none
** Return  : none  
*
** Synopsis: This function decompress coupon data and stores it to destination
*            address, and retrieves coupon mnemonic with CliOp*** function.
*
*            See function TrnStoCouponComp() for the coupon compression.
*===========================================================================
*/
static VOID RflGetCouponItem( UCHAR *puchDest, CONST UCHAR *puchSource, SHORT sType, RflRawStorageClassLen *precLen )
{
    USHORT              usLockHnd;      /* lock handle for CliOp*** function */
	TRANSTORAGECOUPON   TrnCoupon = {0};      /* work buffer for decompressed data */
    ITEMCOUPON          *pCoupon = ( ITEMCOUPON * )puchDest;       /* address of destination buffer */

    /* --- decompress coupon data, and store to work buffer --- */
    RflUnZip( &TrnCoupon, sizeof( TrnCoupon ), (puchSource + precLen->usOffsetToCmp), precLen->usCmpLen);

    /* --- store decompressed data to destination buffer --- */
    pCoupon->lAmount            = TrnCoupon.lAmount;
    pCoupon->lDoubleAmount      = TrnCoupon.lDoubleAmount;
    pCoupon->lTripleAmount      = TrnCoupon.lTripleAmount;
    pCoupon->fbModifier         = TrnCoupon.fbModifier;
    pCoupon->fbStatus[ 0 ]      = TrnCoupon.fbStatus[ 0 ];
    pCoupon->fbStatus[ 1 ]      = TrnCoupon.fbStatus[ 1 ];
    pCoupon->fsPrintStatus      = TrnCoupon.fsPrintStatus;
    pCoupon->uchNoOfItem        = TrnCoupon.uchNoOfItem;
    _tcsncpy(pCoupon->aszMnemonic, TrnCoupon.aszMnemonic, STD_DEPTPLUNAME_LEN);   /* saratoga */
    _tcsncpy(pCoupon->auchPLUNo, TrnCoupon.auchPLUNo, NUM_PLU_LEN);   /* saratoga */
    pCoupon->usDeptNo           = TrnCoupon.usDeptNo;               /* saratoga */
    pCoupon->uchAdjective       = TrnCoupon.uchAdjective;           /* saratoga */
    pCoupon->fbStorageStatus    = TrnCoupon.fbStorageStatus;

	pCoupon->uchHourlyOffset	= TrnCoupon.uchHourlyOffset;	// RflGetCouponItem() Hourly Total bucket for when the coupon was rung up.
	pCoupon->usBonusIndex		= TrnCoupon.usBonusIndex;		// RflGetCouponItem() SR 157 Bonus totals to 100 JHHJ

    _tcsncpy( pCoupon->aszNumber, TrnCoupon.aszNumber, NUM_NUMBER );

    if ( sType == RFL_WITH_MNEM ) { /* function type is inquiry mnemonic */
        if (pCoupon->uchCouponNo) {
			CPNIF    CouponIF;       /* structure for I/F CliOp*** function */

            /* --- retrieve coupon mnemonic --- */
            CouponIF.uchCpnNo = pCoupon->uchCouponNo;
            usLockHnd         = 0;
            CliOpCpnIndRead( &CouponIF, usLockHnd );
            memset( pCoupon->aszMnemonic, 0x00, sizeof(pCoupon->aszMnemonic) );
            _tcsncpy( pCoupon->aszMnemonic, CouponIF.ParaCpn.aszCouponMnem, STD_CPNNAME_LEN );
        } else {
            /* UPC Coupon */
            RflGetTranMnem ( pCoupon->aszMnemonic, TRN_VCPN_ADR);
        }
    }
}

/*
*===========================================================================
** Format  : USHORT RflGetStorageItem(VOID *pDest, VOID *pSource);
*               
*    Input : VOID      *pSource    -start to read pointer
*            SHORT     sType       -RFL_WITH_MNEM
*                                  -RFL_WITHOUT_MNEM
*   Output : VOID      *pDest      -destination buffer address
*    InOut : none
** Return  : USHORT    usReadLen   -read length   
*
** Synopsis: This function gets one item that is in storage.
*
*  Source buffer has following structure  
*  +-----+-----+-----+----------+--------------- - - - 
*  |Len1 |Major|Minor|Len2      |Compressed data
*  |     |     |     |          |
*  +-----+-----+-----+----------+-------------- - - - 
*    Len1  = sizeof(Len1 + Major + Minor)
*    Len2  = sizeof(Len2 + compressed data )
*
*  Source buffer(ITEMSALES) has following structure  
*  +-----+-----+-----+----------+----------+----------------- 
*  |Len1 |Major|Minor|PLU NO    |Dept NO   |Qty                
*  |     |     |     |          |          |                   
*  +-----+-----+-----+----------+----------+-----------------
*                   -+---------------------+----------+---------------- - - - 
*                    |Product              |Len2      | Compressed data
*                    |                     |          |
*                   -+---------------------+----------+---------------- - - - 
*
*    Len1  = sizeof(Len1 + Major + Minor + PluNo + DeptNo + Qty + product)
*    Len2  = sizeof(Len2 + compressed data )
*
*                   WARNING:  Any changes made to the TRANSTORAGESALES struct
*                   and the ITEMSALES struct must be reflected in this
*                   uncompression function, RflGetStorageItemp(), as
*                   well as in the compression function TrnStoSalesCom().
*
*        WARNING: There are many places in the source code which assume that an ITEMSALES struct
*                 is the largest, in terms of number of bytes of memory used, of any of the 
*                 item type structs (ITEMSALES, ITEMTENDER, ITEMDISC, etc.) however this
*                 may not always be the case.
*===========================================================================
*/
USHORT RflGetStorageItem(VOID *pDest, CONST VOID *pSource, SHORT sType)
{
          UCHAR   *puchWrite  = pDest;    /* current write point */
    CONST UCHAR   *puchRead  = pSource;   /* current read point*/
	RflRawStorageClassLen recLen = RflGetRawStorageClassLen(puchRead);

	NHPOS_ASSERT(recLen.usNonCmpLen > 0);
	NHPOS_ASSERT(recLen.usCmpLen < recLen.usItemLen);

    /* write non_compressed part to dest. buffer (1 for Len1 size) */
	if (recLen.usNonCmpLen > 0) {
		memcpy( puchWrite, puchRead + 1, recLen.usNonCmpLen );
		puchWrite += recLen.usNonCmpLen;       /* increment write point to beginning of compressed data area */
	}

	// first byte is noncompressed data length and second and third bytes are uchMajorClass and uchMinorClass
	// which are standard non-compressed data for all transaction data records from transaction file.
    if ( TRANSTORAGEDATAPTR(puchRead)->uchMajorClass == CLASS_ITEMCOUPON ) {
        /* --- decompress data and retreive coupon mnemonic --- */
        RflGetCouponItem ( pDest, pSource, sType, &recLen );
	} else if ( TRANSTORAGEDATAPTR(puchRead)->uchMajorClass == CLASS_ITEMSALES ) {
        /* --- decompress data and retreive coupon mnemonic --- */
        RflGetSalesItem ( pDest, pSource, sType, &recLen );
    } else {
		/* decompress data for other item types that do not require moving data around */                                          
		RflUnZip(puchWrite, (USHORT)(recLen.usItemLen - recLen.usNonCmpLen), puchRead + recLen.usOffsetToCmp, recLen.usCmpLen);
    }

    return( recLen.usTotalLen );  /* return actual record length to provide offset to following transaction data record */
}

/*
*===========================================================================
** Format  : USHORT RflGetStorageItemRequire(UCHAR uchMajorClass, VOID *pDest, VOID *pSource);
*               
*    Input : UCHAR     uchMajorClass  - required major class
*            VOID      *pSource       -start to read pointer
*            SHORT     sType          -RFL_WITH_MNEM
*                                     -RFL_WITHOUT_MNEM
*   Output : VOID      *pDest         -destination buffer address
*    InOut : none
** Return  : USHORT    usReadLen      -read length or zero if major class not matched.
*
** Synopsis: This function gets the uncmpressed item data from a transaction item data record.
*            However before attempting the uncompress, it checks that the major class specified
*            in the non-compressed section of the transaction data record matches the specified
*            major class. if there is not a match then it does not perform an uncompress.
*
*            This function is intended to be used in places where a particular major class is assumed
*            however there is a possibility that the transaction data record in fact has some other
*            type of data instead.
*/
USHORT RflGetStorageItemRequire(UCHAR uchMajorClass, VOID *pDest, CONST VOID *pSource, SHORT sType)
{
	USHORT  usRetLen = 0;

	if (TRANSTORAGEDATAPTR(pSource)->uchMajorClass == uchMajorClass) {
		usRetLen = RflGetStorageItem (pDest, pSource, sType);
	}

	return usRetLen;
}

/*
*===========================================================================
** Format   : BOOL RflIsSalesItemDisc( UCHAR *puchItem )
*
*    Input  : UCHAR *puchItem - address of item data to determine
*   Output  : none
*    InOut  : none
** Return   : TRUE  -   item is sales item with item discount
*             FALSE -   item is not sales item with item discount
*
** Synopsis : 
*       determine specified item is sales item with item discount.
*       single function centralizing test for following functions:
*          - PrtIsSalesIDisc()
*          - MldIsSalesIDisc()
*===========================================================================
*/
BOOL RflIsSalesItemDisc( VOID *puchItem )
{
	// we expect that the data buffer is ITEMSALES, ITEMDISC, etc. and is not
	// a transaction data record from a transaction file so first byte is the
	// uchMajorClass variable.
    if ( ((UCHAR *)puchItem)[ 0 ] == CLASS_ITEMSALES ) {
		// look at the second byte, the uchMinorClass variable.
        switch ( ((UCHAR *)puchItem)[ 1 ] ) {
			case CLASS_DEPTITEMDISC:
			case CLASS_PLUITEMDISC:
			case CLASS_SETITEMDISC:
			case CLASS_OEPITEMDISC:
				return ( TRUE );
				break;

			default:
				break;
        }
    }
    return ( FALSE );
}

/*
*===========================================================================
** Format   : BOOL RflGetStorageItemLen( UCHAR *puchItem )
*
*    Input  : UCHAR *puchItem - address of item data to determine
*   Output  : none
*    InOut  : none
** Return   : TRUE  -   item is sales item with item discount
*             FALSE -   item is not sales item with item discount
*
** Synopsis : 
*		We calculate the various sizes of pieces of information in the item data we have just
*		read in.  The item data is composed of two parts, an uncompressed header and other information
*		part and a compressed part containing the condiment information along with other transaction
*		information such as quantity.
*
*		The first UCHAR in the buffer is the unzipped size of noncompressed information
*		The first USHORT in the next block is the size of the compressed information
*		The total item size is size of noncompressed information and size of compressed information.
*===========================================================================
*/
USHORT RflGetStorageItemLen (VOID *pSource, ULONG ulActualBytes)
{
    USHORT       usCmpLen;     /* compressed data length */
    UCHAR       *puchRead = pSource;     /* current read point */

	if (ulActualBytes < 2)
		return 0xFFFF;

	if (ulActualBytes < puchRead [0])
		return 0xFFFF;

    usCmpLen =  *( (USHORT *)( &puchRead[puchRead [0]] ) );

    return( puchRead [0] + usCmpLen );               /* return read length */
}

/****** End of Source ******/
