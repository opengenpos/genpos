/*
/////////////////////////////////////////////////////////////////////////////
// ECR.H :
//
// Copyright (C) 1992-1998 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date         Version  Author       Comments
// ===========  =======  ===========  =======================================
// Mar-09-1998                        initial
//
/////////////////////////////////////////////////////////////////////////////
*/

#ifndef _INC_ECR
#define _INC_ECR

#include "termcfg.h"

#ifdef __cplusplus
extern "C"{
#endif

/*
/////////////////////////////////////////////////////////////////////////////
*/

#ifndef FALSE
#define FALSE           0
#endif

#ifndef TRUE
#define TRUE            1
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL            0
#else
#define NULL            ((void *)0)
#endif
#endif

#ifndef CONST
#define CONST           const
#endif

#if defined _WIN32_WCE || defined WIN32
#define VOLATILE
#else
#define VOLATILE        volatile
#endif  /* _WIN32_WCE || WIN32 */

#if defined _WIN32_WCE || defined WIN32
#ifndef FAR
#define FAR
#endif
#ifndef NEAR
#define NEAR
#endif
#ifndef INTERRUPT
#define INTERRUPT
#endif
#ifndef VOID
#define VOID            void
#endif
#else
#endif  /* _WIN32_WCE || WIN32 */

/*
/////////////////////////////////////////////////////////////////////////////
*/

#if defined _WIN32_WCE || defined WIN32
#ifndef STUBENTRY
#define STUBENTRY    __stdcall
#endif
#else
#ifndef STUBENTRY
#define STUBENTRY 
#endif
#endif  /* _WIN32_WCE || WIN32 */

/*
/////////////////////////////////////////////////////////////////////////////
*/

#if defined _WIN32_WCE || defined WIN32 || defined PCIF_WINDOWS
#define FARCONST
#define UNINT
#else
#define FARCONST 
#define UNINT  
#endif
//--------------------------------------------
//		General system wide database constants that are defined here.
//		These contants are used in a wide variety of places when defining structs

#define  STD_PLU_NUMBER_LEN		14		// Number of characters in a PLU number  PLU_NUMBER_LEN
#define  STD_NO_BONUS_TTL		100		// Number of Bonus Totals JHHJ 8->100
#define  STD_TENDER_MAX			20		// Number of Tender Keys 11->20 JHHJ 3-16-04
#define	 STD_TOTAL_MAX			20		// Number of Total Keys 9-20 JHHJ 3-24-04
#define	 STD_DISC_ITEMIZERS_MAX	 6		// Number of Discount Itemizers 2(US), 3(CAN) -> 6 JHHJ 3-29-04
#define  STD_TAX_ITEMIZERS   5		// Number of Tax Itemizers 3(US), 4(CAN) ->5 JHHJ 3-30-04
#define  STD_OEP_MAX_NUM		20      // Number of OEP lines/buttons on a single window. OEP enhanced uses window size
#define	 STD_PLU_ITEMIZERS		10		// Number of PLU Itemizers for CANTAX
#define	 STD_PLU_ITEMIZERS_MOD	(STD_PLU_ITEMIZERS - 1)
#define  STD_MAX_NUM_PLUDEPT_CPN 7		// Number of PLUS/DEPT in a single Coupon

//  The following constants are used to mark the number of
//  characters that a particular kind of mnemonic string may
//  contain.  These values are for the max number of characters
//  and do not contain room for the end of string character.
//  To use these constants when defining a mnemonic array, you will
//  need to add one to the constant as in mnemonic[ECR_NUMBER_SIZE + 1].
#define  STD_AUTHCODE_LEN       24    // EPT authorization code
#define  STD_ACCT_NUMBER_LEN    10    // Account Number
#define  STD_ADJMNEMO_LEN        4    // adjective mnemonic from UNINIRAM and pararead () - PARA_ADJMNEMO_LEN
#define  STD_BIRTHDAY_LEN        6    // birthday entry (MMDDYY)
#define  STD_CASHIERNAME_LEN    20    // cashier name field
#define  STD_CASHIERNO_LEN       3    // cashier number field
#define  STD_CPNNAME_LEN       20    // coupon name field - PARA_CPN_LEN
#define  STD_CPACCOUNTNO_LEN    24    // charge posting account number
#define  STD_CPCURRENCY_LEN     10    // charge posting currency value as for totals
#define  STD_CPFOLIONO_LEN       6    // charge posting folio number
#define  STD_CPGUESTID_LEN       2    // charge posting guest id
#define  STD_CPROOMNO_LEN        5    // charge posting room number
#define  STD_CPRSPTEXT_LEN      39    // charge posting response text
#define  STD_CPRSPCO_LEN         6    // charge posting response text
#define  STD_CPRSPCO_EPT_LEN    10    // charge posting response text
#define  STD_DEPTPLUNAME_LEN    20    // dept/PLU name  PARA_DEPT_LEN and PARA_PLU_LEN
#define  STD_EPTNUMBER_LEN      19    // number field as used for EPT transactions
#define  STD_ETK_NAME_LEN       20    // Employee Time Keeping Name
#define  STD_EXPIRATION_LEN      4    // credit card expiration (MMYY)
#define  STD_GUESTCHKID_LEN      6    // guest check transaction number
#define  STD_HEADERMNEMO_LEN    24    // receipt header mnemonic from UNINIRAM and pararead ()
#define  STD_HOTELID_LEN         2    // hotel id mnemonic from UNINIRAM and pararead () - PARA_HOTELID_LEN
#define  STD_LEADTHRUMNEMO_LEN  20    // leadthru mnemonic from UNINIRAM and pararead () - PARA_LEADTHRU_LEN
#define  STD_MAJDEPTMNEMO_LEN   20    // major department name from UNINIRAM and pararead ()
#define  STD_MSRDATA_LEN        40    // MSR data from mag strip card
#define  STD_NUMBER_LEN         25    // number usually as entered by number type key
#define  STD_NAME_LEN           25    // name
#define  STD_OPERATORNAME_LEN   20    // terminal operator name
#define  STD_PLUNAME_LEN        20    // PLU name field - PARA_PLU_LEN
#define  STD_POSTTRAN_LEN        6    // charge posting posted transaction number
#define  STD_PROMOMNEMO_LEN     24    // promotion header mnemonic from UNINIRAM and pararead () - PARA_PROMOTION_LEN
#define  STD_REFERENCENO_LEN    24    // EPT reference number
#define  STD_SEATNO_LEN          2    // charge post and EPT seat number field
#define  STD_SPEMNEMO_LEN        4    // special mnemonic from UNINIRAM and pararead () - PARA_SPEMNEMO_LEN
#define  STD_TRANSMNEM_LEN      20    // transaction mnemonic from UNINIRAM and pararead () - PARA_TRANSMNEMO_LEN
#define  STD_MLD_MNEMONICS_LEN  32    /* Mld Mnemonics Length,      V3.3 */
#define  STD_RSN_MNEMONICS_LEN  42    /* Reason Code Mnemonics Length,      V3.3 */
#define  STD_MAX_COND_NUM		20	  // max number of condiments. JHHJ
#define  STD_NUM_OEP_GROUPS		STD_OEP_MAX_NUM    /* hard-coded '20' changed to STD_OEP_MAX_NUM.
													* number of maximum available group numbers for OEP windows - CSMALL
													*/

//Release 2.1 Additions
#define	 STD_NUM_FUTURE_TTLS		20	  // Number of fincnial totals in TTLCASHIER and others JHHJ
#define	 STD_FILENAME_LENGTH_MAX	64	  // Maximum length of file names, increased from 9, to allow
										  // larger file names to be transfered. JHHJ

#define		STD_NUM_OF_TERMINALS	16
#define		STD_NUM_OF_CASHIERS		300
#define  STD_MAX_NUM_LOGO_FILES	10
#define  STD_MAX_PATH_LOGO		260
#define	 STD_MAX_MISC_SIZE		9


/*
/////////////////////////////////////////////////////////////////////////////
*/

// standard Windows data types that are defined here as ECR.h is included in
// source code files that need these types but do not include Windows.h
#ifndef _WINDOWS_

typedef char            CHAR;
typedef unsigned char   UCHAR;
typedef unsigned short  WCHAR;

typedef const WCHAR     *LPCWSTR;
typedef WCHAR           *LPWSTR;
typedef unsigned long   DWORD;

typedef float           FLOAT;
typedef double          DOUBLE;

typedef int             BOOL;

typedef short           SHORT;
typedef unsigned short  USHORT;

typedef long            LONG;
typedef unsigned long   ULONG;

#endif

//this new type has been defined for AC209, in 
//which we will be using a DWORD to reference the 
//color that a user has chosen for various items CSMALL
typedef DWORD   NHPOSCOLORREF;

#ifndef VOID
#define VOID            void
#endif

#ifndef _WINDEF_

#define PASCAL      __stdcall
#define FAR
#define NEAR
#define LOADDS
#define CDECL       __cdecl
#define SDECL       __stdcall

#endif

// if the wide character macro is not defined, define it
// required for wide character implementation of PCIF
#ifndef WIDE
#define WIDE(s) L##s
#endif


/*
/////////////////////////////////////////////////////////////////////////////
*/

#if     (defined _WIN32_WCE || defined WIN32) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef short           D4DIGITS;

typedef long            D9DIGITS;

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
typedef struct {
    long  lLow9Digits;
    short sBillions;
} D13DIGITS;
#endif

typedef __int64     D13DIGITS;

typedef struct {                            /* 64 bit data */
    char  achHuge[8];
} HUGEINT;

#if     (defined _WIN32_WCE || defined WIN32) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*
/////////////////////////////////////////////////////////////////////////////
*/

extern D13DIGITS CONST NULL13DIGITS; 

/*
/////////////////////////////////////////////////////////////////////////////
*/

#ifdef __cplusplus
}
#endif

#endif                                      /* _INC_ECR */
