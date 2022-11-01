/*
* ---------------------------------------------------------------------------
* Title         :   Cashier Maintenance Header File (AC 20)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A020.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
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
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Sep-11-98 : 03.03.00 : A.Mitsui   : V3.3
* Dec-09-99 :          : K.Yanagida : NCR2172
*
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
/*
* ===========================================================================
*       Define Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Dialog ID
* ===========================================================================
*/
#define IDD_A20_MAX         12001   /* maximum cashier records          */
#define IDD_A20_CUR         12002   /* current cashier records          */
#define IDD_A20_LIST        12003   /* listbox                          */
#define IDD_A20_ADDCHG      12004   /* add, change button               */
#define IDD_A20_DEL         12005   /* delete button                    */
#define IDD_A20_NO          12006   /* cashier No.                      */
#define IDD_A20_NAME        12007   /* cashier name                     */
#define IDD_A20_CONF        12008   /* delete confirmation checkbox     */
#define IDD_A20_BIO_FINGER	12009   // Fingerprint settings dialog
#define IDD_A20_CTRL        12010   /* control code dialog id           */
#define IDD_A20_NOFROM      12011   /* guest check number from          */
#define IDD_A20_NOTO        12012   /* guest check number to            */
#define IDD_A20_CTIP        12013   /* Charge Tip Rate                  */
#define IDD_A20_TEAM        12014   /* Team Number                      */
#define IDD_A20_TERMNO      12015   /* Allowed Terminal Number          */
#define IDD_A20_BIO_FINGER_RESET	12016	//Fingerprint settings warning text
#define IDD_A20_CODE1	    (12017)
#define IDD_A20_CODE2	    (12018)
#define IDD_A20_CODE3	    (12019)

/* ----- Spin Button ID ----- */
#define A20_SPIN_OFFSET		104		//this should be updated if the values below go past 12100
#define IDD_A20_SPIN_OPR    (IDD_A20_NO     + A20_SPIN_OFFSET)  /* cashier No */
#define IDD_A20_SPIN_FROM   (IDD_A20_NOFROM + A20_SPIN_OFFSET)  /* guest check number from */
#define IDD_A20_SPIN_TO     (IDD_A20_NOTO   + A20_SPIN_OFFSET)  /* guest check number to */
#define IDD_A20_SPIN_CTIP   (IDD_A20_CTIP   + A20_SPIN_OFFSET)  /* Charge Tip Rate */
#define IDD_A20_SPIN_TEAM   (IDD_A20_TEAM   + A20_SPIN_OFFSET)  /* Team Number */
#define IDD_A20_SPIN_TERMNO (IDD_A20_TERMNO + A20_SPIN_OFFSET)  /* Allowed Terminal Number */
#define IDD_A20_SPIN1	    (IDD_A20_CODE1 + A20_SPIN_OFFSET)
#define IDD_A20_SPIN2	    (IDD_A20_CODE2 + A20_SPIN_OFFSET)
#define IDD_A20_SPIN3	    (IDD_A20_CODE3 + A20_SPIN_OFFSET)

#define IDD_A20_STRMENT     12030
#define IDD_A20_STRNO       (IDD_A20_STRMENT + 1)
#define IDD_A20_STRSPIN     (IDD_A20_STRMENT + 2)
#define IDD_A20_STRNAME     (IDD_A20_STRMENT + 3)
#define IDD_A20_STRNAMEMAX  (IDD_A20_STRMENT + 4)
#define IDD_A20_STRCTRL     (IDD_A20_STRMENT + 5)
#define IDD_A20_GUEST_CK	(IDD_A20_STRMENT + 6)
#define IDD_A20_FROM		(IDD_A20_STRMENT + 7)
#define IDD_A20_TO			(IDD_A20_STRMENT + 8)
#define IDD_A20_CHGTIP		(IDD_A20_STRMENT + 9)
#define IDD_A20_TEAM_NO		(IDD_A20_STRMENT + 10)
#define IDD_A20_TERM_NO		(IDD_A20_STRMENT + 11)
#define IDD_A20_CHGTIP_RNG	(IDD_A20_STRMENT + 12)
#define IDD_A20_TEAM_NO_RNG	(IDD_A20_STRMENT + 13)
#define IDD_A20_TERM_NO_RNG	(IDD_A20_STRMENT + 14)
#define IDD_A20_SUP_ID_LBL	(IDD_A20_STRMENT + 15)
#define IDD_A20_SUP_ID_NO	(IDD_A20_STRMENT + 16)
#define IDD_A20_SUP_ID_RNG	(IDD_A20_STRMENT + 17)
#define IDD_A20_CTRLSTR_LBL	(IDD_A20_STRMENT + 18)
#define IDD_A20_CTRLSTR_NO	(IDD_A20_STRMENT + 19)
#define IDD_A20_CTRLSTR_RNG	(IDD_A20_STRMENT + 20)
#define IDD_A20_WINDOW_LBL	(IDD_A20_STRMENT + 21)
#define IDD_A20_WINDOW_NO	(IDD_A20_STRMENT + 22)
#define IDD_A20_WINDOW_RNG	(IDD_A20_STRMENT + 23)
#define IDD_A20_CAPTION2	(IDD_A20_STRMENT + 24)
#define IDD_A20_CODE1_RNG	(IDD_A20_STRMENT + 25)
#define IDD_A20_CAPTION3	(IDD_A20_STRMENT + 26)
#define IDD_A20_CODE2_RNG	(IDD_A20_STRMENT + 27)
#define IDD_A20_CAPTION4	(IDD_A20_STRMENT + 28)
#define IDD_A20_CODE3_RNG	(IDD_A20_STRMENT + 29)

#define IDC_ORDERDEC_CHK			12300
#define IDC_ORDERDECSTATIC			12301
#define IDD_A20_ORDERDEC_ENABLED	12302	//Order Declaration

// Address 1 & 2
#define IDD_A20ST01         12340				//Prohibit Operator to Close Own Check
#define IDD_A20ST02         (IDD_A20ST01 +  1)	//Prohibit Non-Guest Check Transaction
#define IDD_A20ST03         (IDD_A20ST01 +  2)	//Prohibit Guest Check Open
#define IDD_A20ST04         (IDD_A20ST01 +  3)	//Training Operator
//Only seven values here, 0x10 is ignored (see A20_CHKBIT1)
#define IDD_A20ST05         (IDD_A20ST01 +  4)	//Prohibit Check Transfer to Other Operator
#define IDD_A20ST06         (IDD_A20ST01 +  5)	//Prohibit Check Transfer from Other Operator
#define IDD_A20ST07         (IDD_A20ST01 +  6)	//Prohibit Operator to Close Any Check

// Address 3 & 4
#define IDD_A20ST08         (IDD_A20ST01 +  7)	//Prohibit Sign-in as Surrogate
#define IDD_A20ST09         (IDD_A20ST01 +  8)	//Release on Finalize at Non-GC Transaction
#define IDD_A20ST10         (IDD_A20ST01 +  9)	//Release on Finalize at Guest Check Transaction
#define IDD_A20ST11         (IDD_A20ST01 + 10)	//Require Secret Code
#define IDD_A20ST12         (IDD_A20ST01 + 11)	//Prohibit Operator Interrupt
#define IDD_A20ST13         (IDD_A20ST01 + 12)	//Use Team
#define IDD_A20ST14         (IDD_A20ST01 + 13)	//B Drawer
#define IDD_A20ST15         (IDD_A20ST01 + 14)	//A Drawer

// Address 5 & 6
#define IDD_A20ST16			(IDD_A20ST01 + 15)	//Open Cash Drawer on Sign-in
#define IDD_A20ST17			(IDD_A20ST01 + 16)	//Order Declare: Terminal Default
#define IDD_A20ST18			(IDD_A20ST01 + 17)	//Order Declare: Forced Entry
#define IDD_A20ST19			(IDD_A20ST01 + 18)	//Open Cash Drawer on Sign-out
#define IDD_A20ST20			(IDD_A20ST01 + 19)	//Require Sup intervention for manual Reg. login when NOT using Mag card on MSR KB.
#define IDD_A20ST21			(IDD_A20ST01 + 20)	//Auto Sign Out Disabled
#define IDD_A20ST22			(IDD_A20ST01 + 21)	//PIN required for sign-in
#define IDD_A20ST23			(IDD_A20ST01 + 22)	//PIN required for time-in

//Address 7 & 8
#define IDD_A20ST24			(IDD_A20ST01 + 23)	//PIN required for supervisor
#define IDD_A20ST25			(IDD_A20ST01 + 24)	//PIN aging
#define IDD_A20ST26			(IDD_A20ST01 + 25)	//Account Disabled
#define IDD_A20ST27			(IDD_A20ST01 + 26)	//PROG mode allowed
#define IDD_A20ST28			(IDD_A20ST01 + 27)	//Future Use 1
#define IDD_A20ST29			(IDD_A20ST01 + 28)	//Future Use 2
#define IDD_A20ST30			(IDD_A20ST01 + 29)	//REG Mode Prohibited
#define IDD_A20ST31			(IDD_A20ST01 + 30)	//Control string repeat

//Address 9 & 10
#define IDD_A20ST32			(IDD_A20ST01 + 31)	//Startup window repeat


/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_A20_MAXCAS          (IDS_A20 + 0)   /* max. cashier records */
#define IDS_A20_MAXNOCAS        (IDS_A20 + 1)   /* max. records/non-cashier */
#define IDS_A20_CURCAS          (IDS_A20 + 2)   /* current records */
#define IDS_A20_DELCONF         (IDS_A20 + 3)   /* delete confirmation */
#define IDS_A20_FLEX_EMPTY      (IDS_A20 + 4)   /* empty flex memory */
#define IDS_A20_OVERRECORD      (IDS_A20 + 5)   /* over maximum */

#define IDS_A20_CAPTION         (IDS_A20 + 6)
#define IDS_A20_MENT            (IDS_A20 + 7)
#define IDS_A20_ADDCHG          (IDS_A20 + 8)
#define IDS_A20_DEL             (IDS_A20 + 9)
#define IDS_A20_NO              (IDS_A20 +10)
#define IDS_A20_SPIN            (IDS_A20 +11)
#define IDS_A20_NAME            (IDS_A20 +12)
#define IDS_A20_NAMEMAX         (IDS_A20 +13)
#define IDS_A20_CONF            (IDS_A20 +14)
#define IDS_A20_CTRL            (IDS_A20 +15)

#define IDS_A20_STAON1          (IDS_A20 +20)
#define IDS_A20_STAON2          (IDS_A20 +21)
#define IDS_A20_STAON3          (IDS_A20 +22)
#define IDS_A20_STAON4          (IDS_A20 +23)
#define IDS_A20_STAON5          (IDS_A20 +24)
#define IDS_A20_STAON6          (IDS_A20 +25)
#define IDS_A20_STAON7          (IDS_A20 +26)
#define IDS_A20_STAON8          (IDS_A20 +27)
#define IDS_A20_STAON9          (IDS_A20 +28)
#define IDS_A20_STAON10         (IDS_A20 +29)
#define IDS_A20_STAON11         (IDS_A20 +30)
#define IDS_A20_STAON12         (IDS_A20 +31)
#define IDS_A20_STAON13         (IDS_A20 +32)

/*** NCR2172 ***/
#define IDS_A20_STAON14         (IDS_A20 +33)
#define IDS_A20_STAON15         (IDS_A20 +34)
#define IDS_A20_STAON16			(IDS_A20 +35)	// Open Cash Drawer on Sign-In
#define IDC_ORDERDEC_STAON1		(IDS_A20 +36)	// Order Declaration - Use Terminal Defaults
#define IDC_ORDERDEC_STAON2		(IDS_A20 +37)	// Order Declaration - Require Forced Entry
#define IDS_A20_STAON18			(IDS_A20 +38)	// Open Cash Drawer on Sign-Out
#define IDS_A20_STAON19			(IDS_A20 +39)	// Prohibit Manual Sign-In on MSR

#define IDS_A20_STAOFF1         (IDS_A20 +40)
#define IDS_A20_STAOFF2         (IDS_A20 +41)
#define IDS_A20_STAOFF3         (IDS_A20 +42)
#define IDS_A20_STAOFF4         (IDS_A20 +43)
#define IDS_A20_STAOFF5         (IDS_A20 +44)
#define IDS_A20_STAOFF6         (IDS_A20 +45)
#define IDS_A20_STAOFF7         (IDS_A20 +46)
#define IDS_A20_STAOFF8         (IDS_A20 +47)
#define IDS_A20_STAOFF9         (IDS_A20 +48)
#define IDS_A20_STAOFF10        (IDS_A20 +49)
#define IDS_A20_STAOFF11        (IDS_A20 +50)
#define IDS_A20_STAOFF12        (IDS_A20 +51)
#define IDS_A20_STAOFF13        (IDS_A20 +52)

/*** NCR2172 ***/
#define IDS_A20_STAOFF14        (IDS_A20 +53)
#define IDS_A20_STAOFF15        (IDS_A20 +54)
#define IDS_A20_STAOFF16		(IDS_A20 +55)	// Not Open Cash Drawer on Sign-in
#define IDC_ORDERDEC_STAOFF1	(IDS_A20 +56)	// Order Declaration - Not Terminal defaults
#define IDC_ORDERDEC_STAOFF2	(IDS_A20 +57)	// Order Declaration - Not requrie Forced Entry
#define IDS_A20_STAOFF18		(IDS_A20 +58)	// Not Open Cash Drawer on Sign-out
#define IDS_A20_STAOFF19		(IDS_A20 +59)	// Allow manual sign-in on MSR

#define IDS_A20_OPTIONS			(IDS_A20 +60)	// Adding new text on the formerly "set" button to be more descriptive
#define IDS_A20_BIO_FINGER		(IDS_A20 +61)	// Label for fingerprint settings button
#define IDS_A20_SUP_ID_LBL		(IDS_A20 +62)	// Label for Supervisor Number input
#define IDS_A20_CTRLSTR_LBL		(IDS_A20 +63)	// Label for Control String Kickoff
#define	IDS_A20_WINDOW_LBL		(IDS_A20 +64)	// Label for the Window
#define IDS_A20_SUP_ID_RNG		(IDS_A20 +62)	// Label for Supervisor Number input
#define IDS_A20_CTRLSTR_RNG		(IDS_A20 +63)	// Label for Control String Kickoff
#define	IDS_A20_WINDOW_RNG		(IDS_A20 +64)	// Label for the Window
#define IDS_A20_BIO_FINGER_RESET (IDS_A20 +65)	// Label for fingerprint database reset button

/*
* ===========================================================================
*       Number
* ===========================================================================
*/
#define A20_NO_LEN          8   /* input maximum size of cashier No. (99999999)  */
#define A20_MNE_LEN        32   /* cashier name & description buffer length */
#define A20_MAX_LEN        96   /* cashier max No. name buffer length       */
#define A20_NAME_LEN (CAS_NUMBER_OF_CASHIER_NAME)
                                /* cashier name length                      */
#define A20_MEM_LEN       256   /* flex memory empty caution message length */
#define A20_NOFROM_LEN      4
#define A20_NOTO_LEN        4
#define A20_CTIP_LEN        1
#define A20_TEAM_LEN        1
#define A20_TERMNO_LEN      2
#define A20_SUP_ID_LEN		3
#define A20_CTRLSTR_LEN		4
#define A20_WINDOW_LEN		4
#define A20_CODE_LEN        2  /* length of job code                   */
#define A20_CODE_MIN        0L  /* minimum value of Code No.            */
#define A20_CODE_MAX       99L  /* maximum value of Code No.            */

#define A20_TRAIN_CHK (CAS_TRAINING_CASHIER)
                                /* training cashier check flag              */
#define A20_CONF_CHK     0x01   /* delete confirmation check flag           */
#define A20_MDC_ADDR       17   /* mdc address for cashier system           */
#define A20_MDC_BIT_D    0x01   /* mdc bit 0(D) for cashier system          */

#define A20_CAS_MAX         99999999
#define A20_CAS_MIN         1
#define A20_NO_MAX          9999
#define A20_NO_MIN          0
#define A20_CTIP_MAX        3
#define A20_CTIP_MIN        0
#define A20_TEAM_MAX        9
#define A20_TEAM_MIN        0
#define A20_TERMNO_MAX      16
#define A20_TERMNO_MIN      0

#define A20_SPIN_STEP       1

#define A20_8BITS           8   /* bits for 1byte */
#define A20_STACODE1        0   /* operator status code address 1 & 2           */
#define A20_STACODE2        1   /* operator status code address 3 & 4           */
#define A20_STACODE3		2	/* operator status code address 5 & 6           */
#define A20_STACODE4		3	//operator status code address  7 & 8
#define A20_STACODE5		4	//operator status code address  9 & 10
#define A20_STACODE6		5	//operator status code address 11 & 12

#define A20_BITMASK      0x01   /* for bit mask */
#define A20_CHKBIT1      0xEF   /* bit check table for Address 1 & 2 (11101111) */

/*** NCR 2172 ***/
/*#define A20_CHKBIT2      0x3F    bit check table for Address 3 & 4 (00111111) */
#define A20_CHKBIT2      0xFF   /* bit check table for Address 3 & 4 (11111111) */

#define A20_STAOFFSET1      0   /* Address 1 = IDS_A20_STA**0 */
#define A20_STAOFFSET2      7   /* Address 3 = IDS_A20_STA**8 */
#define A20_STAOFFSET3		15	/* Address 5 = IDS_A20_STA**16*/
/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef CASIF FAR*  LPCASIF;

/*
* ===========================================================================
*       External Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Function Prototype Declarations
* ===========================================================================
*/
USHORT  A020InitDlg(HWND, LPUSHORT);
USHORT  A020ReadMax(VOID);
VOID    A020InitList(HWND, LPCASIF);
VOID    A020SetMaxRec(HWND, USHORT);
VOID    A020SetCurRec(HWND, USHORT);
VOID    A020SetData(HWND, LPCASIF, USHORT);
VOID    A020SetCtrlData(HWND);
VOID    A020SetChkbox(HWND, WPARAM, BYTE, BYTE);
VOID    A020SetCtrlList(HWND);
VOID    A020SetListContent(HWND, BYTE, BYTE, WPARAM);
BOOL    A020GetData(HWND, WORD wEditId, LPCASIF, LPUSHORT, USHORT);
VOID    A020GetCtrlData(HWND);
VOID    A020GetChkbox(HWND, WPARAM, BYTE, BYTE);
BOOL    A020ChkMax(HWND, WORD wEditId, ULONG, USHORT, USHORT);
DWORD   A020FindNo(HWND, UINT);
BOOL    A020RefreshList(HWND, LPCASIF, LPUSHORT, WPARAM);
int     A020DispDelConf(HWND, ULONG);
VOID    A020ChkExist(HWND);
BOOL    A020ChkData(HWND, WORD wEditId);
VOID    A020ShowErr(HWND, WPARAM);
VOID    A020FinDlg(HWND, WPARAM);

/* ===== End of File (A020.H) ===== */
