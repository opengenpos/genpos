/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------

		NeighborhoodPOS Software Product Development Project
		Department of Information Systems
		College of Information Technology
		Georgia Southern University
		Statesboro, Georgia

		Copyright Georgia Southern University Foundation

*---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// ECR.H :
//
// Copyright (C) 1999 NCR Corporation, All rights reserved.
//
// --------------------------------------------------------------------------
//    Georgia Southern University Research Services Foundation
//    donated by NCR to the research foundation in 2002 and maintained here
//    since.
//       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
//       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
//       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
//       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
//       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
//       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
//       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
//
//    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
// --------------------------------------------------------------------------
// History:
//
// Date         Version  Author       Comments
// ===========  =======  ===========  =======================================
// AUG-20-1999  1.00.00  T.Koyama     initial (2171)
// SEP-17-1999           M.Teraki     Changed type definition of 'D13DIGITS'
// Feb-09-2000           K.Iwata      Added definition of '_DEBUG_BREAK'
// Apr-19-2015  02.02.01 R.Chambers   replace TTL_MAX_BLOCK in ttl.h with STD_MAX_HOURBLK
// Apr-21-2015  02.02.01 R.Chambers   moved CONSOLIMAXSIZE to here from storage.h to eliminate storage.h
// Sep-23-2023  0x.04.00 R.Chambers   removed the unused definition of '_DEBUG_BREAK'
/////////////////////////////////////////////////////////////////////////////
*/

#ifndef _INC_ECR
#define _INC_ECR

#include "termcfg.h"

// To change to GenPOS Rel 2.3.0 and later behavior define this
// Defining this Preprocessor constant will turn on several different Rel 2.3.features and functionality
//    DsiClient functionality to enable EMV chip credit card processing
//    Store and Forward disabled in Supervisor Mode menus
//    import correct type library to allow compatibility with Windows XP when using Visual Studio 2013.
#define GENPOS_REL_020300

// Following define is being used to enable/disable rarely needed logs. These are logs
// that provide little to no value in most cases however actually removing the possibility
// of the log does not seem like a good idea.
// Uncomment the following define to enable the rarely needed logs.
//#define RARELY_NEEDED_LOGS  1

// To choose between the Microsoft SQL Server database engine and SQLite database engine.
// The following defines are used to determine which ODBC Connect String is to be used
// to connect to the database engine for PLU Totals.
//
// Microsoft has provided a version of SQL Server, localDB, which is an embedded version
// of SQL Server. It runs as a COM object in a DLL and appears to have first been introduced
// with SQL Server 2005 and Visual Studio 2005. There are later versions however I haven't
// had much success in getting it to work.
//    Richard Chambers, May-27-2021

//#define USE_PLU_TOTALS_SQLSERVER                // the standard SQL Server Express database engine used since 2004 and NHPOS 2.0.0
#define USE_PLU_TOTALS_SQLITE                 // the SQLite embedded database engine from experiments around 2008
//#define USE_PLU_TOTALS_LOCALDB                  // the standard LocalDB embedded version of SQL Server Express database engine
//#define USE_PLU_TOTALS_JET                    // the Jet Access database engine, Microsoft.Jet.OLEDB.4.0
//#define USE_PLU_TOTALS_SQLSERVER_EMBEDDED     // the SQL Server localDB embedded database engine

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

// This is a holdover from NHPOS Rel 1.4 and earlier.
// The FAR and NEAR defines were for old near/far pointers for a
// segmented memory model such as the old Intel 16 bit 8080/8086
// processors which is no longer used.
// I think at one point both NHPOS/GenPOS and PEP shared many of
// these header files and PEP ran on Windows 3.x.
//    Richard Chambers, Aug-05-2021
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

#ifndef FP_SEG
#define FP_SEG(fp)          (*((USHORT FAR *)&(fp) + 1))
#endif
#ifndef FP_OFF
#define FP_OFF(fp)          (*((USHORT FAR *)&(fp)))
#endif

/*
/////////////////////////////////////////////////////////////////////////////
*/

#ifndef STUBENTRY
#define STUBENTRY       _stdcall
#endif

/*
/////////////////////////////////////////////////////////////////////////////
*/

//  See also the following functions which are used to replace library functions that were used
//  with UCHAR character strings with NHPOS 1.4 and the NCR 7448 but were replaced with
//  TCHAR/WCHAR character strings with NHPOS 2.0 and the move to Windows XP and touch screen.
//
//    TCHAR * tcharnset (TCHAR * string, TCHAR val, size_t count)      -> replaces memset()
//    TCHAR * tcharncpy(TCHAR *tcDest, TCHAR *tcSource, size_t count)  -> replaces memcpy()
//
//    TCHAR *tcharTrimLeading(TCHAR  *tcString)
//    TCHAR *tcharTrimRight(TCHAR  *tcString)

// Determine number of characters based on size of the TCHAR character array
#define TCHARSIZEOF(x)  (sizeof(x)/sizeof(TCHAR))

// Determine number of characters based on size of the UCHAR character array
#define UCHARSIZEOF(x)  (sizeof(x)/sizeof(UCHAR))

// Determine number elements in array based on size of first element
#define ARRAYSIZEOF(x)  (sizeof(x)/sizeof((x)[0]))

#ifndef FARCONST
// This is a holdover from NHPOS Rel 1.4 and earlier.
// While it would seem reasonable to define FARCONST as CONST
// doing so generates lots of warnings.
// The FAR part of this was for old near/far pointers for a
// segmented memory model such as the old Intel 16 bit 8080/8086
// processors which is no longer used.
// I think at one point both NHPOS/GenPOS and PEP shared many of
// these header files and PEP ran on Windows 3.x.
//    Richard Chambers, Aug-05-2021
#define FARCONST
#endif

#ifndef UNINT
#define UNINT
#endif

#ifndef GCNUM
#define GCNUM		USHORT
#endif

// if the constant for the biometrics file is not defined then define it.
#ifndef DFPR_FNAME
#define DFPR_FNAME L"DFPR_DB"
#endif

/*
*===========================================================================
*   Support Pheripheral Device
*   These defines indicate which peripheral devices such as printers and
*   scanners are supported by this build by commenting any of these
*   lines of source out.
*===========================================================================
*/
#define PERIPHERAL_DEVICE_SCANNER             /* See also SCF_TYPE_SCANNER. V3.3 */
#define PERIPHERAL_DEVICE_SCALE               /* See also SCF_TYPE_SCALE. */
#define PERIPHERAL_DEVICE_MSR                 /* See also SCF_TYPE_MSR, SCF_TYPE_MSR1, and SCF_TYPE_MSR2. */
#define PERIPHERAL_DEVICE_VIRTUALKEY          /* See also SCF_TYPE_SCANNER. */
#define PERIPHERAL_DEVICE_WAITER_LOCK         /* See also SCF_TYPE_SCANNER. */
#define PERIPHERAL_DEVICE_OTHER     7

//--------------------------------------------
//		General system wide database constants that are defined here.
//		These contants are used in a wide variety of places when defining structs
#define  STD_PLU_MNEMONIC_LEN   20      // Number of characters in a PLU mnemonic, PLU_MNEMONIC_LEN
#define  STD_PLU_NUMBER_LEN		14		// Number of characters in a PLU number  PLU_NUMBER_LEN
#define  STD_NO_BONUS_TTL		100		// Number of Bonus Totals JHHJ 8->100
#define  STD_NO_FOREIGN_TTL      8      // Number of foreign totals
#define  STD_TENDER_MAX			20		// Number of Tender Keys 11->20 JHHJ 3-16-04
#define  STD_TENDER_MISC        (STD_TENDER_MAX - 3)  // Standard number of lMiscTender[i] tender items.  excludes first 3 types.
#define	 STD_TOTAL_MAX			20		// Number of Total Keys 9-20 JHHJ 3-24-04

#define  STD_OEP_MAX_NUM		20      // Number of OEP lines/buttons on a single window. OEP enhanced uses window size
#define  STD_MAX_NUM_PLUDEPT_CPN 7		// Number of PLUS/DEPT in a single Coupon
#define  STD_MAX_HOURBLK        48      /* Max number of Service Time and Hourly Activity Totals, see also MAX_HOUR_NO */
#define  STD_MAX_SERVICE_TOTAL   3      // maximum number of service totals and maxim number of add check totals

#define	 STD_DISC_ITEMIZERS_MAX	 6		// Number of Discount Itemizers 2(US), 3(CAN), 2(VAT) -> 6 JHHJ 3-29-04
#define	 STD_DISC_ITEMIZERS_CAN	 3		// Number of Discount Itemizers 2(US), 3(CAN), 2(VAT) -> 6 JHHJ 3-29-04
#define	 STD_DISC_ITEMIZERS_US	 2		// Number of Discount Itemizers 2(US), 3(CAN), 2(VAT) -> 6 JHHJ 3-29-04
#define	 STD_DISC_ITEMIZERS_VAT	 2		// Number of Discount Itemizers 2(US), 3(CAN), 2(VAT) -> 6 JHHJ 3-29-04

// WARNING: There are a number of places where tax calculation loops are for non-International (VAT)
//          and those loops use a limit of STD_TAX_ITEMIZERS_CAN though they are used for
//          both Canadian and US tax calculations.
#define  STD_TAX_ITEMIZERS_MAX   5		// Number of Tax Itemizers 3(US), 4(CAN) ->5 JHHJ 3-30-04
#define  STD_TAX_ITEMIZERS_CAN   4		// Number of Canadian Tax Itemizers that are actually used (GST, PST1, PST2, PST3). See also PLU_AFFECT_TAX1, etc.
#define  STD_TAX_ITEMIZERS_US    3		// Number of US Tax Itemizers that are actually used. See also PLU_AFFECT_TAX1, etc.
#define  STD_TAX_ITEMIZERS_VAT   3		// Number of VAT Int'l Tax Itemizers that are actually used. See also PLU_AFFECT_TAX1, etc.

#define	 STD_PLU_ITEMIZERS		10		// Number of PLU tax Itemizers for CANTAX. See list of Canadian tax itemizers below.
#define	 STD_PLU_ITEMIZERS_MOD	 (STD_PLU_ITEMIZERS - 1)    // zero based count of STD_PLU_ITEMIZERS, range of PLU data - see PLU_AFFECT_TAX1, used in several places for max PLU itemizer for Canada tax.
#define  STD_PLU_ITEMIZERS_PLUS  (STD_PLU_ITEMIZERS+2+3)    // Number of PLU Itemizers plus limitable/Tax-On-Tax and GST/PST. see TRANCANADA_LIMITABLE and TRANCANADA_PST1TOT

// Defines for standard ASCII character codes used with communication messages for older or legacy devices.
// Some of these codes are used in various subsystems when sending and receiving messages to devices such
// as printers, scanners, drink portion dispensers, electronic payment, etc. which are using a COM type
// of connection such as through RS-232 or USB. This list is to replace the device specific lists
// such as DIF_STX, UIE_SCANNER_STX, DFL_STX, EEPT_STX, KDS_STX, EPT_STX, 
// See the Wikipedia topic, https://en.wikipedia.org/wiki/Control_character
// See also RFC20 (1969), https://tools.ietf.org/rfc/rfc20.txt
#define STD_ASCII_COMM_SOH              0x01    /* start of heading indicator */
#define STD_ASCII_COMM_STX              0x02    /* start of text indicator */
#define STD_ASCII_COMM_ETX              0x03    /* end of text indicator */
#define STD_ASCII_COMM_EOT              0x04    /* end of transmission indicator */
#define STD_ASCII_COMM_ENQ              0x05    /* enquiry indicator */
#define STD_ASCII_COMM_ACK              0x06    /* acknowledge indicator */
#define STD_ASCII_COMM_BEL              0x06    /* bell sound indicator */
#define STD_ASCII_COMM_DLE              0x10    /* data link escape indicator */
#define STD_ASCII_COMM_DC1              0x11    /* device control 1 indicator, XON character */
#define STD_ASCII_COMM_DC2              0x12    /* device control 2 indicator */
#define STD_ASCII_COMM_DC3              0x13    /* device control 3 indicator, XOFF character */
#define STD_ASCII_COMM_DC4              0x14    /* device control 4 indicator */
#define STD_ASCII_COMM_NAK              0x15    /* not acknowledge indicator */
#define STD_ASCII_COMM_ESC              0x1b    /* Escape character */


/*
 *    Canadian tax calculations use a series of tax buckets such as the following.
 *    See also CANTAX_MEAL, etc. and how they are used as well as TAX_MOD_MASK bits in pItemSales->fbModifier
 *    See also MOD_TAXABLE1, etc. and how they are used with ItemModLocal.fsTaxable.
 *        CanTaxWork->lTaxable[0]           // meal, indicated by CANTAX_MEAL
 *        CanTaxWork->lTaxable[1]           // carbonated beverage, indicated by CANTAX_CARBON
 *        CanTaxWork->lTaxable[2]           // snack, indicated by CANTAX_SNACK
 *        CanTaxWork->lTaxable[3]           // beer/wine, indicated by CANTAX_BEER
 *        CanTaxWork->lTaxable[4]           // liquor, indicated by CANTAX_LIQUOR
 *        CanTaxWork->lTaxable[5]           // grocery (non-taxable), indicated by CANTAX_GROCERY
 *        CanTaxWork->lTaxable[6]           // tip, indicated by CANTAX_TIP
 *        CanTaxWork->lTaxable[7]           // PST1 only, indicated by CANTAX_PST1
 *        CanTaxWork->lTaxable[8]           // baked goods, indicated by CANTAX_BAKED
 *        CanTaxWork->lTaxable[9]           // special baked goods, indicated by CANTAX_SPEC_BAKED (added for Rel 2.0 by JHHJ) - due to Tim Horton's tax calc error, perhaps defects of implementation?
 *
 *        CanTaxWork->lTaxable[10]          // limitable
 *        CanTaxWork->lTaxable[11]          // tax on tax
 *        CanTaxWork->lTaxable[12]          // GST
 *        CanTaxWork->lTaxable[13]          // PST1 limitable
 *        CanTaxWork->lTaxable[14]          // PST1 tax on tax
*/



// the following constant is used for the calculation of buffer sizes in several places.
// we have moved this define from storage.h to here to eliminate the include file storage.h
// however it would really be nice to eliminate this contant altogether.
#define		CONSOLIMAXSIZE		0x1000

//  The following constants are used to mark the number of
//  characters that a particular kind of mnemonic string may
//  contain.  These values are for the max number of characters
//  and do not contain room for the end of string character.
//  To use these constants when defining a mnemonic array, you will
//  need to add one to the constant as in mnemonic[ECR_NUMBER_SIZE + 1].
//
//  For Flexible Memory max sizes such as max number of Cashiers, Number of Deptments, etc.
//  see the FLEX_aaaaa defines in file paraequ.h such as FLEX_DEPT_MAX, FLEX_CAS_MAX, etc.
//
#define  STD_AUTHCODE_LEN       24    // EPT authorization code
#define  STD_ACCT_NUMBER_LEN    10    // Account Number
#define  STD_ADJNUM_GROUP		 5	  // Number of adjectives in a group
#define  STD_ADJMNEMO_LEN        4    // adjective mnemonic from UNINIRAM and pararead () - PARA_ADJMNEMO_LEN
#define  STD_BIRTHDAY_LEN        6    // birthday entry (MMDDYY)
#define  STD_CASHIERNAME_LEN    20    // cashier name field
#define  STD_CASHIERNO_LEN       3    // cashier number field
#define  STD_CPNNAME_LEN        20    // coupon name field - PARA_CPN_LEN
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
#define  STD_TOUCH_MENU_PAGE	10	  // all touchscreen terminals will have the default menu page of 10, this is helpful when doing terminal specific checks.
#define	 STD_MAX_COND_NUM		20	  // Max number of condiments allowed

#define  STD_MAX_MULTI_CHECK     5    // size of arrays for multi check in ITEMTRANSLOCAL

#define  STD_NUM_OEP_GROUPS		STD_OEP_MAX_NUM    /* hard-coded '20' changed to STD_OEP_MAX_NUM.
													* number of maximum available group numbers for OEP windows - CSMALL
													*/

// Following defines are used with the OEP table functionality to indicates
// special PLU group numbers that are used to specify particular actions and
// behavior from the Order Entry Prompting (OEP) functionality.
#define SLOEP_BEGIN_SPL     90  // Group numbers below this are real PLU groups, groups above are special
#define SLOEP_FREEONE       91  // Special group, one items free
#define SLOEP_FREETWO       92  // Special group, two items free
#define SLOEP_FREETHREE     93  // Special group, three items free
#define SLOEP_FREEFOUR      94  // Special group, four items free
#define SLOEP_SELFROMKEY    97  // Special group, operator to key in PLU from following group
#define SLOEP_COMPNO        98  // Special group, one item in next group is mandatory
#define SLOEP_MULTI         99  // Special group, multiple select, repeat until 00 entered to exit
#define SLOEP_DUMMY         0xFF

#define SLOEP_SPL_ASK      201  // Special Group, used for selecting window for Ask, see also BL_OEPGROUP_STR_RESET

#define STD_FORMATMASK_REMOVE      0x0fff    // used to remove any flags from a mnemonic address.
#define STD_FORMATMASK_INDENT_4    0x1000    // used with various mnemonic addresses to indicate that mnemonic should be indented 4 spaces
#define STD_FORMATMASK_NO_AMOUNT   0x2000    // used with various mnemonic addresses to indicate that print line should not include amount

//Release 2.1 Additions
#define	 STD_NUM_FUTURE_TTLS	20	  // Number of fincnial totals in TTLCASHIER and others JHHJ
#define	 STD_FILENAME_LENGTH_MAX 64	  // Maximum length of file names, increased from 9, to allow
									  // larger file names to be transfered. JHHJ

#define	 STD_FILE_PATH_LEN		100	  // max file length
#define  STD_MAX_PRIORITY_CPNS	50	  // maximum number of priority auto coupons JHHJ
#define	 STD_MAX_NUM_DEVICE		50	  // maximum number of devices to be shown in device config

#define	 STD_TTLUPDDB_SIZE_MOD 8	  // modifier to increase size of total delay balance update file 

#define  STD_OD_ASZNUMBER_INDEX   2   // index into aszNumber of TRANSTORAGESALES for order declaration mnemonic for CLASS_ITEMORDERDEC

#define  STD_DIALOG_NEWLINE     _T("\r\n")      // used with EDITTEXT control of Windows dialogs to insert a new line if ES_MULTILINE | ES_WANTRETURN 

// These were previously used constants which will be slowly replaced
// as source code is changed.
#define     NUM_EXPIRA          4                 // charge posting expiration date
#define     NUM_MSRDATA         STD_MSRDATA_LEN   // charge posting msr data
#define     NUM_MSRDATA_PADDED  (STD_MSRDATA_LEN+6)   // msr data padded for encryption/decryption, RflEncryptByteString(), RflDecryptByteString()
#define		NUM_AUTHCODE		24				  // XEPT authorization Code
#define		NUM_REFERENCE		24				  // XEPT Reference Number
#define		NUM_ROUTING_NUM		9				  // XEPT Check Routing Number
#define		NUM_SEQUENCE_NUM	20				  // XEPT Check Sequence Number
#define		NUM_ACQREFDATA		40				  // XEPT Acquirer Reference  Number
#define		NUM_INVOICE_NUM		16				  // XEPT Invoice Number
#define     NUM_RECORD_NO       12                // XEPT size of DSI Client NETePay batch record number for Preauth, Void, etc.

#define		STD_NUM_OF_TERMINALS	16            // maximum number of terminals in a cluster. used for per terminal buffer allocations.

#define		ITM_MSR_SEPA2                    0x3d    /* separator of ISO-II MSR  */

#define     STD_FOLDER_FLASHROOT       _T("C:\\FlashDisk\\NCR\\Saratoga")
#define     STD_FOLDER_TEMPROOT        _T("C:\\TempDisk\\NCR\\Saratoga")
#define		STD_FOLDER_PROGRAMFILES    _T("C:\\FlashDisk\\NCR\\Saratoga\\Program")
#define     STD_FOLDER_DATABASEFILES   _T("C:\\FlashDisk\\NCR\\Saratoga\\Database")
#define     STD_FOLDER_LOGFILES        _T("C:\\FlashDisk\\NCR\\Saratoga\\Database")
#define     STD_FOLDER_ICONFILES       _T("C:\\FlashDisk\\NCR\\Saratoga\\Icons")
#define     STD_FOLDER_WEBFOLDER       _T("C:\\FlashDisk\\NCR\\Saratoga\\Web")
#define     STD_FOLDER_PRINTFOLDER     _T("C:\\FlashDisk\\NCR\\Saratoga\\PrintFiles")
#define		STD_FOLDER_QUERYFOLDER	   _T("C:\\FlashDisk\\NCR\\Saratoga\\QueryFiles")
#define     STD_FOLDER_SAVTTLFILES     _T("C:\\FlashDisk\\NCR\\Saratoga\\Database\\SavedTotal")
#define     STD_FOLDER_IMAGESFILES     _T("C:\\FlashDisk\\NCR\\Saratoga\\Database\\Images")
#define     STD_FOLDER_LOGFOLDER       _T("C:\\FlashDisk\\NCR\\Saratoga\\Log")
#define     STD_FOLDER_AMTRAK_CONFIG   _T("C:\\POSNAME")

//  ----     Standard symbols used with printing and displaying 

/* --- AM/PM symbol for 12 hour time format --- */
#define STD_TIME_AM_SYMBOL       _T("AM")        // must be a text string as is used with format %s
#define STD_TIME_PM_SYMBOL       _T("PM")


/* --- FSC_ASK list of Ask Function Definitions from GP2.0,  Saratoga --- */
#define ITM_ASK_EPT_LOOKUP          3           /* EPT Account lookup by name and number for Veterans Canteen Service, VCS */
#define ITM_ASK_EPT                 4           /* EPT Pre-Inquiry */
#define ITM_ASK_RETURNS             6           /* Connection Engine Interface request for a transaction for a Return. See UifItem() */
#define ITM_ASK_PREAUTH             7           /* Preauth list for Amtrak displayed in OEP style window. See UifItem() */
#define ITM_ASK_CHECK_NO_PUR_1      8           /* Preauth list for Amtrak displayed in OEP style window. See UifItem() */
#define ITM_ASK_CHECK_NO_PUR_2      9           /* Preauth list for Amtrak displayed in OEP style window. See UifItem() */
#define ITM_ASK_CHECK_NO_PUR_3      10          /* Preauth list for Amtrak displayed in OEP style window. See UifItem() */
#define ITM_ASK_PREAUTH_BY_GCN      12          /* Preauth list for Amtrak by Guest Check Number. See UifItem() */
#define ITM_ASK_OPENCHECKS_BY_OPID  14          /* Obtain and display in an OEP style window a list of open checks by Operator ID. See UifItem() */
#define ITM_ASK_OPENCHECKS_BY_GCN   15          /* Obtain and display in an OEP style window a list of open checks by Guest Check Number. See UifItem() */
	// Following report types were part of the old electronic payment interface
	// and the old charge post functionality such as COM2170 charge post
	// application from NHPOS 1.4 and the NCR 2170 and NCR 7448 days.
	//   3/29/2021  Richard Chambers
#define ITM_ASK_REPORT5             15          /* Legacy Report Function */
#define ITM_ASK_REPORT6             16          /* Legacy Report Function */
#define ITM_ASK_REPORT7             17          /* Legacy Report Function */
#define ITM_ASK_REPORT8             18          /* Legacy Report Function */
#define ITM_ASK_REPORT9             19          /* Legacy Report Function */
#define ITM_ASK_REPORT10            20          /* Report Function EEPT_FC2_ASK as well as EEPT_FC2_ACTION_EPT. Now used for EMV pad reset with GenPOS Rel 2.3.0 */

#define EXTFSC_ASK_REPORT16    16                   /* Ask(Report) key, used with FSC_ASK  Saratoga */
#define EXTFSC_ASK_REPORT20    20                   /* Ask(Report) key, used with FSC_ASK  Saratoga */

// ------------------------------
//  Defines for Return Value and Status
//  The following defines are being used to standardize function
//  return status indicating if an error occurred or not and if an
//  error did occur then what is the error.
//
//  We are looking to replace all the various error return codes with
//  this set of standard codes in order to reduce the number of different
//  return code defines and to make it easier to use the various function
//  libraries.
//
//  See use of ETK_SUCCESS, GCF_SUCCESS, PRT_SUCCESS, VOS_SUCCESS, FDT_SUCCESS,
//  ISP_SUCCESS, ITM_SUCCESS, TRN_SUCCESS, KPS_SUCCESS, SERV_SUCCESS, DIF_SUCCESS

#define  STD_RET_SUCCESS                 0     // Normal success return value
#define  STD_RET_ERROR                 (-1)   // An unspecified error that caused the function to fail.
#define  STD_RET_FILEEND_BUSY          (-3)   // The end of file or other data source reached or search failed with item not found or resource busy.
#define  STD_RET_TIMEOUT_FATAL         (-7)   // time out or fatal error
#define  STD_RET_ILLEGAL_NOTFOUND      (-8)   // illegal or invalid parameter, resource not found

/*------------------------------
    DEFINE FOR RETURN VALUE
------------------------------*/

#define     STUB_SUCCESS       STD_RET_SUCCESS          /* normal success */

#define     STUB_M_DOWN           (-1)          /* failed request to master */
#define     STUB_BM_DOWN          (-2)          /* failed request to BM */
#define     STUB_BUSY             (-3)          /* busy master server/ stub */
#define     STUB_FRAME_SEQERR     (-4)          /* frame sequence error */
#define     STUB_UNMATCH_TRNO     (-5)          /* transaction # unmatched */
#define     STUB_STOP_UPDATE      (-6)          /* stop request total update */
#define     STUB_TIME_OUT         (-7)          /* time out detected */
#define     STUB_ILLEGAL          (-8)          /* illegal parameter */
#define     STUB_DUR_INQUIRY      (-9)          /* during inquiry state */

#define     STUB_MULTI_SUCCESS    (-10)         /* multiple request success */
#define     STUB_MULTI_SEND       (-11)         /* multiple sending */
#define     STUB_MULTI_RECV       (-12)         /* multiple receiving */
#define     STUB_REQ_CONFIRM      (-13)         /* confirmation request */
#define     STUB_FAITAL           (-14)         /* faital error */
#define     STUB_POWER_DOWN       (-15)         /* power down my terminal */
#define     STUB_INQ_M_DOWN       (-16)         /* Master down at iquiry */
#define     STUB_INQ_BM_DOWN      (-17)         /* B Master down at inquiry */
#define     STUB_NET_NOT_OPENED   (-18)         /* net work not opened */
#define     STUB_IAM_BUSY         (-19)         /* error on my terminal */
#define     STUB_NOT_MASTER       (-20)         /* I'm not master (O/S) */
#define     STUB_SELF             (-21)         /* It's my terminal */

#define     STUB_IGNORE           (-22)         /* ignore this message */

#define     STUB_DB_FULL		  (-23)			/* the delay balance total file is full.*/
#define     STUB_DB_NEARFULL	  (-24)			/* the delay balance total file is near full.*/

#define		STUB_KPS_BUSY		  (-25)			// Indicates failure due to kitchen printer busy
#define		STUB_NO_BACKUP		  (-26)			// Indicates a Backup Master is not provisioned in cluster
#define     STUB_OP_EOF           (-35)         // Same as OP_EOF, End of File indicator from various OP functions such as OpMajorDeptRead().
#define     STUB_COMERROR         (-37)         // Error found during request
#define		STUB_DB_BSY_UPDTING   (-38)			// Busy Updating Delay Balance File, please wait. JHHJ
#define     STUB_DISCOVERY        (-39)         // target terminal not found.  PIF_ERROR_NET_DISCOVERY
#define     STUB_NOT_UPTODATE     (-40)         // this terminal is not up to date (Master or Backup only)
#define     STUB_BUSY_MULTI       (-43)         /* busy master server/ stub during multi */
#define     STUB_MSG_CLEARED      (-44)         /* the target receiver discarded the message, PIF_ERROR_NET_CLEAR */


#define     STUB_PC_ERROR         (-100)        /* constant of PC STUB error */
#define     STUB_NOT_REGISTERED   (-177)        // software is not registered same value as UNREGISTERED_COPY in BlfWif.h

// The following define used to move error codes to the STUB_RETCODE address space
#define     STUB_RETCODE          (-1000)       // add to STUB_ code to generate generic network layer code

#define     STUB_RETCODE_M_DOWN_ERROR     (STUB_RETCODE+STUB_M_DOWN)
#define     STUB_RETCODE_BM_DOWN_ERROR    (STUB_RETCODE+STUB_BM_DOWN)
#define     STUB_RETCODE_BUSY_ERROR       (STUB_RETCODE+STUB_BUSY)
#define     STUB_RETCODE_TIME_OUT_ERROR   (STUB_RETCODE+STUB_TIME_OUT)
#define     STUB_RETCODE_DISCOVERY_ERROR  (STUB_RETCODE+STUB_DISCOVERY)
#define     STUB_RETCODE_UNMATCH_TRNO     (STUB_RETCODE+STUB_UNMATCH_TRNO)
#define     STUB_RETCODE_DUR_INQUIRY      (STUB_RETCODE+STUB_DUR_INQUIRY)
#define     STUB_RETCODE_MSG_CLEARED      (STUB_RETCODE+STUB_MSG_CLEARED)
#define     STUB_RETCODE_NOT_MASTER_ERROR (STUB_RETCODE+STUB_NOT_MASTER)

// The following define used to move error codes to the ECR_RETCODE address space
// The following are standardized error codes use through out the functionality to indicate
// a detected problem.
#define     ECR_RETCODE           (-2000)       // add to various error codes to ensure out of the STUB_ address space

#define     ECR_FILE_NOT_FOUND     (ECR_RETCODE + (-1))
#define     ECR_FILE_FULL          (ECR_RETCODE + (-2))
#define     ECR_NOT_IN_FILE        (ECR_RETCODE + (-3))
#define     ECR_LOCK               (ECR_RETCODE + (-5))
#define     ECR_NO_MAKE_FILE       (ECR_RETCODE + (-6))
#define     ECR_ALREADY_LOCK       (ECR_RETCODE + (-10))
#define     ECR_NOT_ALLOWED        (ECR_RETCODE + (-19))
#define     ECR_NOT_MASTER         (ECR_RETCODE + (-20))

#define     ECR_SPCL_RETCODE      (-3000)     // add to various error codes in a reusable address space


// Comments on U/D & O/D usage added by cwunn on 12.10.2002
// In the following macro constants, U/D means updating and O/D means Out of Date
//  *Updating the master or backup master occurs quite often: every time a total
//   is adjusted, or an employee times\signs in\out, or a report is taken.  Thus,
//   the master/bmaster is usually only in the state briefly.
//  *Should the master or bmaster be out of date, the user must synchronize the
//   data between the two machines.  Thus, the O/D state lasts much longer than
//   the U/D state, and actually lasts an indefinite time until the user corrects
//   the out of date machine.  Synchronizing Master and Backup is done with AC42
//   in Supervisor Mode.
//
//  To better understand the meaning of these constants look at the function
//  NbDescriptor () which shows how Notice Board decodes the Notice Board flags
//  to display the Master and Backup Master status and then look at the function
//  SerComReadStatus () to see how the Server subsystem interprets the Notice
//  Board flags to create a Server subsystem version of the status using the bits
//  defined below.  Also see how Client uses Notice Board status as used in
//  the function CstComReadStatus () in file csstbcom.c.
//
// See file csstubin.h in the Client library of the BusinessLogic subsystem
// for additional information for these defines.
// Please make sure that changes to these defines coincide with changes
// to the CLI_STS_zzzz defines in file csstubin.h such as CLI_STS_M_UPDATE
#define     BLI_STS_M_UPDATE     0x0001     /* master U/D or O/D, CLI_STS_M_UPDATE */
#define     BLI_STS_BM_UPDATE    0x0002     /* B. master U/D or O/D, CLI_STS_BM_UPDATE */
#define     BLI_STS_M_OFFLINE    0x0004     /* master off-line, CLI_STS_M_OFFLINE */
#define     BLI_STS_BM_OFFLINE   0x0008     /* B. master off-line, CLI_STS_BM_OFFLINE */
#define     BLI_STS_STOP         0x0020     /* STOP request, CLI_STS_STOP */
#define     BLI_STS_INQUIRY      0x0040     /* inquiry status, CLI_STS_INQUIRY */
#define     BLI_STS_INQTIMEOUT   0x0080     /* inquiry time out, CLI_STS_INQTIMEOUT */
#define     BLI_STS_BACKUP       0x0100     // indicates that a Backup Master is provisioned, CLI_STS_BACKUP_FOUND
#define     BLI_STS_M_REACHABLE  0x0200     /* master off-line but is reachable, CLI_STS_M_REACHABLE */
// 0xf000, upper nibble is reserved for local defines such as ISP_COMSTS_AC85 in ISP subsystem

/*
/////////////////////////////////////////////////////////////////////////////
*/

// The following two constants are used to determine the low and
// high values for the range of PLU numbers which are special PLUs.
// What makes these special is that these PLUs will not be displayed
// in the receipt window of the terminal nor will these PLUs be printed
// on the receipt.
//
// See function RflIsSpecialPlu() for usage of these constants.
//
// The purpose of these special PLUs is to provide a way of causing an
// OEP popup window to display a list of other PLUs associated with
// the special PLU.  The defines below specify the complete PLU number.
// The actual range of PLUs is 9950 through 9999.
#define MLD_NO_DISP_PLU_LOW    _T("00000000099500")
#define MLD_NO_DISP_PLU_HIGH   _T("00000000099999")

// The following constant is used to test that a PLU number is valid rather
// than being empty or a dummy. In most places data areas are initialized with
// values of binary zero so this is used to test if a PLU number array has a
// value or not.  This is same as TCHAR    auchDummy[NUM_PLU_LEN] = {0};
#define MLD_NO_DISP_PLU_DUMMY  _T("\0\0\0\0\0\0\0\0\0\0\0\0\0\0")
#define MLD_ZERO_PLU           _T("00000000000000")    // aszDummy[]  or invalid PLU number

// The following constant is used to test that a PLU number data field is not
// being used for a PLU. This constant is primarily used with the struct PRTIDX
// and the PLU number data field in that struct for transaction data storage files.
#define MLD_NONSALESITEM_PLU   _T("--------------")


// following constants used in various places for sizing buffers used
// with broadcast of data and with backups between terminals.
#define  OP_BACKUP_WORK_SIZE     450
#define  OP_BROADCAST_SIZE       450


#ifndef _WINDEF_
typedef int				BOOL;
typedef unsigned short  WCHAR;
typedef unsigned short TCHAR;
typedef CONST WCHAR *LPCWSTR, *PCWSTR;
#endif

typedef char            CHAR;
typedef short           SHORT;
typedef unsigned long   DWORD;

#ifndef _INC_WINDOWS
typedef long            LONG;
typedef unsigned long   HWND;
typedef unsigned int    UINT;

// following are duplicated from WINDEF.H from Microsoft
typedef UINT WPARAM;
typedef LONG LPARAM;
typedef LONG LRESULT;

typedef float           FLOAT;
typedef double          DOUBLE;

typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned long   ULONG;
typedef unsigned long long  ULONGLONG;
#endif

typedef unsigned long   NHPOSCOLORREF;	//colorpalette-changes

// -----------------------------------------------------------------------
//  Following sizes are used for transaction data files to specify file
//  size and file offset for transaction data.
//
//  See also typedefs in pif.h for PifFileHandle and PifSemHandle
typedef USHORT  TrnFileSize;
typedef USHORT  TrnVliOffset;
typedef SHORT   TrnStorageType;      // see list of TRN_TYPE_ such as TRN_TYPE_ITEMSTORAGE.

/*------------------------------------------
    External symbol access for files.
	We have three different file locations that are used for general
	operations within GenPOS.
	 - the standard Database folder, C:\FlashDisk\NCR\Saratoga\Database
	 - the temporary file folder, C:\TempDisk\NCR\Saratoga\Database
     - the saved total folder which is located in the Database folder, C:\FlashDisk\NCR\Saratoga\Database\SavedTotal

	See the definitions for these variables in file nwop.c in the Notice Board functionality.
-------------------------------------------*/
extern UCHAR CONST  auchNEW_FILE_WRITE[];        /* to csop.c */
extern UCHAR CONST  auchNEW_FILE_READ_WRITE[];
extern UCHAR CONST  auchOLD_FILE_WRITE[];        /* to csop.c */
extern UCHAR CONST  auchOLD_FILE_READ_WRITE[];   /* to csop.c */
extern UCHAR CONST  auchOLD_FILE_READ[];         /* to csop.c */

extern UCHAR CONST  auchTEMP_NEW_FILE_WRITE[];
extern UCHAR CONST  auchTEMP_NEW_FILE_READ_WRITE[];
extern UCHAR CONST  auchTEMP_OLD_FILE_WRITE[];
extern UCHAR CONST  auchTEMP_OLD_FILE_READ_WRITE[];
extern UCHAR CONST  auchTEMP_OLD_FILE_READ[];

extern UCHAR CONST  auchNEW_FILE_WRITE_FLASH[];
extern UCHAR CONST  auchNEW_FILE_READ_WRITE_FLASH[];   
extern UCHAR CONST  auchOLD_FILE_WRITE_FLASH[];
extern UCHAR CONST  auchOLD_FILE_READ_WRITE_FLASH[];  
extern UCHAR CONST  auchOLD_FILE_READ_WRITE_FLASH_EX[];   

extern UCHAR CONST  auchNEW_FILE_WRITE_SAVTTL[];
extern UCHAR CONST  auchNEW_FILE_READ_WRITE_SAVTTL[];
extern UCHAR CONST  auchOLD_FILE_WRITE_SAVTTL[];
extern UCHAR CONST  auchOLD_FILE_READ_WRITE_SAVTTL[];

/*
/////////////////////////////////////////////////////////////////////////////
*/

#if     (defined _WIN32_WCE || defined WIN32) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef short       D4DIGITS;
typedef long        D9DIGITS;
typedef __int64		D13DIGITS;

#if defined(USE_2170_ORIGINAL)
typedef struct {                            /* 64 bit data */
    char  achHuge[8];
} HUGEINT;
#endif

// standard currency value for amounts and totals, etc.
// DCURRENCY is the type for currency amounts. Any format used to display $ formatted data on printer or screen requires
//           that a variable that is not of DCURRENCY is casted to DCURRENCY in order to print or display correctly.
//           Functions affected include but are not limited to PmgPrintf(), PrtPrintf(), MldPrintf()
//           as these all end up using function _RflFormStr() for formatted prints.
// DCLL is a macro to force a constant to be either long or long long in the
// argument list of a function call taking variable arguments such as UiePrintf().
// V_DCURRENCY is a macro which is used in the totals database update
// to correctly set the element of the VARIANT or COleVariant variable
// used to provide data for the SQL statements used with SQL Server.
//
//    WARNING:    Some functions use print formats whose behavior depends on the
//                interpretation of format specifiers of function _RflFormStr().
//                Any changes as to variable type or the behavior of the function
//                _RflFormStr() will affect the displayed output.
//
//                A format specifier such as %6.2$ which requires a SHORT is being used to print
//                a percentage value and a format specifier such as %7.*l$ is being used to print
//                a scale weight(the * in the specifier allows for an additional argument to
//                specify the actual number of decimal places which depends on whether the scale
//                weighs hundredths or thousandths of a weight unit such as pounds or kilos)
//                The typedef DWEIGHT has been introduced to help identify places where the
//                value being printed with a currency specifier is something other than a
//                currency value.
#if 0
// following define is used in places where the DCURRENCY type is used to trigger other
// changes in behavior such as SQL statements in Total PLU. Not defined if not 64 bit totals.
//#define  DCURRENCY_LONGLONG

typedef short DSCOUNTER;    // used with totals for counts, non-64 bit is short but change to long
typedef long  DLCOUNTER;    // used with totals for counts
typedef long  DCURRENCY;    // used for variables that hold a currency amount. also used to cast for variables in prints that use $ field such as RPTPLU * uspData->usPercent
typedef long  DWEIGHT;      // used to cast for variables holding a scale weight for prints that use %l$ format specifier to print decimal in hundredths or thousandths.
typedef long  DUNITPRICE;   // used for unit price 

#define V_DCURRENCY(x) V_I4(x)
#define DCLL(x)  x##L

// Special constants used in the business logic which may change
#define  STD_MAX_TOTAL_AMT     999999999L    // Maximum number for total currency amount
#define  STD_PLU_MAX_PRICE       9999999L    /* limitation on PLU price for building up PLU, constrained by PLU record limitation. */
#define  STD_ATFOR_MAX_QTY           999L    /* limitation on @/For quantity */
#define  STD_MLD_MINUS_10_DIGITS   -1000000000L    /* lTotal below this value is considered possible error in MLD, see MLD_MINUS_10DIGITS */

#else
// following define is used in places where the DCURRENCY type is used to trigger other
// changes in behavior such as SQL statements in Total PLU.
#define  DCURRENCY_LONGLONG

typedef long  DSCOUNTER;    // used with totals for counts, non-64 bit is short but change to long
typedef long  DLCOUNTER;    // used with totals for counts
typedef long long	DCURRENCY;// used for variables that hold a currency amount. also used to cast for variables in prints that use $ field such as RPTPLU * uspData->usPercent
typedef long long   DWEIGHT;  // used to cast for variables holding a scale weight for prints that use %l$ format specifier to print decimal in hundredths or thousandths.
typedef long  DUNITPRICE;     // used for unit price 

#define V_DCURRENCY(x) V_I8(x)
#define DCLL(x)  x##LL

// Special constants used in the business logic which may change
// Redefine STD_MAX_TOTAL_AMT the Maximum number for total currency amount with long long DCURRENCY
#define  STD_MAX_TOTAL_AMT     99999999999999LL 
// before change in define for STD_PLU_MAX_PRICE the storage size of price in the PLU
// database must be changed. See functions RflConv4bto3b() and RflConv3bto4b() as well.
#define  STD_PLU_MAX_PRICE       9999999L    /* limitation on PLU price for building up PLU, constrained by PLU record limitation. */
#define  STD_ATFOR_MAX_QTY           999L    /* limitation on @/For quantity */
#define  STD_MLD_MINUS_10_DIGITS   -1000000000000LL    /* lTotal below this value is considered possible error in MLD, see MLD_MINUS_10DIGITS */

#endif

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

//-----------  Begin of stuff for OuputDebugString functionality
#if defined(_DEBUG) || defined(DEBUG)

#if !defined(OutputDebugString)

// declare the standard debug print function which prints to the IDE Debug window.
// This function is part of the standard Kernel32.dll and is defined in Kernel32.lib.
// According to the MSDN library,
//    If the application has no debugger, the system debugger displays the string. If the
//    application has no debugger and the system debugger is not active, OutputDebugString
//    does nothing."
// In order to use this function we also need some other things defined for us.
// If OutputDebugString is not defined then none of the rest of this is probably defined
// either but we do guard these to make sure.

#if !defined(_KERNEL32_) && ! defined(DECLSPEC_IMPORT)
#define DECLSPEC_IMPORT __declspec(dllimport)
#define WINBASEAPI DECLSPEC_IMPORT
#else
#define WINBASEAPI
#endif

#if !defined(WINAPI)
#define WINAPI      __stdcall
#endif

#if !defined TCHAR
typedef WCHAR  TCHAR;
#define TCHAR WCHAR
#endif

// The following ensures that the TEXT macro will be defined properly even if the
// including file being compiled does not include Windows.h.
#if !defined(TEXT)
#if defined(UNICODE) || defined(_UNICODE)
#define TEXT(x) L##x
#else
#define TEXT(x) x
#endif
#endif

WINBASEAPI
VOID
WINAPI
OutputDebugStringW(
    LPCWSTR lpOutputString
    );

#ifdef UNICODE
#define OutputDebugString  OutputDebugStringW
#else
#define OutputDebugString  OutputDebugStringA
#endif // !UNICODE

#endif // !OutputDebugString


#endif
//-----------  End of stuff for OuputDebugString functionality

#ifdef __cplusplus
}
#endif

// Following are also defined in file pif.h for those files which do not include ecr.h
#if 1
#if !defined(PifLog)
#define PifLog(usModuleId,usExceptionCode)  PifLogExtended (usModuleId, usExceptionCode, __FILE__ , __LINE__)
#endif
#endif

#if 1
#if !defined(PifAbort)
#define PifAbort(usModuleId,usExceptionCode)  PifAbortExtended (usModuleId, usExceptionCode, __FILE__ , __LINE__)
#endif
#endif

// NOTES and STANDARDS for ASSRTLOG logs to assist customer support.
// The logs that are generated by using the ASSRTLOG functionality are useful for several notifications.
// We are using the following standard prefixes or leads in log text to categorize ASSRTLOG logs that may be useful.
//  ==PROVISION  -> these logs indicate why an error dialog (usually Prohibited) has been generated due to provisioning
//  ==NOTE       -> these logs indicate something that may be useful for investigating a field issue
//  ==ACTION     -> these logs indicate an event involving an action such as a Cashier Sign-in
//  ==STATE      -> these logs indicate some kind of a state change
//  ==WARNING    -> these logs indicate an unexpected result which may indicate a problem
//  **ERROR      -> these logs indicate an error condition that has been discovered
//
// Also there are some, rarely needed logs that have been #ifdefed out. The source for these logs is surrounded
// by a preprocessor check of #if defined(RARELY_NEEDED_LOGS) which if not defined then the logs will not be
// generated. The define for RARELY_NEEDED_LOGS is in this file, ECR.H, near the top of the file.
#if !defined(NHPOS_ASSERT)
#define NHPOS_ASSERT(x) if (!(x)) { PifLogAbort( (UCHAR *) #x , (UCHAR *)  __FILE__ , (UCHAR *)  "**ERROR: ASSERT FAIL" , __LINE__ );}
#endif
#if !defined(NHPOS_ASSERT_TEXT)
#define NHPOS_ASSERT_TEXT(x, txt) if (!(x)) { PifLogAbort( (UCHAR *)  #x , (UCHAR *)  __FILE__ , (UCHAR *) txt , __LINE__ );}
#endif
#if !defined(NHPOS_NONASSERT_TEXT)
#define NHPOS_NONASSERT_TEXT(txt) PifLogNoAbort( (UCHAR *)"" , (UCHAR *)__FILE__ , (UCHAR *) txt , __LINE__ );
#endif
#if !defined(NHPOS_NONASSERT_NOTE)
#define NHPOS_NONASSERT_NOTE(id,txt) PifLogNoAbort( (UCHAR *)id, (UCHAR *)__FILE__ , (UCHAR *)txt , __LINE__ );
#endif
// check that the size of an array x is at least as large as the specified number of elements, s
#if !defined(NHPOS_ASSERT_ARRAYSIZE)
#define NHPOS_ASSERT_ARRAYSIZE(x,s) NHPOS_ASSERT(sizeof(x) >= (s)*sizeof((x)[0]))
#endif
// check that the size of an array x is the same as an array y
#if !defined(NHPOS_ASSERT_ARRAYSIZEEQU)
#define NHPOS_ASSERT_ARRAYSIZEEQU(x,y) NHPOS_ASSERT(sizeof(x)/sizeof((x)[0]) == sizeof(y)/sizeof((y)[0]))
#endif
// check that the size of an array x is the same as or greater than an array y
#if !defined(NHPOS_ASSERT_ARRAYSIZEEQUGT)
#define NHPOS_ASSERT_ARRAYSIZEEQUGT(x,y) NHPOS_ASSERT(sizeof(x)/sizeof((x)[0]) => sizeof(y)/sizeof((y)[0]))
#endif

#if !defined(NHPOS_DEBUG_DUMP)
#define NHPOS_DEBUG_DUMP(structure, type, size) { PifDebugDump( #type, __FILE__ , __LINE__,(structure) ,size);}
#endif

#endif                                      /* _INC_ECR */
