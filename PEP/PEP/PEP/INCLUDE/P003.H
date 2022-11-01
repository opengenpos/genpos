/*
* ---------------------------------------------------------------------------
* Title         :   Function Selection Code Header File (Prog. 3)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P003.H
* Copyright (C) :   1995, AT&T Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Abstract :
			This file contains the various defines used by the P03 dialog
			for assigning FSC codes to keyboard keys.

			When adding new FSC codes to PEP, you will need to do the following:
			  - add new P03_xxxxx   #define to describe the new FSC
			  - add new IDD_P03_xxxxx  #define to provide the toolbox dialog
			    button control ID
			  - add new IDS_P03_xxxxx  #define to provide the display string
			    for the new FSC including if the new FSC has an extended FSC
			  - add new IDS_P03_DSP_xxxxx #define to provide the keyboard dialog
			    display string for the new FSC
			  - update the value for #define P03_FSC_NO
			  - modify the PEP resource files to add new button and string resources
			  - modify the various switch statements in the P003.c source code
			    file to add the necessary code to handle the new FSC if the new FSC
				has an extended FSC.  See P003ToolDlgProc(), P03SetStr() and P03MakeExtFsc()
			  - update the awExtRngTbl[] table in file P003TBL.C with new FSC range

			WARNING: See warning below about NHPOS FSC and PEP FSC codes must agree.
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Dec-22-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Apr-18-95 : 03.00.00 : H.Ishida   : Add Shift Page 
* Jul-25-95 : 03.00.00 : hkato      : R3.0 FVT Comment.
* Jan-23-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-04-98 : 03.03.00 : A.Mitsui   : Add V3.3
* Nov-22-99 :          : K.Yanagida : NCR2172
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

#ifndef __P03__
#define  __P03__

#define P03_DEF_PAGENO          1           /* default page no.             */
#define P03_PAGE_LEN            8           /* length of page number        */
#define P03_PAGE_MAX            9           /* number of max page           */

#define P03_AREA_WIDTH         46           /* string placed area width     */
#define P03_AREA_HIGHT         38           /* string placed area hight     */
#define P03_MICR_AREA_WIDTH    39           /* string placed area width     */
#define P03_MICR_AREA_HIGHT    28           /* string placed area hight     */

#define P03_MICR_XITEM_NO      11           /* cx No of items (16 x 11)     */
#define P03_MICR_YITEM_NO      16           /* cy No of items (16 x 11)     */

#define P03_MICR_ITEM_NO  (P03_MICR_XITEM_NO * P03_MICR_YITEM_NO)
                                            /* Max No. of Keys in Keyboard      */

#define P03_CONV_FIXED_NO       12          /* No of Fixed Key(8x10 / 6x10) */
#define P03_MICR_FIXED_NO       11          /* No of Fixed Key(Micromotion) */
#define P03_WEDGE68_FIXED_NO	25			/* No of Fixed Key(WEDGE 64 key keyboard) */
#define P03_WEDGE78_FIXED_NO	 0			/* No of Fixed Key(WEDGE 78 key keyboard) */
#define P03_TICKET_FIXED_NO		26			/* No of Fixed Key(WEDGE 78 key keyboard) */

#define P03_FIX                 1           /* fixed key code               */

#define P03_IDD_OFS           400           /* convert FSC to WPARAM ID     */
#define P03_DSP_INF_OFS       100           /* IDS (DSP-INF)                */
/*** NCR2172 ***/
/* #define P03_FSC_NO             80 */          /* number of FSC items          */

//#define P03_FSC_NO             87           /* number of FSC items          *//* ### MOD Saratoga (04 --> 87)(051700) */
                                            /* --> added FSmod WICtrans WICmod */

#define P03_SPIN_STEP           1           /* step of spin button          */
#define P03_SPIN_TURBO          0           /* not use spin button turbo    */

#define P03_NORMAL_END          0           /* normal termination code      */
#define P03_MEM_ERROR           1           /* memory allocation error code */

#define P03_EXT_MIN             1           /* minimum data range           */
#define P03_KEYEDPLU_CONV_MAX   68          /* Keyed PLU Extended FSC Max.  */

#define P03_WHITEBRUSH  RGB(255, 255, 255)  /* white brush color code       */

#define P03_CUR_RESET           0           /* cursor reset flag            */
#define P03_CUR_HOLD            1           /* cursor change flag(hold)     */

#define P03_DSP_LEN            16           /* display string length        */
#define P03_INF_LEN            32           /* information string length    */
#define P03_CAP_LEN            32           /* caption length               */
#define P03_DSC_LEN           256           /* description length           */
#define P03_ERR_LEN           128           /* error message length         */
#define P03_PLU_LEN            32           /* display string length(for PLU) */

#define P03_TYPE_MAX            2           /* maximum of PLU Label type    */
#define P03_DEPT_MAX         9999L          /* maximum of DEPT No           */
#define P03_DEPT_LEN            4           /* Max. length of Dept No.      */
#define P03_FSC_LEN             2           /* Max. length of FSC(Dlg for Dept) */

/*
* ===========================================================================
*       Address definition
*           (w/)    with Extended FSC
*           (D)     Drop FSC
*
*	WARNING:  These defines must be in the same order as is specified in
*				the file fsc.h in the BusinessLogic subsystem of NHPOS.
*				These values are used to create the major FSC codes within
*				Prog 3 which are then inserted into the FSC table being built.
*				Since the FSC table is used verbatim by NHPOS, if there is a
*				descrepancy between these codes and the codes used in NHPOS,
*				the terminal key presses will not translate to the correct
*				function within NHPOS in the UIE subsystem.
* ===========================================================================
*/      
#define P03_NOFUNC          0               /* no function key          */
#define P03_KEYEDPLU        1               /* keyed PLU key        (w/)*/
#define P03_KEYEDDPT        2               /* keyed dept. key      (w/)*/
#define P03_PRTMOD          3               /* print modifier key   (w/)*/
#define P03_ADJ             4               /* adjective key        (w/)*/
#define P03_TENDER          5               /* tender key           (w/)*/
#define P03_FC              6               /* foreign currency key (w/)*/
#define P03_TTL             7               /* total key            (w/)*/
#define P03_ADDCHK          8               /* add check key        (w/)*/
#define P03_IDISC           9               /* item discount key    (w/)*/
#define P03_RDISC           10              /* regular discount key (w/)*/
#define P03_TAXMOD          11              /* tax modifier key     (w/)*/
#define P03_AKEY            12              /* sign-in/A/super # key    */
#define P03_BKEY            13              /* B key                    */
#define P03_SERVNO          14              /* server no. key           */
#define P03_NEWCHK          15              /* new check key            */
#define P03_PB              16              /* previous balance key     */
#define P03_GCNO            17              /* guest check no. key      */
#define P03_GCTRANS         18              /* guest check transfer key */
#define P03_NOPERSON        19              /* no. of person key        */
#define P03_TBLNO           20              /* table no. key            */
#define P03_LINENO          21              /* line no. key             */
#define P03_FEED            22              /* feed/release key         */
#define P03_RCTNORCT        23              /* receipt/no receipt key   */
#define P03_TIPSPO          24              /* tips paid out key        */
#define P03_DECTIP          25              /* declared tips key        */
#define P03_NOSALE          26              /* no sale key              */
#define P03_PO              27              /* paid out key             */
#define P03_ROA             28              /* received on account key  */
#define P03_CHGCOMP         29              /* change computation key   */
#define P03_CANCEL          30              /* cancel/abort key         */
#define P03_VOID            31              /* void key                 */
#define P03_EC              32              /* error correct key        */
#define P03_VTRAN           33              /* transaction void key     */
#define P03_QTY             34              /* quantity key             */
#define P03_SHIFT           35              /* menu page shift key      */
#define P03_NOTYPE          36              /* number/type key          */
#define P03_PRTDEM          37              /* print on demand key      */
/* #define P03_ORDERNO         38   */      /* order no. display key    */
#define P03_CHARTIP         39              /* charge tips key          */
#define P03_PLUNO           40              /* PLU number key           */
#define P03_00KEY           41              /* 00 key                   */
#define P03_AC              42              /* action code key          */
#define P03_RECFEED         43              /* receipt feed key         */
#define P03_JOUFEED         44              /* journal feed key         */
#define P03_DECPT           45              /* decimal point key        */
#define P03_SCALE           46              /* scale tare key           */
#define P03_OFFTEND         47              /* Offline Tender           */
#define P03_MONEYDEC        49              /* money declaration NCR2172 */
#define P03_RVSPRI          52              /* reverse print key        */
#define P03_PRECASH         53              /* preset cash tender   (w/)*/
#define P03_DRCTSHFT        54              /* direct shift key     (w/)*/
#define P03_TIMEIN          55              /* time in key              */
#define P03_TIMEOUT         56              /* time out key             */
#define P03_KEYEDCPN        57              /* Keyed Coupon Key         */
#define P03_CPNNO           58              /* Coupon No Key            */
#define P03_UP              59              /* Scroll Up Key            */
#define P03_DOWN            60              /* Scroll Down Key          */
#define P03_RIGHT           61              /* Right Arrow Key          */
#define P03_LEFT            62              /* Left Arrow Key           */
#define P03_WAIT            63              /* Wait Key                 */
#define P03_PAID_REC        64              /* Paid Order Recall Key    */
#define P03_KEYEDSTR        65              /* Keyed Control String Key */
#define P03_STRNO           66              /* Control String No Key    */
#define P03_ALPHA           67              /* Alpha Name Key, R3.0     */
#define P03_LEFTDISPLAY     68              /* Left Display Key, R3.0   */
#define P03_BARTENDER       69              /* Bartender Sign-in Key, R3.1   */
#define P03_SPLIT           70              /* Split Key, R3.1          */
#define P03_SEAT            71              /* Seat # Key, R3.1         */
#define P03_ITEMTRANS       72              /* Item Transfer Key, R3.1  */
#define P03_REPEAT          73              /* Repeat Key, R3.1         */
#define P03_CURSORVOID      74              /* Cursor Void Key, R3.1    */
#define P03_PRICECHECK      75              /* Price Check Key, R3.1    */

#define P03_OPRINT_A        78              /* Operator Interrupt Sign-In A V3.3 */
#define P03_OPRINT_B        79              /* Operator Interrupt Sign-In B V3.3 */
/*** NCR2172 ***/
#define P03_ASK             76              /* ask (for EPT)    NCR2172 */
#define P03_ADJPLS          80              /* adjective price level set NCR2172 */
#define P03_DEPTNO          81              /* department number NCR2172 */
#define P03_UPCEVER         82              /* UPC E-version    NCR2172 */
#define P03_PRICEENT        83              /* price enter key  NCR2172 */
/* ### ADD Saratoga (052700) */
#define P03_FOODSTAMPMOD    84              /* Food Stamp Modifier Key  */
#define P03_WICTRANS        85              /* WIC Transaction Key      */
#define P03_WICMOD          86              /* WIC Modifier Key         */

// ADD SR 143 ////
#define P03_HALO_OVERRIDE	87				// HALO Override Key
#define P03_AT_FOR			88				// @/For Key

// ADD SR 47 ////
#define P03_VALIDATION		89				//Validation Key
#define P03_ALPHANUM        90              // alphanumeric character key uses ASCII character set
#define P03_CLEARFSC        91              // Clear key used when assigning Clear to a key

// Add Release 2.1 ////
#define P03_EDIT_CONDIMENT 92
#define P03_TENT		   93
#define P03_ORDER_DEC	   94
#define P03_ADJ_MOD		   95
#define P03_GIFT_CARD	   96
#define P03_SUPR_INTRVN    97            //Supervisor Intervention button FSC_SUPR_INTRVN
#define P03_DOC_LAUNCH     98            //Document launch button FSC_DOC_LAUNCH
#define P03_OPR_PICKUP     99            //Document launch button FSC_OPR_PICKUP
#define P03_EDIT_COND_TBL  100

//  Need to update following define if more FSCs added.  also awExtRngTbl[P03_FSC_NO]
#define P03_FSC_NO			101             /*number of FSC items, last item + 1*/
#define P03_INF_NO         P03_FSC_NO       /* number of information string */
#define P03_DSP_NO      (P03_FSC_NO + 2)    /* number of diplay string      */


//---------------------------------
//  Special resources
#define P03_MOVING         101              /* moving                   */
#define P03_INDIC          102              /* indicating               */
#define P03_MENUWINDREMOVE  103              /* dismiss popup window for touchscreen */
#define P03_MENUWIND        104              /* popup window for touchscreen JHHJ 2.0 TS*/
#define P03_AUTO_SIGN_OUT	105				//Auto Sign Out

/*
* ===========================================================================
*       Dialog ID definition
* ===========================================================================
*/      
#define IDD_P03_TOOL        20300           /* toolbox button ID        */
#define IDD_P03_EXTMNEMO    20301           /* ext fsc mnemonic ID      */
#define IDD_P03_EXTFSC      20302           /* ext fsc ID               */
#define IDD_P03_EXTRANGE    20304           /* ext fsc range ID         */
#define IDD_P03_SPIN        20305           /* Spin Button ID           */
#define IDD_P03_ENTRY       20306           /* ext fsc entry box        */
#define IDD_P03_PAGE        20307           /* shift page ID            */
#define IDD_P03_EXTEND_MSG  20308           /* ext fcs message          */
#define IDD_P03_PLUENTRY    20309           /* ext fsc entry box(PLU No), Saratoga */
#define IDD_P03_LABEL       20310           /* Label type, Saratoga     */
#define IDD_P03_DEPTENTRY   20311           /* ext fsc entry box(DEPT No), Saratoga */
#define IDD_P03_DEPTIN      20312           /* DEPT NO, Saratoga        */
#define IDD_P03_DEPTSPIN    20313           /* Spin button ID for DEPT  */
#define IDD_P03_CB_KBTYPE	20314			/*Keyboard type Selection*/
#define IDD_P03_KBTYPE		20315			/*label for keyboard type combo box*/
#define IDD_P03_PAGE_RNG	20316


#define IDD_P03_NOFUNC      P03_IDD_OFS
#define IDD_P03_KEYEDPLU    (IDD_P03_NOFUNC + P03_KEYEDPLU)
#define IDD_P03_KEYEDDPT    (IDD_P03_NOFUNC + P03_KEYEDDPT)
#define IDD_P03_PRTMOD      (IDD_P03_NOFUNC + P03_PRTMOD)
#define IDD_P03_ADJ         (IDD_P03_NOFUNC + P03_ADJ)
#define IDD_P03_TENDER      (IDD_P03_NOFUNC + P03_TENDER)
#define IDD_P03_FC          (IDD_P03_NOFUNC + P03_FC)
#define IDD_P03_TTL         (IDD_P03_NOFUNC + P03_TTL)
#define IDD_P03_ADDCHK      (IDD_P03_NOFUNC + P03_ADDCHK)
#define IDD_P03_IDISC       (IDD_P03_NOFUNC + P03_IDISC)
#define IDD_P03_RDISC       (IDD_P03_NOFUNC + P03_RDISC)
#define IDD_P03_TAXMOD      (IDD_P03_NOFUNC + P03_TAXMOD)
#define IDD_P03_AKEY        (IDD_P03_NOFUNC + P03_AKEY)
#define IDD_P03_BKEY        (IDD_P03_NOFUNC + P03_BKEY)
/* #define IDD_P03_SERVNO      (IDD_P03_NOFUNC + P03_SERVNO) */
#define IDD_P03_NEWCHK      (IDD_P03_NOFUNC + P03_NEWCHK)
#define IDD_P03_PB          (IDD_P03_NOFUNC + P03_PB)
#define IDD_P03_GCNO        (IDD_P03_NOFUNC + P03_GCNO)
#define IDD_P03_GCTRANS     (IDD_P03_NOFUNC + P03_GCTRANS)
#define IDD_P03_NOPERSON    (IDD_P03_NOFUNC + P03_NOPERSON)
#define IDD_P03_TBLNO       (IDD_P03_NOFUNC + P03_TBLNO)
#define IDD_P03_LINENO      (IDD_P03_NOFUNC + P03_LINENO)
#define IDD_P03_FEED        (IDD_P03_NOFUNC + P03_FEED)
#define IDD_P03_RCTNORCT    (IDD_P03_NOFUNC + P03_RCTNORCT)
#define IDD_P03_TIPSPO      (IDD_P03_NOFUNC + P03_TIPSPO)
#define IDD_P03_DECTIP      (IDD_P03_NOFUNC + P03_DECTIP)
#define IDD_P03_NOSALE      (IDD_P03_NOFUNC + P03_NOSALE)
#define IDD_P03_PO          (IDD_P03_NOFUNC + P03_PO)
#define IDD_P03_ROA         (IDD_P03_NOFUNC + P03_ROA)
#define IDD_P03_CHGCOMP     (IDD_P03_NOFUNC + P03_CHGCOMP)
#define IDD_P03_CANCEL      (IDD_P03_NOFUNC + P03_CANCEL) 
#define IDD_P03_VOID        (IDD_P03_NOFUNC + P03_VOID)
#define IDD_P03_EC          (IDD_P03_NOFUNC + P03_EC)
#define IDD_P03_VTRAN       (IDD_P03_NOFUNC + P03_VTRAN)
#define IDD_P03_QTY         (IDD_P03_NOFUNC + P03_QTY)
#define IDD_P03_SHIFT       (IDD_P03_NOFUNC + P03_SHIFT)
#define IDD_P03_NOTYPE      (IDD_P03_NOFUNC + P03_NOTYPE)
#define IDD_P03_PRTDEM      (IDD_P03_NOFUNC + P03_PRTDEM)
/* #define IDD_P03_ORDERNO     (IDD_P03_NOFUNC + P03_ORDERNO) */
#define IDD_P03_CHARTIP     (IDD_P03_NOFUNC + P03_CHARTIP)
#define IDD_P03_PLUNO       (IDD_P03_NOFUNC + P03_PLUNO)
#define IDD_P03_00KEY       (IDD_P03_NOFUNC + P03_00KEY)
#define IDD_P03_AC          (IDD_P03_NOFUNC + P03_AC)
#define IDD_P03_RECFEED     (IDD_P03_NOFUNC + P03_RECFEED)
#define IDD_P03_JOUFEED     (IDD_P03_NOFUNC + P03_JOUFEED)
#define IDD_P03_DECPT       (IDD_P03_NOFUNC + P03_DECPT)
#define IDD_P03_SCALE       (IDD_P03_NOFUNC + P03_SCALE)
#define IDD_P03_RVSPRI      (IDD_P03_NOFUNC + P03_RVSPRI)
#define IDD_P03_PRECASH     (IDD_P03_NOFUNC + P03_PRECASH)
#define IDD_P03_DRCTSHFT    (IDD_P03_NOFUNC + P03_DRCTSHFT)
#define IDD_P03_TIMEIN      (IDD_P03_NOFUNC + P03_TIMEIN)
#define IDD_P03_TIMEOUT     (IDD_P03_NOFUNC + P03_TIMEOUT)
#define IDD_P03_OFFTEND     (IDD_P03_NOFUNC + P03_OFFTEND)
/*** NCR2172 ***/
#define IDD_P03_MONEYDEC    (IDD_P03_NOFUNC + P03_MONEYDEC)
#define IDD_P03_KEYEDCPN    (IDD_P03_NOFUNC + P03_KEYEDCPN)
#define IDD_P03_CPNNO       (IDD_P03_NOFUNC + P03_CPNNO)
#define IDD_P03_UP          (IDD_P03_NOFUNC + P03_UP)
#define IDD_P03_DOWN        (IDD_P03_NOFUNC + P03_DOWN)
#define IDD_P03_RIGHT       (IDD_P03_NOFUNC + P03_RIGHT)
#define IDD_P03_LEFT        (IDD_P03_NOFUNC + P03_LEFT)
#define IDD_P03_WAIT        (IDD_P03_NOFUNC + P03_WAIT)
#define IDD_P03_PAID_REC    (IDD_P03_NOFUNC + P03_PAID_REC)
#define IDD_P03_KEYEDSTR    (IDD_P03_NOFUNC + P03_KEYEDSTR)
#define IDD_P03_STRNO       (IDD_P03_NOFUNC + P03_STRNO)
#define IDD_P03_ALPHA       (IDD_P03_NOFUNC + P03_ALPHA)
#define IDD_P03_LEFTDISPLAY (IDD_P03_NOFUNC + P03_LEFTDISPLAY)
#define IDD_P03_BARTENDER   (IDD_P03_NOFUNC + P03_BARTENDER)
#define IDD_P03_SPLIT       (IDD_P03_NOFUNC + P03_SPLIT)
#define IDD_P03_SEAT        (IDD_P03_NOFUNC + P03_SEAT)
#define IDD_P03_ITEMTRANS   (IDD_P03_NOFUNC + P03_ITEMTRANS)
#define IDD_P03_REPEAT      (IDD_P03_NOFUNC + P03_REPEAT)
#define IDD_P03_CURSORVOID  (IDD_P03_NOFUNC + P03_CURSORVOID)
#define IDD_P03_PRICECHECK  (IDD_P03_NOFUNC + P03_PRICECHECK)

#define IDD_P03_OPRINT_A    (IDD_P03_NOFUNC + P03_OPRINT_A)
#define IDD_P03_OPRINT_B    (IDD_P03_NOFUNC + P03_OPRINT_B)

/*** NCR2172 ***/
#define IDD_P03_ASK         (IDD_P03_NOFUNC + P03_ASK)
#define IDD_P03_ADJPLS      (IDD_P03_NOFUNC + P03_ADJPLS)
#define IDD_P03_DEPTNO      (IDD_P03_NOFUNC + P03_DEPTNO)
#define IDD_P03_UPCEVER     (IDD_P03_NOFUNC + P03_UPCEVER)
#define IDD_P03_PRICEENT    (IDD_P03_NOFUNC + P03_PRICEENT)
/* ### ADD Saratoga (052700) */
#define IDD_P03_FOODSTAMPMOD (IDD_P03_NOFUNC + P03_FOODSTAMPMOD)
#define IDD_P03_WICTRANS     (IDD_P03_NOFUNC + P03_WICTRANS)
#define IDD_P03_WICMOD       (IDD_P03_NOFUNC + P03_WICMOD)


// ADD SR 143 //////////
#define IDD_P03_HALO_OVERRIDE	(IDD_P03_NOFUNC + P03_HALO_OVERRIDE)
#define	IDD_P03_AT_FOR			(IDD_P03_NOFUNC + P03_AT_FOR)
////////////////////////

// ADD SR 47 ///////////
#define IDD_P03_VALIDATION	(IDD_P03_NOFUNC + P03_VALIDATION)

#define IDD_P03_ALPHANUM     (IDD_P03_NOFUNC + P03_ALPHANUM)
#define IDD_P03_CLEARFSC     (IDD_P03_NOFUNC + P03_CLEARFSC)

#define IDD_P03_EDIT_CONDIMENT (IDD_P03_NOFUNC + P03_EDIT_CONDIMENT)
#define IDD_P03_EDIT_COND_TBL (IDD_P03_NOFUNC + P03_EDIT_COND_TBL)

#define IDD_P03_TENT (IDD_P03_NOFUNC + P03_TENT)

#define IDD_P03_ORDER_DEC (IDD_P03_NOFUNC + P03_ORDER_DEC)

#define IDD_P03_ADJ_MOD		(IDD_P03_NOFUNC + P03_ADJ_MOD)

#define IDD_P03_GIFT_CARD     (IDD_P03_NOFUNC + P03_GIFT_CARD)
#define IDD_P03_SUPR_INTRVN   (IDD_P03_NOFUNC + P03_SUPR_INTRVN)  // FSC_SUPR_INTRVN
#define IDD_P03_DOC_LAUNCH    (IDD_P03_NOFUNC + P03_DOC_LAUNCH)   // FSC_DOC_LAUNCH
#define IDD_P03_OPR_PICKUP    (IDD_P03_NOFUNC + P03_OPR_PICKUP)   // FSC_OPR_PICKUP
#define IDD_P03_AUTO_SIGN_OUT (IDD_P03_NOFUNC + P03_AUTO_SIGN_OUT) //Auto Sign Out

// When new keys are added, IDD_P03_LASTKEY should always point to last key
#define IDD_P03_LASTKEY      (IDD_P03_EDIT_COND_TBL)     // Used in loops to show last key available see WM_SETFONT in P003ToolDlgProc ()

////////////////////////

#define IDD_P03_MENUWIND     (IDD_P03_NOFUNC + P03_MENUWIND)
#define IDD_P03_MENUWINDREMOVE  (IDD_P03_NOFUNC + P03_MENUWINDREMOVE)

#define IDD_P03_KEY_00      (IDD_P03_NOFUNC + 60)
#define IDD_P03_KEY_10      (IDD_P03_KEY_00 + 1)
#define IDD_P03_KEY_20      (IDD_P03_KEY_00 + 2)
#define IDD_P03_KEY_30      (IDD_P03_KEY_00 + 3)
#define IDD_P03_KEY_40      (IDD_P03_KEY_00 + 4)
#define IDD_P03_KEY_50      (IDD_P03_KEY_00 + 5)
#define IDD_P03_KEY_60      (IDD_P03_KEY_00 + 6)
#define IDD_P03_KEY_70      (IDD_P03_KEY_00 + 7)
#define IDD_P03_KEY_80      (IDD_P03_KEY_00 + 8)
#define IDD_P03_KEY_90      (IDD_P03_KEY_00 + 9)
#define IDD_P03_KEY_A0      (IDD_P03_KEY_00 + 10)
#define IDD_P03_KEY_01      (IDD_P03_KEY_00 + 11)
#define IDD_P03_KEY_11      (IDD_P03_KEY_00 + 12)
#define IDD_P03_KEY_21      (IDD_P03_KEY_00 + 13)
#define IDD_P03_KEY_31      (IDD_P03_KEY_00 + 14)
#define IDD_P03_KEY_41      (IDD_P03_KEY_00 + 15)
#define IDD_P03_KEY_51      (IDD_P03_KEY_00 + 16)
#define IDD_P03_KEY_61      (IDD_P03_KEY_00 + 17)
#define IDD_P03_KEY_71      (IDD_P03_KEY_00 + 18)
#define IDD_P03_KEY_81      (IDD_P03_KEY_00 + 19)
#define IDD_P03_KEY_91      (IDD_P03_KEY_00 + 20)
#define IDD_P03_KEY_A1      (IDD_P03_KEY_00 + 21)
#define IDD_P03_KEY_02      (IDD_P03_KEY_00 + 22)
#define IDD_P03_KEY_12      (IDD_P03_KEY_00 + 23)
#define IDD_P03_KEY_22      (IDD_P03_KEY_00 + 24)
#define IDD_P03_KEY_32      (IDD_P03_KEY_00 + 25)
#define IDD_P03_KEY_42      (IDD_P03_KEY_00 + 26)
#define IDD_P03_KEY_52      (IDD_P03_KEY_00 + 27)
#define IDD_P03_KEY_62      (IDD_P03_KEY_00 + 28)
#define IDD_P03_KEY_72      (IDD_P03_KEY_00 + 29)
#define IDD_P03_KEY_82      (IDD_P03_KEY_00 + 30)
#define IDD_P03_KEY_92      (IDD_P03_KEY_00 + 31)
#define IDD_P03_KEY_A2      (IDD_P03_KEY_00 + 32)
#define IDD_P03_KEY_03      (IDD_P03_KEY_00 + 33)
#define IDD_P03_KEY_13      (IDD_P03_KEY_00 + 34)
#define IDD_P03_KEY_23      (IDD_P03_KEY_00 + 35)
#define IDD_P03_KEY_33      (IDD_P03_KEY_00 + 36)
#define IDD_P03_KEY_43      (IDD_P03_KEY_00 + 37)
#define IDD_P03_KEY_53      (IDD_P03_KEY_00 + 38)
#define IDD_P03_KEY_63      (IDD_P03_KEY_00 + 39)
#define IDD_P03_KEY_73      (IDD_P03_KEY_00 + 40)
#define IDD_P03_KEY_83      (IDD_P03_KEY_00 + 41)
#define IDD_P03_KEY_93      (IDD_P03_KEY_00 + 42)
#define IDD_P03_KEY_A3      (IDD_P03_KEY_00 + 43)
#define IDD_P03_KEY_04      (IDD_P03_KEY_00 + 44)
#define IDD_P03_KEY_14      (IDD_P03_KEY_00 + 45)
#define IDD_P03_KEY_24      (IDD_P03_KEY_00 + 46)
#define IDD_P03_KEY_34      (IDD_P03_KEY_00 + 47)
#define IDD_P03_KEY_44      (IDD_P03_KEY_00 + 48)
#define IDD_P03_KEY_54      (IDD_P03_KEY_00 + 49)
#define IDD_P03_KEY_64      (IDD_P03_KEY_00 + 50)
#define IDD_P03_KEY_74      (IDD_P03_KEY_00 + 51)
#define IDD_P03_KEY_84      (IDD_P03_KEY_00 + 52)
#define IDD_P03_KEY_94      (IDD_P03_KEY_00 + 53)
#define IDD_P03_KEY_A4      (IDD_P03_KEY_00 + 54)
#define IDD_P03_KEY_05      (IDD_P03_KEY_00 + 55)
#define IDD_P03_KEY_15      (IDD_P03_KEY_00 + 56)
#define IDD_P03_KEY_25      (IDD_P03_KEY_00 + 57)
#define IDD_P03_KEY_35      (IDD_P03_KEY_00 + 58)
#define IDD_P03_KEY_45      (IDD_P03_KEY_00 + 59)
#define IDD_P03_KEY_55      (IDD_P03_KEY_00 + 60)
#define IDD_P03_KEY_65      (IDD_P03_KEY_00 + 61)
#define IDD_P03_KEY_75      (IDD_P03_KEY_00 + 62)
#define IDD_P03_KEY_85      (IDD_P03_KEY_00 + 63)
#define IDD_P03_KEY_95      (IDD_P03_KEY_00 + 64)
#define IDD_P03_KEY_A5      (IDD_P03_KEY_00 + 65)
#define IDD_P03_KEY_06      (IDD_P03_KEY_00 + 66)
#define IDD_P03_KEY_16      (IDD_P03_KEY_00 + 67)
#define IDD_P03_KEY_26      (IDD_P03_KEY_00 + 68)
#define IDD_P03_KEY_36      (IDD_P03_KEY_00 + 69)
#define IDD_P03_KEY_46      (IDD_P03_KEY_00 + 70)
#define IDD_P03_KEY_56      (IDD_P03_KEY_00 + 71)
#define IDD_P03_KEY_66      (IDD_P03_KEY_00 + 72)
#define IDD_P03_KEY_76      (IDD_P03_KEY_00 + 73)
#define IDD_P03_KEY_86      (IDD_P03_KEY_00 + 74)
#define IDD_P03_KEY_96      (IDD_P03_KEY_00 + 75)
#define IDD_P03_KEY_A6      (IDD_P03_KEY_00 + 76)
#define IDD_P03_KEY_07      (IDD_P03_KEY_00 + 77)
#define IDD_P03_KEY_17      (IDD_P03_KEY_00 + 78)
#define IDD_P03_KEY_27      (IDD_P03_KEY_00 + 79)
#define IDD_P03_KEY_37      (IDD_P03_KEY_00 + 80)
#define IDD_P03_KEY_47      (IDD_P03_KEY_00 + 81)
#define IDD_P03_KEY_57      (IDD_P03_KEY_00 + 82)
#define IDD_P03_KEY_67      (IDD_P03_KEY_00 + 83)
#define IDD_P03_KEY_77      (IDD_P03_KEY_00 + 84)
#define IDD_P03_KEY_87      (IDD_P03_KEY_00 + 85)
#define IDD_P03_KEY_97      (IDD_P03_KEY_00 + 86)
#define IDD_P03_KEY_A7      (IDD_P03_KEY_00 + 87)
#define IDD_P03_KEY_08      (IDD_P03_KEY_00 + 88)
#define IDD_P03_KEY_18      (IDD_P03_KEY_00 + 89)
#define IDD_P03_KEY_28      (IDD_P03_KEY_00 + 90)
#define IDD_P03_KEY_38      (IDD_P03_KEY_00 + 91)
#define IDD_P03_KEY_48      (IDD_P03_KEY_00 + 92)
#define IDD_P03_KEY_58      (IDD_P03_KEY_00 + 93)
#define IDD_P03_KEY_68      (IDD_P03_KEY_00 + 94)
#define IDD_P03_KEY_78      (IDD_P03_KEY_00 + 95)
#define IDD_P03_KEY_88      (IDD_P03_KEY_00 + 96)
#define IDD_P03_KEY_98      (IDD_P03_KEY_00 + 97)
#define IDD_P03_KEY_A8      (IDD_P03_KEY_00 + 98)
#define IDD_P03_KEY_09      (IDD_P03_KEY_00 + 99)
#define IDD_P03_KEY_19      (IDD_P03_KEY_00 + 100)
#define IDD_P03_KEY_29      (IDD_P03_KEY_00 + 101)
#define IDD_P03_KEY_39      (IDD_P03_KEY_00 + 102)
#define IDD_P03_KEY_49      (IDD_P03_KEY_00 + 103)
#define IDD_P03_KEY_59      (IDD_P03_KEY_00 + 104)
#define IDD_P03_KEY_69      (IDD_P03_KEY_00 + 105)
#define IDD_P03_KEY_79      (IDD_P03_KEY_00 + 106)
#define IDD_P03_KEY_89      (IDD_P03_KEY_00 + 107)
#define IDD_P03_KEY_99      (IDD_P03_KEY_00 + 108)
#define IDD_P03_KEY_A9      (IDD_P03_KEY_00 + 109)
#define IDD_P03_KEY_0A      (IDD_P03_KEY_00 + 110)
#define IDD_P03_KEY_1A      (IDD_P03_KEY_00 + 111)
#define IDD_P03_KEY_2A      (IDD_P03_KEY_00 + 112)
#define IDD_P03_KEY_3A      (IDD_P03_KEY_00 + 113)
#define IDD_P03_KEY_4A      (IDD_P03_KEY_00 + 114)
#define IDD_P03_KEY_5A      (IDD_P03_KEY_00 + 115)
#define IDD_P03_KEY_6A      (IDD_P03_KEY_00 + 116)
#define IDD_P03_KEY_7A      (IDD_P03_KEY_00 + 117)
#define IDD_P03_KEY_8A      (IDD_P03_KEY_00 + 118)
#define IDD_P03_KEY_9A      (IDD_P03_KEY_00 + 119)
#define IDD_P03_KEY_AA      (IDD_P03_KEY_00 + 120)
#define IDD_P03_KEY_0B      (IDD_P03_KEY_00 + 121)
#define IDD_P03_KEY_1B      (IDD_P03_KEY_00 + 122)
#define IDD_P03_KEY_2B      (IDD_P03_KEY_00 + 123)
#define IDD_P03_KEY_3B      (IDD_P03_KEY_00 + 124)
#define IDD_P03_KEY_4B      (IDD_P03_KEY_00 + 125)
#define IDD_P03_KEY_5B      (IDD_P03_KEY_00 + 126)
#define IDD_P03_KEY_6B      (IDD_P03_KEY_00 + 127)
#define IDD_P03_KEY_7B      (IDD_P03_KEY_00 + 128)
#define IDD_P03_KEY_8B      (IDD_P03_KEY_00 + 129)
#define IDD_P03_KEY_9B      (IDD_P03_KEY_00 + 130)
#define IDD_P03_KEY_AB      (IDD_P03_KEY_00 + 131)
#define IDD_P03_KEY_0C      (IDD_P03_KEY_00 + 132)
#define IDD_P03_KEY_1C      (IDD_P03_KEY_00 + 133)
#define IDD_P03_KEY_2C      (IDD_P03_KEY_00 + 134)
#define IDD_P03_KEY_3C      (IDD_P03_KEY_00 + 135)
#define IDD_P03_KEY_4C      (IDD_P03_KEY_00 + 136)
#define IDD_P03_KEY_5C      (IDD_P03_KEY_00 + 137)
#define IDD_P03_KEY_6C      (IDD_P03_KEY_00 + 138)
#define IDD_P03_KEY_7C      (IDD_P03_KEY_00 + 139)
#define IDD_P03_KEY_8C      (IDD_P03_KEY_00 + 140)
#define IDD_P03_KEY_9C      (IDD_P03_KEY_00 + 141)
#define IDD_P03_KEY_AC      (IDD_P03_KEY_00 + 142)
#define IDD_P03_KEY_0D      (IDD_P03_KEY_00 + 143)
#define IDD_P03_KEY_1D      (IDD_P03_KEY_00 + 144)
#define IDD_P03_KEY_2D      (IDD_P03_KEY_00 + 145)
#define IDD_P03_KEY_3D      (IDD_P03_KEY_00 + 146)
#define IDD_P03_KEY_4D      (IDD_P03_KEY_00 + 147)
#define IDD_P03_KEY_5D      (IDD_P03_KEY_00 + 148)
#define IDD_P03_KEY_6D      (IDD_P03_KEY_00 + 149)
#define IDD_P03_KEY_7D      (IDD_P03_KEY_00 + 150)
#define IDD_P03_KEY_8D      (IDD_P03_KEY_00 + 151)
#define IDD_P03_KEY_9D      (IDD_P03_KEY_00 + 152)
#define IDD_P03_KEY_AD      (IDD_P03_KEY_00 + 153)
#define IDD_P03_KEY_0E      (IDD_P03_KEY_00 + 154)
#define IDD_P03_KEY_1E      (IDD_P03_KEY_00 + 155)
#define IDD_P03_KEY_2E      (IDD_P03_KEY_00 + 156)
#define IDD_P03_KEY_3E      (IDD_P03_KEY_00 + 157)
#define IDD_P03_KEY_4E      (IDD_P03_KEY_00 + 158)
#define IDD_P03_KEY_5E      (IDD_P03_KEY_00 + 159)
#define IDD_P03_KEY_6E      (IDD_P03_KEY_00 + 160)
#define IDD_P03_KEY_7E      (IDD_P03_KEY_00 + 161)
#define IDD_P03_KEY_8E      (IDD_P03_KEY_00 + 162)
#define IDD_P03_KEY_9E      (IDD_P03_KEY_00 + 163)
#define IDD_P03_KEY_AE      (IDD_P03_KEY_00 + 164)
#define IDD_P03_KEY_0F      (IDD_P03_KEY_00 + 165)
#define IDD_P03_KEY_1F      (IDD_P03_KEY_00 + 166)
#define IDD_P03_KEY_2F      (IDD_P03_KEY_00 + 167)
#define IDD_P03_KEY_3F      (IDD_P03_KEY_00 + 168)
#define IDD_P03_KEY_4F      (IDD_P03_KEY_00 + 169)
#define IDD_P03_KEY_5F      (IDD_P03_KEY_00 + 170)
#define IDD_P03_KEY_6F      (IDD_P03_KEY_00 + 171)
#define IDD_P03_KEY_7F      (IDD_P03_KEY_00 + 172)
#define IDD_P03_KEY_8F      (IDD_P03_KEY_00 + 173)
#define IDD_P03_KEY_9F      (IDD_P03_KEY_00 + 174)
#define IDD_P03_KEY_AF      (IDD_P03_KEY_00 + 175)
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#define IDD_P03_DEPTIN      (IDD_P03_KEY_00 + 176)           /* DEPT NO, Saratoga        */
#define IDD_P03_DEPTSPIN    (IDD_P03_KEY_00 + 177)           /* Spin button ID for DEPT  */
#endif

// Special FSC used by touchscreen to simulate keyboard keys

#define IDC_P03_RCLICK		(IDD_P03_KEY_00 + 178)
#define IDD_P03_CLEAR		(IDD_P03_KEY_00 + 179)		//4-21-03 RPH Clear key

//ROOM FOR ADDITIONAL KEYS
#define IDD_P03_DATA_RANGE	(IDD_P03_KEY_00 + 300)
#define IDD_P03_LABEL_TYPE	(IDD_P03_KEY_00 + 301)
#define IDD_P03_DATA_RANGE2	(IDD_P03_KEY_00 + 302)
#define IDD_P03_DEPT_NUM	(IDD_P03_KEY_00 + 303)

/*
#define IDD_P03_LAST        (IDC_P03_RCLICK)
*/
/*
* ===========================================================================
*       Resource ID definition
* ===========================================================================
*/
#define IDS_P03_CUR_HOLD        (IDS_P03)
#define IDS_P03_CONVPLUKEY      (IDS_P03 + 1)
#define IDS_P03_CONVSERKEY      (IDS_P03 + 2)

#define IDS_P03_INFO            (IDS_P03 + 16)
#define IDS_P03_EXTEND          (IDS_P03 + 32)
#define IDS_P03_PLU_NO          (IDS_P03 + 33)
#define IDS_P03_DETAIL_CAP      (IDS_P03 + 48)
#define IDS_P03_EXTEND_MSG      (IDS_P03 + 49)
#define IDS_P03_PLU_MSG         (IDS_P03 + 50)

#define IDS_P03_TYPE_NORMAL     (IDS_P03 + 51)  /* Saratoga */
#define IDS_P03_TYPE_EVERSION   (IDS_P03 + 52)  /* Saratoga */
#define IDS_P03_LABEL           (IDS_P03 + 53)  /* Saratoga */
#define IDS_P03_LABEL_MSG       (IDS_P03 + 54)  /* Saratoga */
#define IDS_P03_DEPT_NO         (IDS_P03 + 55)  /* ADD Saratoga */

#define IDS_P03_INF_NOFUNC      (IDS_P03 + 80)
#define IDS_P03_INF_KEYEDPLU    (IDS_P03_INF_NOFUNC + P03_KEYEDPLU)
#define IDS_P03_INF_KEYEDDPT    (IDS_P03_INF_NOFUNC + P03_KEYEDDPT)
#define IDS_P03_INF_PRTMOD      (IDS_P03_INF_NOFUNC + P03_PRTMOD)
#define IDS_P03_INF_ADJ         (IDS_P03_INF_NOFUNC + P03_ADJ)
#define IDS_P03_INF_TENDER      (IDS_P03_INF_NOFUNC + P03_TENDER)
#define IDS_P03_INF_FC          (IDS_P03_INF_NOFUNC + P03_FC)
#define IDS_P03_INF_TTL         (IDS_P03_INF_NOFUNC + P03_TTL)
#define IDS_P03_INF_ADDCHK      (IDS_P03_INF_NOFUNC + P03_ADDCHK)
#define IDS_P03_INF_IDISC       (IDS_P03_INF_NOFUNC + P03_IDISC)
#define IDS_P03_INF_RDISC       (IDS_P03_INF_NOFUNC + P03_RDISC)
#define IDS_P03_INF_TAXMOD      (IDS_P03_INF_NOFUNC + P03_TAXMOD)
#define IDS_P03_INF_AKEY        (IDS_P03_INF_NOFUNC + P03_AKEY)
#define IDS_P03_INF_BKEY        (IDS_P03_INF_NOFUNC + P03_BKEY)
/* #define IDS_P03_INF_SERVNO      (IDS_P03_INF_NOFUNC + P03_SERVNO) */
#define IDS_P03_INF_NEWCHK      (IDS_P03_INF_NOFUNC + P03_NEWCHK)
#define IDS_P03_INF_PB          (IDS_P03_INF_NOFUNC + P03_PB)
#define IDS_P03_INF_GCNO        (IDS_P03_INF_NOFUNC + P03_GCNO)
#define IDS_P03_INF_GCTRANS     (IDS_P03_INF_NOFUNC + P03_GCTRANS)
#define IDS_P03_INF_NOPERSON    (IDS_P03_INF_NOFUNC + P03_NOPERSON)
#define IDS_P03_INF_TBLNO       (IDS_P03_INF_NOFUNC + P03_TBLNO)
#define IDS_P03_INF_LINENO      (IDS_P03_INF_NOFUNC + P03_LINENO)
#define IDS_P03_INF_FEED        (IDS_P03_INF_NOFUNC + P03_FEED)
#define IDS_P03_INF_RCTNORCT    (IDS_P03_INF_NOFUNC + P03_RCTNORCT)
#define IDS_P03_INF_TIPSPO      (IDS_P03_INF_NOFUNC + P03_TIPSPO)
#define IDS_P03_INF_DECTIP      (IDS_P03_INF_NOFUNC + P03_DECTIP)
#define IDS_P03_INF_NOSALE      (IDS_P03_INF_NOFUNC + P03_NOSALE)
#define IDS_P03_INF_PO          (IDS_P03_INF_NOFUNC + P03_PO)
#define IDS_P03_INF_ROA         (IDS_P03_INF_NOFUNC + P03_ROA)
#define IDS_P03_INF_CHGCOMP     (IDS_P03_INF_NOFUNC + P03_CHGCOMP)
#define IDS_P03_INF_CANCEL      (IDS_P03_INF_NOFUNC + P03_CANCEL)
#define IDS_P03_INF_VOID        (IDS_P03_INF_NOFUNC + P03_VOID)
#define IDS_P03_INF_EC          (IDS_P03_INF_NOFUNC + P03_EC)
#define IDS_P03_INF_VTRAN       (IDS_P03_INF_NOFUNC + P03_VTRAN)
#define IDS_P03_INF_QTY         (IDS_P03_INF_NOFUNC + P03_QTY)
#define IDS_P03_INF_SHIFT       (IDS_P03_INF_NOFUNC + P03_SHIFT)
#define IDS_P03_INF_NOTYPE      (IDS_P03_INF_NOFUNC + P03_NOTYPE)
#define IDS_P03_INF_PRTDEM      (IDS_P03_INF_NOFUNC + P03_PRTDEM)
/* #define IDS_P03_INF_ORDERNO     (IDS_P03_INF_NOFUNC + P03_ORDERNO) */
#define IDS_P03_INF_CHARTIP     (IDS_P03_INF_NOFUNC + P03_CHARTIP)
#define IDS_P03_INF_PLUNO       (IDS_P03_INF_NOFUNC + P03_PLUNO)
#define IDS_P03_INF_00KEY       (IDS_P03_INF_NOFUNC + P03_00KEY)
#define IDS_P03_INF_AC          (IDS_P03_INF_NOFUNC + P03_AC)
#define IDS_P03_INF_RECFEED     (IDS_P03_INF_NOFUNC + P03_RECFEED)
#define IDS_P03_INF_JOUFEED     (IDS_P03_INF_NOFUNC + P03_JOUFEED)
#define IDS_P03_INF_DECPT       (IDS_P03_INF_NOFUNC + P03_DECPT)
#define IDS_P03_INF_SCALE       (IDS_P03_INF_NOFUNC + P03_SCALE)
#define IDS_P03_INF_OFFTEND     (IDS_P03_INF_NOFUNC + P03_OFFTEND)
/*** NCR2172 ***/
#define IDS_P03_INF_MONEYDEC    (IDS_P03_INF_NOFUNC + P03_MONEYDEC)
#define IDS_P03_INF_RVSPRI      (IDS_P03_INF_NOFUNC + P03_RVSPRI)
#define IDS_P03_INF_PRECASH     (IDS_P03_INF_NOFUNC + P03_PRECASH)
#define IDS_P03_INF_DRCTSHFT    (IDS_P03_INF_NOFUNC + P03_DRCTSHFT)
#define IDS_P03_INF_TIMEIN      (IDS_P03_INF_NOFUNC + P03_TIMEIN)
#define IDS_P03_INF_TIMEOUT     (IDS_P03_INF_NOFUNC + P03_TIMEOUT)
#define IDS_P03_INF_KEYEDCPN    (IDS_P03_INF_NOFUNC + P03_KEYEDCPN)
#define IDS_P03_INF_CPNNO       (IDS_P03_INF_NOFUNC + P03_CPNNO)
#define IDS_P03_INF_UP          (IDS_P03_INF_NOFUNC + P03_UP)
#define IDS_P03_INF_DOWN        (IDS_P03_INF_NOFUNC + P03_DOWN)
#define IDS_P03_INF_RIGHT       (IDS_P03_INF_NOFUNC + P03_RIGHT)
#define IDS_P03_INF_LEFT        (IDS_P03_INF_NOFUNC + P03_LEFT)
#define IDS_P03_INF_WAIT        (IDS_P03_INF_NOFUNC + P03_WAIT)
#define IDS_P03_INF_PAID_REC    (IDS_P03_INF_NOFUNC + P03_PAID_REC)
#define IDS_P03_INF_KEYEDSTR    (IDS_P03_INF_NOFUNC + P03_KEYEDSTR)
#define IDS_P03_INF_STRNO       (IDS_P03_INF_NOFUNC + P03_STRNO)
#define IDS_P03_INF_ALPHA       (IDS_P03_INF_NOFUNC + P03_ALPHA)
#define IDS_P03_INF_LEFTDISPLAY (IDS_P03_INF_NOFUNC + P03_LEFTDISPLAY)
#define IDS_P03_INF_BARTENDER   (IDS_P03_INF_NOFUNC + P03_BARTENDER)
#define IDS_P03_INF_SPLIT       (IDS_P03_INF_NOFUNC + P03_SPLIT)
#define IDS_P03_INF_SEAT        (IDS_P03_INF_NOFUNC + P03_SEAT)
#define IDS_P03_INF_ITEMTRANS   (IDS_P03_INF_NOFUNC + P03_ITEMTRANS)
#define IDS_P03_INF_REPEAT      (IDS_P03_INF_NOFUNC + P03_REPEAT)
#define IDS_P03_INF_CURSORVOID  (IDS_P03_INF_NOFUNC + P03_CURSORVOID)
#define IDS_P03_INF_PRICECHECK  (IDS_P03_INF_NOFUNC + P03_PRICECHECK)

#define IDS_P03_INF_OPRINT_A    (IDS_P03_INF_NOFUNC + P03_OPRINT_A)
#define IDS_P03_INF_OPRINT_B    (IDS_P03_INF_NOFUNC + P03_OPRINT_B)

/*** NCR2172 ***/
#define IDS_P03_INF_ASK         (IDS_P03_INF_NOFUNC + P03_ASK)
#define IDS_P03_INF_ADJPLS      (IDS_P03_INF_NOFUNC + P03_ADJPLS)
#define IDS_P03_INF_DEPTNO      (IDS_P03_INF_NOFUNC + P03_DEPTNO)
#define IDS_P03_INF_UPCEVER     (IDS_P03_INF_NOFUNC + P03_UPCEVER)
#define IDS_P03_INF_PRICEENT    (IDS_P03_INF_NOFUNC + P03_PRICEENT)
/* ### ADD Saratoga (052700) */
#define IDS_P03_INF_FOODSTAMPMOD (IDS_P03_INF_NOFUNC + P03_FOODSTAMPMOD)
#define IDS_P03_INF_WICTRANS    (IDS_P03_INF_NOFUNC + P03_WICTRANS    )
#define IDS_P03_INF_WICMOD      (IDS_P03_INF_NOFUNC + P03_WICMOD      )

// ADD SR 143 ////////////
#define IDS_P03_INF_HALO_OVERRIDE	(IDS_P03_INF_NOFUNC + P03_HALO_OVERRIDE)
#define IDS_P03_INF_AT_FOR			(IDS_P03_INF_NOFUNC + P03_AT_FOR)
//////////////////////////

// ADD SR 47 /////////////
#define IDS_P03_INF_VALIDATION		(IDS_P03_INF_NOFUNC + P03_VALIDATION)

#define IDS_P03_INF_ALPHANUM		(IDS_P03_INF_NOFUNC + P03_ALPHANUM)
#define IDS_P03_INF_CLEARFSC   		(IDS_P03_INF_NOFUNC + P03_CLEARFSC)

#define IDS_P03_INF_EDIT_CONDIMENT		(IDS_P03_INF_NOFUNC + P03_EDIT_CONDIMENT)
#define IDS_P03_INF_EDIT_COND_TBL	(IDS_P03_INF_NOFUNC + P03_EDIT_COND_TBL)
#define IDS_P03_INF_TENT			(IDS_P03_INF_NOFUNC + P03_TENT		)

#define IDS_P03_INF_ORDER_DEC		(IDS_P03_INF_NOFUNC + P03_ORDER_DEC)
#define IDS_P03_INF_GIFT_CARD		(IDS_P03_INF_NOFUNC + P03_GIFT_CARD)

#define IDS_P03_INF_ADJ_MOD			(IDS_P03_INF_NOFUNC + P03_ADJ_MOD)

//////////////////////////


#define IDS_P03_DSP_NOFUNC      (IDS_P03_INF_NOFUNC + P03_DSP_INF_OFS)
#define IDS_P03_DSP_KEYEDPLU    (IDS_P03_DSP_NOFUNC + P03_KEYEDPLU)
#define IDS_P03_DSP_KEYEDDPT    (IDS_P03_DSP_NOFUNC + P03_KEYEDDPT)
#define IDS_P03_DSP_PRTMOD      (IDS_P03_DSP_NOFUNC + P03_PRTMOD)
#define IDS_P03_DSP_ADJ         (IDS_P03_DSP_NOFUNC + P03_ADJ)
#define IDS_P03_DSP_TENDER      (IDS_P03_DSP_NOFUNC + P03_TENDER)
#define IDS_P03_DSP_FC          (IDS_P03_DSP_NOFUNC + P03_FC)
#define IDS_P03_DSP_TTL         (IDS_P03_DSP_NOFUNC + P03_TTL)
#define IDS_P03_DSP_ADDCHK      (IDS_P03_DSP_NOFUNC + P03_ADDCHK)
#define IDS_P03_DSP_IDISC       (IDS_P03_DSP_NOFUNC + P03_IDISC)
#define IDS_P03_DSP_RDISC       (IDS_P03_DSP_NOFUNC + P03_RDISC)
#define IDS_P03_DSP_TAXMOD      (IDS_P03_DSP_NOFUNC + P03_TAXMOD)
#define IDS_P03_DSP_AKEY        (IDS_P03_DSP_NOFUNC + P03_AKEY)
#define IDS_P03_DSP_BKEY        (IDS_P03_DSP_NOFUNC + P03_BKEY)
/* #define IDS_P03_DSP_SERVNO      (IDS_P03_DSP_NOFUNC + P03_SERVNO) */
#define IDS_P03_DSP_NEWCHK      (IDS_P03_DSP_NOFUNC + P03_NEWCHK)
#define IDS_P03_DSP_PB          (IDS_P03_DSP_NOFUNC + P03_PB)
#define IDS_P03_DSP_GCNO        (IDS_P03_DSP_NOFUNC + P03_GCNO)
#define IDS_P03_DSP_GCTRANS     (IDS_P03_DSP_NOFUNC + P03_GCTRANS)
#define IDS_P03_DSP_NOPERSON    (IDS_P03_DSP_NOFUNC + P03_NOPERSON)
#define IDS_P03_DSP_TBLNO       (IDS_P03_DSP_NOFUNC + P03_TBLNO)
#define IDS_P03_DSP_LINENO      (IDS_P03_DSP_NOFUNC + P03_LINENO)
#define IDS_P03_DSP_FEED        (IDS_P03_DSP_NOFUNC + P03_FEED)
#define IDS_P03_DSP_RCTNORCT    (IDS_P03_DSP_NOFUNC + P03_RCTNORCT)
#define IDS_P03_DSP_TIPSPO      (IDS_P03_DSP_NOFUNC + P03_TIPSPO)
#define IDS_P03_DSP_DECTIP      (IDS_P03_DSP_NOFUNC + P03_DECTIP)
#define IDS_P03_DSP_NOSALE      (IDS_P03_DSP_NOFUNC + P03_NOSALE)
#define IDS_P03_DSP_PO          (IDS_P03_DSP_NOFUNC + P03_PO)
#define IDS_P03_DSP_ROA         (IDS_P03_DSP_NOFUNC + P03_ROA)
#define IDS_P03_DSP_CHGCOMP     (IDS_P03_DSP_NOFUNC + P03_CHGCOMP)
#define IDS_P03_DSP_CANCEL      (IDS_P03_DSP_NOFUNC + P03_CANCEL)
#define IDS_P03_DSP_VOID        (IDS_P03_DSP_NOFUNC + P03_VOID)
#define IDS_P03_DSP_EC          (IDS_P03_DSP_NOFUNC + P03_EC)
#define IDS_P03_DSP_VTRAN       (IDS_P03_DSP_NOFUNC + P03_VTRAN)
#define IDS_P03_DSP_QTY         (IDS_P03_DSP_NOFUNC + P03_QTY)
#define IDS_P03_DSP_SHIFT       (IDS_P03_DSP_NOFUNC + P03_SHIFT)
#define IDS_P03_DSP_NOTYPE      (IDS_P03_DSP_NOFUNC + P03_NOTYPE)
#define IDS_P03_DSP_PRTDEM      (IDS_P03_DSP_NOFUNC + P03_PRTDEM)
/* #define IDS_P03_DSP_ORDERNO     (IDS_P03_DSP_NOFUNC + P03_ORDERNO) */
#define IDS_P03_DSP_CHARTIP     (IDS_P03_DSP_NOFUNC + P03_CHARTIP)
#define IDS_P03_DSP_PLUNO       (IDS_P03_DSP_NOFUNC + P03_PLUNO)
#define IDS_P03_DSP_00KEY       (IDS_P03_DSP_NOFUNC + P03_00KEY)
#define IDS_P03_DSP_AC          (IDS_P03_DSP_NOFUNC + P03_AC)
#define IDS_P03_DSP_RECFEED     (IDS_P03_DSP_NOFUNC + P03_RECFEED)
#define IDS_P03_DSP_JOUFEED     (IDS_P03_DSP_NOFUNC + P03_JOUFEED)
#define IDS_P03_DSP_DECPT       (IDS_P03_DSP_NOFUNC + P03_DECPT)
#define IDS_P03_DSP_SCALE       (IDS_P03_DSP_NOFUNC + P03_SCALE)
#define IDS_P03_DSP_OFFTEND     (IDS_P03_DSP_NOFUNC + P03_OFFTEND)
/*** NCR2172 ***/
#define IDS_P03_DSP_MONEYDEC    (IDS_P03_DSP_NOFUNC + P03_MONEYDEC)
#define IDS_P03_DSP_RVSPRI      (IDS_P03_DSP_NOFUNC + P03_RVSPRI)
#define IDS_P03_DSP_PRECASH     (IDS_P03_DSP_NOFUNC + P03_PRECASH)
#define IDS_P03_DSP_DRCTSHFT    (IDS_P03_DSP_NOFUNC + P03_DRCTSHFT)
#define IDS_P03_DSP_TIMEIN      (IDS_P03_DSP_NOFUNC + P03_TIMEIN)
#define IDS_P03_DSP_TIMEOUT     (IDS_P03_DSP_NOFUNC + P03_TIMEOUT)
#define IDS_P03_DSP_KEYEDCPN    (IDS_P03_DSP_NOFUNC + P03_KEYEDCPN)
#define IDS_P03_DSP_CPNNO       (IDS_P03_DSP_NOFUNC + P03_CPNNO)
#define IDS_P03_DSP_UP          (IDS_P03_DSP_NOFUNC + P03_UP)
#define IDS_P03_DSP_DOWN        (IDS_P03_DSP_NOFUNC + P03_DOWN)
#define IDS_P03_DSP_RIGHT       (IDS_P03_DSP_NOFUNC + P03_RIGHT)
#define IDS_P03_DSP_LEFT        (IDS_P03_DSP_NOFUNC + P03_LEFT)
#define IDS_P03_DSP_WAIT        (IDS_P03_DSP_NOFUNC + P03_WAIT)
#define IDS_P03_DSP_PAID_REC    (IDS_P03_DSP_NOFUNC + P03_PAID_REC)
#define IDS_P03_DSP_KEYEDSTR    (IDS_P03_DSP_NOFUNC + P03_KEYEDSTR)
#define IDS_P03_DSP_STRNO       (IDS_P03_DSP_NOFUNC + P03_STRNO)
#define IDS_P03_DSP_ALPHA       (IDS_P03_DSP_NOFUNC + P03_ALPHA)
#define IDS_P03_DSP_LEFTDISPLAY (IDS_P03_DSP_NOFUNC + P03_LEFTDISPLAY)
#define IDS_P03_DSP_BARTENDER   (IDS_P03_DSP_NOFUNC + P03_BARTENDER)
#define IDS_P03_DSP_SPLIT       (IDS_P03_DSP_NOFUNC + P03_SPLIT)
#define IDS_P03_DSP_SEAT        (IDS_P03_DSP_NOFUNC + P03_SEAT)
#define IDS_P03_DSP_ITEMTRANS   (IDS_P03_DSP_NOFUNC + P03_ITEMTRANS)
#define IDS_P03_DSP_REPEAT      (IDS_P03_DSP_NOFUNC + P03_REPEAT)
#define IDS_P03_DSP_CURSORVOID  (IDS_P03_DSP_NOFUNC + P03_CURSORVOID)
#define IDS_P03_DSP_PRICECHECK  (IDS_P03_DSP_NOFUNC + P03_PRICECHECK)

#define IDS_P03_DSP_MOVING       (IDS_P03_DSP_NOFUNC + P03_MOVING)
#define IDS_P03_DSP_INDIC        (IDS_P03_DSP_NOFUNC + P03_INDIC)

#define IDS_P03_DSP_OPRINT_A    (IDS_P03_DSP_NOFUNC + P03_OPRINT_A)
#define IDS_P03_DSP_OPRINT_B    (IDS_P03_DSP_NOFUNC + P03_OPRINT_B)

/*** NCR2172 ***/
#define IDS_P03_DSP_ASK         (IDS_P03_DSP_NOFUNC + P03_ASK)
#define IDS_P03_DSP_ADJPLS      (IDS_P03_DSP_NOFUNC + P03_ADJPLS)
#define IDS_P03_DSP_DEPTNO      (IDS_P03_DSP_NOFUNC + P03_DEPTNO)
#define IDS_P03_DSP_UPCEVER     (IDS_P03_DSP_NOFUNC + P03_UPCEVER)
#define IDS_P03_DSP_PRICEENT    (IDS_P03_DSP_NOFUNC + P03_PRICEENT)
/* ### ADD Saratoga (052700) */
#define IDS_P03_DSP_FOODSTAMPMOD (IDS_P03_DSP_NOFUNC + P03_FOODSTAMPMOD)
#define IDS_P03_DSP_WICTRANS    (IDS_P03_DSP_NOFUNC + P03_WICTRANS    )
#define IDS_P03_DSP_WICMOD      (IDS_P03_DSP_NOFUNC + P03_WICMOD      )

// ADD SR 143 //////////////
#define IDS_P03_DSP_HALO_OVERRIDE	(IDS_P03_DSP_NOFUNC + P03_HALO_OVERRIDE)
#define IDS_P03_DSP_AT_FOR			(IDS_P03_DSP_NOFUNC + P03_AT_FOR)
////////////////////////////

// ADD SR 47 ///////////////
#define IDS_P03_DSP_VALIDATION		(IDS_P03_DSP_NOFUNC + P03_VALIDATION)

#define IDS_P03_DSP_ALPHANUM		(IDS_P03_DSP_NOFUNC + P03_ALPHANUM)
#define IDS_P03_DSP_CLEARFSC   		(IDS_P03_DSP_NOFUNC + P03_CLEARFSC)


#define IDS_P03_EDIT_CONDIMENT		(IDS_P03_DSP_NOFUNC + P03_EDIT_CONDIMENT)
#define IDS_P03_EDIT_COND_TBL		(IDS_P03_DSP_NOFUNC + P03_EDIT_COND_TBL)
#define IDS_P03_TENT		(IDS_P03_DSP_NOFUNC + P03_TENT)

#define IDS_P03_ORDER_DEC		(IDS_P03_DSP_NOFUNC + P03_ORDER_DEC)
#define IDS_P03_GIFT_CARD		(IDS_P03_DSP_NOFUNC + P03_GIFT_CARD)
#define IDS_P03_SUPR_INTRVN		(IDS_P03_DSP_NOFUNC + P03_SUPR_INTRVN)  //Supervisor Intervention button FSC_SUPR_INTRVN
#define IDS_P03_DOC_LAUNCH		(IDS_P03_DSP_NOFUNC + P03_DOC_LAUNCH)   //Document launch button FSC_DOC_LAUNCH
#define IDS_P03_OPR_PICKUP		(IDS_P03_DSP_NOFUNC + P03_OPR_PICKUP)   //Reg mode Cash pickup FSC_OPR_PICKUP

#define IDS_P03_ADJ_MOD		    (IDS_P03_DSP_NOFUNC + P03_ADJ_MOD)

////////////////////////////

/* Saratoga */
#define IDS_P03_OVERRANGE       (IDS_P03_DSP_NOFUNC + IDS_P03_DSP_PRICEENT)
#define IDS_P03_ERR_PLUNO       (IDS_P03_DSP_NOFUNC + IDS_P03_OVERRANGE)
#define IDS_P03_ERR_LABEL       (IDS_P03_DSP_NOFUNC + IDS_P03_ERR_PLUNO)
#define IDS_P03_DEPTOVERRANGE   (IDS_P03_DSP_NOFUNC + IDS_P03_ERR_LABEL)

/*
* ===========================================================================
*       Structure type defintions
* ===========================================================================
*/
#pragma pack(push, 1)
//  The following data structs are used to contain the FSC data which
//  is used as part of the keyboard key conversion logic in the terminal.
typedef struct _P03DATA {   /* a Page Data Buffer           */
    UCHAR   uchFsc;             /* Major FSC Data           */
    UCHAR   uchExt;             /* Extended FSC Data        */
} P03DATA, *LPP03DATA;

typedef struct _P03TBLDATA {   /* Data Buffer for Prog. #3  */
    PARAFSCTBL UNINT FSC[MAX_FSC_TBL];             /* Major FSC Data           */
} P03TBLDATA, *LPP03TBLDATA;

#ifdef __P03__
typedef struct _P03PLU {
    UCHAR   uchAdjective;                       /* Adjective Number */
    UCHAR   uchModStat;                         /* Modifier Status  */
    WCHAR   szPluNo[PLU_MAX_DIGIT + 1];         /* PLU No.          */
} P03PLU, *LPP03PLU;

typedef struct _P03DATA2 {
    P03PLU  PluKey[MAX_PAGE_NO][FSC_PLU_MICROMAX];
} P03DATA2, *LPP03DATA2;
#endif

typedef struct _P03DEPT {
    USHORT   usDeptNumber;                      /* Dept No.          */
} P03DEPT, *LPP03DEPT;

typedef struct _P03DEPTDATA {
    P03DEPT  DeptKey[MAX_PAGE_NO][FSC_DEPT_MAX];
} P03DEPTDATA, *LPP03DEPTDATA;
#pragma pack(pop)

/*
* ===========================================================================
*       Macro Difinitions
* ===========================================================================
*/
    /* ----- Retrieve Major FSC Code with Current Selected Button ID ----- */
#define P03MAJORFSCCODE(wID)    (UCHAR)((WORD)wID - IDD_P03_NOFUNC)

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
VOID    P03SetPageData(HWND, LPP03DATA, LPP03TBLDATA);
BOOL    P03InitDlg(HWND, LPHCURSOR, LPP03DATA FAR *, LPP03TBLDATA FAR *);
VOID    P03InitData(HWND, LPP03DATA, LPP03TBLDATA);
BOOL    P03ChkFixKey(WORD);
VOID    P03SaveData(LPP03TBLDATA);
VOID    P03GetData(LPP03DATA, LPP03DATA);
VOID    P03DspMove(HWND, HWND);
VOID    P03RepStr(HWND, HWND, PPOINTS, LPP03DATA);
VOID    P03DspStr(HWND, HWND, LPP03DATA);
VOID    P03DspPre(HWND, PPOINTS, LPP03DATA);
VOID    P03SetStr(HWND, short, LPP03DATA);
VOID    P03ShowStat(HWND, HWND, LPP03DATA);
VOID    P03ChgCursor(BOOL, HCURSOR);
BOOL    P03Comp2Pos(HWND, PPOINTS, PPOINTS);
HWND    P03ChkPos(HWND, PPOINTS);
BOOL    P03MakeExtFsc(HWND);
VOID    P03ChkExtRng(HWND, UINT);
BOOL    P03GetExtFsc(HWND, UINT);
VOID    P03ChgBtnStyle(HWND, WORD);
HBRUSH  P03DrawWnd(WPARAM, LPARAM, BOOL, LPHBRUSH, LPHBRUSH);
VOID    P03ExecAbort(HWND, BOOL, LPHBRUSH, LPHBRUSH,  LPHCURSOR);
VOID    P03ConvPluKey(HWND hDlg, LPP03TBLDATA lpTbl);

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
VOID    P03ConvDeptKey(HWND hDlg, LPP03TBLDATA lpTbl);         /* Saratoga */
#endif
BOOL    P03GetExtFscPlu(HWND hDlg);                            /* Saratoga */
VOID    P03ChkDeptRng(HWND hDlg);                              /* Saratoga */
BOOL    P03GetExtDept(HWND hDlg);                              /* Saratoga */

VOID	InitP03DlgStrings(HWND hDlg);	//4-22-03 RPH Multilingual
VOID	InitToolboxDlgStrings(HWND hDlg);  //4-22-03 RPH Multilingual

#endif
/* ===== End of P003.H ===== */

