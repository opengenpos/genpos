/*
* ---------------------------------------------------------------------------
* Title         :   Flexible Memory Assignment Header File (Prog. 2)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P097.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
*
* ===========================================================================
* ===========================================================================
*/
/*
* ===========================================================================
*       Define Declarations
* ===========================================================================
*/

/*
* ===========================================================================
*       Value definition
* ===========================================================================
*/


/*
* ===========================================================================
*       Dialog ID definition
* ===========================================================================
*/

/*    Dialog ID default value */

#define IDD_P98_ITEM         20401

#define IDD_P98_CB_KEYNO			    (IDD_P98_ITEM +  1)
#define IDD_P98_IPADD11        			(IDD_P98_ITEM +  2)
#define IDD_P98_IPADDSPN11        		(IDD_P98_ITEM +  3)
#define IDD_P98_IPADD12        			(IDD_P98_ITEM +  4)
#define IDD_P98_IPADDSPN12        		(IDD_P98_ITEM +  5)
#define IDD_P98_IPADD13         		(IDD_P98_ITEM +  6)
#define IDD_P98_IPADDSPN13         		(IDD_P98_ITEM +  7)
#define IDD_P98_IPADD14         		(IDD_P98_ITEM +  8)
#define IDD_P98_IPADDSPN14         		(IDD_P98_ITEM +  9)
#define IDD_P98_EB_MERCHANTID      		(IDD_P98_ITEM + 10)
#define IDD_P98_KEYOPTION               (IDD_P98_ITEM + 11)
#define IDD_P98_KEYOPTIONSPN            (IDD_P98_ITEM + 12)
#define IDD_P98_TEND_AC                 (IDD_P98_ITEM + 13)
#define IDD_P98_TEND_AC_SPN             (IDD_P98_ITEM + 14)
#define IDC_P98_PORTNO                  (IDD_P98_ITEM + 15)
#define IDC_P98_COMBO_TRANTYPE          (IDD_P98_ITEM + 16)
#define IDC_P98_COMBO_TRANCODE          (IDD_P98_ITEM + 17)
#define IDC_P98_EB_MNEMONIC             (IDD_P98_ITEM + 18)

#define IDC_P98_RADIO_TENDMDC01         (IDD_P98_ITEM + 20)
#define IDC_P98_RADIO_TENDMDC02         (IDD_P98_ITEM + 21)
#define IDC_P98_RADIO_TENDMDC03         (IDD_P98_ITEM + 22)
#define IDC_P98_CHECK_TENDMDC01         (IDD_P98_ITEM + 23) 
#define IDC_P98_CHECK_TENDMDC02         (IDD_P98_ITEM + 24)
#define IDC_P98_CHECK_TENDMDC03         (IDD_P98_ITEM + 25) 
#define IDC_P98_CHECK_TENDMDC04         (IDD_P98_ITEM + 26) 
#define IDC_P98_CHECK_TENDMDC05         (IDD_P98_ITEM + 27)
#define IDC_P98_CHECK_TENDMDC06         (IDD_P98_ITEM + 28) 
#define IDC_P98_CHECK_TENDMDC07         (IDD_P98_ITEM + 29)
#define IDC_P98_CHECK_TENDMDC08         (IDD_P98_ITEM + 30)
#define IDC_P98_CHECK_TENDMDC09         (IDD_P98_ITEM + 31)
#define IDC_P98_CHECK_TENDMDC010        (IDD_P98_ITEM + 32)
#define IDD_P98_BTN_MERCHANTID_RESET	(IDD_P98_ITEM + 33)

#define IDD_P98_STATIC_FIRST			(IDD_P98_ITEM + 40)

#define IDD_P98_STATIC_PORTNO           (IDD_P98_STATIC_FIRST)
#define IDD_P98_STATIC_KEYNO            (IDD_P98_STATIC_PORTNO+1)
#define IDD_P98_STATIC_IPADDRESSFRAME   (IDD_P98_STATIC_KEYNO+1)
#define IDD_P98_KEYOPTIONSTR1           (IDD_P98_STATIC_IPADDRESSFRAME+1)
#define IDD_P98_IPSTR1                  (IDD_P98_KEYOPTIONSTR1+1)
#define IDD_P98_KEYOPTION_FRAME         (IDD_P98_IPSTR1+1)
#define IDD_P98_TEND_AC_RANGE_STATIC    (IDD_P98_KEYOPTION_FRAME+1)
#define IDD_P98_TEND_AC_STATIC          (IDD_P98_TEND_AC_RANGE_STATIC+1)
#define IDD_P98_TEND_AC_FRAME           (IDD_P98_TEND_AC_STATIC+1)
#define IDD_P98_KEYOPTION_STATIC        (IDD_P98_TEND_AC_FRAME+1)
#define IDD_P98_STATIC_MERCHID			(IDD_P98_KEYOPTION_STATIC+1)
#define IDC_P98_AMT_ENTRY_FRAME			(IDD_P98_STATIC_MERCHID+1)
#define IDC_P98_STATIC_MNEMONIC			(IDC_P98_AMT_ENTRY_FRAME+1)
#define IDD_P98_STATIC_LAST				IDC_P98_AMT_ENTRY_FRAME

#define P98_IP_DATA_MIN                 0
#define P98_IP_DATA_MAX               255
#define P98_IP_PORT_MIN					0
#define P98_IP_PORT_MAX				65535
#define P98_TO_DATA_MIN                 0
#define P98_TO_DATA_MAX                20     // Max. Value of Data, Total Key #.  See also P62_DATA_MAX */
#define P98_AC_DATA_MIN                 0
#define P98_AC_DATA_MAX                 9
#define P98_SPIN_STEP                   1
#define P98_SPIN_TURBO                  0
#define P98_SPIN_OFFSET                 1

// Transation types and codes.  These are used in PEP.rc for strings
// Transaction types for P98 dialog control IDC_P98_COMBO_TRANTYPE
#define IDS_P098_TRANTYPE_FIRST					(IDD_P98_ITEM+40)
#define IDS_P098_TRANTYPE_CREDIT				(IDS_P098_TRANTYPE_FIRST)
#define IDS_P098_TRANTYPE_DEBIT					(IDS_P098_TRANTYPE_CREDIT+1)
#define IDS_P098_TRANTYPE_EBT					(IDS_P098_TRANTYPE_DEBIT+1)
#define IDS_P098_TRANTYPE_CASH					(IDS_P098_TRANTYPE_EBT+1)
#define IDS_P098_TRANTYPE_CHECK					(IDS_P098_TRANTYPE_CASH+1)
#define	IDS_P098_TRANTYPE_PREPAID				(IDS_P098_TRANTYPE_CHECK+1)
#define	IDS_P098_TRANTYPE_EPD					(IDS_P098_TRANTYPE_PREPAID+1)
#define	IDS_P098_TRANTYPE_EMEAL					(IDS_P098_TRANTYPE_EPD+1)
#define IDS_P098_TRANTYPE_LAST					(IDS_P098_TRANTYPE_EMEAL)

// Transaction codes for P98 dialog control IDC_P98_COMBO_TRANCODE
#define IDS_P098_TRANCODE_FIRST					(IDS_P098_TRANTYPE_LAST+1)
#define IDS_P098_TRANCODE_SALE					(IDS_P098_TRANCODE_FIRST)
#define IDS_P098_TRANCODE_RETURN				(IDS_P098_TRANCODE_SALE+1)
#define IDS_P098_TRANCODE_VOID_SALE				(IDS_P098_TRANCODE_RETURN+1)
#define IDS_P098_TRANCODE_VOID_RETURN			(IDS_P098_TRANCODE_VOID_SALE+1)
#define IDS_P098_TRANCODE_ADJUST				(IDS_P098_TRANCODE_VOID_RETURN+1)
#define IDS_P098_TRANCODE_VOUCHER				(IDS_P098_TRANCODE_ADJUST+1)
#define IDS_P098_TRANCODE_CASHSALE				(IDS_P098_TRANCODE_VOUCHER+1)
#define IDS_P098_TRANCODE_CASHRETURN			(IDS_P098_TRANCODE_CASHSALE+1)
#define IDS_P098_TRANCODE_FOODSTAMPSALE			(IDS_P098_TRANCODE_CASHRETURN+1)
#define IDS_P098_TRANCODE_FOODSTAMPRETURN		(IDS_P098_TRANCODE_FOODSTAMPSALE+1)
#define IDS_P098_TRANCODE_DRIVERLICENSE_MANUAL	(IDS_P098_TRANCODE_FOODSTAMPRETURN+1)
#define IDS_P098_TRANCODE_DRIVERLICENSE_STRIPE	(IDS_P098_TRANCODE_DRIVERLICENSE_MANUAL+1)
#define IDS_P098_TRANCODE_MICR_READER			(IDS_P098_TRANCODE_DRIVERLICENSE_STRIPE+1)
#define IDS_P098_TRANCODE_MICR_MANUAL			(IDS_P098_TRANCODE_MICR_READER+1)
#define IDS_P098_TRANCODE_PREAUTH				(IDS_P098_TRANCODE_MICR_MANUAL+1)
#define IDS_P098_TRANCODE_PREAUTH_CAPTURE		(IDS_P098_TRANCODE_PREAUTH+1)
#define IDS_P098_TRANCODE_ISSUE					(IDS_P098_TRANCODE_PREAUTH_CAPTURE+1)
#define IDS_P098_TRANCODE_RELOAD				(IDS_P098_TRANCODE_ISSUE+1)
#define IDS_P098_TRANCODE_VOID_RELOAD			(IDS_P098_TRANCODE_RELOAD+1)
#define IDS_P098_TRANCODE_BALANCE				(IDS_P098_TRANCODE_VOID_RELOAD+1)
#define IDS_P098_TRANCODE_NONSFSALE				(IDS_P098_TRANCODE_BALANCE+1)
#define IDS_P098_TRANCODE_VOID_ISSUE			(IDS_P098_TRANCODE_NONSFSALE+1)
#define IDS_P098_TRANCODE_EPD_PAYMENT			(IDS_P098_TRANCODE_VOID_ISSUE+1)
#define IDS_P098_TRANCODE_EPD_VOIDPAYMENT		(IDS_P098_TRANCODE_EPD_PAYMENT+1)
#define IDS_P098_TRANCODE_EPD_LOOKUPNAMESS4		(IDS_P098_TRANCODE_EPD_VOIDPAYMENT+1)
#define IDS_P098_TRANCODE_EPD_LOOKUPEPDACCTNO	(IDS_P098_TRANCODE_EPD_LOOKUPNAMESS4+1)
#define IDS_P098_TRANCODE_LAST					(IDS_P098_TRANCODE_EPD_LOOKUPEPDACCTNO)

// following are string constants which reuse the MDC bit strings
// which are currently used for the Tender Key settings.
#define IDS_P098_TENDMDC_001            25616  // "Allow amount entry."
#define IDS_P098_TENDMDC_002            25618  // "Require amount entry."
#define IDS_P098_TENDMDC_003            25619  // "Prohibit amount entry."
#define IDS_P098_TENDMDC_011            25620  // "Allow over amount entry."
#define IDS_P098_TENDMDC_012            25621  // "Prohibit over amount entry."
#define IDS_P098_TENDMDC_021            25622  // "Allow partial amount entry."
#define IDS_P098_TENDMDC_022            25623  // "Prohibit partial amount entry."
#define IDS_P098_TENDMDC_031            25632  // "Prohibit validation print."
#define IDS_P098_TENDMDC_032            25633  // "Require validation print."
#define IDS_P098_TENDMDC_041            25634  // "Open the drawer."
#define IDS_P098_TENDMDC_042            25635  // "Not open the drawer."
#define IDS_P098_TENDMDC_051            25636  // "Optional account # entry."
#define IDS_P098_TENDMDC_052            25637  // "Compulsory account # entry."
#define IDS_P098_TENDMDC_061            25638  // "Optional Expiration Date entry."
#define IDS_P098_TENDMDC_062            25639  // "Compulsory Expiration Date entry."
#define IDS_P098_TENDMDC_071            27456  // "Not Perform ROOM Charge Operation."
#define IDS_P098_TENDMDC_072            27457  // "Perform ROOM Charge Operation."
#define IDS_P098_TENDMDC_081            27458  // "Not Perform ACCOUNT Charge Operation."
#define IDS_P098_TENDMDC_082            27459  // "Perform ACCOUNT Charge Operation."
#define IDS_P098_TENDMDC_091            27460  // "Not Perform Credit Authorization Operation."
#define IDS_P098_TENDMDC_092            27461  // "Perform Credit Authorization Operation."
#define IDS_P098_TENDMDC_101            27462  // "Not Perform EPT."
#define IDS_P098_TENDMDC_102            27463  // "Perform EPT."


/*
* ===========================================================================
*       Resource ID definition
* ===========================================================================
*/


/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct _P98PARA {
    TENDERKEYINFO aCurt[20];     /* current parameter */
    TENDERKEYINFO aPrev[20];     /* previous parameter */
} P98PARA,  *LPP98PARA;

typedef struct tagDlgItemStrings {
	int nDlgId;
	int nDlgIds;
} DlgItemStrings;

typedef struct _tagDlgItemTranCodeTable {
	DlgItemStrings	nDlgTranCode[8];
} DlgItemTranCodeTable;

DlgItemTranCodeTable tranCodeTable [] = {
	{
		{	/* Credit transaction codes allowed: TENDERKEY_TRANTYPE_CREDIT */
			{ TENDERKEY_TRANCODE_SALE, IDS_P098_TRANCODE_SALE},
			{ TENDERKEY_TRANCODE_RETURN, IDS_P098_TRANCODE_RETURN},
			{ TENDERKEY_TRANCODE_VOID_SALE, IDS_P098_TRANCODE_VOID_SALE},
			{ TENDERKEY_TRANCODE_VOID_RETURN, IDS_P098_TRANCODE_VOID_RETURN},
			{ TENDERKEY_TRANCODE_PREAUTH, IDS_P098_TRANCODE_PREAUTH},
			{ TENDERKEY_TRANCODE_PREAUTHCAPTURE, IDS_P098_TRANCODE_PREAUTH_CAPTURE},
			{ 0, 0}
		}
	},
	{
		{	/* Debit transaction codes allowed: TENDERKEY_TRANTYPE_DEBIT */ 
			{ TENDERKEY_TRANCODE_SALE, IDS_P098_TRANCODE_SALE},
			{ TENDERKEY_TRANCODE_RETURN, IDS_P098_TRANCODE_RETURN},
			{ TENDERKEY_TRANCODE_VOID_SALE, IDS_P098_TRANCODE_VOID_SALE},
			{ TENDERKEY_TRANCODE_VOID_RETURN, IDS_P098_TRANCODE_VOID_RETURN},
			{ 0, 0}
		}
	},
	{
		{	/* EBT transaction codes allowed: TENDERKEY_TRANTYPE_EBT */
			{ TENDERKEY_TRANCODE_CASHSALE, IDS_P098_TRANCODE_CASHSALE},
			// Currently no servers available for this option
			// Not allowing this option till supported
			//{ TENDERKEY_TRANCODE_CASHRETURN, IDS_P098_TRANCODE_CASHRETURN},
			{ TENDERKEY_TRANCODE_FOODSTAMPSALE, IDS_P098_TRANCODE_FOODSTAMPSALE},
			{ TENDERKEY_TRANCODE_FOODSTAMPRETURN, IDS_P098_TRANCODE_FOODSTAMPRETURN},
			{ 0, 0}
		}
	},
	{
		{	// Charge transaction codes allowed: TENDERKEY_TRANTYPE_CHARGE 
			{ TENDERKEY_TRANCODE_SALE, IDS_P098_TRANCODE_SALE},
			{ TENDERKEY_TRANCODE_RETURN, IDS_P098_TRANCODE_RETURN},
			{ TENDERKEY_TRANCODE_VOID_SALE, IDS_P098_TRANCODE_VOID_SALE},
			{ TENDERKEY_TRANCODE_VOID_RETURN, IDS_P098_TRANCODE_VOID_RETURN},
			{ 0, 0}
		}
	},
	{
		{	/* Check transaction codes allowed: TENDERKEY_TRANTYPE_CHECK */
		//	{ TENDERKEY_TRANCODE_DRIVERLICENSE_MANUAL, IDS_P098_TRANCODE_DRIVERLICENSE_MANUAL},
		//	{ TENDERKEY_TRANCODE_DRIVERLICENSE_STRIPE, IDS_P098_TRANCODE_DRIVERLICENSE_STRIPE},
		//	{ TENDERKEY_TRANCODE_MICR_READER, IDS_P098_TRANCODE_MICR_READER},
			{ TENDERKEY_TRANCODE_MICR_MANUAL, IDS_P098_TRANCODE_MICR_MANUAL},
			{ 0, 0}
		}
	},
		// Gift Card Implementation
	{
		{	// Pre Paid transaction codes allowed: TENDERKEY_TRANTYPE_PREPAID
			{ TENDERKEY_TRANCODE_NONSFSALE, IDS_P098_TRANCODE_SALE},
			{ TENDERKEY_TRANCODE_RETURN, IDS_P098_TRANCODE_RETURN},
			{ TENDERKEY_TRANCODE_VOID_SALE, IDS_P098_TRANCODE_VOID_SALE},
			{ TENDERKEY_TRANCODE_VOID_RETURN, IDS_P098_TRANCODE_VOID_RETURN},
			{ 0, 0}
		}
	},
	{
		{	// EPD - Employee Payroll Deduction transaction codes allowed: TENDERKEY_TRANTYPE_EPD
			{ TENDERKEY_TRANCODE_SALE, IDS_P098_TRANCODE_SALE},
			{ TENDERKEY_TRANCODE_RETURN, IDS_P098_TRANCODE_RETURN},
			{ TENDERKEY_TRANCODE_VOID_SALE, IDS_P098_TRANCODE_VOID_SALE},
			{ TENDERKEY_TRANCODE_VOID_RETURN, IDS_P098_TRANCODE_VOID_RETURN},
			{ 0, 0}
		}
	},
	{
		{	// EMEAL - E-Meal transaction codes allowed: TENDERKEY_TRANTYPE_EMEAL
			{ TENDERKEY_TRANCODE_SALE, IDS_P098_TRANCODE_SALE},
			{ TENDERKEY_TRANCODE_RETURN, IDS_P098_TRANCODE_RETURN},
			{ TENDERKEY_TRANCODE_VOID_SALE, IDS_P098_TRANCODE_VOID_SALE},
			{ TENDERKEY_TRANCODE_VOID_RETURN, IDS_P098_TRANCODE_VOID_RETURN},
			{ 0, 0}
		}
	}
};

BOOL    P98InitDlg(HWND, LPDWORD, LPDWORD, LPP98PARA, LPHGLOBAL);
BOOL    P98FinDlg(HWND, LPP98PARA, HGLOBAL, WPARAM);
BOOL    P98InitDlgData(HWND hDlg, TENDERKEYINFO *pTermInfo, int nIndex);
BOOL    P98GetDlgData(HWND hDlg, TENDERKEYINFO *pTermInfo, int nIndex);
BOOL    P98ValidateIP(HWND, WPARAM);  //Checks range of IP - PDINU
VOID	P98DispErrorIP(HWND, WPARAM); //Displays error if IP out of bounds - PDINU 
VOID	P98DispErrorKey(HWND, WPARAM); // display an error if keyoption out of bounds - LDelhomme
VOID	P98DispErrorPort(HWND, WPARAM);// display error if ip port out of bounds - LDelhomme


/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/

#define P098_STORENO_MAX	9999         /* Max. Store Number ESMITH  */
#define P098_STORENO_MIN    0		     /* Min. Store Number ESMITH  */
#define P098_REGNO_MAX		999			 /* Max. Register Number ESMITH  */
#define P098_REGNO_MIN		0		     /* Min. Register Number ESMITH  */

#define P098_STORENO_ERR			WIDE("Specify a value between 0 and 9999")
#define P098_REGNO_ERR				WIDE("Specify a value between 0 and 999")
#define P097_IP_ERR					WIDE("Specify a value between 0 and 255")
#define P098_ERR_CHECK				2
#define P098_MAX_STORELEN			4
#define P098_MAX_REGNOLEN			3


/*added 7-13-04 to implement the use of getting and setting MDC settings
for Tender keys. Uses same values from P01 - cdh */
#define P098_CHK_ADDR				2       /* Check Odd / Even Address */
#define P098_BIT_NO                 4
#define P098_GET_LOBIT(x)           (BYTE)(0x0f & x)
#define P098_GET_HIBIT(x)           (BYTE)((0xf0 & x) >> 4)
#define P098_SET_LOBIT(dest, src)   (BYTE)((0xf0 & dest) | (0x0f & src))
#define P098_SET_HIBIT(dest, src)   (BYTE)((0x0f & dest) | ((0x0f & src) << 4))
#define P098_CHK_BIT				0x01
#define P098_1BITSHIFT				1
#define P098_2BITSHIFT				2
#define P098_3BITSHIFT				3
#define P098_BIT_ON					1
#define P098_BIT_OFF				0
     
/* ===== End of P098.H ===== */