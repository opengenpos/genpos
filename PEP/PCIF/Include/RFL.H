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
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-08-92 : 00.00.01 : K.Nakajima :                                    
*           :          :            :                                    
* Jan-25-00 : 01.00.00 : K.Yanagida :
*
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
;+          L O C A L    D E F I N I T I O N s
;========================================================================
**/

#define     RFL_SUCCESS            0
#define     RFL_FAIL              -1

#define     RFL_FEED_ON            0                 /* auto feed on */
#define     RFL_FEED_OFF           1                 /* auto feed off */

#define     RFL_USTAX_1            0                 /* tax type */
#define     RFL_USTAX_2            1
#define     RFL_USTAX_3            2
#define     RFL_USTAX_4            3

#define     RFL_HIT                  0 
#define     RFL_HEAD_OF_TABLE       -1
#define     RFL_DONT_HIT            -2 
#define     RFL_TAIL_OF_TABLE       -3

#define     RFL_WITH_MNEM            1
#define     RFL_WITHOUT_MNEM         0

/*********************************************
    Label Analysis : RFLLABEL.C,RFLRANDM.C 2172
*********************************************/

/*--- Label Analysis Return Status Code ---*/
#define     LABEL_OK          0
#define     LABEL_ERR        -1

/*--- Label Analysis Modifier / Input Status Code ---*/
#define     LA_NORMAL        0x00
#define     LA_EMOD          0x01
#define     LA_VEL           0x02
#define     LA_UPC_VEL       0x04   /* UPC Velocity Code (R2.0) */

#define     LA_SCANNER       0x80

/*--- Label Analysis Label Type ---*/
#define     LABEL_UPCA        1
#define     LABEL_EAN13       2
#define     LABEL_UPCE        3
#define     LABEL_EAN8        4
#define     LABEL_VELOC       5
#define		LABEL_RSS14		  6
#define     LABEL_COUPON     11
#define     LABEL_RANDOM     21
#define		LABEL_RSS14VAR	 22

/*--- Label Analysis Lookup/Non-Lookup Status ---*/
#define     LA_NOT_LOOKUP    0
#define     LA_EXE_LOOKUP    1

/*--- Label Analysis Flag of What Random Weight Label,  R2.0 ---*/
#define     LA_F_ARTINO      0x0001
#define     LA_F_PRICE       0x0002
#define     LA_F_WEIGHT      0x0004
#define     LA_F_QUANTITY    0x0008
#define     LA_F_DEPT        0x0010
#define     LA_F_NUMBER      0x0020
#define     LA_F_CONS_NO     0x0040
#define     LA_F_SKU_NO      0x0080
#define     LA_F_FREQ        0x0100
#define		LA_F_DATE		 0x0200 //Label Date Flag

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

/* V3.3 */
typedef struct {
    LONG    lLow9Digits ;
    HUGEINT huBillions ;
} D24DIGITS ;

typedef struct {
    UCHAR   auchEHuge[12] ;
} EHUGEINT ;

/*--- Label Analysis Interface STRUCTURE, 2172 ---*/
typedef struct {
    WCHAR   aszScanPlu[STD_PLU_NUMBER_LEN+60+1];     /* scan/input-key PLU#              */
    UCHAR   fchModifier;            /* modifier / input status          */
    WCHAR   aszLookPlu[STD_PLU_NUMBER_LEN+1];       /* look up PLU#                     */
    WCHAR   aszMaskLabel[STD_PLU_NUMBER_LEN+1];     /* look plu after mask label        */
    UCHAR   uchType;                /* label type                       */
    WCHAR   aszPrice[15+1];         /* price/weight/quantity on label (R2.0)  */
	UCHAR	uchPriceDecimal;		// Decimal places in price
	WCHAR	aszISOCurCode[3+1];		// RSS14 ISO Currency Code
	WCHAR	aszMeasurement[3][6+1];	// RSS14 Weight, Length, Volume, etc Up to 3 dimensions
	ULONG	ulMeasureType[3];		// RSS14 Measure Type, Unit, decimal place BitMap
	WCHAR	aszDate[6+1];			// RSS14 Date
	UCHAR	uchDateType;			// Type of Date on Label
    WCHAR   aszDept[5+1];           /* dept number on label             */
    WCHAR   aszNumber[10+1];        /* SKU/Consumer/Account Number      */
    UCHAR   uchLookup;              /* look up / not look up            */
    USHORT  fsBitMap;               /* WHAT on label,           R2.0    */
} LABELANALYSIS;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/**
;========================================================================
;+          P R O T O T Y P E    D E C L A R A T I O N s
;========================================================================
**/
SHORT  RflZip(VOID *pDest, USHORT usDestLen, VOID *pSource, 
                           USHORT usSourceLen); /* compress data :RflCmp.c */
SHORT  RflUnZip(VOID *pDest, USHORT usDestLen, VOID *pSource, 
                     USHORT usSourceLen);      /* decompress data :RflCmp.c */
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

SHORT  RflRoundHuge(LONG *plModAmount, HUGEINT *phuAmount, UCHAR uchType);
SHORT  RflRound(LONG *plModAmount, LONG lAmount, UCHAR uchType);        /* rounding :RflRound.c */
SHORT  RflRateCalc1(LONG *plModAmount, LONG lAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */
SHORT  RflRateCalc2(LONG *plModAmount, LONG lAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */
SHORT  RflRateCalc3(LONG *plModAmount, LONG lAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */
SHORT  RflRateCalc4(LONG *plModAmount, LONG lAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */

SHORT   RflRateCalc5(D13DIGITS *pd13ModAmount, D13DIGITS *pd13Amount,ULONG ulRate, UCHAR uchType);
                                                                                    /* rate :RflRound.c */
SHORT  RflRoundD24(D13DIGITS *pd13ModAmount, D24DIGITS *pd24Amount, UCHAR uchType);
                                                                                    /* rate :RflRound.c */
SHORT  RflRoundHugeHuge(HUGEINT *phuModAmount, HUGEINT *phuAmount, UCHAR uchType);
SHORT  RflRateCalcHuge1(HUGEINT *phuModAmount, HUGEINT *phuAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */
SHORT  RflRateCalcHuge2(HUGEINT *phuModAmount, HUGEINT *phuAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */
SHORT  RflRateCalcHuge3(HUGEINT *phuModAmount, HUGEINT *phuAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */
SHORT  RflRateCalcHuge4(HUGEINT *phuModAmount, HUGEINT *phuAmount, ULONG ulRate, UCHAR uchType);  /* rate :RflRound.c */

UCHAR  RflMakeCD(ULONG ulNumber);          /* generate check digits :RflCD.c */
SHORT  RflCheckCD(ULONG ulNumber);         /* confirm check digits :RflCD.c */
SHORT  RflHALO(LONG lAmount, CHAR chHALO); /* HALO check :RflHALO.c */
VOID   RflSetHALOStatus(USHORT status);		/*SR 143 HALO Override Key cwunn :RflHALO.c */
SHORT  RflTax1(LONG *plTax, LONG lAmount, USHORT usType);                   /* Calc. Tax with systemram taxtable :RflTax.c */
SHORT  RflTax2(LONG *plTax, LONG lAmount, USHORT usType, VOID *pTaxTable);  /* Calc. Tax with specified taxtable :RflTax.c */
USHORT RflSPrintf(WCHAR *pszDest, USHORT usDestLen, const WCHAR FAR *pszFormat, ...);     /* simulate sprintf() :RflStr.c */
UCHAR  RflStrAdjust(WCHAR *pszDest,  USHORT usDestLen, WCHAR *pszSource, UCHAR uchMaxColumn, BOOL fAutoFeed); /* adjust ASCII string :RflStr.c */

VOID   RflConv3bto4b(ULONG *ulPrice, UCHAR *auchPrice);
VOID   RflConv4bto3b(UCHAR *auchPrice, ULONG ulPrice);
SHORT  Rfl_SpBsearch(VOID *pKey, USHORT usRecord_Size, VOID *pTableBaseAddress, USHORT usNumberOfEntry,
                     VOID **pHitPoint,
                     SHORT (*psCompar)(VOID *pKey, VOID *pusHPoint));
USHORT RflGetStorageItem(VOID *pDest, VOID *pSource, SHORT sType); /* RflGetIt.c */
CHAR *RflD13ToDecimalStr(D13DIGITS *, CHAR *);

/* RflCalc.asm */
VOID   RflMul32by32(HUGEINT *, LONG, LONG);
VOID   RflDiv64by32(HUGEINT *, LONG *, LONG);     
VOID   RflAdd64to64(HUGEINT *, HUGEINT *);        
VOID   RflCast64to32(LONG *, HUGEINT *);          
VOID   RflCast32to64(HUGEINT *, LONG);            
VOID   RflCast32to96(EHUGEINT *, LONG);
VOID   RflMul64by32(EHUGEINT *, HUGEINT *, LONG);
VOID   RflDiv96by32(EHUGEINT *, LONG *, LONG);
VOID   RflAdd96to96(EHUGEINT *, EHUGEINT *);
VOID   RflCast96to64(HUGEINT *, EHUGEINT *);
VOID   RflCast64to16(short *, HUGEINT *);


USHORT _RflFormStr(const WCHAR FAR *pszFmt, VOID *psArgs, WCHAR *pszStr, USHORT usLength);/* :RflForm.c */
UCHAR  _RflSetDecimal(UCHAR);         /* :RflForm.c */
UCHAR  _RflSetComma(UCHAR);           /* :RflForm.c */

VOID FAR * _RflFMemCpy(VOID FAR *pDest, const VOID FAR *pSrc, USHORT usLen);
VOID FAR * _RflFMemSet(VOID FAR *pDest, SHORT c, ULONG count);

WCHAR FAR * _RflFStrCat(WCHAR FAR *, const WCHAR FAR *);   /* _fstrcat */
CHAR FAR * _RflFStrChr(const CHAR FAR *, USHORT);       /* _fstrchr */

CHAR * _RflStrRChr(const CHAR *, USHORT);               /* strrchr */

/*--- Label Analysis , 2172---*/
SHORT  RflLabelAnalysis(LABELANALYSIS *pData);             /* RflLabel.C   */

USHORT RflConvScan2Manual(UCHAR *aszDest, UCHAR *aszSrc);  /* RflLabel.C   */


/* ----- RFLBCD.C ----- 2172 */
VOID    RflPLUAscii2BCD(UCHAR *puchBCD, UCHAR *pszAscii);
VOID    RflPLUBCD2Ascii(UCHAR *pszAscii, UCHAR *puchBCD);
SHORT   RflPLUAs2BCDRight(UCHAR *puchBCD, UCHAR *pszAscii);
SHORT   RflPLUBCD2AsRight(WCHAR *pszAscii, UCHAR *puchBCD);
SHORT   RflPLUBCD2AsRight(WCHAR *pszAscii, UCHAR *puchBCD);
SHORT   RflNCFAs2BCDRight(UCHAR *puchBCD, UCHAR *pszAscii);     /* R2.0 */
SHORT   RflNCFBCD2AsRight(UCHAR *pszAscii, UCHAR *puchBCD);     /* R2.0 */

/* ----- RFLPLU.c ----- 2172 */
VOID    RflConvertPLU(WCHAR *pszDestPLU, WCHAR *pszSourcePLU);

