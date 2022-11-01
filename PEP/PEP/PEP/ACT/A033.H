/*
* ---------------------------------------------------------------------------
* Title         :   Set Control String Table (AC 33)
* Category      :   Maintenance, AT&T 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A033.H
* Copyright (C) :   1995, AT&T Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Apr-12-95 : 03.00.00 : H.Ishida   : Initial (Migrate from HP US Model)
* Jul-25-95 : 03.00.00 : hkato      : R3.0 FVT Comment.
* Mar-18-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-11-98 : 03.03.00 : A.Mitsui   : V3.3
* Dec-10-99 :          : K.Yanagida : NCR2172
* ===========================================================================
* ===========================================================================
* PVCS Entry
* ---------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
* ===========================================================================
*/
#define A33_IDD_OFS           400           /* convert FSC to WPARAM ID     */
#define A33_DSP_INF_OFS       150           /* IDS (DSP-INF)                */
#define A33_FSC_NO			  141			/* number of FSC items, UPDATE IF YOU ADD A NEW FSC TO awExtRngTbl1
                                            /* --> added FSC84,85,86        */
#define A33_FXK_NO             18           /* number of Fix Key Code items */

#define A33_SPIN_STEP           1           /* step of spin button          */
#define A33_SPIN_TURBO          0           /* not use spin button turbo    */

#define A33_NO_LEN              3           /* display string length        */
#define A33_DATAMIN             1           /* Minimum Value of No.         */
#define A33_EXT_MIN             1           /* Minimum Value of Extend data */
#define A33_NO_MAX            400           /* Maximum Value of No.         */
#define A33_MODE_EXT            2           /* Mode Extend Value            */
#define A33_ALPHA_BASIC      0x30           /* alpha/numeric key            */

#define A33_ALPHA_STR_LEN      19

/*
* ===========================================================================
*       Address definition
*           (w/)    with Extended FSC
*           (D)     Drop FSC
* ===========================================================================
*/
#define A33_NOFUNC          0               //FSC_NO_FUNC			- No Function
#define A33_KEYEDPLU        1               //FSC_KEYED_PLU			- Keyed PLU Key
#define A33_KEYEDDPT        2               //FSC_KEYED_DEPT		- Keyed Department Key
#define A33_PRTMOD          3               //FSC_PRINT_MODIF		- Print Modifier
#define A33_ADJ             4               //FSC_ADJECTIVE			- Adjective Key
#define A33_TENDER          5               //FSC_TENDER			- Tender Key
#define A33_FC              6               //FSC_FOREIGN			- Foreign Currency Key
#define A33_TTL             7               //FSC_TOTAL				- Total Key
#define A33_ADDCHK          8               //FSC_ADD_CHECK			- Add Check Key
#define A33_IDISC           9               //FSC_ITEM_DISC			- Item Discount Key
#define A33_RDISC           10              //FSC_REGULAR_DISC		- Regular Discount Key
#define A33_TAXMOD          11              //FSC_TAX_MODIF			- Tax Modifier Key
#define A33_AKEY            12              //FSC_SIGN_IN			- Sign-In / A / Super # Key
#define A33_BKEY            13              //FSC_B					- B Key
#define A33_SERVNO          14              //FSC_WAITER			- Waiter No.
#define A33_NEWCHK          15              //FSC_NEW_CHK			- New Check
#define A33_PB              16              //FSC_PREVIOUS			- Previous Balance
#define A33_GCNO            17              //FSC_GUEST_CHK_NO		- Guest Check
#define A33_GCTRANS         18              //FSC_GUEST_CHK_TRN		- Guest Check Transfer
#define A33_NOPERSON        19              //FSC_NUM_PERSON		- Number of Person
#define A33_TBLNO           20              //FSC_TABLE_NO			- Table No.
#define A33_LINENO          21              //FSC_LINE_NO			- Line No.
#define A33_FEED            22              //FSC_FEED_REL			- Feed / Release
#define A33_RCTNORCT        23              //FSC_RECEIPT			- Receipt / No Receipt
#define A33_TIPSPO          24              //FSC_TIPS_PAID			- Tips Paid Out
#define A33_DECTIP          25              //FSC_DECLAR_TIPS		- Declared Tips
#define A33_NOSALE          26              //FSC_NO_SALE			- No Sale
#define A33_PO              27              //FSC_PAID_OUT			- Paid Out
#define A33_ROA             28              //FSC_ROA				- Received on Account
#define A33_CHGCOMP         29              //FSC_COMPUT			- Change Computation
#define A33_CANCEL          30              //FSC_CANCEL			- Cancel / Abort
#define A33_VOID            31              //FSC_VOID				- Void
#define A33_EC              32              //FSC_EC				- E/C
#define A33_VTRAN           33              //FSC_PRE_VOID			- Preselect Void
#define A33_QTY             34              //FSC_QTY				- Qty
#define A33_SHIFT           35              //FSC_MENU_SHIFT		- Menu Shift
#define A33_NOTYPE          36              //FSC_NUM_TYPE			- # / Type
#define A33_PRTDEM          37              //FSC_PRT_DEMAND		- Print on Demand
//#define A33_ORDERNO		38				// order no. display key
#define A33_CHARTIP         39              //FSC_CHARGE_TIPS		- Charge Tips
#define A33_PLUNO           40              //FSC_PLU				- PLU No.
#define A33_00KEY           41              //FSC_00				- 00 Key
#define A33_AC              42              //FSC_AC				- Action Code Key
#define A33_RECFEED         43              //FSC_RECEIPT_FEED		- Receipt Feed
#define A33_JOUFEED         44              //FSC_JOURNAL_FEED		- Journal Feed
#define A33_DECPT           45              //FSC_DECIMAL			- Decimal Point
#define A33_SCALE           46              //FSC_TARE				- Scale/tare key
#define A33_OFFTEND         47              //FSC_OFFTEND_MODIF		- Off Line Tender Modifier Key

#define A33_MONEY           49              //FSC_MONEY				- Money key					| NCR2172


#define A33_RVSPRI          52              //FSC_REVERSE_PRINT		- Reverse Print Key
#define A33_PRECASH         53              //FSC_PRESET_AMT		- Preset Amount Key
#define A33_DRCTSHFT        54              //FSC_D_MENU_SHIFT		- Direct Menu Shift Key
#define A33_TIMEIN          55              //FSC_TIME_IN			- Time In Key
#define A33_TIMEOUT         56              //FSC_TIME_OUT			- Time Out Key
#define A33_KEYEDCPN        57              //FSC_KEYED_COUPON		- Keyed Coupon Key
#define A33_CPNNO           58              //FSC_COUPON			- Coupon No Key
#define A33_UP              59              //FSC_SCROLL_UP			- Scroll-Up Key
#define A33_DOWN            60              //FSC_SCROLL_DOWN		- Scroll-Down Key
#define A33_RIGHT           61              //FSC_RIGHT_ARROW		- Right Arrow Key
#define A33_LEFT            62              //FSC_LEFT_ARROW		- Left Arrow Key
#define A33_WAIT            63              //FSC_WAIT				- Wait Key
#define A33_PAID_REC        64              //FSC_PAID_RECALL		- Paid Order Recall Key


#define A33_ALPHA           67              //FSC_ALPHA				- Alpha Entry Key
#define A33_LEFTDISPLAY     68              //FSC_LEFT_DISPLAY		- Cursor on Left Display
#define A33_BARTENDER       69				//FSC_BAR_SIGNIN		- Bartender Sign-in Key		| R3.1
#define A33_SPLIT           70				//FSC_SPLIT				- Split Key					| R3.1
#define A33_SEAT            71				//FSC_SEAT				- Seat No Key				| R3.1
#define A33_ITEMTRANS       72				//FSC_TRANSFER			- Transfer Key				| R3.1
#define A33_REPEAT          73				//FSC_REPEAT			- Repeat Key				| R3.1
#define A33_CURSORVOID      74				//FSC_CURSOR_VOID		- Cursor Void Key			| R3.1
#define A33_PRICECHECK      75				//FSC_PRICE_CHECK		- Price Check Key			| R3.1
#define A33_ASK             76              //FSC_ASK				- Ask Key					| NCR2172
#define A33_OPRINT_A        78              //FSC_CASINT			- Cashier Interrupt			| R3.3
#define A33_OPRINT_B        79              //FSC_CASINT_B			- Cashier Interrupt B key	| R3.3
#define A33_LEVEL           80              //FSC_ADJ_SHIFT			- Adjective Shift			| NCR2172
#define A33_DEPTNUMBER      81              //FSC_DEPT				- Dept. No. Key				| NCR2172
#define A33_UPCVER          82              //FSC_E_VERSION			- E Version Modifier Key	| NCR2172
#define A33_PRICEKEY        83              //FSC_PRICE_ENTER		- Price Enter Key			| NCR2172
#define A33_FSMOD           84              //FSC_FOODSTAMP			- Foodstamp Modifier Key	| ### ADD Saratoga (052400)
#define A33_WICTRANS        85              //FSC_WIC_TRN			- WIC Transaction Key		| ### ADD Saratoga (052400)
#define A33_WICMOD          86              //FSC_WIC_MODIF			- WIC Modifier Key			| ### ADD Saratoga (052400)
#define A33_HALO_OVR	    87				//FSC_HALO				- Halo override key			| (SR 143 cwunn)
#define A33_ATFOR		    88				//FSC_FOR				- @/For Key					| (SR 143 cwunn)
#define A33_VALIDATION	    89				//FSC_VALIDATION		- Manual Validation Key		| SR 47
#define A33_ALPHANUM        90				//FSC_ALPHANUM			- Alphanumeric ASCII value as extended FSC
#define A33_CLEARFSC        91				//FSC_CLEARFSC			- Clear key assignable FSC 
#define A33_EDIT_CONDIMENT	92				//FSC_EDIT_CONDIMENT	- Condiment Editing SR 192 JHHJ
#define A33_RECEIPT_ID		93				//FSC_TENT				- Receipt ID implementation
#define A33_ORDER_DEC		94				//FSC_ORDER_DEC			- Order Declaration Feature JHHJ

#define A33_DOC_LAUNCH		98				//FSC_DOC_LAUNCH		- launch a document through shortcut in Database folder

#define	A33_WINDOW_DISPLAY	101				//FSC_WINDOW_DISPLAY	- Display a window whose ID label (character digits) follows
#define	A33_WINDOW_DISMISS	102				//FSC_WINDOW_DISMISS	- Dismiss a displayed window whose ID label (character digits) follows

#define A33_LOCK            104              /* Lock Switch              */
#define A33_REGS            105              /* Regs Switch              */
#define A33_SUPER           106              /* Super Switch             */
#define A33_PROG            107              /* Prog Switch              */
#define A33_PAUSE           108              /* Pause Key                */
#define A33_CALLSTR         109              /* Call another control string Key */
#define A33_ZERO            110              /* numeric 0 key            */
#define A33_ONE             111              /* numeric 1 key            */
#define A33_TWO             112              /* numeric 2 key            */
#define A33_THREE           113              /* numeric 3 key            */
#define A33_FOUR            114              /* numeric 4 key            */
#define A33_FIVE            115              /* numeric 5 key            */
#define A33_SIX             116             /* numeric 6 key            */
#define A33_SEVEN           117              /* numeric 7 key            */
#define A33_EIGHT           118              /* numeric 8 key            */
#define A33_NINE            119              /* numeric 9 key            */
#define A33_CLEAR           120              /* Clear Key                */
#define A33_P1              121              /* P1 Key                   */
#define A33_P2              122              /* P2 Key                   */
#define A33_P3              123              /* P3 Key                   */
#define A33_P4              124              /* P4 Key                   */
#define A33_P5              125              /* P5 Key                   */


#define A33_MOVING          130              /* moving                   */
#define A33_INDIC           131              /* indicating               */
#define A33_NOUSE           132              /* no use                   */


/*
* ===========================================================================
*       Dialog ID definition
* ===========================================================================
*/
#define A33_SPIN_OFFSET        1

#define IDD_A33_ENTRY       13300                    /* toolbox button ID        */
#define IDD_A33_NO          (IDD_A33_ENTRY + 1)      /* C.String No Button ID    */
#define IDD_A33_NO_SPIN     (IDD_A33_NO+A33_SPIN_OFFSET)
#define IDD_A33_LIST        (IDD_A33_ENTRY + 4)      /* Control Strings ListBox  */
#define IDD_A33_ADDREC      (IDD_A33_ENTRY + 5)      /* Addition/Append Recodes  */
#define IDD_A33_ADDCHG      (IDD_A33_ENTRY + 6)      /* Add/Change Button        */
#define IDD_A33_DEL         (IDD_A33_ENTRY + 7)      /* Delete Button            */
#define IDD_A33_STRING      (IDD_A33_ENTRY + 8)      /* string entry button ID   */
#define IDD_A33_EXTMNEMO    (IDD_A33_ENTRY + 9)      /* ext fsc mnemonic ID      */
#define IDD_A33_EXTFSC      (IDD_A33_ENTRY + 10)     /* ext fsc ID               */
#define IDD_A33_EXTRANGE    (IDD_A33_ENTRY + 11)     /* ext fsc range ID         */
#define IDD_A33_SPIN        (IDD_A33_ENTRY + 12)     /* Spin Button ID           */
#define	IDD_A33_UPSWITCH	(IDD_A33_ENTRY + 13)	 /* Up move button ID		 */
#define IDD_A33_DOWNSWITCH	(IDD_A33_ENTRY + 14)	 /* Down Move Button ID		 */
#define	IDD_A33_CLEARALL		(IDD_A33_ENTRY + 15)	 /* Clear Control String Button ID */

#define IDD_A33_STRDLG      13330       /* alpha mnemonic entry dlg */
#define IDD_A33_STREDIT     13331       /* alpha mnemonic edittext  */

#define IDD_A33_NOFUNC      A33_IDD_OFS
#define IDD_A33_KEYEDPLU    (IDD_A33_NOFUNC + A33_KEYEDPLU)
#define IDD_A33_KEYEDDPT    (IDD_A33_NOFUNC + A33_KEYEDDPT)
#define IDD_A33_PRTMOD      (IDD_A33_NOFUNC + A33_PRTMOD)
#define IDD_A33_ADJ         (IDD_A33_NOFUNC + A33_ADJ)
#define IDD_A33_TENDER      (IDD_A33_NOFUNC + A33_TENDER)
#define IDD_A33_FC          (IDD_A33_NOFUNC + A33_FC)
#define IDD_A33_TTL         (IDD_A33_NOFUNC + A33_TTL)
#define IDD_A33_ADDCHK      (IDD_A33_NOFUNC + A33_ADDCHK)
#define IDD_A33_IDISC       (IDD_A33_NOFUNC + A33_IDISC)
#define IDD_A33_RDISC       (IDD_A33_NOFUNC + A33_RDISC)
#define IDD_A33_TAXMOD      (IDD_A33_NOFUNC + A33_TAXMOD)
#define IDD_A33_AKEY        (IDD_A33_NOFUNC + A33_AKEY)
#define IDD_A33_BKEY        (IDD_A33_NOFUNC + A33_BKEY)
#define IDD_A33_SERVNO      (IDD_A33_NOFUNC + A33_SERVNO)
#define IDD_A33_NEWCHK      (IDD_A33_NOFUNC + A33_NEWCHK)
#define IDD_A33_PB          (IDD_A33_NOFUNC + A33_PB)
#define IDD_A33_GCNO        (IDD_A33_NOFUNC + A33_GCNO)
#define IDD_A33_GCTRANS     (IDD_A33_NOFUNC + A33_GCTRANS)
#define IDD_A33_NOPERSON    (IDD_A33_NOFUNC + A33_NOPERSON)
#define IDD_A33_TBLNO       (IDD_A33_NOFUNC + A33_TBLNO)
#define IDD_A33_LINENO      (IDD_A33_NOFUNC + A33_LINENO)
#define IDD_A33_FEED        (IDD_A33_NOFUNC + A33_FEED)
#define IDD_A33_RCTNORCT    (IDD_A33_NOFUNC + A33_RCTNORCT)
#define IDD_A33_TIPSPO      (IDD_A33_NOFUNC + A33_TIPSPO)
#define IDD_A33_DECTIP      (IDD_A33_NOFUNC + A33_DECTIP)
#define IDD_A33_NOSALE      (IDD_A33_NOFUNC + A33_NOSALE)
#define IDD_A33_PO          (IDD_A33_NOFUNC + A33_PO)
#define IDD_A33_ROA         (IDD_A33_NOFUNC + A33_ROA)
#define IDD_A33_CHGCOMP     (IDD_A33_NOFUNC + A33_CHGCOMP)
#define IDD_A33_CANCEL      (IDD_A33_NOFUNC + A33_CANCEL)
#define IDD_A33_VOID        (IDD_A33_NOFUNC + A33_VOID)
#define IDD_A33_EC          (IDD_A33_NOFUNC + A33_EC)
#define IDD_A33_VTRAN       (IDD_A33_NOFUNC + A33_VTRAN)
#define IDD_A33_QTY         (IDD_A33_NOFUNC + A33_QTY)
#define IDD_A33_SHIFT       (IDD_A33_NOFUNC + A33_SHIFT)
#define IDD_A33_NOTYPE      (IDD_A33_NOFUNC + A33_NOTYPE)
#define IDD_A33_PRTDEM      (IDD_A33_NOFUNC + A33_PRTDEM)
/* #define IDD_A33_ORDERNO     (IDD_A33_NOFUNC + A33_ORDERNO) */
#define IDD_A33_CHARTIP     (IDD_A33_NOFUNC + A33_CHARTIP)
#define IDD_A33_PLUNO       (IDD_A33_NOFUNC + A33_PLUNO)
#define IDD_A33_00KEY       (IDD_A33_NOFUNC + A33_00KEY)
#define IDD_A33_AC          (IDD_A33_NOFUNC + A33_AC)
#define IDD_A33_RECFEED     (IDD_A33_NOFUNC + A33_RECFEED)
#define IDD_A33_JOUFEED     (IDD_A33_NOFUNC + A33_JOUFEED)
#define IDD_A33_DECPT       (IDD_A33_NOFUNC + A33_DECPT)
#define IDD_A33_SCALE       (IDD_A33_NOFUNC + A33_SCALE)
#define IDD_A33_RVSPRI      (IDD_A33_NOFUNC + A33_RVSPRI)
#define IDD_A33_PRECASH     (IDD_A33_NOFUNC + A33_PRECASH)
#define IDD_A33_DRCTSHFT    (IDD_A33_NOFUNC + A33_DRCTSHFT)
#define IDD_A33_TIMEIN      (IDD_A33_NOFUNC + A33_TIMEIN)
#define IDD_A33_TIMEOUT     (IDD_A33_NOFUNC + A33_TIMEOUT)
#define IDD_A33_OFFTEND     (IDD_A33_NOFUNC + A33_OFFTEND)
#define IDD_A33_KEYEDCPN    (IDD_A33_NOFUNC + A33_KEYEDCPN)
#define IDD_A33_CPNNO       (IDD_A33_NOFUNC + A33_CPNNO)
#define IDD_A33_UP          (IDD_A33_NOFUNC + A33_UP)
#define IDD_A33_DOWN        (IDD_A33_NOFUNC + A33_DOWN)
#define IDD_A33_RIGHT       (IDD_A33_NOFUNC + A33_RIGHT)
#define IDD_A33_LEFT        (IDD_A33_NOFUNC + A33_LEFT)
#define IDD_A33_WAIT        (IDD_A33_NOFUNC + A33_WAIT)
#define IDD_A33_PAID_REC    (IDD_A33_NOFUNC + A33_PAID_REC)
#define IDD_A33_ALPHA       (IDD_A33_NOFUNC + A33_ALPHA)
#define IDD_A33_LEFTDISPLAY (IDD_A33_NOFUNC + A33_LEFTDISPLAY)
#define IDD_A33_BARTENDER   (IDD_A33_NOFUNC + A33_BARTENDER)
#define IDD_A33_SPLIT       (IDD_A33_NOFUNC + A33_SPLIT)
#define IDD_A33_SEAT        (IDD_A33_NOFUNC + A33_SEAT)
#define IDD_A33_ITEMTRANS   (IDD_A33_NOFUNC + A33_ITEMTRANS)
#define IDD_A33_REPEAT      (IDD_A33_NOFUNC + A33_REPEAT)
#define IDD_A33_CURSORVOID  (IDD_A33_NOFUNC + A33_CURSORVOID)
#define IDD_A33_PRICECHECK  (IDD_A33_NOFUNC + A33_PRICECHECK)
#define IDD_A33_OPRINT_A    (IDD_A33_NOFUNC + A33_OPRINT_A)         /* V3.3 */
#define IDD_A33_OPRINT_B    (IDD_A33_NOFUNC + A33_OPRINT_B)         /* V3.3 */
#define IDD_A33_LOCK        (IDD_A33_NOFUNC + A33_LOCK)
#define IDD_A33_REGS        (IDD_A33_NOFUNC + A33_REGS)
#define IDD_A33_SUPER       (IDD_A33_NOFUNC + A33_SUPER)
#define IDD_A33_PROG        (IDD_A33_NOFUNC + A33_PROG)
#define IDD_A33_PAUSE       (IDD_A33_NOFUNC + A33_PAUSE)
#define IDD_A33_CALLSTR     (IDD_A33_NOFUNC + A33_CALLSTR)
#define IDD_A33_ZERO        (IDD_A33_NOFUNC + A33_ZERO)
#define IDD_A33_ONE         (IDD_A33_NOFUNC + A33_ONE)
#define IDD_A33_TWO         (IDD_A33_NOFUNC + A33_TWO)
#define IDD_A33_THREE       (IDD_A33_NOFUNC + A33_THREE)
#define IDD_A33_FOUR        (IDD_A33_NOFUNC + A33_FOUR)
#define IDD_A33_FIVE        (IDD_A33_NOFUNC + A33_FIVE)
#define IDD_A33_SIX         (IDD_A33_NOFUNC + A33_SIX)
#define IDD_A33_SEVEN       (IDD_A33_NOFUNC + A33_SEVEN)
#define IDD_A33_EIGHT       (IDD_A33_NOFUNC + A33_EIGHT)
#define IDD_A33_NINE        (IDD_A33_NOFUNC + A33_NINE)
#define IDD_A33_CLEAR       (IDD_A33_NOFUNC + A33_CLEAR)
#define IDD_A33_P1          (IDD_A33_NOFUNC + A33_P1)
#define IDD_A33_P2          (IDD_A33_NOFUNC + A33_P2)
#define IDD_A33_P3          (IDD_A33_NOFUNC + A33_P3)
#define IDD_A33_P4          (IDD_A33_NOFUNC + A33_P4)
#define IDD_A33_P5          (IDD_A33_NOFUNC + A33_P5)

/*** NCR2172 ***/

#define IDD_A33_MONEY       (IDD_A33_NOFUNC + A33_MONEY)
#define IDD_A33_ASK         (IDD_A33_NOFUNC + A33_ASK)
#define IDD_A33_LEVEL       (IDD_A33_NOFUNC + A33_LEVEL)
#define IDD_A33_DEPTNUMBER  (IDD_A33_NOFUNC + A33_DEPTNUMBER)
#define IDD_A33_UPCVER      (IDD_A33_NOFUNC + A33_UPCVER)
#define IDD_A33_PRICEKEY    (IDD_A33_NOFUNC + A33_PRICEKEY)
#define IDD_A33_FSMOD       (IDD_A33_NOFUNC + A33_FSMOD)       /* ### ADD Saratoga (052500) */
#define IDD_A33_WICTRANS    (IDD_A33_NOFUNC + A33_WICTRANS)    /* ### ADD Saratoga (052500) */
#define IDD_A33_WICMOD      (IDD_A33_NOFUNC + A33_WICMOD)      /* ### ADD Saratoga (052500) */
#define IDD_A33_HALO_OVR	(IDD_A33_NOFUNC + A33_HALO_OVR)
#define IDD_A33_ATFOR		(IDD_A33_NOFUNC + A33_ATFOR)
#define IDD_A33_VALIDATION	(IDD_A33_NOFUNC + A33_VALIDATION)
#define IDD_A33_RECEIPT_ID	(IDD_A33_NOFUNC + A33_RECEIPT_ID)
#define IDD_A33_DOC_LAUNCH	(IDD_A33_NOFUNC + A33_DOC_LAUNCH)							//FSC_DOC_LAUNCH
#define IDD_A33_WINDOW_DISPLAY			(IDD_A33_NOFUNC + A33_WINDOW_DISPLAY)			//FSC_WINDOW_DISPLAY
#define IDD_A33_WINDOW_DISMISS			(IDD_A33_NOFUNC + A33_WINDOW_DISMISS)			//FSC_WINDOW_DISMISS
#define IDD_A33_ORDER_DEC				(IDD_A33_NOFUNC + A33_ORDER_DEC)			    //FSC_ORDER_DEC

#define IDD_A33_STRNO       13350
#define IDD_A33_STRNOLEN    (IDD_A33_STRNO + 1)
#define IDD_A33_STRDESC     (IDD_A33_STRNO + 2)
#define IDD_A33_STRTOOL     (IDD_A33_STRNO + 3)
#define IDD_A33_STRFSCNO    (IDD_A33_STRNO + 4)
#define IDD_A33_STRFSCLEN   (IDD_A33_STRNO + 5)
#define IDD_A33_STRMNEMO    (IDD_A33_STRNO + 6)
#define IDD_A33_STRMNEMOMAX (IDD_A33_STRNO + 7)

/*
* ===========================================================================
*       String ID definition
* ===========================================================================
*/
#define IDS_A33_EMPTY           (IDS_A33 +  6)
#define IDS_A33_DELETE          (IDS_A33 +  8)
#define IDS_A33_UPDATE          (IDS_A33 + 10)
#define IDS_A33_MEMOVR          (IDS_A33 + 12)
#define IDS_A33_BRNDATA         (IDS_A33 + 14)
#define IDS_A33_DSP_ASCII       (IDS_A33 + 15)
#define IDS_A33_CONVSERKEY      (IDS_A33 + 16)      /* V3.3 */
#define	IDS_A33_CLRMSG			(IDS_A33 + 17)

#define IDS_A33_INF_NOFUNC      (IDS_A33 + 20)
#define IDS_A33_INF_KEYEDPLU    (IDS_A33_INF_NOFUNC + A33_KEYEDPLU)
#define IDS_A33_INF_KEYEDDPT    (IDS_A33_INF_NOFUNC + A33_KEYEDDPT)
#define IDS_A33_INF_PRTMOD      (IDS_A33_INF_NOFUNC + A33_PRTMOD)
#define IDS_A33_INF_ADJ         (IDS_A33_INF_NOFUNC + A33_ADJ)
#define IDS_A33_INF_TENDER      (IDS_A33_INF_NOFUNC + A33_TENDER)
#define IDS_A33_INF_FC          (IDS_A33_INF_NOFUNC + A33_FC)
#define IDS_A33_INF_TTL         (IDS_A33_INF_NOFUNC + A33_TTL)
#define IDS_A33_INF_ADDCHK      (IDS_A33_INF_NOFUNC + A33_ADDCHK)
#define IDS_A33_INF_IDISC       (IDS_A33_INF_NOFUNC + A33_IDISC)
#define IDS_A33_INF_RDISC       (IDS_A33_INF_NOFUNC + A33_RDISC)
#define IDS_A33_INF_TAXMOD      (IDS_A33_INF_NOFUNC + A33_TAXMOD)
#define IDS_A33_INF_AKEY        (IDS_A33_INF_NOFUNC + A33_AKEY)
#define IDS_A33_INF_BKEY        (IDS_A33_INF_NOFUNC + A33_BKEY)
#define IDS_A33_INF_SERVNO      (IDS_A33_INF_NOFUNC + A33_SERVNO)
#define IDS_A33_INF_NEWCHK      (IDS_A33_INF_NOFUNC + A33_NEWCHK)
#define IDS_A33_INF_PB          (IDS_A33_INF_NOFUNC + A33_PB)
#define IDS_A33_INF_GCNO        (IDS_A33_INF_NOFUNC + A33_GCNO)
#define IDS_A33_INF_GCTRANS     (IDS_A33_INF_NOFUNC + A33_GCTRANS)
#define IDS_A33_INF_NOPERSON    (IDS_A33_INF_NOFUNC + A33_NOPERSON)
#define IDS_A33_INF_TBLNO       (IDS_A33_INF_NOFUNC + A33_TBLNO)
#define IDS_A33_INF_LINENO      (IDS_A33_INF_NOFUNC + A33_LINENO)
#define IDS_A33_INF_FEED        (IDS_A33_INF_NOFUNC + A33_FEED)
#define IDS_A33_INF_RCTNORCT    (IDS_A33_INF_NOFUNC + A33_RCTNORCT)
#define IDS_A33_INF_TIPSPO      (IDS_A33_INF_NOFUNC + A33_TIPSPO)
#define IDS_A33_INF_DECTIP      (IDS_A33_INF_NOFUNC + A33_DECTIP)
#define IDS_A33_INF_NOSALE      (IDS_A33_INF_NOFUNC + A33_NOSALE)
#define IDS_A33_INF_PO          (IDS_A33_INF_NOFUNC + A33_PO)
#define IDS_A33_INF_ROA         (IDS_A33_INF_NOFUNC + A33_ROA)
#define IDS_A33_INF_CHGCOMP     (IDS_A33_INF_NOFUNC + A33_CHGCOMP)
#define IDS_A33_INF_CANCEL      (IDS_A33_INF_NOFUNC + A33_CANCEL)
#define IDS_A33_INF_VOID        (IDS_A33_INF_NOFUNC + A33_VOID)
#define IDS_A33_INF_EC          (IDS_A33_INF_NOFUNC + A33_EC)
#define IDS_A33_INF_VTRAN       (IDS_A33_INF_NOFUNC + A33_VTRAN)
#define IDS_A33_INF_QTY         (IDS_A33_INF_NOFUNC + A33_QTY)
#define IDS_A33_INF_SHIFT       (IDS_A33_INF_NOFUNC + A33_SHIFT)
#define IDS_A33_INF_NOTYPE      (IDS_A33_INF_NOFUNC + A33_NOTYPE)
#define IDS_A33_INF_PRTDEM      (IDS_A33_INF_NOFUNC + A33_PRTDEM)
/* #define IDS_A33_INF_ORDERNO     (IDS_A33_INF_NOFUNC + A33_ORDERNO) */
#define IDS_A33_INF_CHARTIP     (IDS_A33_INF_NOFUNC + A33_CHARTIP)
#define IDS_A33_INF_PLUNO       (IDS_A33_INF_NOFUNC + A33_PLUNO)
#define IDS_A33_INF_00KEY       (IDS_A33_INF_NOFUNC + A33_00KEY)
#define IDS_A33_INF_AC          (IDS_A33_INF_NOFUNC + A33_AC)
#define IDS_A33_INF_RECFEED     (IDS_A33_INF_NOFUNC + A33_RECFEED)
#define IDS_A33_INF_JOUFEED     (IDS_A33_INF_NOFUNC + A33_JOUFEED)
#define IDS_A33_INF_DECPT       (IDS_A33_INF_NOFUNC + A33_DECPT)
#define IDS_A33_INF_SCALE       (IDS_A33_INF_NOFUNC + A33_SCALE)
#define IDS_A33_INF_OFFTEND     (IDS_A33_INF_NOFUNC + A33_OFFTEND)
#define IDS_A33_INF_RVSPRI      (IDS_A33_INF_NOFUNC + A33_RVSPRI)
#define IDS_A33_INF_PRECASH     (IDS_A33_INF_NOFUNC + A33_PRECASH)
#define IDS_A33_INF_DRCTSHFT    (IDS_A33_INF_NOFUNC + A33_DRCTSHFT)
#define IDS_A33_INF_TIMEIN      (IDS_A33_INF_NOFUNC + A33_TIMEIN)
#define IDS_A33_INF_TIMEOUT     (IDS_A33_INF_NOFUNC + A33_TIMEOUT)
#define IDS_A33_INF_KEYEDCPN    (IDS_A33_INF_NOFUNC + A33_KEYEDCPN)
#define IDS_A33_INF_CPNNO       (IDS_A33_INF_NOFUNC + A33_CPNNO)
#define IDS_A33_INF_UP          (IDS_A33_INF_NOFUNC + A33_UP)
#define IDS_A33_INF_DOWN        (IDS_A33_INF_NOFUNC + A33_DOWN)
#define IDS_A33_INF_RIGHT       (IDS_A33_INF_NOFUNC + A33_RIGHT)
#define IDS_A33_INF_LEFT        (IDS_A33_INF_NOFUNC + A33_LEFT)
#define IDS_A33_INF_WAIT        (IDS_A33_INF_NOFUNC + A33_WAIT)
#define IDS_A33_INF_PAID_REC    (IDS_A33_INF_NOFUNC + A33_PAID_REC)
#define IDS_A33_INF_ALPHA       (IDS_A33_INF_NOFUNC + A33_ALPHA)
#define IDS_A33_INF_LEFTDISPLAY (IDS_A33_INF_NOFUNC + A33_LEFTDISPLAY)
#define IDS_A33_INF_BARTENDER   (IDS_A33_INF_NOFUNC + A33_BARTENDER)
#define IDS_A33_INF_SPLIT       (IDS_A33_INF_NOFUNC + A33_SPLIT)
#define IDS_A33_INF_SEAT        (IDS_A33_INF_NOFUNC + A33_SEAT)
#define IDS_A33_INF_ITEMTRANS   (IDS_A33_INF_NOFUNC + A33_ITEMTRANS)
#define IDS_A33_INF_REPEAT      (IDS_A33_INF_NOFUNC + A33_REPEAT)
#define IDS_A33_INF_CURSORVOID  (IDS_A33_INF_NOFUNC + A33_CURSORVOID)
#define IDS_A33_INF_PRICECHECK  (IDS_A33_INF_NOFUNC + A33_PRICECHECK)
#define IDS_A33_INF_OPRINT_A    (IDS_A33_INF_NOFUNC + A33_OPRINT_A)         /* V3.3 */
#define IDS_A33_INF_OPRINT_B    (IDS_A33_INF_NOFUNC + A33_OPRINT_B)         /* V3.3 */
#define IDS_A33_INF_LOCK        (IDS_A33_INF_NOFUNC + A33_LOCK)
#define IDS_A33_INF_REGS        (IDS_A33_INF_NOFUNC + A33_REGS)
#define IDS_A33_INF_SUPER       (IDS_A33_INF_NOFUNC + A33_SUPER)
#define IDS_A33_INF_PROG        (IDS_A33_INF_NOFUNC + A33_PROG)
#define IDS_A33_INF_PAUSE       (IDS_A33_INF_NOFUNC + A33_PAUSE)
#define IDS_A33_INF_CALLSTR     (IDS_A33_INF_NOFUNC + A33_CALLSTR)
#define IDS_A33_INF_ZERO        (IDS_A33_INF_NOFUNC + A33_ZERO)
#define IDS_A33_INF_ONE         (IDS_A33_INF_NOFUNC + A33_ONE)
#define IDS_A33_INF_TWO         (IDS_A33_INF_NOFUNC + A33_TWO)
#define IDS_A33_INF_THREE       (IDS_A33_INF_NOFUNC + A33_THREE)
#define IDS_A33_INF_FOUR        (IDS_A33_INF_NOFUNC + A33_FOUR)
#define IDS_A33_INF_FIVE        (IDS_A33_INF_NOFUNC + A33_FIVE)
#define IDS_A33_INF_SIX         (IDS_A33_INF_NOFUNC + A33_SIX)
#define IDS_A33_INF_SEVEN       (IDS_A33_INF_NOFUNC + A33_SEVEN)
#define IDS_A33_INF_EIGHT       (IDS_A33_INF_NOFUNC + A33_EIGHT)
#define IDS_A33_INF_NINE        (IDS_A33_INF_NOFUNC + A33_NINE)
#define IDS_A33_INF_CLEAR       (IDS_A33_INF_NOFUNC + A33_CLEAR)
#define IDS_A33_INF_P1          (IDS_A33_INF_NOFUNC + A33_P1)
#define IDS_A33_INF_P2          (IDS_A33_INF_NOFUNC + A33_P2)
#define IDS_A33_INF_P3          (IDS_A33_INF_NOFUNC + A33_P3)
#define IDS_A33_INF_P4          (IDS_A33_INF_NOFUNC + A33_P4)
#define IDS_A33_INF_P5          (IDS_A33_INF_NOFUNC + A33_P5)

/*** NCR2172 ***/

#define IDS_A33_INF_MONEY       (IDS_A33_INF_NOFUNC + A33_MONEY)
#define IDS_A33_INF_ASK         (IDS_A33_INF_NOFUNC + A33_ASK)
#define IDS_A33_INF_LEVEL       (IDS_A33_INF_NOFUNC + A33_LEVEL)
#define IDS_A33_INF_DEPTNUMBER  (IDS_A33_INF_NOFUNC + A33_DEPTNUMBER)
#define IDS_A33_INF_UPCVER      (IDS_A33_INF_NOFUNC + A33_UPCVER)
#define IDS_A33_INF_PRICEKEY    (IDS_A33_INF_NOFUNC + A33_PRICEKEY)
#define IDS_A33_INF_FSMOD       (IDS_A33_INF_NOFUNC + A33_FSMOD)       /* ### ADD Saratoga (052500) */
#define IDS_A33_INF_WICTRANS    (IDS_A33_INF_NOFUNC + A33_WICTRANS)    /* ### ADD Saratoga (052500) */
#define IDS_A33_INF_WICMOD      (IDS_A33_INF_NOFUNC + A33_WICMOD)      /* ### ADD Saratoga (052500) */
#define IDS_A33_INF_HALO_OVR	(IDS_A33_INF_NOFUNC + A33_HALO_OVR)
#define IDS_A33_INF_ATFOR		(IDS_A33_INF_NOFUNC + A33_ATFOR)
#define IDS_A33_INF_VALIDATION	(IDS_A33_INF_NOFUNC + A33_VALIDATION)
#define IDS_A33_INF_DOC_LAUNCH	(IDS_A33_INF_NOFUNC + A33_DOC_LAUNCH)					//FSC_DOC_LAUNCH
#define IDS_A33_INF_WINDOW_DISPLAY	(IDS_A33_INF_NOFUNC + A33_WINDOW_DISPLAY)			//FSC_WINDOW_DISPLAY
#define IDS_A33_INF_WINDOW_DISMISS	(IDS_A33_INF_NOFUNC + A33_WINDOW_DISMISS)			//FSC_WINDOW_DISMISS
#define IDS_A33_INF_ORDER_DEC		(IDS_A33_INF_NOFUNC + A33_ORDER_DEC)			    //FSC_ORDER_DEC

#define IDS_A33_DSP_NOFUNC      (IDS_A33_INF_NOFUNC + A33_DSP_INF_OFS)
#define IDS_A33_DSP_KEYEDPLU    (IDS_A33_DSP_NOFUNC + A33_KEYEDPLU)
#define IDS_A33_DSP_KEYEDDPT    (IDS_A33_DSP_NOFUNC + A33_KEYEDDPT)
#define IDS_A33_DSP_PRTMOD      (IDS_A33_DSP_NOFUNC + A33_PRTMOD)
#define IDS_A33_DSP_ADJ         (IDS_A33_DSP_NOFUNC + A33_ADJ)
#define IDS_A33_DSP_TENDER      (IDS_A33_DSP_NOFUNC + A33_TENDER)
#define IDS_A33_DSP_FC          (IDS_A33_DSP_NOFUNC + A33_FC)
#define IDS_A33_DSP_TTL         (IDS_A33_DSP_NOFUNC + A33_TTL)
#define IDS_A33_DSP_ADDCHK      (IDS_A33_DSP_NOFUNC + A33_ADDCHK)
#define IDS_A33_DSP_IDISC       (IDS_A33_DSP_NOFUNC + A33_IDISC)
#define IDS_A33_DSP_RDISC       (IDS_A33_DSP_NOFUNC + A33_RDISC)
#define IDS_A33_DSP_TAXMOD      (IDS_A33_DSP_NOFUNC + A33_TAXMOD)
#define IDS_A33_DSP_AKEY        (IDS_A33_DSP_NOFUNC + A33_AKEY)
#define IDS_A33_DSP_BKEY        (IDS_A33_DSP_NOFUNC + A33_BKEY)
#define IDS_A33_DSP_SERVNO      (IDS_A33_DSP_NOFUNC + A33_SERVNO)
#define IDS_A33_DSP_NEWCHK      (IDS_A33_DSP_NOFUNC + A33_NEWCHK)
#define IDS_A33_DSP_PB          (IDS_A33_DSP_NOFUNC + A33_PB)
#define IDS_A33_DSP_GCNO        (IDS_A33_DSP_NOFUNC + A33_GCNO)
#define IDS_A33_DSP_GCTRANS     (IDS_A33_DSP_NOFUNC + A33_GCTRANS)
#define IDS_A33_DSP_NOPERSON    (IDS_A33_DSP_NOFUNC + A33_NOPERSON)
#define IDS_A33_DSP_TBLNO       (IDS_A33_DSP_NOFUNC + A33_TBLNO)
#define IDS_A33_DSP_LINENO      (IDS_A33_DSP_NOFUNC + A33_LINENO)
#define IDS_A33_DSP_FEED        (IDS_A33_DSP_NOFUNC + A33_FEED)
#define IDS_A33_DSP_RCTNORCT    (IDS_A33_DSP_NOFUNC + A33_RCTNORCT)
#define IDS_A33_DSP_TIPSPO      (IDS_A33_DSP_NOFUNC + A33_TIPSPO)
#define IDS_A33_DSP_DECTIP      (IDS_A33_DSP_NOFUNC + A33_DECTIP)
#define IDS_A33_DSP_NOSALE      (IDS_A33_DSP_NOFUNC + A33_NOSALE)
#define IDS_A33_DSP_PO          (IDS_A33_DSP_NOFUNC + A33_PO)
#define IDS_A33_DSP_ROA         (IDS_A33_DSP_NOFUNC + A33_ROA)
#define IDS_A33_DSP_CHGCOMP     (IDS_A33_DSP_NOFUNC + A33_CHGCOMP)
#define IDS_A33_DSP_CANCEL      (IDS_A33_DSP_NOFUNC + A33_CANCEL)
#define IDS_A33_DSP_VOID        (IDS_A33_DSP_NOFUNC + A33_VOID)
#define IDS_A33_DSP_EC          (IDS_A33_DSP_NOFUNC + A33_EC)
#define IDS_A33_DSP_VTRAN       (IDS_A33_DSP_NOFUNC + A33_VTRAN)
#define IDS_A33_DSP_QTY         (IDS_A33_DSP_NOFUNC + A33_QTY)
#define IDS_A33_DSP_SHIFT       (IDS_A33_DSP_NOFUNC + A33_SHIFT)
#define IDS_A33_DSP_NOTYPE      (IDS_A33_DSP_NOFUNC + A33_NOTYPE)
#define IDS_A33_DSP_PRTDEM      (IDS_A33_DSP_NOFUNC + A33_PRTDEM)
/* #define IDS_A33_DSP_ORDERNO     (IDS_A33_DSP_NOFUNC + A33_ORDERNO) */
#define IDS_A33_DSP_CHARTIP     (IDS_A33_DSP_NOFUNC + A33_CHARTIP)
#define IDS_A33_DSP_PLUNO       (IDS_A33_DSP_NOFUNC + A33_PLUNO)
#define IDS_A33_DSP_00KEY       (IDS_A33_DSP_NOFUNC + A33_00KEY)
#define IDS_A33_DSP_AC          (IDS_A33_DSP_NOFUNC + A33_AC)
#define IDS_A33_DSP_RECFEED     (IDS_A33_DSP_NOFUNC + A33_RECFEED)
#define IDS_A33_DSP_JOUFEED     (IDS_A33_DSP_NOFUNC + A33_JOUFEED)
#define IDS_A33_DSP_DECPT       (IDS_A33_DSP_NOFUNC + A33_DECPT)
#define IDS_A33_DSP_SCALE       (IDS_A33_DSP_NOFUNC + A33_SCALE)
#define IDS_A33_DSP_OFFTEND     (IDS_A33_DSP_NOFUNC + A33_OFFTEND)
#define IDS_A33_DSP_RVSPRI      (IDS_A33_DSP_NOFUNC + A33_RVSPRI)
#define IDS_A33_DSP_PRECASH     (IDS_A33_DSP_NOFUNC + A33_PRECASH)
#define IDS_A33_DSP_DRCTSHFT    (IDS_A33_DSP_NOFUNC + A33_DRCTSHFT)
#define IDS_A33_DSP_TIMEIN      (IDS_A33_DSP_NOFUNC + A33_TIMEIN)
#define IDS_A33_DSP_TIMEOUT     (IDS_A33_DSP_NOFUNC + A33_TIMEOUT)

#define IDS_A33_DSP_KEYEDCPN     (IDS_A33_DSP_NOFUNC + A33_KEYEDCPN)
#define IDS_A33_DSP_CPNNO        (IDS_A33_DSP_NOFUNC + A33_CPNNO)
#define IDS_A33_DSP_UP           (IDS_A33_DSP_NOFUNC + A33_UP)
#define IDS_A33_DSP_DOWN         (IDS_A33_DSP_NOFUNC + A33_DOWN)
#define IDS_A33_DSP_RIGHT        (IDS_A33_DSP_NOFUNC + A33_RIGHT)
#define IDS_A33_DSP_LEFT         (IDS_A33_DSP_NOFUNC + A33_LEFT)
#define IDS_A33_DSP_WAIT         (IDS_A33_DSP_NOFUNC + A33_WAIT)
#define IDS_A33_DSP_PAID_REC     (IDS_A33_DSP_NOFUNC + A33_PAID_REC)
#define IDS_A33_DSP_ALPHA        (IDS_A33_DSP_NOFUNC + A33_ALPHA)
#define IDS_A33_DSP_LEFTDISPLAY  (IDS_A33_DSP_NOFUNC + A33_LEFTDISPLAY)
#define IDS_A33_DSP_BARTENDER    (IDS_A33_DSP_NOFUNC + A33_BARTENDER)
#define IDS_A33_DSP_SPLIT        (IDS_A33_DSP_NOFUNC + A33_SPLIT)
#define IDS_A33_DSP_SEAT         (IDS_A33_DSP_NOFUNC + A33_SEAT)
#define IDS_A33_DSP_ITEMTRANS    (IDS_A33_DSP_NOFUNC + A33_ITEMTRANS)
#define IDS_A33_DSP_REPEAT       (IDS_A33_DSP_NOFUNC + A33_REPEAT)
#define IDS_A33_DSP_CURSORVOID   (IDS_A33_DSP_NOFUNC + A33_CURSORVOID)
#define IDS_A33_DSP_PRICECHECK   (IDS_A33_DSP_NOFUNC + A33_PRICECHECK)
#define IDS_A33_DSP_OPRINT_A     (IDS_A33_DSP_NOFUNC + A33_OPRINT_A)            /* V3.3 */
#define IDS_A33_DSP_OPRINT_B     (IDS_A33_DSP_NOFUNC + A33_OPRINT_B)            /* V3.3 */
#define IDS_A33_DSP_LOCK         (IDS_A33_DSP_NOFUNC + A33_LOCK)
#define IDS_A33_DSP_REGS         (IDS_A33_DSP_NOFUNC + A33_REGS)
#define IDS_A33_DSP_SUPER        (IDS_A33_DSP_NOFUNC + A33_SUPER)
#define IDS_A33_DSP_PROG         (IDS_A33_DSP_NOFUNC + A33_PROG)
#define IDS_A33_DSP_PAUSE        (IDS_A33_DSP_NOFUNC + A33_PAUSE)
#define IDS_A33_DSP_CALLSTR      (IDS_A33_DSP_NOFUNC + A33_CALLSTR)
#define IDS_A33_DSP_ZERO         (IDS_A33_DSP_NOFUNC + A33_ZERO)
#define IDS_A33_DSP_ONE          (IDS_A33_DSP_NOFUNC + A33_ONE)
#define IDS_A33_DSP_TWO          (IDS_A33_DSP_NOFUNC + A33_TWO)
#define IDS_A33_DSP_THREE        (IDS_A33_DSP_NOFUNC + A33_THREE)
#define IDS_A33_DSP_FOUR         (IDS_A33_DSP_NOFUNC + A33_FOUR)
#define IDS_A33_DSP_FIVE         (IDS_A33_DSP_NOFUNC + A33_FIVE)
#define IDS_A33_DSP_SIX          (IDS_A33_DSP_NOFUNC + A33_SIX)
#define IDS_A33_DSP_SEVEN        (IDS_A33_DSP_NOFUNC + A33_SEVEN)
#define IDS_A33_DSP_EIGHT        (IDS_A33_DSP_NOFUNC + A33_EIGHT)
#define IDS_A33_DSP_NINE         (IDS_A33_DSP_NOFUNC + A33_NINE)
#define IDS_A33_DSP_CLEAR        (IDS_A33_DSP_NOFUNC + A33_CLEAR)
#define IDS_A33_DSP_P1           (IDS_A33_DSP_NOFUNC + A33_P1)
#define IDS_A33_DSP_P2           (IDS_A33_DSP_NOFUNC + A33_P2)
#define IDS_A33_DSP_P3           (IDS_A33_DSP_NOFUNC + A33_P3)
#define IDS_A33_DSP_P4           (IDS_A33_DSP_NOFUNC + A33_P4)
#define IDS_A33_DSP_P5           (IDS_A33_DSP_NOFUNC + A33_P5)
#define IDS_A33_DSP_NOUSE        (IDS_A33_DSP_NOFUNC + A33_NOUSE)

/*** NCR2172 ***/

#define IDS_A33_DSP_MONEY       (IDS_A33_DSP_NOFUNC + A33_MONEY)
#define IDS_A33_DSP_ASK         (IDS_A33_DSP_NOFUNC + A33_ASK)
#define IDS_A33_DSP_LEVEL       (IDS_A33_DSP_NOFUNC + A33_LEVEL)
#define IDS_A33_DSP_DEPTNUMBER  (IDS_A33_DSP_NOFUNC + A33_DEPTNUMBER)
#define IDS_A33_DSP_UPCVER      (IDS_A33_DSP_NOFUNC + A33_UPCVER)
#define IDS_A33_DSP_PRICEKEY    (IDS_A33_DSP_NOFUNC + A33_PRICEKEY)
#define IDS_A33_DSP_FSMOD       (IDS_A33_DSP_NOFUNC + A33_FSMOD)       /* ### ADD Saratoga (052500) */
#define IDS_A33_DSP_WICTRANS    (IDS_A33_DSP_NOFUNC + A33_WICTRANS)    /* ### ADD Saratoga (052500) */
#define IDS_A33_DSP_WICMOD      (IDS_A33_DSP_NOFUNC + A33_WICMOD)      /* ### ADD Saratoga (052500) */
#define IDS_A33_DSP_HALO_OVR	(IDS_A33_DSP_NOFUNC + A33_HALO_OVR)
#define IDS_A33_DSP_ATFOR		(IDS_A33_DSP_NOFUNC + A33_ATFOR)
#define IDS_A33_DSP_VALIDATION	(IDS_A33_DSP_NOFUNC + A33_VALIDATION)
#define IDS_A33_DSP_RECEIPT_ID	(IDS_A33_DSP_NOFUNC + A33_RECEIPT_ID)

#define IDS_A33_DSP_DOC_LAUNCH	(IDS_A33_DSP_NOFUNC + A33_DOC_LAUNCH)					//FSC_DOC_LAUNCH
#define IDS_A33_DSP_WINDOW_DISPLAY	(IDS_A33_DSP_NOFUNC + A33_WINDOW_DISPLAY)			//FSC_WINDOW_DISPLAY
#define IDS_A33_DSP_WINDOW_DISMISS	(IDS_A33_DSP_NOFUNC + A33_WINDOW_DISMISS)			//FSC_WINDOW_DISMISS
#define IDS_A33_DSP_ORDER_DEC	    (IDS_A33_DSP_NOFUNC + A33_ORDER_DEC)			    //FSC_ORDER_DEC

#define IDS_A33_CAPTION         (IDS_A33            + 600)
#define IDS_A33_NO              (IDS_A33_CAPTION    +   1)
#define IDS_A33_NOLEN           (IDS_A33_CAPTION    +   2)
#define IDS_A33_DESC            (IDS_A33_CAPTION    +   3)
#define IDS_A33_ADRREC          (IDS_A33_CAPTION    +   4)
#define IDS_A33_TOOL            (IDS_A33_CAPTION    +   5)
#define IDS_A33_ADDCHG          (IDS_A33_CAPTION    +   6)
#define IDS_A33_DEL             (IDS_A33_CAPTION    +   7)
#define IDS_A33_STRING          (IDS_A33_CAPTION    +   8)
#define IDS_A33_KEYEDPLU        (IDS_A33_CAPTION    +   9)
#define IDS_A33_KEYEDDPT        (IDS_A33_CAPTION    +  10)
#define IDS_A33_PRTMOD          (IDS_A33_CAPTION    +  11)
#define IDS_A33_ADJ             (IDS_A33_CAPTION    +  12)
#define IDS_A33_TENDER          (IDS_A33_CAPTION    +  13)
#define IDS_A33_FC              (IDS_A33_CAPTION    +  14)
#define IDS_A33_TTL             (IDS_A33_CAPTION    +  15)
#define IDS_A33_ADDCHK          (IDS_A33_CAPTION    +  16)
#define IDS_A33_IDISC           (IDS_A33_CAPTION    +  17)
#define IDS_A33_RDISC           (IDS_A33_CAPTION    +  18)
#define IDS_A33_TAXMOD          (IDS_A33_CAPTION    +  19)
#define IDS_A33_AKEY            (IDS_A33_CAPTION    +  20)
#define IDS_A33_BKEY            (IDS_A33_CAPTION    +  21)
#define IDS_A33_SERVNO          (IDS_A33_CAPTION    +  22)
#define IDS_A33_NEWCHK          (IDS_A33_CAPTION    +  23)
#define IDS_A33_PB              (IDS_A33_CAPTION    +  24)
#define IDS_A33_GCNO            (IDS_A33_CAPTION    +  25)
#define IDS_A33_GCTRANS         (IDS_A33_CAPTION    +  26)
#define IDS_A33_NOPERSON        (IDS_A33_CAPTION    +  27)
#define IDS_A33_TBLNO           (IDS_A33_CAPTION    +  28)
#define IDS_A33_LINENO          (IDS_A33_CAPTION    +  29)
#define IDS_A33_FEED            (IDS_A33_CAPTION    +  30)
#define IDS_A33_RCTNORCT        (IDS_A33_CAPTION    +  31)
#define IDS_A33_TIPSPO          (IDS_A33_CAPTION    +  32)
#define IDS_A33_DECTIP          (IDS_A33_CAPTION    +  33)
#define IDS_A33_NOSALE          (IDS_A33_CAPTION    +  34)
#define IDS_A33_PO              (IDS_A33_CAPTION    +  35)
#define IDS_A33_ROA             (IDS_A33_CAPTION    +  36)
#define IDS_A33_CHGCOMP         (IDS_A33_CAPTION    +  37)
#define IDS_A33_CANCEL          (IDS_A33_CAPTION    +  38)
#define IDS_A33_VOID            (IDS_A33_CAPTION    +  39)
#define IDS_A33_EC              (IDS_A33_CAPTION    +  40)
#define IDS_A33_VTRAN           (IDS_A33_CAPTION    +  41)
#define IDS_A33_QTY             (IDS_A33_CAPTION    +  42)
#define IDS_A33_SHIFT           (IDS_A33_CAPTION    +  43)
#define IDS_A33_NOTYPE          (IDS_A33_CAPTION    +  44)
#define IDS_A33_PRTDEM          (IDS_A33_CAPTION    +  45)
#define IDS_A33_CHARTIP         (IDS_A33_CAPTION    +  46)
#define IDS_A33_PLUNO           (IDS_A33_CAPTION    +  47)
#define IDS_A33_00KEY           (IDS_A33_CAPTION    +  48)
#define IDS_A33_AC              (IDS_A33_CAPTION    +  49)
#define IDS_A33_RECFEED         (IDS_A33_CAPTION    +  50)
#define IDS_A33_JOUFEED         (IDS_A33_CAPTION    +  51)
#define IDS_A33_DECPT           (IDS_A33_CAPTION    +  52)
#define IDS_A33_SCALE           (IDS_A33_CAPTION    +  53)
#define IDS_A33_OFFTEND         (IDS_A33_CAPTION    +  54)
#define IDS_A33_RVSPRI          (IDS_A33_CAPTION    +  55)
#define IDS_A33_PRECASH         (IDS_A33_CAPTION    +  56)
#define IDS_A33_DRCTSHFT        (IDS_A33_CAPTION    +  57)
#define IDS_A33_TIMEIN          (IDS_A33_CAPTION    +  58)
#define IDS_A33_TIMEOUT         (IDS_A33_CAPTION    +  59)
#define IDS_A33_WAIT            (IDS_A33_CAPTION    +  60)
#define IDS_A33_KEYEDCPN        (IDS_A33_CAPTION    +  61)
#define IDS_A33_CPNNO           (IDS_A33_CAPTION    +  62)
#define IDS_A33_PAID_REC        (IDS_A33_CAPTION    +  63)
#define IDS_A33_ALPHA           (IDS_A33_CAPTION    +  64)
#define IDS_A33_LEFTDISPLAY     (IDS_A33_CAPTION    +  65)
#define IDS_A33_UP              (IDS_A33_CAPTION    +  66)
#define IDS_A33_CALLSTR         (IDS_A33_CAPTION    +  67)
#define IDS_A33_PAUSE           (IDS_A33_CAPTION    +  68)
#define IDS_A33_LEFT            (IDS_A33_CAPTION    +  69)
#define IDS_A33_DOWN            (IDS_A33_CAPTION    +  70)
#define IDS_A33_RIGHT           (IDS_A33_CAPTION    +  71)
#define IDS_A33_BARTENDER       (IDS_A33_CAPTION    +  72)
#define IDS_A33_SPLIT           (IDS_A33_CAPTION    +  73)
#define IDS_A33_SEAT            (IDS_A33_CAPTION    +  74)
#define IDS_A33_ITEMTRANS       (IDS_A33_CAPTION    +  75)
#define IDS_A33_REPEAT          (IDS_A33_CAPTION    +  76)
#define IDS_A33_CURSORVOID      (IDS_A33_CAPTION    +  77)
#define IDS_A33_PRICECHECK      (IDS_A33_CAPTION    +  78)
#define IDS_A33_OPRINT_A        (IDS_A33_CAPTION    +  79)          /* V3.3 */
#define IDS_A33_OPRINT_B        (IDS_A33_CAPTION    +  80)          /* V3.3 */
#define IDS_A33_P5              (IDS_A33_CAPTION    +  81)
#define IDS_A33_P1              (IDS_A33_CAPTION    +  82)
#define IDS_A33_P2              (IDS_A33_CAPTION    +  83)
#define IDS_A33_P3              (IDS_A33_CAPTION    +  84)
#define IDS_A33_P4              (IDS_A33_CAPTION    +  85)
#define IDS_A33_SEVEN           (IDS_A33_CAPTION    +  86)
#define IDS_A33_EIGHT           (IDS_A33_CAPTION    +  87)
#define IDS_A33_NINE            (IDS_A33_CAPTION    +  88)
#define IDS_A33_PROG            (IDS_A33_CAPTION    +  89)
#define IDS_A33_FOUR            (IDS_A33_CAPTION    +  90)
#define IDS_A33_FIVE            (IDS_A33_CAPTION    +  91)
#define IDS_A33_SIX             (IDS_A33_CAPTION    +  92)
#define IDS_A33_SUPER           (IDS_A33_CAPTION    +  93)
#define IDS_A33_ONE             (IDS_A33_CAPTION    +  94)
#define IDS_A33_TWO             (IDS_A33_CAPTION    +  95)
#define IDS_A33_THREE           (IDS_A33_CAPTION    +  96)
#define IDS_A33_REGS            (IDS_A33_CAPTION    +  97)
#define IDS_A33_ZERO            (IDS_A33_CAPTION    +  98)
#define IDS_A33_NOFUNC          (IDS_A33_CAPTION    +  99)
#define IDS_A33_CLEAR           (IDS_A33_CAPTION    + 100)
#define IDS_A33_LOCK            (IDS_A33_CAPTION    + 101)
#define IDS_A33_FSCCAPTION      (IDS_A33_CAPTION    + 102)
#define IDS_A33_FSCNO           (IDS_A33_CAPTION    + 103)
#define IDS_A33_FSCLEN          (IDS_A33_CAPTION    + 104)
#define IDS_A33_MNEMOCAPTION    (IDS_A33_CAPTION    + 105)
#define IDS_A33_MNEMO           (IDS_A33_CAPTION    + 106)
#define IDS_A33_MNEMOMAX        (IDS_A33_CAPTION    + 107)

/*** NCR2172 ***/

#define IDS_A33_MONEY           (IDS_A33_CAPTION    + 108)
#define IDS_A33_ASK             (IDS_A33_CAPTION    + 109)
#define IDS_A33_LEVEL           (IDS_A33_CAPTION    + 110)
#define IDS_A33_DEPTNUMBER      (IDS_A33_CAPTION    + 111)
#define IDS_A33_UPCVER          (IDS_A33_CAPTION    + 112)
#define IDS_A33_PRICEKEY        (IDS_A33_CAPTION    + 113)
#define IDS_A33_FSMOD           (IDS_A33_CAPTION    + 114)   /* ### ADD Saratoga (052500) */
#define IDS_A33_WICTRANS        (IDS_A33_CAPTION    + 115)   /* ### ADD Saratoga (052500) */
#define IDS_A33_WICMOD          (IDS_A33_CAPTION    + 116)   /* ### ADD Saratoga (052500) */
#define IDS_A33_HALO_OVR		(IDS_A33_CAPTION	+ 117)
#define IDS_A33_ATFOR			(IDS_A33_CAPTION	+ 118)
#define IDS_A33_VALIDATION		(IDS_A33_CAPTION	+ 119)

#define IDS_A33_CLEARALL		(IDS_A33_CAPTION	+ 120)
#define	IDS_A33_UPSWITCH		(IDS_A33_CAPTION	+ 121)
#define IDS_A33_DOWNSWITCH		(IDS_A33_CAPTION	+ 122)
#define IDS_A33_RECEIPT_ID		(IDS_A33_CAPTION	+ 123)
#define IDS_A33_DOC_LAUNCH		(IDS_A33_CAPTION	+ 124)			//FSC_DOC_LAUNCH

//FSC_WINDOW_DISPLAY and FSC_WINDOW_DISMISS
#define IDS_A33_WINDOW_DISPLAY			(IDS_A33_CAPTION	+ 125)
#define IDS_A33_WINDOW_DISMISS			(IDS_A33_CAPTION	+ 126)
#define IDS_A33_ORDER_DEC				(IDS_A33_CAPTION	+ 127)

#define IDS_A33_FSCLEN_0          (IDS_A33_CAPTION    + 128)

/*
* ===========================================================================
*       Structure type defintions
* ===========================================================================
*/
typedef OPCSTR_PARAENTRY  FAR*   LPCSTRPARA;

typedef OPCSTR_INDEXENTRY FAR*   LPCSTRINDEX;


/*
* ===========================================================================
*       Macro Difinitions
* ===========================================================================
*/
    /* ----- Retrieve Major FSC Code with Current Selected Button ID ----- */
#define A33MAJORFSCCODE(wID)    (wID - IDD_A33_NOFUNC)
#define A33MAJORFXKCODE(wID)    (wID - IDD_A33_PAUSE)

/*
* ===========================================================================
*       External Valiables
* ===========================================================================
*/
/*
* ===========================================================================
*       Function Prototypes
* ===========================================================================
*/
BOOL    A033ReadMax(HWND hDlg, LPUINT lpunMaxCstr);
VOID    A033DelData(HWND hDlg);
VOID	A033ClearData(HWND hDlg);
VOID	A033DataUp(HWND hDlg);
VOID	A033DataDown(HWND hDlg);
VOID    A033ChgData(HWND hDlg);
BOOL    A033AllocMem(HWND hDlg, LPHGLOBAL lphIndtb, LPHGLOBAL lphPara);
VOID    A033InitDlg(HWND hDlg);
VOID    A033FreeMem(LPHGLOBAL lphIndtb, LPHGLOBAL lphPara);
VOID    A033ReadData();
VOID    A033GetList(HWND hDlg);
VOID    A033FinDlg(HWND hDlg, WPARAM wParam);
VOID    A033ChkRng(HWND hDlg, WORD wEditID, UINT unMax);
VOID    A33ChkExtRng(HWND hDlg, UINT unVal);
BOOL    A033ChkUpdate(HWND hDlg);
BOOL    A33GetExtFsc(HWND hDlg, WORD wCurBtnID, UINT unVal);
VOID    A033SetData(HWND hDlg, UCHAR  uchMajorData, UCHAR  uchMinorData);
VOID    A033SetList(HWND hDlg);
VOID    A033ChgList(HWND hDlg, LPCSTRPARA  lpCstr);
VOID    A033SetExtFsc(HWND hDlg);
VOID    A033SaveString(HWND hDlg, LPWSTR lpszSource );

/* ===== End of A033.H ===== */

