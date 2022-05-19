/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reentrant Functions Library Headder File           
* Category    : Reentrant Functions Library, NCR 2170 Application
* Program Name: RflIn.h                                                      
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-08-92 : 00.00.01 : K.Nakajima :                                    
*
** NCR2171 **
* AUG-20-99 : 1.00.00  : T.Koyama   : initial (for 2171)
* AUG-26-99 : 1.00.00  : M.Teraki   : add following function prototype decl.
*           :          :            : _RflFMemSet, _RflFStrCat, _RflFStrChr
*           :          :            : _RflStrRChr
* Sep-21-99 :          : M.Teraki   : add following function prototype decl.
*           :          :            : RflD13ToDecimalStr
* OCT-05-99 : 1.00.00  : M.Teraki   : add #pragma pack(...)
*           :          :            :
*===========================================================================
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*       2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/

#if !defined(RFL_H_INCLUDED)
#define RFL_H_INCLUDED

// other includes we depend on

#include "ecr.h"
#include "regstrct.h"
#include "para.h"

/**
;========================================================================
;+          L O C A L    D E F I N I T I O N s
;========================================================================
**/
#define     RFL_SUCCESS            0
#define     RFL_FAIL              -1

#define     RFL_FEED_ON            0                 /* auto feed on */
#define     RFL_FEED_OFF           1                 /* auto feed off */

// Standard character defines used to define special characters within text strings.
// Some of these are actual characters and some are indicators for special states

#define     RFL_SPACE                  0x20        /* character space */
#define     RFL_TAB                    0x09        /* character tab */
#define     RFL_RETURN                 0x0A        /* character return */
#define     RFL_ESCAPE                 0x1B        /* character escape */
#define     RFL_DOUBLE                 0x12        /* indicates next character is 'double wide character'. See also FSC_DOUBLE_WIDE and KPS_DOUBLE and PRT_DOUBLE */


// See function RflTax2() for AC 124 Tax Table calculation and how these constants
// are used to determine which section of the AC 124 Tax Table to use.
#define     RFL_USTAX_1            0                 /* tax type, US tax 1 or Canadian GST */
#define     RFL_USTAX_2            1                 /* tax type, US tax 2 or Canadian PST1 */
#define     RFL_USTAX_3            2                 /* tax type, US tax 3 or Canadian PST2 */
#define     RFL_USTAX_4            3                 /* tax type, Canadian PST3 only */

#define     RFL_DISC_RATE_MUL      1000L             // The discount rate multiplier to make units appropriate for RflRateCalc1() and AC 86 discount rate

// Following defines are return values from function Rfl_SpBsearch().
#define     RFL_HIT                  0       // item at *pHitPoint matches the key at pKey.
#define     RFL_HEAD_OF_TABLE       -1       // item at *pHitPoint is lower than key at pKey and pKey is first table entry
#define     RFL_DONT_HIT            -2       // item at *pHitPoint is not equal to key at pKey but is the point where it should be inserted.
#define     RFL_TAIL_OF_TABLE       -3       // item at *pHitPoint is higher than key at pKey and pKey is last table entry

// Following defines are argument to function RflGetStorageItem().
// The mnemonic is not stored with all of the various types of items so a
// separate fetch from the Master Terminal is required to get the mnemonic in
// some cases. These defines indicate whether the mnemonic is needed or not.
#define     RFL_WITH_MNEM            1      /* perform a lookup for the mnemonic associated with the item */
#define     RFL_WITHOUT_MNEM         0      /* do not perform a lookup for the mnemonic of an item */

// Following defines are used with function _RflSetCommaDecimal()
#define _RFL_USE_ERROR		0				/* Error on input value */ //ESMITH CURRENCY
#define _RFL_USE_COMMA		1				/* Use a comma for seperator */	
#define _RFL_USE_DECIMAL	2				/* Use a decimal for seperator */

#define _RFL_DECIMAL_SEP	0				/* Set flag for decimal seperator */
#define _RFL_DIGITGROUP_SEP	1				/* Set flag	for digit grouping seperator */ 

// following defines used with USHORT bit map returned by function RflGetScaleUnits()
// to indicate the MDC Address 29 settings for Scale.
#define RFL_SCALE_UNITS_01   0x0001         /* minimum unit of auto scale is 0.01 LB/Kg, two decimal places */
#define RFL_SCALE_UNITS_005  0x0002         /* minimum unit of auto scale is 0.005 LB/Kg, three decimal places */
#define RFL_SCALE_UNITS_001  0x0003         /* minimum unit of auto scale is 0.001 LB/Kg, three decimal places */
#define RFL_SCALE_ACCUM_1    0x0010         /* scale weight accumulated by 0.1 LB/Kg */
#define RFL_SCALE_ACCUM_01   0x0020         /* scale weight accumulated by 0.01 LB/Kg */

#define RFL_SCALE_PLACES_2   0x0001         /* bit mask indicating if weight in two decimal places */
#define RFL_SCALE_PLACES_3   0x0002         /* bit mask indicating if weight in three decimal places */

/*********************************************
    Label Analysis : RFLLABEL.C,RFLRANDM.C 2172
*********************************************/

/*--- Label Analysis Return Status Code ---*/
#define     LABEL_OK            0
#define     LABEL_ERR         (-1)       // general label processing error

#define     LABEL_DIGIT_ERR   (-2)       // PLU number digits are incorrect
#define     LABEL_ANALYS_ERR  (-3)       // RflLabelAnalysis() returned an error
#define     LABEL_TYPE_ERR    (-4)       // PLU number analysis indicates not a PLU

/*--- Label Analysis Modifier / Input Status Code ---*/
#define     LA_NORMAL        0x00
#define     LA_EMOD          0x01
#define     LA_VEL           0x02   /* EAN Velocity Code, similar to UPC Velocity Code, seldom used now */
#define     LA_UPC_VEL       0x04   /* UPC Velocity Code, seldom used now (R2.0) */

#define     LA_SCANNER       0x80

/*--- Label Analysis Label Type, see uchType of LABELANALYSIS ---
 *
 *    These are various types of bar code data formats which vary by
 *    the number of characters encoded in the bar code as well as what
 *    type of characters, alphanumeric or numeric only.
 *
 *    Some versions of bar codes are designed to reduce the size of the bar code
 *    and are used with small articles (UPC E, EAN-8).
 *
 *    Bar codes also vary as to the kinds of information that can be
 *    encoded in the bar code such as the PLU number, weight or quantity, etc.
*/
#define     LABEL_UPCA        1    // UPC A is standard version of UPC code with 12 digits
#define     LABEL_EAN13       2    // EAN-13 is standard version of EAN code with 13 digits
#define     LABEL_UPCE        3    // UPC E is the short form of UPC A product code
#define     LABEL_EAN8        4    // EAN-8 is the short form of EAN-13 product code, digits only
#define     LABEL_VELOC       5    // EAN-Velocity is a special form of EAN-8 product code. mostly unnecessary these days.
#define		LABEL_RSS14		  6    // RSS 14, a 14 digit EAN product code
#define     LABEL_COUPON     11
#define     LABEL_RANDOM     21
#define		LABEL_RSS14VAR	 22

/*--- Label Analysis Lookup/Non-Lookup Status ---*/
#define     LA_NOT_LOOKUP    0
#define     LA_EXE_LOOKUP    1

/*--- Label Analysis Flag of What Random Weight Label, see fsBitMap of LABELANALYSIS  R2.0 ---
 *    see function LaInst_CheckLabel() and variations LaInst_CheckLabelA(), LaInst_CheckLabelB(),
 *    and LaInst_CheckLabelC() as well as _RflRssVar().
*/
#define     LA_F_ARTINO      0x0001    // label contains Article number
#define     LA_F_PRICE       0x0002    // label contains Price
#define     LA_F_WEIGHT      0x0004    // label contains Weight
#define     LA_F_QUANTITY    0x0008    // label contains Quantity
#define     LA_F_DEPT        0x0010    // label is Department number
#define     LA_F_NUMBER      0x0020    // label is Account number
#define     LA_F_CONS_NO     0x0040    // label is Consumer number
#define     LA_F_SKU_NO      0x0080    // label is SKU number (merchant's Stock Keeping Unit) and not a PLU number (manufacturer's Price Lookup Unit)
#define     LA_F_FREQ        0x0100    // label is Frequent Shopper number
#define		LA_F_DATE		 0x0200    // Label Date Flag

//if Date is on Label LA_F_DATE
#define		LA_DATE_PROD	 1 //Date is Production Date
#define		LA_DATE_DUE		 2 //Date is Due Date
#define		LA_DATE_PACK	 3 //Date is Packaging Date
#define		LA_DATE_BEST	 4 //Date is Best Before Date
#define		LA_DATE_EXP		 5 //Date is Expiration Date

#define		LA_QTY_WEIGHT	0x00000001
#define		LA_QTY_LENGTH	0x00000002
#define		LA_QTY_WIDTH	0x00000004
#define		LA_QTY_HEIGHT	0x00000008
#define		LA_QTY_AREA		0x00000010
#define		LA_QTY_VOLUME	0x00000020

#define		LA_QTY_LB		0x00000100
#define		LA_QTY_IN		0x00000200
#define		LA_QTY_FT		0x00000400
#define		LA_QTY_YD		0x00000800
#define		LA_QTY_TO		0x00001000
#define		LA_QTY_OZ		0x00002000
#define		LA_QTY_QT		0x00004000
#define		LA_QTY_USG		0x00008000
#define		LA_QTY_KG		0x00010000
#define		LA_QTY_M		0x00020000
#define		LA_QTY_L		0x00040000

//Decimal Places
#define		LA_DEC_0		0x00100000
#define		LA_DEC_1		0x00200000
#define		LA_DEC_2		0x00400000
#define		LA_DEC_3		0x00800000
#define		LA_DEC_4		0x01000000
#define		LA_DEC_5		0x02000000
#define		LA_DEC_6		0x04000000
#define		LA_DEC_7		0x08000000
#define		LA_DEC_8		0x10000000
#define		LA_DEC_9		0x20000000


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

#if defined(USE_2170_ORIGINAL)
/* V3.3 */
typedef struct {
    LONG    lLow9Digits ;
    HUGEINT huBillions ;
} D24DIGITS ;

typedef struct {
    UCHAR    auchEHuge[12] ;
} EHUGEINT ;
#endif

/*--- Label Analysis Interface STRUCTURE, 2172 ---*/
// there are several functions that determine the bar code label type and from that determine
// how to copy the bar code data into the LABELANALYSIS struct. See function LaInst_CheckLabel() and its
// derivative functions which do this parsing.
#define     LA_LEN_NUMBER      10      // max number of alphanumeric characters for SKU/Consumer/Account Number from scanned bar codes.
#define     LA_LEN_PRICE2       2      /* R2.0 */
#define     LA_LEN_PRICE3       3      /* R2.0 */
#define     LA_LEN_PRICE4       4
#define     LA_LEN_PRICE5       5
#define     LA_LEN_PRICE6       6
#define     LA_LEN_DEPT4        4
#define     LA_LEN_DEPT5        5
#define     LA_LEN_SKU5         5

typedef struct {                                  /* Saratoga */
    TCHAR   aszScanPlu[NUM_PLU_SCAN_LEN];         /* scan/input-key PLU#     *///allow for Additional scan data with + RPH
    UCHAR   fchModifier;                          /* modifier / input status          */
    TCHAR   aszLookPlu[STD_PLU_NUMBER_LEN+1];     /* look up PLU#          */
    TCHAR   aszMaskLabel[STD_PLU_NUMBER_LEN+1];   /* look plu after mask label     */
    UCHAR   uchType;                              /* label type. see LABEL_UPCA, LABEL_RSS14VAR, etc.  */
    TCHAR   aszPrice[15+1];                        /* price/weight/quantity on label (R2.0)  */
	UCHAR	uchPriceDecimal;						// Decimal places in price
	TCHAR	aszISOCurCode[3+1];						// RSS14 ISO Currency Code
	TCHAR	aszMeasurement[3][6+1];					// RSS14 Weight, Length, Volume, etc Up to 3 dimensions
	ULONG	ulMeasureType[3];						// RSS14 Measure Type, Unit, decimal place BitMap
	TCHAR	aszDate[6+1];							// RSS14 Date
	UCHAR	uchDateType;							// Type of Date on Label
    TCHAR   aszDept[5+1];                         /* dept number on label             */
    TCHAR   aszLabelNumber[LA_LEN_NUMBER + 1];    /* SKU/Consumer/Account Number. see also UIF_SKUNUMBER and SKU_MODIFIER  */
    UCHAR   uchLookup;                            /* look up / not look up            */
    USHORT  fsBitMap;                             /* WHAT on label,  R2.0 see label analysis flags - LA_F_PRICE, etc.   */
} LABELANALYSIS;

typedef struct {
	SHORT   sRetStatus;
	USHORT  usGcNumber;
	UCHAR   uchGcCd;
} RflGcfNo;

typedef struct {
	USHORT  usTran;        // transaction mnemonic for this tender, total
	USHORT  usSym;         // special mnemonic for this tender, total
} RflMoneyMnem;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/**
;========================================================================
;+          P R O T O T Y P E    D E C L A R A T I O N s
;========================================================================
**/
SHORT  RflZip(VOID *pDest, USHORT usDestLen, CONST VOID *pSource, USHORT usSourceLen); /* compress data :RflCmp.c */
SHORT  RflZipByType (VOID *pBuffer, USHORT BuffSize, CONST VOID *pData, USHORT NoCmpSize, USHORT CmpSize);
SHORT  RflUnZip(VOID *pDest, USHORT usDestLen, CONST VOID *pSource, USHORT usSourceLen);      /* decompress data :RflCmp.c */

#if defined(USE_2170_ORIGINAL)
VOID   RflAdd13to13(D13DIGITS  *p13Dest, D13DIGITS *p13Source);         /* add 13digits data to 13digits data :RflTtl.c */
VOID   RflAdd9to13(D13DIGITS  *p13Dest, D9DIGITS l9Digits);             /* add 9digits data to 13digits data :RflTtl.c */
VOID   RflSubt13from13(D13DIGITS  *p13Dest, D13DIGITS *p13Source);      /* subtract 13digits data from 13digits data :RflTtl.c */
VOID   RflSubt9from13(D13DIGITS  *p13Dest, D9DIGITS l9Digits);          /* subtract 9digits data from 13digits data :RflTtl.c */

VOID   RflCastLONGtoD13(D13DIGITS* p13Dest, LONG  lSource);          /* LONG data to 13digits data :RflTtl.c */
VOID   RflCastLONGtoD24(D24DIGITS* p24Dest, LONG  lSource);          /* LONG data to 24digits data :RflTtl.c */
VOID   RflMulD13byD9(D24DIGITS *pd24Product, D13DIGITS *pd13Multiplicant,LONG lMulriplier);
                                       /* multiply "Structure 13digits" by long  :RflTtl.c */
VOID  RflDivD24byD9(D24DIGITS *pd24Product, LONG *plRemain,LONG lDivisor);
                                       /* divides "Structure 24digits" by LONG data :RflTtl.c */
VOID   RflAddD24toD24(D24DIGITS  *p24Dest, D24DIGITS *p24Source);
                                       /* Add "Structure 24digits" to  "Structure 24digits"  :RflTtl.c */
VOID   RflCastD24toD13(D13DIGITS *pd13ModAmount, D24DIGITS *pd24Amount);
#endif

SHORT  RflRoundDcurrency (DCURRENCY *plModAmount, D13DIGITS d13Amount, UCHAR uchType);
SHORT  RflRound(DCURRENCY *plModAmount, DCURRENCY lAmount, UCHAR uchType);        /* rounding :RflRound.c */
SHORT  RflRateCalc1(DCURRENCY *plModAmount, DCURRENCY lAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */
SHORT  RflRateCalc2(DCURRENCY *plModAmount, DCURRENCY lAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */
SHORT  RflRateCalc3(DCURRENCY *plModAmount, DCURRENCY lAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */
SHORT  RflRateCalc4(DCURRENCY *plModAmount, DCURRENCY lAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */

SHORT   RflRateCalc5(D13DIGITS *pd13ModAmount, D13DIGITS *pd13Amount,ULONG ulRate, UCHAR uchType);
                                                                                    /* rate :RflRound.c */
#if defined(USE_2170_ORIGINAL)
SHORT  RflRoundD24(D13DIGITS *pd13ModAmount, D24DIGITS *pd24Amount, UCHAR uchType);
                                                                                    /* rate :RflRound.c */
#endif
SHORT  RflRoundHugeHuge(D13DIGITS *phuModAmount, D13DIGITS *phuAmount, UCHAR uchType);
SHORT  RflRoundD13D13(D13DIGITS *pd13ModAmount, D13DIGITS d13Amount, UCHAR uchType);
SHORT  RflRateCalcHuge1(D13DIGITS *phuModAmount, D13DIGITS *phuAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */
SHORT  RflRateCalcHuge2(D13DIGITS *phuModAmount, D13DIGITS *phuAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */
SHORT  RflRateCalcHuge3(D13DIGITS *phuModAmount, D13DIGITS *phuAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */
SHORT  RflRateCalcHuge4(D13DIGITS *phuModAmount, D13DIGITS *phuAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */

BOOL   RflIsSalesItemDisc( VOID *puchItem );
USHORT RflGetStorageItemLen (VOID *pSource, ULONG ulActualBytes);

UCHAR  RflMakeCD(ULONG ulNumber);          /* generate check digits :RflCD.c */
SHORT  RflCheckCD(ULONG ulNumber);         /* confirm check digits :RflCD.c */
RflGcfNo  RflPerformGcfCd (ULONG  ulNumber);
DCURRENCY  RflLLabs (DCURRENCY llValue);        // replacement for labs() for use with DCURRENCY

SHORT  RflHALO(DCURRENCY lAmount, CHAR chHALO); /* HALO check :RflHALO.c */
SHORT  RflHALO_OverRide(DCURRENCY *plAmount, CHAR chHALO, USHORT usMdc, UCHAR uchField);
VOID   RflSetHALOStatus(USHORT status);		/*SR 143 HALO Override Key cwunn :RflHALO.c */
SHORT  RflTax1(DCURRENCY *plTax, DCURRENCY lAmount, USHORT usType);                   /* Calc. Tax with systemram taxtable :RflTax.c */
SHORT  RflTax2(DCURRENCY *plTax, DCURRENCY lAmount, USHORT usType, VOID *pTaxTable);  /* Calc. Tax with specified taxtable :RflTax.c */

const char * RflPathChop(const char *aszFilePath);

// The following logic is used to override the RflSPrintf function so that we can
// generate debug output in the ASSRTLOG file if there is a buffer overrun or
// similar problem with the Rfl library.
// WARNING: Enabling this will cause problems with assignment of the return value being wrong.
#if 0
#define RflSPrintf RflSPrintf_Debug_line(__FILE__,__LINE__), RflSPrintf_Debug
USHORT  RflSPrintf_Debug_line(char *aszFilePath, int nLineNo);
USHORT RflSPrintf_Debug(TCHAR *pszDest, USHORT usDestLen, const TCHAR FAR *pszFormat, ...);     /* simulate sprintf() :RflStr.c */
#else
USHORT RflSPrintf(TCHAR *pszDest, USHORT usDestLen, const TCHAR FAR *pszFormat, ...);     /* simulate sprintf() :RflStr.c */
#endif
USHORT  RflStrAdjust(TCHAR *pszDest,  USHORT usDestLen, TCHAR *pszSource, USHORT usMaxColumn, BOOL fAutoFeed); /* adjust ASCII string :RflStr.c */

// sets the first count TCHAR characters of string to be val.
TCHAR * tcharnset(TCHAR* string, TCHAR val, size_t count);
// copies count TCHAR characters from tcSource to tcDest ignoring any end of string characters.
TCHAR * tcharncpy (TCHAR *tcDest, CONST TCHAR *tcSource, size_t count);
USHORT tcharlen(CONST TCHAR *tcSource);
TCHAR *tcharTrimLeading (TCHAR  *tcString);
TCHAR *tcharTrimRight (TCHAR  *tcString);

VOID   RflConv3bto4b(ULONG *ulPrice, CONST UCHAR auchPrice[MAX_PLUPRICE_SIZE]);
ULONG  RflFunc3bto4b(CONST UCHAR auchPrice[MAX_PLUPRICE_SIZE]);
VOID   RflConv4bto3b(UCHAR auchPrice[MAX_PLUPRICE_SIZE], ULONG ulPrice);
SHORT  Rfl_SpBsearch(VOID *pKey, USHORT usRecord_Size, VOID *pTableBaseAddress, ULONG ulNumberOfEntry, VOID **pHitPoint, SHORT (*psCompar)(VOID *pKey, VOID *pusHPoint));
FLEXMEM  RflGetFlexMemByType(UCHAR uchAddress);
USHORT RflGetSystemType (VOID);
ULONG  RflGetMaxRecordNumberByType (UCHAR uchAddress);
USHORT RflGetPtdFlagByType (UCHAR uchAddress);
USHORT RflGetScaleUnits(VOID);
USHORT RflGetSetDecimalCommaType (VOID);
USHORT RflGetNoTermsInCluster (VOID);
UCHAR  RflGetMdcData(USHORT usAddr);
int    RflIsSpecialPlu (CONST TCHAR *auchPLUNo, CONST TCHAR *auchMldSpecialPlu);
TCHAR *RflCopyPlu (TCHAR *auchPluNoDest, CONST TCHAR *auchPluNoSrc);
int    RflChkSpecialPluRange (CONST TCHAR *auchPLUNo, CONST TCHAR *auchMldSpecialPluLow, CONST TCHAR *auchMldSpecialPluHigh);
int    RflChkIfNoDisplayRange (CONST TCHAR *auchPLUNo);
USHORT RflGetStorageItemClassLen (CONST VOID *pItemData);               /* RflGetIt.c */
USHORT RflGetStorageTranRecordItemClassLen (CONST VOID *pTranRecData);  /* RflGetIt.c */
USHORT RflGetStorageTranRecordLen (CONST VOID *pTranRecData);           /* RflGetIt.c */
USHORT RflGetStorageItem(VOID *pDest, CONST VOID *pSource, SHORT sType);      /* RflGetIt.c */
USHORT RflGetStorageItemRequire(UCHAR uchMajorClass, VOID *pDest, CONST VOID *pSource, SHORT sType);
TCHAR *RflD13ToDecimalStr(D13DIGITS *, TCHAR *);

/* RflCalc.asm */
#if defined(USE_2170_ORIGINAL)
VOID   RflMul32by32(HUGEINT *, LONG, LONG);
VOID   RflDiv64by32(HUGEINT *, LONG *, LONG);     
VOID   RflAdd64to64(HUGEINT *, HUGEINT *);        
VOID   RflCast64to32(LONG *, HUGEINT *);          
VOID   RflCast32to64(HUGEINT *, LONG);            
#endif

/* :RflForm.c */  
#if 0
#pragma message("  ====++++====   _RflFormStr_Debug() is ENABLED     ====++++====")
USHORT _RflFormStr_Debug(const TCHAR FAR *pszFmt, VOID *psArgs, TCHAR *pszStr, USHORT usLength, char *aszFilePath, int nLineNo);
#define _RflFormStr(pszFmt,psArgs,pszStr,usLength) _RflFormStr_Debug(pszFmt, psArgs, pszStr, usLength, __FILE__,__LINE__)
#else
USHORT _RflFormStr(const TCHAR FAR *pszFmt, VOID *psArgs, TCHAR *pszStr, USHORT usLength);
#endif
UCHAR  _RflSetDecimal(UCHAR);         /* :RflForm.c */
UCHAR  _RflSetComma(UCHAR);           /* :RflForm.c */
UCHAR  _RflSetCommaDecimal(UCHAR, UCHAR);	  /* :RflForm.c */ //ESMITH CURRENCY

VOID  RflMemRand (VOID *pDest, USHORT usLen);
ULONG  RflMemHash (const VOID *pSrc, ULONG ulLen, ULONG ulHashStart);

/*--- Label Analysis , 2172---*/
SHORT  RflSpecialPluCheck (TCHAR *auchPLUNo);              /* RflLabel.C   */
SHORT  RflLabelAnalysis(LABELANALYSIS *pData);             /* RflLabel.C   */
USHORT RflConvScan2Manual(TCHAR *aszDest, TCHAR *aszSrc);  /* RflLabel.C   */

/* ----- RFLBCD.C ----- 2172 */
VOID    RflPLUAscii2BCD(UCHAR *puchBCD, UCHAR *pszAscii);
VOID    RflPLUBCD2Ascii(UCHAR *pszAscii, UCHAR *puchBCD);
SHORT   RflPLUAs2BCDRight(UCHAR *puchBCD, UCHAR *pszAscii);
SHORT   RflPLUBCD2AsRight(TCHAR *pszAscii, UCHAR *puchBCD);
SHORT   RflPLUBCD2AsRight(TCHAR *pszAscii, UCHAR *puchBCD);
SHORT   RflNCFAs2BCDRight(UCHAR *puchBCD, UCHAR *pszAscii);     /* R2.0 */
SHORT   RflNCFBCD2AsRight(UCHAR *pszAscii, UCHAR *puchBCD);     /* R2.0 */

/* ----- RFLPLU.c ----- 2172 */
VOID    RflConvertPLU(TCHAR *pszDestPLU, TCHAR *pszSourcePLU);

// defined in file rflstr.c
LONG  RflConvertCharFieldToLongCurrency (CHAR *auchCurrency, int iMaxChars);
CHAR *RflConvertLongCurrencyToCharField (LONG lCurrency, CHAR *auchCurrency, int iMaxChars);
void _RflStrcpyUchar2Tchar (TCHAR *aszDest, UCHAR *aszSource);
void _RflStrncpyUchar2Tchar (TCHAR *aszDest, UCHAR *aszSource, USHORT usMaxChars);
void _RflStrcpyTchar2Uchar (UCHAR *aszDest, TCHAR *aszSource);
void _RflStrncpyTchar2Uchar (UCHAR *aszDest, TCHAR *aszSource, USHORT usMaxChars);
void _RflMemcpyTchar2Uchar (UCHAR *aszDest, TCHAR *aszSource, USHORT usNoChars);
void _RflMemcpyUchar2Tchar (TCHAR *aszDest, UCHAR *aszSource, USHORT usNoChars);
int   RflMatch(const wchar_t *regexp, const wchar_t *text);
TCHAR *RflCleanupMnemonic (TCHAR *tcsmnemonic);
TCHAR *RflCopyMnemonic (TCHAR *tcsDest, CONST TCHAR *tcsSource, int nMaxCharCount);
TCHAR *RflGetMnemonicByClass (USHORT usClass, TCHAR *pszDest, USHORT usAddress);
TCHAR *RflGetTranMnem (TCHAR *pszDest, USHORT usAddress);
TCHAR *RflGetReportMnem(TCHAR *pszDest, USHORT usAddress);
TCHAR *RflGet24Mnem (TCHAR  *pszDest, USHORT usAddress);
TCHAR *RflGetLead (TCHAR  *pszDest, USHORT usAddress);
TCHAR *RflGetSpecMnem (TCHAR *pszDest, USHORT usAddress);
TCHAR *RflGetAdj (TCHAR *pszDest, USHORT usAdjective);
TCHAR *RflGetMajorDeptMnem(TCHAR *pszDest, USHORT usAddress);
RflMoneyMnem    RflGetMoneyMnem(UCHAR uchTendMinor);
TCHAR *RflGetPauseMsg(TCHAR *pszDest, USHORT usPauseMsgId);
USHORT RflGetHaloData (USHORT usAddress);
ULONG  RflGetLimitFloor (USHORT usAddress);
SHORT  RflGetSlipMaxLines (VOID);

// following are VOID pointers to allow for compile.
USHORT  RflChkTtlStandardAdr(UCHAR uchMinorClass);
USHORT  RflChkTtlAdr(VOID *pItem);       // ITEMTOTAL *pItem
USHORT  RflChkTendAdr(VOID *pItem);      // ITEMTENDER *pItem
USHORT  RflChkDiscAdr(VOID *pItem);      // ITEMDISC *pItem

VOID    RflUpdateDisplayDescriptorMaps (USHORT usMaps, ULONG  ulDescripToSetMap, ULONG  ulDescripToClearMap);
VOID    RFLUpdateDisplayByMajorMinorClass (UCHAR uchMajorClass, UCHAR uchMinorClass, UCHAR fbSaveControl);
VOID    RFLUpdateDisplayedStatusDescriptors (USHORT usMaps, ULONG  ulDescripToSetMap, ULONG  ulDescripToClearMap);
VOID    RflClearDisplay (USHORT usMaps, ULONG  ulDescripToSetMap, ULONG  ulDescripToClearMap);

SHORT  RflLoggingRulesTransactions (void);

ULONG   RflTruncateEmployeeNumber(ULONG ulEmployeeNumber);
int  RflConstructEncryptByteStringClass (CONST CHAR *szPassword);
VOID RflDestructEncryptByteStringClass (VOID);
USHORT RflFormatLineTwoColumn (TCHAR *aszPrintLine, TCHAR *aszMnemLeft, TCHAR *aszMnemRight, int iLineLen);
int  RflConvertSubStringToInt (CONST UCHAR *auchString, int iLen);

#if 0
int     RflRehashEncryptDecrypt_Debug (CONST CHAR *szPassword, char *aszFilePath, int nLineNo);
UCHAR * RflEncryptByteString_Debug (UCHAR *bsString, int nByteCount, char *aszFilePath, int nLineNo);
UCHAR * RflDecryptByteString_Debug (UCHAR *bsString, int nByteCount, char *aszFilePath, int nLineNo);
#define RflRehashEncryptDecrypt(szPassword)        RflRehashEncryptDecrypt_Debug ((szPassword), __FILE__, __LINE__)
#define RflEncryptByteString(bsString,nByteCount)  RflEncryptByteString_Debug ((bsString), (nByteCount),  __FILE__, __LINE__)
#define RflDecryptByteString(bsString,nByteCount)  RflDecryptByteString_Debug ((bsString), (nByteCount),  __FILE__, __LINE__)
#else
int     RflRehashEncryptDecrypt (CONST CHAR *szPassword);
UCHAR * RflEncryptByteString (UCHAR *bsString, int nByteCount);
UCHAR * RflDecryptByteString (UCHAR *bsString, int nByteCount);
#endif
#if defined(__cplusplus)
}   //end extern C {
#endif

#endif  //  RFL_H_INCLUDED