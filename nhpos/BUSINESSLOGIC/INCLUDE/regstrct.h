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
-----------------------------------------------------------------------
* Title       : REG MODE HEADER FILE
* Category    : REG MODE Application Program - NCR 2170 US Hospitality Application
* Program Name: regstrct.h
* --------------------------------------------------------------------------
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
* ----------------------------------------------------------------------------------
* Abstract:
* ----------------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name      : Description
* Feb-23-95: 03.00.00 :   hkato     : R3.0
* Jun-22-95: 03.00.01 : T.Nakahata  : add
* Nov-08-95: 03.01.00 :   hkato     : R3.1
* Jun-02-98: 03.03.00 :  M.Ozawa    : add Cashier Interrupt
*
** NCR2172 **
*
* Oct-05-99: 01.00.00 : M.Teraki    :initial (for Win32)
* Apr-22-15: 02.02.01 : R.Chambers  : changed fbModifier to fbModifierTransOpen so easier to find.
*                                   : changed ulAmount to ulAmountTransOpen so easier to find.
*===================================================================================
*===================================================================================
-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
*   PVCS ENTRY
*    $Revision$
*    $Date$
*    $Author$
*    $Log$
\*-----------------------------------------------------------------------*/

#if !defined(REGSTRUCT_H_INCLUDED)
#define REGSTRUCT_H_INCLUDED

#include <paraequ.h>

/*--------------------------------------------------------------------------
*       ITEM MODULE interface data (major class)
*       used with the varous UIF structs of UIFITEMDATA as well as
*       the struct REGDISPMSG for display of data to the operator.
*
*       See also CLASS_UIFREGDISP and other defines in display.h.
*
*    ** WARNING: These defines must be compatible with the defines of
*                 CLASS_ITEMOPEOPEN, CLASS_ITEMSALES, etc. in that the values
*                 of these defines should not be the reused for those defines.
*                 These must be two different number spaces so that there
*                 will not be a compiler error if some of these defines are
*                 used with some of the CLASS_ITEM type defines in a switch().
*                 In other words mixing these CLASS_UIF type defines with the
*                 CLASS_ITEM type defines in switch() statement case statements
*                 should remain legal.  Two defines with the same meaning
*                 are used in places in several places such as file itprevus.c
*                 in function ItemPreviousItemClearStore().
--------------------------------------------------------------------------*/

#define     CLASS_UIFREGOPEN         1          /* operator open, used by DispWrite() with switch */
#define     CLASS_UIFREGTRANSOPEN    2          /* transaction open, used by DispWrite() with switch */
#define     CLASS_UIFREGSALES       10          /* sales, used by DispWrite() with switch, see also CLASS_ITEMSALES */
#define     CLASS_UIFREGDISC        11          /* discount, used by DispWrite() with switch, see also CLASS_ITEMDISC */
#define     CLASS_UIFREGTOTAL       12          /* total key, used by DispWrite() with switch, see also CLASS_ITEMTOTAL */
#define     CLASS_UIFREGTENDER      13          /* regular tender, used by DispWrite() with switch, see also CLASS_ITEMTENDER, FSC_TENDER */
#define     CLASS_UIFREGMISC        14          /* misc. transaction, used by DispWrite() with switch, see also CLASS_ITEMMISC */
#define     CLASS_UIFREGMODIFIER    15          /* modifier, used by DispWrite() with switch */
#define     CLASS_UIFREGOTHER       16          /* other, used with DispWrite(). See also define CLASS_ITEMOTHER, used with TrnOther().  */
#define     CLASS_UIFREGPAMTTENDER  17          /* preset amount cash tender, used by DispWrite() with switch, FSC_PRESET_AMT, uses P15 Preset Cash Amount table - Para.ParaPresetAmount */
#define     CLASS_UIFREGCOUPON      18          /* coupon, used by DispWrite() with switch */
#define     CLASS_UIFREGOPECLOSE    50          /* operator close, used by DispWrite() with switch, see also CLASS_ITEMOPECLOSE */
#define     CLASS_UIFREGMODE        60          /* mode-in, used by DispWrite() with switch */
// See also CLASS_UIFREGDISP        70          /* display module, used by DispWrite() with switch, defined in display.h */
// See also CLASS_REGDISPCOMPLSRY   71          /* display module, used by DispWrite() with switch, defined in display.h */
// See also CLASS_REGDISP_SALES     72          /* display module, used by DispWrite() with switch, defined in display.h */


/*--------------------------------------------------------------------------
*       ITEM MODULE interface data (minor class)
*       used with various structs to indicate the sub-type of the major class type.
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
*       OPERATOR OPEN DATA - struct UIFREGOPEOPEN, major class CLASS_UIFREGOPEN
--------------------------------------------------------------------------*/

#define     CLASS_UICASHIERIN        1          /* cashier A sign-in, use drawer A */
#define     CLASS_UIB_IN             2          /* cashier B sign-in, use drawer B if available */
#define     CLASS_UIWAITERIN        11          /* waiter sign-in */
#define     CLASS_UIWAITERLOCKIN    12          /* waiter lock sign-in */
#define     CLASS_UIBARTENDERIN     21          /* bartender or surrogate sign-in,   R3.1 */

#define     CLASS_UICASINTIN        31          /* cashier interrupt A sign-in, use drawer A, R3.3 */
#define     CLASS_UICASINTB_IN      32          /* cashier interrupt B sign-in, use drawer B if available, R3.3 */

/*--------------------------------------------------------------------------
*       TRANSACTION OPEN DATA - struct UIFREGTRANSOPEN, major class CLASS_UIFREGTRANSOPEN
--------------------------------------------------------------------------*/

#define     CLASS_UINEWCHECK         2          /* newcheck */
#define     CLASS_UIREORDER          3          /* reorder, PB or Previous Balance key */
#define     CLASS_UIADDCHECK1        4          /* addcheck 1 */
#define     CLASS_UIADDCHECK2        5          /* addcheck 2 */
#define     CLASS_UIADDCHECK3        6          /* addcheck 3 */
#define     CLASS_UIRECALL           7          /* recall check for Store Recall counter or delivery terminal */
#define     CLASS_UICASINTOPEN       8          /* cashier interrupt, R3.3 */
#define     CLASS_UICASINTRECALL     9          /* cashier interrupt recall, R3.3 */
#define     CLASS_UINEWKEYSEQ       11          /* New Key Sequence,    V3.3 */
#define     CLASS_UINEWKEYSEQGC     12          /* New Key Sequence,    V3.3 */
#define     CLASS_UIRETURNCHECK     13          /* Retrieve existing paid guest check and all return operations on it. */



/*--------------------------------------------------------------------------
*       SALES DATA - struct UIFREGSALES, major class CLASS_UIFREGSALES
--------------------------------------------------------------------------*/

#define     CLASS_UIDEPT             1          /* dept */
#define     CLASS_UIPLU             11          /* PLU */
#define     CLASS_UIPRINTMODIFIER   31          /* print modifier */
#define     CLASS_UIMODDISC         41          /* modifier discount */
#define     CLASS_UIDEPTREPEAT      51          /* dept repeat */
#define     CLASS_UIPLUREPEAT       52          /* PLU repeat */

#define     CLASS_UIPRICECHECK      61          /* price check,      R3.1 */
#define     CLASS_UIREPEAT          62          /* repeat,           R3.1 */
#define     CLASS_UIDISPENSER       71          /* Beverage Dispenser R3.1 */
#define		CLASS_ITEMORDERDEC		72			// Order Declaration, should be same as CLASS_ORDERDEC
#define		CLASS_ITEMMODIFICATION	73			/* Adjective Modification JHHJ */

/*--------------------------------------------------------------------------
*       DISCOUNT DATA classes identifiers used with UIFREGDISC and ITEMDISC
--------------------------------------------------------------------------*/
#define     CLASS_UIITEMDISC1        1          /* item discount */
#define     CLASS_UIREGDISC1        11          /* regular discount 1 */
#define     CLASS_UIREGDISC2        12          /* regular discount 2 */
#define     CLASS_UIREGDISC3        13          /* regular discount 3,  R3.1 */
#define     CLASS_UIREGDISC4        14          /* regular discount 4,  R3.1 */
#define     CLASS_UIREGDISC5        15          /* regular discount 5,  R3.1 */
#define     CLASS_UIREGDISC6        16          /* regular discount 6,  R3.1 */
#define     CLASS_UICHARGETIP       21          /* charge tip */
#define     CLASS_UICHARGETIP2      22          /* charge tip, V3.3 */
#define     CLASS_UICHARGETIP3      23          /* charge tip, V3.3 */

#define     CLASS_UIAUTOCHARGETIP   31          /* auto charge tip, V3.3 */
#define     CLASS_UIAUTOCHARGETIP2  32          // not used with UIFREGDISC but is used with ITEMDISC
#define     CLASS_UIAUTOCHARGETIP3  33          // not used with UIFREGDISC but is used with ITEMDISC

/*--------------------------------------------------------------------------
*       COUPON DATA
--------------------------------------------------------------------------*/

#define     CLASS_UICOMCOUPON        1          /* combination coupon */
#define     CLASS_UICOMCOUPONREPEAT  2          /* coupon repeat */
#define     CLASS_UIUPCCOUPON       11          /* UPC coupon 2172 */


/*--------------------------------------------------------------------------
*       TOTAL KEY DATA - uchMinorClass indicating the specific total key number
*       used with both UIFREGTOTAL and ITEMTENDER.
--------------------------------------------------------------------------*/

#define     CLASS_UITOTAL1           1          /* total #1(subtotal).  See also define CLASS_TOTAL1 */
#define     CLASS_UITOTAL2           2          /* total #2(check total) */
#define     CLASS_UITOTAL3           3          /* total #3 */
#define     CLASS_UITOTAL4           4          /* total #4 */
#define     CLASS_UITOTAL5           5          /* total #5 */
#define     CLASS_UITOTAL6           6          /* total #6 */
#define     CLASS_UITOTAL7           7          /* total #7 */
#define     CLASS_UITOTAL8           8          /* total #8 */
#define     CLASS_UITOTAL9           9          /* total #9 */
#define     CLASS_UITOTAL10         10          /* total #10 */
#define     CLASS_UITOTAL11         11          /* total #11 */
#define     CLASS_UITOTAL12         12          /* total #12 */
#define     CLASS_UITOTAL13         13          /* total #13 */
#define     CLASS_UITOTAL14         14          /* total #14 */
#define     CLASS_UITOTAL15         15          /* total #15 */
#define     CLASS_UITOTAL16         16          /* total #16 */
#define     CLASS_UITOTAL17         17          /* total #17 */
#define     CLASS_UITOTAL18         18          /* total #18 */
#define     CLASS_UITOTAL19         19          /* total #19 */
#define     CLASS_UITOTAL20         20          /* total #20. See also define CLASS_TOTAL20 */
#define     CLASS_UITOTAL2_FS       21          /* total #2 for FS, Saratoga */
	// following defines are for UIFREGTOTAL only and do not map to ITEMTOTAL uchMinorClass
#define     CLASS_UITOTALPRESET     22          // special preset total for tender key assigned zero.
#define     CLASS_UISPLIT           (CLASS_UITOTALPRESET+1)   /* split key,   R3.1 */
#define     CLASS_UICASINT          (CLASS_UISPLIT+1)         /* cashier interrupt, R3.3 */
#define     CLASS_UIKEYOUT          (CLASS_UICASINT+1)        /* sign-out by server lock (see ItemCasSignOutFinalize()), V3.3 */


/*---------------------------------------------------------------------------
*       TENDER DATA - uchMinorClass indicating the specific tender key number
*       used with UIFREGTENDER. See also ITEMTENDER and ITEMTOTAL minor classes.
*
*       WARNING: There are differences between the values used for these
*                defines after CLASS_UITENDER20 which conflict with the same
*                defines after CLASS_TENDER20 used with ITEMTENDER.
*
*                See difference between CLASS_UIFOREIGN1 and CLASS_FOREIGN1.
*                See difference between CLASS_UIFOREIGNTOTAL1 and CLASS_FOREIGNTOTAL1
--------------------------------------------------------------------------*/

#define     CLASS_UITENDER1          1          /* tender #1, see also CLASS_TENDER1 */
#define     CLASS_UITENDER2          2          /* tender #2 */
#define     CLASS_UITENDER3          3          /* tender #3 */
#define     CLASS_UITENDER4          4          /* tender #4 */
#define     CLASS_UITENDER5          5          /* tender #5 */
#define     CLASS_UITENDER6          6          /* tender #6 */
#define     CLASS_UITENDER7          7          /* tender #7 */
#define     CLASS_UITENDER8          8          /* tender #8 */
#define     CLASS_UITENDER9          9          /* tender #9 */
#define     CLASS_UITENDER10        10          /* tender #10 */
#define     CLASS_UITENDER11        11          /* tender #11 */
#define     CLASS_UITENDER12        12          /* tender #12 */
#define     CLASS_UITENDER13        13          /* tender #13 */
#define     CLASS_UITENDER14        14          /* tender #14 */
#define     CLASS_UITENDER15        15          /* tender #15 */
#define     CLASS_UITENDER16        16          /* tender #16 */
#define     CLASS_UITENDER17        17          /* tender #17 */
#define     CLASS_UITENDER18        18          /* tender #18 */
#define     CLASS_UITENDER19        19          /* tender #19 */
#define     CLASS_UITENDER20        20          /* tender #20 */

#define     CLASS_UIFOREIGNTOTAL1   (CLASS_UITENDER20+1)               /* FC total #1, see also CLASS_FOREIGNTOTAL1 */
#define     CLASS_UIFOREIGNTOTAL2   (CLASS_UIFOREIGNTOTAL1+1)          /* FC total #2 */
#define     CLASS_UIFOREIGNTOTAL3   (CLASS_UIFOREIGNTOTAL2+1)          /* FC total #3, Saratoga */
#define     CLASS_UIFOREIGNTOTAL4   (CLASS_UIFOREIGNTOTAL3+1)          /* FC total #4, Saratoga */
#define     CLASS_UIFOREIGNTOTAL5   (CLASS_UIFOREIGNTOTAL4+1)          /* FC total #5, Saratoga */
#define     CLASS_UIFOREIGNTOTAL6   (CLASS_UIFOREIGNTOTAL5+1)          /* FC total #6, Saratoga */
#define     CLASS_UIFOREIGNTOTAL7   (CLASS_UIFOREIGNTOTAL6+1)          /* FC total #7, Saratoga */
#define     CLASS_UIFOREIGNTOTAL8   (CLASS_UIFOREIGNTOTAL7+1)          /* FC total #8, Saratoga */

#define     CLASS_UIFOREIGN1        (CLASS_UIFOREIGNTOTAL8+1)          /* FC tender #1,Saratoga, see also CLASS_FOREIGN1 */
#define     CLASS_UIFOREIGN2        (CLASS_UIFOREIGN1+1)               /* FC tender #2,Saratoga */
#define     CLASS_UIFOREIGN3        (CLASS_UIFOREIGN2+1)               /* FC tender #3,Saratoga */
#define     CLASS_UIFOREIGN4        (CLASS_UIFOREIGN3+1)               /* FC tender #4,Saratoga */
#define     CLASS_UIFOREIGN5        (CLASS_UIFOREIGN4+1)               /* FC tender #5,Saratoga */
#define     CLASS_UIFOREIGN6        (CLASS_UIFOREIGN5+1)               /* FC tender #6,Saratoga */
#define     CLASS_UIFOREIGN7        (CLASS_UIFOREIGN6+1)               /* FC tender #7,Saratoga */
#define     CLASS_UIFOREIGN8        (CLASS_UIFOREIGN7+1)               /* FC tender #8,Saratoga */

#define     CLASS_UIFFSCHANGE       (CLASS_UIFOREIGN8+1)          /* FS change for display, V2.01.04  */


/*--------------------------------------------------------------------------
*       MISC. TRANSACTION DATA
*  These uchMinorClass codes are used with struct UIFREGMISC and with struct
*  REGDISPMSG to indicates particular Miscellaneous actions.
*
*  See for instance the display to the Operator in function DispRegMisc20().
*  See function UifSingleTrans() for translaton of FSC to uchMajorClass and
*  how these uchMinorClass codes are used with struct UIFREGMISC.
--------------------------------------------------------------------------*/

#define     CLASS_UINOSALE               1          /* nosale */
#define     CLASS_UIPO                   2          /* paid out */
#define     CLASS_UIRA                   3          /* received on account */
#define     CLASS_UITIPSPO               4          /* tips paid out */
#define     CLASS_UITIPSDECLARED         5          /* tips declared */
#define     CLASS_UICANCEL               7          /* cancel */
#define     CLASS_UICHECKTRANSFERIN     11          /* check transfer in */
#define     CLASS_UICHECKTRANSFER       12          /* check transfer */
#define     CLASS_UICHECKTRANSFEROUT    13          /* check transfer out */
#define     CLASS_UIETKIN               14          /* ETK time in */
#define     CLASS_UIETKOUT              15          /* ETK time out */
#define     CLASS_UIPOSTRECEIPT         16          /* post receipt */
#define     CLASS_UIPARKING             17          /* parking */

#define     CLASS_UIFMONEY_IN       18      /* money declaration-in, 2172        */
#define     CLASS_UIFMONEY          19      /* money declaration, 2172           */
#define     CLASS_UIFMONEY_OUT      20      /* money declaration-out, 2172       */
#define     CLASS_UIFMONEY_FOREIGN  21      /* money declaration (foreign), 2172 */
#define		CLASS_UIGIFTRECEIPT		22		/*Gift Receipt*/
#define		CLASS_UIPODREPRINT		23		/*Print on Demand reprint receipt*/

/*--------------------------------------------------------------------------
*       MODIFIER DATA
*  These uchMinorClass codes are used with struct UIFREGMODIFIER and with struct
*  REGDISPMSG to indicates particular Modifier Key actions.
*
*  See for instance the display to the Operator in function DispRegModifier20().
*  In some cases the FSC key press is translated into a UIFREGMODIFIER struct data
*  and in some cases a function is called to implement the request without building
*  a UIFREGMODIFIER struct first.
--------------------------------------------------------------------------*/

#define     CLASS_UIPVOID            1          /* preselect void, FSC_PRE_VOID */
#define     CLASS_UINORECEIPT        2          /* no receipt */
#define     CLASS_UIMENUSHIFT        3          /* menu shift */
#define     CLASS_UITARE             4          /* scale/tare, FSC_TARE pressed or PLU usTareInformation & PLU_REQ_TARE_MASK */
#define     CLASS_UIFWICTRAN         5          /* WIC transaction */
#define     CLASS_UIFFOR			 6		    /* @/For Key SR 143 cwunn*/
#define		CLASS_UIFPRICE			 7			/* Price disp SR281 ESMITH */
#define		CLASS_UIFTRETURN		 8			/* Transaction return for a return, FSC_PRE_VOID */

#define     CLASS_UITABLENO         11          /* table No */
#define     CLASS_UINOPERSON        12          /* No of person */
#define     CLASS_UILINENO          13          /* line No */

// tax modifier range varies depending on the tax system in use.
//  - US tax (tax modifiers #1, #2, and #3 as well as tax exempt #1)
//  - Canadian tax (tax modifiers #1 through #9 as well as tax exempt #1 and #2).
#define     CLASS_UITAXMODIFIER1    21          /* tax modifier #1 (see MOD_TAXABLE1), FSC_TAX_MODIF extended code 1 - CANTAX_MEAL */
#define     CLASS_UITAXMODIFIER2    22          /* tax modifier #2 (see MOD_TAXABLE2), FSC_TAX_MODIF extended code 2 - CANTAX_CARBON */
#define     CLASS_UITAXMODIFIER3    23          /* tax modifier #3 (see MOD_TAXABLE3), FSC_TAX_MODIF extended code 3 - CANTAX_SNACK */
#define     CLASS_UITAXMODIFIER4    24          /* tax modifier #4 (see MOD_TAXABLE4), FSC_TAX_MODIF extended code 4 - CANTAX_BEER */
#define     CLASS_UITAXMODIFIER5    25          /* tax modifier #5 (see MOD_TAXABLE5), FSC_TAX_MODIF extended code 5 - CANTAX_LIQUOR */
#define     CLASS_UITAXMODIFIER6    26          /* tax modifier #6 (see MOD_TAXABLE6), FSC_TAX_MODIF extended code 6 - CANTAX_GROCERY */
#define     CLASS_UITAXMODIFIER7    27          /* tax modifier #7 (see MOD_TAXABLE7), FSC_TAX_MODIF extended code 7 - CANTAX_TIP */
#define     CLASS_UITAXMODIFIER8    28          /* tax modifier #8 (see MOD_TAXABLE8), FSC_TAX_MODIF extended code 8 - CANTAX_PST1 */
#define     CLASS_UITAXMODIFIER9    29          /* tax modifier #9 (see MOD_TAXABLE9), FSC_TAX_MODIF extended code 9 - CANTAX_BAKED */
#define     CLASS_UITAXEXEMPT1      30          /* tax exempt #1 (see MOD_GSTEXEMPT or MOD_USEXEMPT), FSC_TAX_MODIF extended code 10 */
#define     CLASS_UITAXEXEMPT2      31          /* tax exempt #2 (see MOD_PSTEXEMPT), FSC_TAX_MODIF extended code 11 */

#define     CLASS_UIOFFTENDER       32          /* off line tender */
#define     CLASS_UIALPHANAME       33          /* customer name, R3.0 */
#define     CLASS_UIFDISP_FSMOD     34          /* food stamp modifier key */

#define     CLASS_UISEATNO1         41          /* seat number,   R3.1 */
#define     CLASS_UISEATNO2         42          /* seat number for read, R3.1 */

#define     CLASS_UIADJSHIFT        43          /* adjective shift, 2172 */
#define     CLASS_UIMODNUMBER       44          /* number, 2172 */

/*--------------------------------------------------------------------------
*       OTHER DATA - minor classes
*           used with major class CLASS_UIFREGOTHER for DispWrite().
*           used with major class CLASS_ITEMOTHER for TrnOther().
--------------------------------------------------------------------------*/

#define     CLASS_UIPRINTDEMAND      1       /* print demand */
#define     CLASS_UISLIPFEED         2       /* slip feed */
#define     CLASS_UISLIPRELEASE      3       /* slip release */
#define     CLASS_UIRECEIPTFEED      4       /* receipt feed */
#define     CLASS_UIJOURNALFEED      5       /* journal feed */
#define     CLASS_UICLEAR           11       /* clear */
#define     CLASS_UIEC              12       /* error correct */
#define     CLASS_UICOMPUTATION     21       /* computation */
#define     CLASS_UIORDERDISPLAY    22       /* order No display */
#define     CLASS_UILOWMNEMO        23       /* lower mnemonics display */
#define     CLASS_UISCROLLUP        31       /* scroll-up */
#define     CLASS_UISCROLLDOWN      32       /* scroll-down */
#define     CLASS_UIRIGHTARROW      33       /* right arrow */
#define     CLASS_UILEFTARROW       34       /* left arrow */
#define     CLASS_UIWAIT            35       /* wait */
#define     CLASS_UIPAIDORDERRECALL 36       /* paid order recall */

#define     CLASS_UICURSORVOID      41       /* cursor void,         R3.1 */
#define     CLASS_UITRANSFER        42       /* transfer,            R3.1 */
#define     CLASS_UICURSORTRETURN1  43       /* cursor transaction return */
#define     CLASS_UICURSORTRETURN2  44       /* cursor transaction return */
#define     CLASS_UICURSORTRETURN3  45       /* cursor transaction return */

#define     CLASS_UIFASK            51      /* ask key, generated by FSC_ASK   2172 */

#define     CLASS_UIFLABEL          52      /* label entry at number dialog, 2172 */
#define		CLASS_UIFOR				53		/* SR 143 cwunn @/For Key identifier*/

/*--------------------------------------------------------------------------
*       OPERATOR CLOSE DATA
*           used with major class CLASS_UIFREGOPECLOSE for DispWrite().
*           used with major class CLASS_ITEMOPECLOSE for TrnClose().
--------------------------------------------------------------------------*/

#define     CLASS_UICASHIEROUT       1       /* cashier A sign-out */
#define     CLASS_UIB_OUT            2       /* cashier B sign-out */
#define     CLASS_UICASRELONFIN      3       /* release on finalize (cashier) */
#define     CLASS_UIWAITEROUT       11       /* waiter sign-out */
#define     CLASS_UIWAITERLOCKOUT   12       /* waiter lock sign-out */
#define     CLASS_UIWAIKEYRELONFIN  13       /* release finalize (waiter sign-in by waiter lock) */
#define     CLASS_UIWAIREMOVEKEY    14       /* release finalize (waiter sign-in by waiter key) */
#define     CLASS_UIMODELOCK        21       /* mode lock key */

#define     CLASS_UIBARTENDEROUT    31       /* bartender surrogate sign-out, R3.1 */

#define     CLASS_UICASINTOUT       41       /* cashier interrupt A sign-out, R3.3 */
#define     CLASS_UICASINTB_OUT     42       /* cashier interrupt B sign-out, R3.3 */
#define     CLASS_UICASINTOUT2      43       /* for new cashier sign-in error case */
#define     CLASS_UICASINT_PREOUT   44       /* cashier interrupt A pre sign-out, R3.3 */
#define     CLASS_UICASINTB_PREOUT  45       /* cashier interrupt B pre sign-out, R3.3 */


/*--------------------------------------------------------------------------
*       MODE-IN DATA
--------------------------------------------------------------------------*/

#define     CLASS_UIREGMODEIN        1          /* reg mode-in */


/*--------------------------------------------------------------------------
*       GIFT CARD DATA
--------------------------------------------------------------------------*/
#define		CLASS_NONSFSALE			0
#define		CLASS_UIBALANCE			1		// balance of giftcard
#define		CLASS_FREEDOMPAY		2		// Freedom Pay
#define		CLASS_EPAYMENT			3		// balance for Electronic Payment such as Employee Payroll Deductoin

#define		CLASS_UIVOID_ISSUE		1		// voids issue of giftcard
#define		CLASS_UIRELOAD			3		// reloads of giftcard
#define		CLASS_UIVOID_RELOAD		4		// voids reload of giftcard


/*--------------------------------------------------------------------------
*       TRANSACTION MODULE interface data (MAJOR CLASS)
*       These values should be in the range of 0 to 255 since
*       they are sometimes combined with MLD types.
*       See macros MLD_TYPE_GET_CLASS() and MLD_TYPE_PUT_CLASS().
*
*       See also macro ITEMSTORAGENONPTR(x) and struct ITEMGENERICHEADER
*       which are used to access uchMajorClass and uchMinorClass in switch()
*       statements as part of processing structs using these defines.
*
*    ** WARNING: These defines must be compatible with the defines of
*                 CLASS_UIFREGOPEN, CLASS_UIFREGSALES, etc. in that the values
*                 of these defines should not be the reused for those defines.
*                 These must be two different number spaces so that there
*                 will not be a compiler error if some of these defines are
*                 used with some of the CLASS_UIF type defines in a switch().
*                 In other words mixing these CLASS_ITEM type defines with the
*                 CLASS_UIF type defines in switch() statement case statements
*                 should remain legal.  Two defines with the same meaning
*                 are used in places in several places such as file itprevus.c
*                 in function ItemPreviousItemClearStore().
--------------------------------------------------------------------------*/

#define     CLASS_ITEMOPEOPEN        3          /* operator open */
#define     CLASS_ITEMTRANSOPEN      4          /* transaction open */
#define     CLASS_ITEMSALES         30          /* sales, see also CLASS_UIFREGSALES */
#define     CLASS_ITEMDISC          31          /* discount, see also CLASS_UIFREGDISC */
#define     CLASS_ITEMTOTAL         32          /* total key, see also CLASS_UIFREGDISC */
#define     CLASS_ITEMTENDER        33          /* tender, see also CLASS_UIFREGDISC, FSC_TENDER */
#define     CLASS_ITEMMISC          34          /* misc. transaction, see also CLASS_UIFREGMISC */
#define     CLASS_ITEMMODIFIER      35          /* modifier */
#define     CLASS_ITEMOTHER         36          /* other, used with TrnOther() and TrnItem(). See also define CLASS_UIFREGOTHER, used with DispWrite(). */
#define     CLASS_ITEMAFFECTION     37          /* affection */
#define     CLASS_ITEMPRINT         38          /* print */
#define     CLASS_ITEMMULTI         39          /* multi check payment */
#define     CLASS_ITEMTENDEREC      40          /* E/C tender for charge post */
#define     CLASS_ITEMCOUPON        41          /* coupon */
#define     CLASS_ITEMPREVCOND		50			/*Condiment Editing, JHHJ*/
#define     CLASS_ITEMOPECLOSE      51          /* operator close. See also define CLASS_UIFREGOPECLOSE */
#define     CLASS_ITEMTRANSCLOSE    52          /* transaction close */
#define     CLASS_ITEMMODEIN        61          /* mode-in */

/*-------------------------------------------------------------------------
*       TRANSACTION MODULE INTERFACE DATA (minor class)
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
*  OPERATOR OPEN DATA (ITEMOPEOPEN)
*      uchMajorClass CLASS_ITEMOPEOPEN, uchMinorClass identifiers.
*      uchMajorClass CLASS_UIFREGOPEN, uchMinorClass identifiers.
--------------------------------------------------------------------------*/
#define     CLASS_CASHIERIN          1          /* cashier A sign-in, use drawer A */
#define     CLASS_B_IN               2          /* cashier B sign-in, use drawer B if available */
#define     CLASS_WAITERIN          11          /* waiter sign-in */
#define     CLASS_WAITERLOCKIN      12          /* waiter lock sign-in */
#define     CLASS_BARTENDERIN       21          /* bartender surrogate sign-in,   R3.1 */

#define     CLASS_CASINTIN          31          /* cashier interrupt A sign-in, use drawer A, R3.3 */
#define     CLASS_CASINTB_IN        32          /* cashier interrupt B sign-in, use drawer B if available, R3.3 */
#define		CLASS_NO_CHANGE			 3			// used for the auto sign out and does not specify A or B key

/*--------------------------------------------------------------------------
*  TRANSACTION OPEN DATA (ITEMTRANSOPEN) - uchMajorClass CLASS_ITEMTRANSOPEN, uchMinorClass identifiers.
--------------------------------------------------------------------------*/
#define     CLASS_CASHIER            1          /* cashier operation */
#define     CLASS_NEWCHECK           2          /* newcheck */
#define     CLASS_REORDER            3          /* reorder */
#define     CLASS_ADDCHECK           4          /* addcheck */
#define     CLASS_OPENPOSTCHECK      5          /* transaction header at postcheck */
#define     CLASS_STORE_RECALL       6          /* store/recall trans, R3.0 */
#define     CLASS_STORE_RECALL_DELIV 7          /* store/recall trans(delivery) , R3.1 */
#define     CLASS_CASINTOPEN         8          /* cashier interrupt, R3.3 */
#define     CLASS_CASINTRECALL       9          /* cashier interrupt recall, R3.3 */
#define     CLASS_OPENNOSALE        11          /* nosale */
#define     CLASS_OPENPO            12          /* paid out */
#define     CLASS_OPENRA            13          /* received on account */
#define     CLASS_OPENTIPSPO        14          /* tips paid out */
#define     CLASS_OPENTIPSDECLARED  15          /* tips declared */
#define     CLASS_OPENCHECKTRANSFER 16          /* check transfer */
#define     CLASS_MANUALPB          21          /* manual Previous Balance, used with Pre-Guest Check */
#define     CLASS_OPENSEAT          31          /* Seat# Trans Open, R3.1 */
#define     CLASS_OPENPRINT         41          /* post receipt, R3.1 */

#define     CLASS_OPENMONEY         51      /* money declaration,   2172 */

#define     CLASS_OPEN_LOAN         101     /* loan transaction    2172 */
#define     CLASS_OPEN_PICK         102     /* pick-up transaction 2172 */
#define     CLASS_OPEN_TRANSBATCH   103     /* open a transaction using stored data like for Preauth batch */


/*--------------------------------------------------------------------------
*  SALES DATA (ITEMSALES) - uchMajorClass CLASS_ITEMSALES, uchMinorClass identifiers.
--------------------------------------------------------------------------*/
#define     CLASS_DEPT               1          /* dept */
#define     CLASS_DEPTITEMDISC       2          /* dept w/item discount */
#define     CLASS_DEPTMODDISC        3          /* dept w/modifier discount */
#define     CLASS_PLU               11          /* PLU */
#define     CLASS_PLUITEMDISC       12          /* PLU w/item discount */
#define     CLASS_PLUMODDISC        13          /* PLU w/modifier discount */
#define     CLASS_SETMENU           21          /* set menu from AC116 Promotional PLU processing. see ItemSalesSetMenu() */
#define     CLASS_SETITEMDISC       22          /* set menu from AC116 w/item discount */
#define     CLASS_SETMODDISC        23          /* set menu from AC116 w/modifier discount */
#define     CLASS_OEPPLU            31          /* order entry promot item */
#define     CLASS_OEPITEMDISC       32          /* OEP item w/item discount */
#define     CLASS_OEPMODDISC        33          /* OEP item w/modifier discount */

#if 0
#define     CLASS_UPCA              11      /* PLU, UPC-A             */
#define     CLASS_EAN13             12      /* PLU, EAN-13            */
#define     CLASS_UPCE              13      /* PLU, UPC-E             */
#define     CLASS_EAN8              14      /* PLU, EAN-8             */
#define     CLASS_VELOC             15      /* PLU, velocity          */
#define     CLASS_ADJECTIVE         16      /* PLU, adjective         */
#define     CLASS_PPI_UPCA          31      /* prom. pricing PLU      */
#define     CLASS_PPI               32      /* prom. pricing PLU      */
#define     CLASS_RANDOM            41      /* random weight PLU      */
#define     CLASS_MIXMATCH          61      /* mix-match discount     */
#define     CLASS_LINKPLU           71      /* link PLU               */
#endif
/*--------------------------------------------------------------------------
*  DISCOUNT DATA (ITEMDISC) - uchMajorClass CLASS_ITEMDISC, uchMinorClass identifiers.
--------------------------------------------------------------------------*/

#define     CLASS_ITEMDISC1         CLASS_UIITEMDISC1         /* item discount */
#define     CLASS_REGDISC1          CLASS_UIREGDISC1          /* regular discount 1 */
#define     CLASS_REGDISC2          CLASS_UIREGDISC2          /* regular discount 2 */
#define     CLASS_REGDISC3          CLASS_UIREGDISC3          /* regular discount 3,  R3.1 */
#define     CLASS_REGDISC4          CLASS_UIREGDISC4          /* regular discount 4,  R3.1 */
#define     CLASS_REGDISC5          CLASS_UIREGDISC5          /* regular discount 5,  R3.1 */
#define     CLASS_REGDISC6          CLASS_UIREGDISC6          /* regular discount 6,  R3.1 */
#define     CLASS_CHARGETIP         CLASS_UICHARGETIP         /* charge tip */
#define     CLASS_CHARGETIP2        CLASS_UICHARGETIP2        /* charge tip, V3.3 */
#define     CLASS_CHARGETIP3        CLASS_UICHARGETIP3        /* charge tip, V3.3 */

#define     CLASS_AUTOCHARGETIP     CLASS_UIAUTOCHARGETIP     /* auto charge tip, V3.3 */
#define     CLASS_AUTOCHARGETIP2    CLASS_UIAUTOCHARGETIP2    /* auto charge tip, V3.3 */
#define     CLASS_AUTOCHARGETIP3    CLASS_UIAUTOCHARGETIP3    /* auto charge tip, V3.3 */

/*--------------------------------------------------------------------------
*   COUPON DATA (ITEMCOUPON) - uchMajorClass CLASS_ITEMCOUPON, uchMinorClass identifiers.
--------------------------------------------------------------------------*/
#define     CLASS_COMCOUPON      1          /* combination coupon */
#define     CLASS_UPCCOUPON     11          /* UPC coupon 2172 */


/*--------------------------------------------------------------------------
*  TOTAL KEY DATA (ITEMTOTAL) - uchMajorClass CLASS_ITEMTOTAL, uchMinorClass identifiers.
*  
*     found in ItemTotal->auchTotalStatus[0]
--------------------------------------------------------------------------*/
#define     CLASS_TOTAL1             1          /* total #1, hard coded as Subtotal */
#define     CLASS_TOTAL2             2          /* total #2, hard coded as Check Total */
#define     CLASS_TOTAL3             3          /* total #3 */
#define     CLASS_TOTAL4             4          /* total #4 */
#define     CLASS_TOTAL5             5          /* total #5 */
#define     CLASS_TOTAL6             6          /* total #6 */
#define     CLASS_TOTAL7             7          /* total #7 */
#define     CLASS_TOTAL8             8          /* total #8 */
#define     CLASS_TOTAL9             9          /* total #9 */
#define		CLASS_TOTAL10			10			/* total #10*/
#define		CLASS_TOTAL11			11			/* total #11*/
#define		CLASS_TOTAL12			12			/* total #12*/
#define		CLASS_TOTAL13			13			/* total #13*/
#define		CLASS_TOTAL14			14			/* total #14*/
#define		CLASS_TOTAL15			15			/* total #15*/
#define		CLASS_TOTAL16			16			/* total #16*/
#define		CLASS_TOTAL17			17			/* total #17*/
#define		CLASS_TOTAL18			18			/* total #18*/
#define		CLASS_TOTAL19			19			/* total #19*/
#define		CLASS_TOTAL20			20			/* total #20*/
#define     CLASS_TOTAL2_FS         21          /* total #2 for FS */
	// following defines are for ITEMTOTAL only and do not map to UIFREGTOTAL uchMinorClass
#define     CLASS_STUB3             22          /* stub #3, total key #1 (Subtotal) and #2 (Check Total) are special purpose  */
#define     CLASS_STUB4             23          /* stub #4 */
#define     CLASS_STUB5             24          /* stub #5 */
#define     CLASS_STUB6             25          /* stub #6 */
#define     CLASS_STUB7             26          /* stub #7 */
#define     CLASS_STUB8             27          /* stub #8 */
#define     CLASS_STUB9             28          /* stub #9 */
#define     CLASS_STUB10            29          /* stub #10 */
#define     CLASS_STUB11            30          /* stub #11 */
#define     CLASS_STUB12            31          /* stub #12 */
#define     CLASS_STUB13            32          /* stub #13 */
#define     CLASS_STUB14            33          /* stub #14 */
#define     CLASS_STUB15            34          /* stub #15 */
#define     CLASS_STUB16            35          /* stub #16 */
#define     CLASS_STUB17            36          /* stub #17 */
#define     CLASS_STUB18            37          /* stub #18 */
#define     CLASS_STUB19            38          /* stub #19 */
#define     CLASS_STUB20            39          /* stub #20 */
#define     CLASS_ORDER3            40          /* order #3, total key #1 (Subtotal) and #2 (Check Total) are special purpose */
#define     CLASS_ORDER4            41          /* order #4 */
#define     CLASS_ORDER5            42          /* order #5 */
#define     CLASS_ORDER6            43          /* order #6 */
#define     CLASS_ORDER7            44          /* order #7 */
#define     CLASS_ORDER8            45          /* order #8 */
#define     CLASS_ORDER9            46          /* ORDER #9 */
#define     CLASS_ORDER10           47          /* ORDER #10 */
#define     CLASS_ORDER11           48          /* ORDER #11 */
#define     CLASS_ORDER12           49          /* ORDER #12 */
#define     CLASS_ORDER13           50          /* ORDER #13 */
#define     CLASS_ORDER14           51          /* ORDER #14 */
#define     CLASS_ORDER15           52          /* ORDER #15 */
#define     CLASS_ORDER16           53          /* ORDER #16 */
#define     CLASS_ORDER17           54          /* ORDER #17 */
#define     CLASS_ORDER18           55          /* ORDER #18 */
#define     CLASS_ORDER19           56          /* ORDER #19 */
#define     CLASS_ORDER20           57          /* ORDER #20 */

#define		CLASS_STUB_OFFSET		(CLASS_STUB3 - CLASS_TOTAL3)	// Offset from the total key to its stub
#define		CLASS_ORDER_OFFSET		(CLASS_ORDER3 - CLASS_TOTAL3)	// Offset from the total key to its order

#define     CLASS_TOTALAUDACT       60          /* total for audaction */
#define     CLASS_TOTALPOSTCHECK    61          /* total for postcheck buffering print */
#define     CLASS_STUBPOSTCHECK     62          /* stub for postcheck total */
#define     CLASS_PREAUTHTOTAL      63          /* pre-authorization total */

#define     CLASS_SPLIT             71          /* split key,   R3.1 */

#define     CLASS_FOREIGNTOTAL1     81          /* foreign currency total #1 R3.1 */
#define     CLASS_FOREIGNTOTAL2     82          /* foreign currency total #2 R3.1 */
#define     CLASS_FOREIGNTOTAL3     83          /* foreign currency total #2 R3.1 */
#define     CLASS_FOREIGNTOTAL4     84          /* foreign currency total #2 R3.1 */
#define     CLASS_FOREIGNTOTAL5     85          /* foreign currency total #2 R3.1 */
#define     CLASS_FOREIGNTOTAL6     86          /* foreign currency total #2 R3.1 */
#define     CLASS_FOREIGNTOTAL7     87          /* foreign currency total #2 R3.1 */
#define     CLASS_FOREIGNTOTAL8     88          /* foreign currency total #2 R3.1 */

#define     CLASS_SOFTCHK_BASE_TTL 100          /* total base value for soft check */
#define     CLASS_SOFTCHK_TOTAL1   101          /* total #1 for soft check */
#define     CLASS_SOFTCHK_TOTAL2   102          /* total #2 for soft check */
#define     CLASS_SOFTCHK_TOTAL3   103          /* total #3 for soft check */
#define     CLASS_SOFTCHK_TOTAL4   104          /* total #4 for soft check */
#define     CLASS_SOFTCHK_TOTAL5   105          /* total #5 for soft check */
#define     CLASS_SOFTCHK_TOTAL6   106          /* total #6 for soft check */
#define     CLASS_SOFTCHK_TOTAL7   107          /* total #7 for soft check */
#define     CLASS_SOFTCHK_TOTAL8   108          /* total #8 for soft check */
#define     CLASS_SOFTCHK_TOTAL9   109          /* total #9 for soft check */
#define     CLASS_SOFTCHK_TOTAL10   110          /* total #10 for soft check */
#define     CLASS_SOFTCHK_TOTAL11   111          /* total #11 for soft check */
#define     CLASS_SOFTCHK_TOTAL12   112          /* total #12 for soft check */
#define     CLASS_SOFTCHK_TOTAL13   113          /* total #13 for soft check */
#define     CLASS_SOFTCHK_TOTAL14   114          /* total #14 for soft check */
#define     CLASS_SOFTCHK_TOTAL15   115          /* total #15 for soft check */
#define     CLASS_SOFTCHK_TOTAL16   116          /* total #16 for soft check */
#define     CLASS_SOFTCHK_TOTAL17   117          /* total #17 for soft check */
#define     CLASS_SOFTCHK_TOTAL18   118          /* total #18 for soft check */
#define     CLASS_SOFTCHK_TOTAL19   119          /* total #19 for soft check */
#define     CLASS_SOFTCHK_TOTAL20   120          /* total #20 for soft check */

#define CHECK_TOTAL_TYPE		21 // we use this when checking to see what the total type should be.

// following are the Total Key types from ItemTotal.auchTotalStatus[0] / CHECK_TOTAL_TYPE
// see macro ITM_TTL_GET_TTLTYPE() for calculation.  see also function ItemTotalTypeIndex().
//
// WARNING: Some of these are used in the Kitchen Display System protocol so values should
//          not be changed.
#define     PRT_NOFINALIZE  0x0001      /* no finalaize total */
#define     PRT_SERVICE1    0x0002      /* service total without stub */
#define     PRT_SERVICE2    0x0003      /* service total with stub printed */
#define     PRT_FINALIZE1   0x0004      /* finalize total without stub */
#define     PRT_FINALIZE2   0x0005      /* finalize total with stub printed */
#define     PRT_TRAY1       0x0006      /* tray total witout stub */
#define     PRT_TRAY2       0x0007      /* tray total with stub printed */
#define     PRT_CASINT1     0x0008      /* cashier interrupt total without stub (similar to PRT_SERVICE1 in most cases), R3.3 */
#define     PRT_CASINT2     0x0009      /* cashier interrupt total with stub (similar to PRT_SERVICE2 in most cases), R3.3 */


/*--------------------------------------------------------------------------
*  TENDER DATA (ITEMTENDER) - uchMajorClass CLASS_ITEMTENDER, uchMinorClass identifiers.
*
*       WARNING: There are differences between the values used for these
*                defines after CLASS_TENDER20 which conflict with the same
*                defines after CLASS_UITENDER20 used with UIFREGTENDER.
*
*                See difference between CLASS_UIFOREIGN1 and CLASS_FOREIGN1.
*                See difference between CLASS_UIFOREIGNTOTAL1 and CLASS_FOREIGNTOTAL1
--------------------------------------------------------------------------*/
#define     CLASS_TENDER1            1          /* tender #1, Normally Cash Tender key. See also CLASS_UITENDER1 */
#define     CLASS_TENDER2            2          /* tender #2, Normally Food Stamp Tender key - MDC 354 */
#define     CLASS_TENDER3            3          /* tender #3, Typically Credit Tender key */
#define     CLASS_TENDER4            4          /* tender #4 */
#define     CLASS_TENDER5            5          /* tender #5 */
#define     CLASS_TENDER6            6          /* tender #6 */
#define     CLASS_TENDER7            7          /* tender #7 */
#define     CLASS_TENDER8            8          /* tender #8, possible Check No Purchase key */
#define     CLASS_TENDER9            9          /* tender #9, possible Check No Purchase key */
#define     CLASS_TENDER10          10          /* tender #10, possible Check No Purchase key */
#define     CLASS_TENDER11          11          /* tender #11, possible second Food Stamp Tender key - MDC 354 */
#define     CLASS_TENDER12          12          /* tender #12 */
#define     CLASS_TENDER13          13          /* tender #13 */
#define     CLASS_TENDER14          14          /* tender #14 */
#define     CLASS_TENDER15          15          /* tender #15 */
#define     CLASS_TENDER16          16          /* tender #16 */
#define     CLASS_TENDER17          17          /* tender #17 */
#define     CLASS_TENDER18          18          /* tender #18 */
#define     CLASS_TENDER19          19          /* tender #19 */
#define     CLASS_TENDER20          20          /* tender #20 */

#define     CLASS_FOREIGN1          23          /* FC tender #1. See also CLASS_UIFOREIGN1 */
#define     CLASS_FOREIGN2          24          /* FC tender #2 */
#define     CLASS_FOREIGN3          25          /* FC tender #3, 2172 */
#define     CLASS_FOREIGN4          26          /* FC tender #4, 2172 */
#define     CLASS_FOREIGN5          27          /* FC tender #3, 2172 */
#define     CLASS_FOREIGN6          28          /* FC tender #4, 2172 */
#define     CLASS_FOREIGN7          29          /* FC tender #3, 2172 */
#define     CLASS_FOREIGN8          30          /* FC tender #4, 2172 */

#define     CLASS_CPTENDEC          40          /* error correct for charge posting */

#define     CLASS_EUROTENDER        50          /* Euro amount trailer, V3.4 */

#define     CLASS_TEND_FSCHANGE     51          /* Food Stamp change for coupons with change in coupons and currency */
#define     CLASS_TEND_FSVAL        52          /* FS Tend for Val, R2.0  */
#define     CLASS_TEND_TIPS_RETURN  55          /* Charge Tips Void or Return for Amtrak */


/*--------------------------------------------------------------------------
*   MISC. TRANSACTION DATA (ITEMMISC) - uchMajorClass CLASS_ITEMMISC, uchMinorClass identifiers.
--------------------------------------------------------------------------*/
#define     CLASS_NOSALE             1          /* nosale */
#define     CLASS_PO                 2          /* paid out */
#define     CLASS_RA                 3          /* received on account */
#define     CLASS_TIPSPO             4          /* tips paid out */
#define     CLASS_TIPSDECLARED       5          /* tips declared */
#define     CLASS_CHECKTRANSFER      6          /* guest check transfer from current to cashier */
#define     CLASS_CANCEL             7          /* cancel */
#define     CLASS_ETKIN              8          /* Employee Time Keeping time in */
#define     CLASS_ETKOUT             9          /* Employee Time Keeping time out */
#define     CLASS_ETKOUT_WOETKIN    10          /* ETK time out w/o ETK in */
#define     CLASS_EC                11          /* error correct */
#define     CLASS_CHECKTRANS_FROM   21          /* guest check transfer from cashier */
#define     CLASS_CHECKTRANS_TO     22          /* guest check transfer to cashier */
#define     CLASS_MONEY             23          /* money declaration, 2172 */
#define		CLASS_HALO_OVR			24			/* High Amount Lock Out override SR 143 cwunn */
#define     CLASS_SIGNIN            25          /* Operator Sign in */
#define     CLASS_SIGNOUT           26          /* Operator Sign out */


/*--------------------------------------------------------------------------
*       MODIFIER DATA (ITEMMODIFIER) - CLASS_ITEMMODIFIER
*         values for uchMinorClass of ITEMMODIFIER struct to indicate the
*         type of transaction modification that is being done for function TrnItem().
--------------------------------------------------------------------------*/
#define     CLASS_PVOID              1          /* preselect void, FSC_PRE_VOID */
#define     CLASS_NORECEIPT          2          /* no receipt */
#define     CLASS_WICTRN             4          /* WIC transaction, FSC_WIC_TRN */
#define     CLASS_PRETURN            8          /* preselect return, FSC_PRE_VOID */

#define     CLASS_TABLENO           11          /* table No */
#define     CLASS_NOPERSON          12          /* No of person */
#define     CLASS_LINENO            13          /* line No */
#define     CLASS_ALPHANAME         14          /* Customer Name,      R3.0 */
#define     CLASS_TAXMODIFIER       15          /* Tax Modifier,       2172 */
#define     CLASS_MODNUMBER         16          /* Number              2172 */
#define     CLASS_ALPHATENT         17          /* Customer Name, used with FSC_TENT */

#define     CLASS_SEATNO            21          /* Seat#,              R3.1 */
#define     CLASS_SEAT_SINGLE       31          /* Seat# Tender, TRN_SPL_SEAT        R3.1 */
#define     CLASS_SEAT_MULTI        32          /* Multi Seat# Tender, TRN_SPL_MULTI R3.1 */
#define     CLASS_SEAT_TYPE2        33          /* Type 2, TRN_SPL_TYPE2             R3.1 */

/*--------------------------------------------------------------------------
*  OTHER DATA (ITEMOTHER) - uchMajor class CLASS_ITEMOTHER, uchMinorClass identifiers.
--------------------------------------------------------------------------*/
#define     CLASS_PRINTDEMAND        1          /* print demand */
#define     CLASS_SLIPRELEASE        3          /* slip release */
#define     CLASS_INSIDEFEED         4          /* inside feed  */
#define     CLASS_OUTSIDEFEED        5          /* outside feed */
#define     CLASS_POSTRECEIPT        6          /* post receipt */
#define     CLASS_POSTRECPREBUF      7          /* post receipt for pre-check buffer */
#define     CLASS_CP_EPT_DUPLI       8          /* Dupli-Receipt for EPT, CP*/
#define     CLASS_LP_INSIDEFEED      9          /* inside feed for loan/pickup, Saratoga */
#define     CLASS_PARKING            10         /* parking */
#define     CLASS_CP_EPT_FULLDUP     11         /* 5/30/96 full duplicate receipt for CP */
#define     CLASS_RESETLOG           12         /* PC I/F reset log, V3.3 */
#define     CLASS_POWERDOWNLOG       13         /* Power Down Log,      Saratoga*/
#define     CLASS_PREINQ_EPT         21         /* EPT Pre-Inquiry,     Saratoga */
#define     CLASS_SUMMARY_RECEIPT    22         /* Summary receipt for use with split tender  */


/*--------------------------------------------------------------------------
*       AFFECTION
*       uchMinorClass identifiers.
--------------------------------------------------------------------------*/
#define     CLASS_HOURLY             1          /* hourly total */
#define     CLASS_SERVICE            2          /* service total */
#define     CLASS_CANCELTOTAL        4          /* cancel total */
#define     CLASS_CASHIERCHECK       5          /* cashier close check */
#define     CLASS_WAITERCHECK        6          /* waiter close check */
#define     CLASS_NEWWAITER          8          /* check transfer new waiter */
#define     CLASS_OLDWAITER          9          /* check transfer old waiter */
#define     CLASS_CHARGETIPWAITER   10          /* charge tip amount for waiter. No longer used. See ItemPrevChargeTipAffection(). Does not Affect totals */
#define     CLASS_CHARGETIPWAITER2  11          /* charge tip#2 amount for waiter, V3.3 No longer used. See ItemPrevChargeTipAffection(). Does not Affect totals */
#define     CLASS_CHARGETIPWAITER3  12          /* charge tip#3 amount for waiter, V3.3 No longer used. See ItemPrevChargeTipAffection(). Does not Affect totals */
#define     CLASS_TAXAFFECT         21          /* tax for affection */
#define     CLASS_TAXPRINT          22          /* tax for print */
#define     CLASS_ADDCHECKTOTAL     31          /* auto addcheck total */
#define     CLASS_MANADDCHECKTOTAL  32          /* manual addcheck total */
#define     CLASS_CASHIEROPENCHECK  41          /* cashier open check */

#define     CLASS_SERVICETIME       51          /* service time,     R3.1 */

#define     CLASS_SERVICE_VAT       61          /* VATable service,     V3.3 */
#define     CLASS_VATAFFECT         62          /* VAT for affection,   V3.3 */
#define     CLASS_SERVICE_NON       63          /* non-VATable service, V3.3 */
#define     CLASS_VAT_SUBTOTAL      64          /* Subtotal for VAT,    V3.3 */

#define     CLASS_LOANAFFECT        71          /* Loan,        Saratoga */
#define     CLASS_PICKAFFECT        72          /* Pickup,      Saratoga */
#define     CLASS_LOANCLOSE         73          /* Loan End,    Saratoga */
#define     CLASS_PICKCLOSE         74          /* Pickup End,  Saratoga */
#define     CLASS_EPT_ERROR_AFFECT    78        /* Affect the Electronic Payment Error totals */

// following defines are used to indicate which of the bonus totals are being used to track some
// type of Electronic Payment transaction errors.
#define     BONUS_ERROR_AFFECT_SALE         0        // the bonus total number affected by an Electronic Payment Error. See CLASS_EPT_ERROR_AFFECT
#define     BONUS_ERROR_AFFECT_VOIDSALE     1        // the bonus total number affected by an Electronic Payment Error. See CLASS_EPT_ERROR_AFFECT
#define     BONUS_ERROR_AFFECT_RETURN       2        // the bonus total number affected by an Electronic Payment Error. See CLASS_EPT_ERROR_AFFECT
#define     BONUS_ERROR_AFFECT_VOIDRETURN   3        // the bonus total number affected by an Electronic Payment Error. See CLASS_EPT_ERROR_AFFECT

/*--------------------------------------------------------------------------
*       PRINT
*       uchMinorClass identifiers.
--------------------------------------------------------------------------*/
#define     CLASS_HEADER             1          /* header */
#define     CLASS_DOUBLEHEADER       2          /* header(double receipt) */
#define     CLASS_TRAILER1           3          /* header/trailer(normal) */
#define     CLASS_TRAILER2           4          /* header/trailer(newcheck,reorder) */
#define     CLASS_PROMOTION          5          /* promotion header */
#define     CLASS_TRAILER3           6          /* header/trailer(ticket) */
#define     CLASS_TRAYTRAILER        7          /* header/trailer(tray total) */
#define     CLASS_TRAILER4           8          /* header/trailer(for soft check) */
#define     CLASS_TRAILER_MONEY      9          /* header/trailer(money),   Saratoga */
#define     CLASS_CHECKPAID         11          /* check paid print */
#define     CLASS_SPLITPAID         12          /* split check paid print, R3.1 */
#define     CLASS_PRINT_TRAIN       21          /* training header */
#define     CLASS_PRINT_PVOID       22          /* transaction void header */
#define     CLASS_POST_RECPT        23          /* post receipt */
#define     CLASS_PARK_RECPT        24          /* parking receipt,  R3.0 */
#define     CLASS_START             31          /* print start */
#define     CLASS_RESTART           32          /* print restart */
#define     CLASS_CP_GUSLINE        33          /* charge posting for guest line # */
#define     CLASS_CP_ERROR          34          /* error print for charge posting and EPT */
#define     CLASS_EPT_TRAILER       35          /* Electronic Payment Terminal trailer/header (Logo)*/
#define     CLASS_EPT_STUB          36          /* Electronic Payment Terminal duplicate print      */
#define     CLASS_ERROR             37          /* Electronic Payment Terminal error                */
#define     CLASS_CP_STUB           38          /* Charge Post duplicate print       */
#define     CLASS_LOANPICKSTART     39          /* loan/pickup print start, Saratoga */

#define     CLASS_NUMBER            40          /* number print, 2172 */
#define     CLASS_AGELOG            41          /* Age Verify Log,   2172 */
#define     CLASS_FOOD_CREDIT       42          /* food stamp credit        */
#define     CLASS_CHECK_ENDORSE     43          /* Check Endorsement,   DTREE#2 Dec/18/2000 */
#define		CLASS_EPT_STUB_NOACCT   44			/* EPT duplicate not Acct# and date */
#define		CLASS_GIFT_RECEIPT		45			/* Gift Receipt JHHJ*/
#define		CLASS_BALANCE			46			//print Gift Card Balance   

#define     CLASS_MONEY_HEADER      51          /* Menoy Header,        Saratoga */
#define     CLASS_RSPTEXT           61          /* Response Text Print, Saratoga */
#define		CLASS_SPLITA_EPT		62			/* EPT trailer/Promotional trailer */
#define		CLASS_PRINT_TRETURN		63		/* transaction return, CURQUAL_TRANS_RETURN  */
#define     CLASS_RETURNS_TRAILER   64          /* Returns trailer/header (Logo)*/
#define		CLASS_ORDERDEC			72			/* Print Order Dec,  lets keep this same as CLASS_ITEMORDERDEC */


/*--------------------------------------------------------------------------
*       ORDER DECLARATION
*       uchMinorClass identifiers.
--------------------------------------------------------------------------*/
#define		CLASS_ORDER_DEC_NEW		1			/* New Order Declaration on Top */
#define		CLASS_ORDER_DEC_MID		2			/* Order Declaration is pressed mid transaction*/
#define		CLASS_ORDER_DEC_FSC		3			/* Order Declaration FSC is pressed new*/


/*--------------------------------------------------------------------------
*       TRANSACTION CLOSE
*       uchMinorClass identifiers.
--------------------------------------------------------------------------*/
#define     CLASS_CLSTOTAL1          1          /* total #1 */
#define     CLASS_CLSTOTAL2          2          /* total #2 */
#define     CLASS_CLSTOTAL3          3          /* total #3 */
#define     CLASS_CLSTOTAL4          4          /* total #4 */
#define     CLASS_CLSTOTAL5          5          /* total #5 */
#define     CLASS_CLSTOTAL6          6          /* total #6 */
#define     CLASS_CLSTENDER1        11          /* tender #1 */
#define     CLASS_CLSTENDER2        12          /* tender #2 */
#define     CLASS_CLSTENDER3        13          /* tender #3 */
#define     CLASS_CLSTENDER4        14          /* tender #4 */
#define     CLASS_CLSTENDER5        15          /* tender #5 */
#define     CLASS_CLSTENDER6        16          /* tender #6 */
#define     CLASS_CLSTENDER7        17          /* tender #7 */
#define     CLASS_CLSTENDER8        18          /* tender #8 */
#define     CLASS_CLSFOREIGN1       21          /* FC tender #1 */
#define     CLASS_CLSFOREIGN2       22          /* FC tender #2 */
#define     CLASS_CLSNOSALE         31          /* nosale */
#define     CLASS_CLSPO             32          /* paid out */
#define     CLASS_CLSRA             33          /* received on account */
#define     CLASS_CLSTIPSPO         34          /* tips paid out */
#define     CLASS_CLSTIPSDECLARED   35          /* tips declared */
#define     CLASS_CLSCHECKTRANSFER  36          /* check transfer */
#define     CLASS_CLSCANCEL         37          /* cancel */
#define     CLASS_CLSPOSTRECPT      38          /* post receipt */
#define     CLASS_CLSETK            39          /* ETK */
#define     CLASS_CLSCPM            40          /* charge posting */
#define     CLASS_CLSDELIVERY       41          /* close service time at delivery,  R3.1 */
#define     CLASS_CLSSEATTRANS      42          /* close seat# transaction,         R3.1 */
#define     CLASS_CLSPRINT          43          /* post receipt,                    R3.1 */
#define     CLASS_CLSSPLITSERV      44          /* close service total of split,    R3.1 */
#define     CLASS_CLSSPLITSERVALL   45          /* close service total of split,    R3.1 */
#define     CLASS_CLSSEATTRANSDUP1  46          /* 6/4/96 close seat# transaction, for CP full duplicate Rec. R3.1 */
#define     CLASS_CLSSEATTRANSDUP2  47          /* 6/4/96 close seat# transaction, for CP tender duplicate Rec. R3.1 */
#define     CLASS_CLSMONEY          48          /* Money,   Saratoga */
#define     CLASS_CLS_LOAN          101         /* loan transaction,    Saratoga */
#define     CLASS_CLS_PICK          102         /* pick-up transaction, Saratoga */
#define		CLASS_CLS_GIFT_RECEIPT	103			/* Gift receipt*/
#define		CLASS_CLS_PREAUTH_RECEIPT	104		/* Preauth Capture (Finalization) receipt */

/*--------------------------------------------------------------------------
*       OPERATOR CLOSE DATA
*       uchMinorClass identifiers.
--------------------------------------------------------------------------*/
#define     CLASS_CASHIEROUT         1          /* cashier A sign-out */
#define     CLASS_B_OUT              2          /* cashier B sign-out */
#define     CLASS_WAITEROUT         11          /* waiter sign-out */
#define     CLASS_WAITERLOCKOUT     12          /* waiter lock sign-out */
#define     CLASS_MODELOCK          21          /* mode lock key */

#define     CLASS_BARTENDEROUT      31          /* bartender surrogate sign-out, R3.1 */
#define     CLASS_CASHIERMISCOUT    32          /* cashier misc. sign-out */

#define     CLASS_CASINTOUT         41          /* cashier interrupt A sign-out, R3.3 */
#define     CLASS_CASINTB_OUT       42          /* cashier interrupt B sign-out, R3.3 */

#define		CLASS_AUTO_SIGN_OUT		50			//AUTO SIGN OUT ENABLED

/*--------------------------------------------------------------------------
*       MODE-IN DATA
*       uchMinorClass identifiers.
--------------------------------------------------------------------------*/
#define     CLASS_REGMODEIN          1          /* reg mode-in */
#define     CLASS_SUPMODEIN         101         /* loan/pick-up,    Saratoga */

/*--------------------------------------------------------------------------
*       MULTI CHECK DATA
*       uchMinorClass identifiers.
--------------------------------------------------------------------------*/
#define     CLASS_MULTICHECKPAYMENT     1       /* multi check payment */
#define     CLASS_MULTIPOSTCHECK        2       /* multi check, for postcheck */
#define     CLASS_MULTITOTAL            3       /* multi check, grand total */
#define     CLASS_MULTIPOSTTOTAL        4       /* multi check, grand total (postcheck) */




/*--------------------------------------------------------------------------
*       PRINT STATUS
*     used with several member variables for determining the printer target and
*     an options used for the printing.
*     Several different structs have the fsPrintStatus member (ITEMSALES, ITEMTENDER, etc.)
*
*     Also memory resident data structures that control printing activities (TRANCURQUAL).
*        fbNoPrint  indicates which printer targets to supress or to not print to
*        fbCompPrint indicates which printer targets are compulsory and must print
--------------------------------------------------------------------------*/
#define     PRT_RECEIPT             0x0001          /* receipt print,  should be same as  CURQUAL_NO_R */
#define     PRT_JOURNAL             0x0002          /* journal print,  should be same as  CURQUAL_NO_J */
#define     PRT_VALIDATION          0x0004          /* validation print, should be same as CURQUAL_NO_V, often requires MDC_VALIDATION_ADR, MDC 30 Bit B */
#define     PRT_SLIP                0x0008          /* slip print, should be same as CURQUAL_NO_S */

#define     PRT_SLIP_MAX_LINES      100             // max number of lines per slip page

//  codes in the range of 0x0010 through 0x0080 are used for other purposes
//  see the following defines
//     CURQUAL_NO_EPT_LOGO_SIG     0x0010    /* no print EPT Logo or signature line if tender below floor limit */
//     CURQUAL_NO_SECONDCOPY       0x0040    /* no print second copy of receipt */

#define     PRT_SINGLE_RECPT        0x0100          /* issue single receipt */
#define     PRT_DOUBLE_RECPT        0x0200          /* issue double receipt */
#define     PRT_HEADER24_PRINT      0x0400          /* 24 char header print status */
#define     PRT_SPCL_PRINT          0x0800          /* special print status. see also PLU_SPECIAL with pItemSales->ControlCode.auchPluStatus[0] */
#define     PRT_MANUAL_WEIGHT       0x1000          /* manual weight of an item, see mnemonic TRN_MSCALE_ADR  */
#define     PRT_NO_UNBUFFING_PRINT  0x2000          /* do not execute unbuffering print, V3.3 */
#define		PRT_GIFT_BALANCE		0x4000			/* gift card balance */



/* -------------------------------------------------------------------------
*       TAXABLE/VOID/OFF LINE TENDER MODIFIER STATUS (US/CANADA)
*
*		Used with fbModifier of structs such as ITEMTENDER, ITEMCOUPON, ITEMSALES,
*       or UIFREGTRANSOPEN.  The Tax and Food Stamp modifiers are also used with
*       the fbStatus[0] element of the status array of ITEMCOUPON.
*
*    WARNING:  There are dependencies on these specific values which are used in
*              bit operations of various kinds against MDC bits.
*              Do not change any of these lightly!
--------------------------------------------------------------------------*/

#define     VOID_MODIFIER       0x0001            /* void modifier */
#define		VOID_NO_MODIFY_QTY	0x0040			// This item is a void that should not modify coupon count in 
#define     VOIDSEARCH_MODIFIER 0x0080            /* TAR191625, void reg. discount is found  */

#define     TAX_MOD_SHIFT       1                 // US and VAT tax only. number of bits to left shift for tax itemizer status to transform from MOD_USTAXMOD to TAX_MOD_MASK.
#define     TAX_MOD_MASK        0x001e            /* taxable modifier bit mask data, bit 0 excluded because is VOID_MODIFIER */
#define     TAX_MODIFIER1       0x0002            /* affect US taxable itemizer #1, must be same as ODD_MDC_BIT1, also Canadian tax index bit 0 */
#define     TAX_MODIFIER2       0x0004            /* affect US taxable itemizer #2, must be same as ODD_MDC_BIT2, also Canadian tax index bit 1 */
#define     TAX_MODIFIER3       0x0008            /* affect US taxable itemizer #3, must be same as ODD_MDC_BIT3, also Canadian tax index bit 2 */
#define     FOOD_MODIFIER       0x0010            /* affect US Food stamp itemizer, also Canadian tax index bit 3, also VAT service ITM_COUPON_AFFECT_SERV */
#define     WIC_MODIFIER        0x0020            /* WIC override modifier  */

#define     RETURNS_MODIFIER_1  0x0100            // Signifies item is a Returns #1 item
#define     RETURNS_MODIFIER_2  0x0200            // Signifies item is a Returns #2 item
#define     RETURNS_MODIFIER_3  0x0400            // Signifies item is a Returns #3 item
#define     RETURNS_TRANINDIC   0x0100            // signifies item/discount/tender action is a returns action
#define     RETURNS_ORIGINAL    0x0800            // signifies item/discount/tender is from the original transaction for a return
#define     STORED_EPT_MODIF    0x1000            /* Electronic Payment tender was stored modifier */
#define     APPROVED_EPT_MODIF  0x2000            /* Electronic Payment tender was approved modifier */
#define     DECLINED_EPT_MODIF  0x4000            /* Electronic Payment tender was declined modifier */

// used with fbModifier member of ITEMTENDER struct repurposing some bits from ITEMSALES
// such as FOOD_MODIFIER and WIC_MODIFIER.  also used with fbModifier of ITEMTENDERLOCAL struct.
#define     OFFEPTTEND_MODIF    0x0010            /* ept off line tender modifier */
#define     OFFCPTEND_MODIF     0x0020            /* cp off line tender modifier */
#define     OFFCPAUTTEND_MODIF  0x0040            /* credit autho. off line tender modifier */
#define     EPTTEND_SET_MODIF   0x0080            /* Electronic Payment Interface tender modifier */

// used with fbModifier member of ITEMAFFECTION struct
#define     EXCLUDE_VAT_SER     0x02            /* exclude VAT/service, V3.3 */
#define     PRESET_MODIFIER     0x04            /* preset modifier,     V3.3 */
#define     PRICE_LEVEL         0x08            /* price level modifier,V3.3 */

// The range for the following Canada tax indicators needs to fit in a nibble.
// TRN_MASKSTATCAN is used as a mask to obtain the tax type.
// see also definition of STD_PLU_ITEMIZERS and STD_PLU_ITEMIZERS_MOD
// these values are often left shift one to line up with TAX_MOD_MASK bits in pItemSales->fbModifier.
// Often defined constant MOD_CANTAXMOD or MOD_CANTAXMOD shifted  is used to isolate this four bit value.
#define     CANTAX_MEAL         0x01            /* meal item */
#define     CANTAX_CARBON       0x02            /* carbonated beverage item */
#define     CANTAX_SNACK        0x03            /* snack item */
#define     CANTAX_BEER         0x04            /* beer/wine item */
#define     CANTAX_LIQUOR       0x05            /* liquor item */
#define     CANTAX_GROCERY      0x06            /* grocery(non-taxable) item */
#define     CANTAX_TIP          0x07            /* tip item */
#define     CANTAX_PST1         0x08            /* PST1 only item */
#define     CANTAX_BAKED        0x09            /* baked item */
#define		CANTAX_SPEC_BAKED	0x0a			/* Special baked item - (added for Rel 2.0 by JHHJ) */

// The range for the following VAT Int'l tax indicators needs to fit in two bits
// as a two bit value with a range is 1 to 3.
// these values are often left shift one to line up with TAX_MOD_MASK bits in pItemSales->fbModifier.
// Often defined constant MOD_VATTAXMOD or MOD_VATTAXMOD shifted is used to isolate this two bit value.
#define     INTL_VAT1           0x01            /* VATable #1,  V3.3 */
#define     INTL_VAT2           0x02            /* VATable #2,  V3.3 */
#define     INTL_VAT3           0x03            /* VATable #3,  V3.3 */

/*
 * flags used with fbModifer2 of ITEMSALES and UIFREGSALES.
 * flags used also with global indicator variable fchUifSalesMod
**/
#define     EVER_MODIFIER       0x01    /* E-version modifier, 2172     */
#define     VELO_MODIFIER       0x02    /* UPC-Velocity modifier, 2172  */
#define     PCHK_MODIFIER       0x04    /* price check modifier, 2172   */
#define     SKU_MODIFIER        0x08    /* SKU number (merchant's Stock Keeping Unit) entry modifier, 2172. see UIF_SKUNUMBER and LA_F_SKU_NO   */

// flags used with uchDiscountSignifier of ITEMSALES struct
#define     DISC_SIG_DISCOUNT   0x01    // Signifies item has a discount applied.

/* -------------------------------------------------------------------------
*       transaction storage status
	The following bit mask constants are used with the fbStorageStatus member
	of the ITEMSALES, ITEMAFFECTION, ITEMPRINT, and other ITEM type data
	structures to indicates storage types and status.
* ------------------------------------------------------------------------*/

#define     NOT_ITEM_CONS          0x01         /* not item consolidation */
#define     NOT_ITEM_STORAGE       0x02         /* not store item storage */
#define     NOT_CONSOLI_STORAGE    0x04         /* not store consoli storage */
#define     NOT_SEND_KDS           0x08         /* not send to KDS R3.1 */
#define     STORAGE_STATUS_MASK    0x0f         /* storage status mask data */
#define     IS_CONDIMENT_EDIT      0x10         // We are in edit condiment mode


/* -------------------------------------------------------------------------
*       INPUT STATUS
*		used with following variables:
*			UifRegDiaWork.fsInputStatus
*			UifRegData.regsales.fbInputStatus
*			UifRegData.regopeopen.fbInputStatus
*			UifRegData.regopeclose.fbInputStatus
*			UifRegData.regtransopen.fbInputStatus
--------------------------------------------------------------------------*/

#define     INPUT_0_ONLY        0x01            /* '0' input meaning that pData->SEL.INPUT.lData == 0L because digit zero entered */
#define     INPUT_MSR           0x02            /* MSR input R3.1 */
#define     INPUT_DEPT          0x04            /* Entered Dept No. Data, FSC_DEPT   2172  */
#define     INPUT_PRICE         0x08            /* Entered Price Data, FSC_PRICE_ENTER  */


/* -------------------------------------------------------------------------
*       Split Reduce Status(ITEMSALES, ITEMDISC, ITEMCOUPON: fbReduceStatus)    R3.1
--------------------------------------------------------------------------*/
#define     REDUCE_ITEM         0x01            /* Reduced Item */
#define     REDUCE_NOT_HOURLY   0x02            /* Not Affect Hourly */
#define     REDUCE_RETURNED     0x04            // used to indicate that the item is a returned item
#define     REDUCE_PREAUTH      0x08            // used to indicate that the tender is a Preauth tender


/* -------------------------------------------------------------------------
*       number of mnemonic
--------------------------------------------------------------------------*/
#define     NUM_NUMBER_EPT      (19+1)                     // number as used for EPT interface */
//#define     NUM_NUMBER          (19+1)                     // number */
#define     NUM_NUMBER          (STD_NUMBER_LEN+1)         // increase number of digits in #/type entry to 25, US Customs SCER  3/18/03 by cwunn*/
#define     NUM_NUMBER_PADDED   (STD_NUMBER_LEN+9)         // padded for encrypt/decrypt
#define     NUM_NAME            (STD_NUMBER_LEN+1)         // Customer Name, R3.0 increase number of chars to match NUM_NUMBER, US Custons by cwunn 4/2/03 */
#define     NUM_GUEST           (STD_GUESTCHKID_LEN+1)     // GCF No */
#define     NUM_ID              (3+1)                      // ID */
#define     NUM_OPERATOR        (STD_CASHIERNAME_LEN+1)    // PARA_CASHIER_LEN operator, V3.3 */
#define     NUM_SPECMNEM        (STD_SPEMNEMO_LEN+1)       // PARA_SPEMNEMO_LEN special mnemonic */
#define     NUM_TRANSMNEM       (STD_TRANSMNEM_LEN+1)      // PARA_TRANSMNEMO_LEN transaction mnemonic */
#define     NUM_DEPTPLU         (STD_DEPTPLUNAME_LEN+1)    // dept/PLU mnemonic, 2172 */
#define     NUM_HEADER          (STD_HEADERMNEMO_LEN+1)    // header mnemonic */
#define     NUM_LEADTHRU        (STD_LEADTHRUMNEMO_LEN+1)  // PARA_LEADTHRU_LEN lead through mnemonic */
#define     NUM_ROOM            (STD_CPROOMNO_LEN+1)       // charge posting room number */
#define     NUM_GUESTID         (STD_CPGUESTID_LEN+1)      // charge posting guest id */
#define     NUM_FOLIO           (STD_CPFOLIONO_LEN+1)      // charge posting folio number */
#define     NUM_POSTTRAN        (6+1)                      // charge posting posted transaction number */
#define     NUM_CPRSPTEXT       (STD_CPRSPTEXT_LEN+1)      // charge posting response text */
#if defined(GENPOS_REL_020300)
#define     NUM_CPRSPCO_DISP        12                     // charge posting response text displayed in receipt window */
#define     NUM_CPRSPCO             38                     // charge posting response text */
#define     NUM_CPRSPCO_EPT         40                     // charge posting response text,    Saratoga */
#define     NUM_CPRSPCO_CARDHOLDER  38                     // offset in response text for the card holders name for Electronic Payment
#define     NUM_CPRSPCO_CARDLABEL   39                     // offset in response text for the card label for Electronic Payment
#define     NUM_MAX_LINE            30                     // Max. Line#,      Saratoga */
#else
#define     NUM_CPRSPCO_DISP         8                     // charge posting response text displayed in receipt window */
#define     NUM_CPRSPCO              8                     // charge posting response text */
#define     NUM_CPRSPCO_EPT         10                     // charge posting response text,    Saratoga */
#define     NUM_CPRSPCO_CARDHOLDER   8                     // offset in response text for the card holders name for Electronic Payment
#define     NUM_CPRSPCO_CARDLABEL    9                     // offset in response text for the card label for Electronic Payment
#define     NUM_MAX_LINE            10                     // Max. Line#,      Saratoga */
#endif
#define     NUM_MAX_CRSPCO_BUF   ((NUM_CPRSPCO_EPT) * (NUM_CPRSPTEXT))  // size of EPT auchMsgText in struct EEPTRSPDATA
#define     NUM_APPROVAL        6                        // approval code          */
#define     NUM_ETK_NAME        (STD_ETK_NAME_LEN+1)       // ETK Name,   R3.1 */
#define     NUM_SCAN_NUMBER     13                       // Account Number,  V3.3 */
#define     NUM_ACCT_NUMBER     10                       // Account Number,  V3.3 */
#define     NUM_BIRTYHDAY         6                      // birthday entry, 2172  */
#define     NUM_PLU_LEN         STD_PLU_NUMBER_LEN       // STD_PLU_NUMBER_LEN
//#define       NUM_PLU_BCD_LEN     7                    // length of PLU by BCD, 2172
#define     NUM_PLU_SCAN_LEN    (74 + 3)                      // length of PLU by Scan, 2172 */ //RSS14 Expanded is up to 74 numbers also a 3 character prefix
#define     NUM_ACCT_LABEL      10                       // Account# R.W.L,  Saratoga */
#define     NUM_GUESTNAME       16                       // Guest Name,      Saratoga */
#define     NUM_REC_1LINE       24                       // Receipt 1 Line,  Saratoga */
#define     NUM_SLIP_1LINE      40                       // Slip 1 Line,     Saratoga */
#define		NUM_CPNTYPE			13				         // Coupon Type length
#define     NUM_TOTAL_STATUS     7                       // number of bytes in the auchTotalStatus member of TRANSTORAGETOTAL, ITEMTOTAL
#define     NUM_UNIQUE_IDENT    24                       // number of elements in the uchUniqueIdentifier member of TRANGCFQUAL and others
/*
	SR206 Requires a later database change that affects GCF_DATA_BLOCK_SIZE
	
	When making this change, must also consider that sometimes uchSeatNo may
	also have values other than 0 through NUM_SEAT.  There are places where
	the struct member could also have values of 'B' and '0'.
*/
#define		NUM_SEAT			9              // use and finalize seat#, size for a number of seat related data structures
#if defined(GENPOS_REL_020300)
#define     NUM_SEAT_CARD       2              // size for several credit card and gift card related data structures used with seats.
#else
#define     NUM_SEAT_CARD       9              // size for several credit card and gift card related data structures used with seats.
#endif

/*--------------------------------------------------------------------------
        number of array
--------------------------------------------------------------------------*/

#define     NUM_VAT              3              /* number VAT itemizers. see also STD_TAX_ITEMIZERS_VAT  */

/*--------------------------------------------------------------------------
        number of #/Type entries stored US Customs cwunn 4/21/03
--------------------------------------------------------------------------*/

#define     NUM_OF_NUMTYPE_ENT   5              /* number of #/Type entries stored in aszNumber matrix US Customs cwunn 4/21/03
													(length of each entry is NUM_NUMBER) */

// The following define is used to bias PLU quantity in item transactions for non-scalable items. I am
// not sure why quantity is scaled by 1000L but I think it has to do with providing a normalized range
// for quantity for both scalable and non-scalable items. Weighing scales have versions that
// measure in one thousandths. For non-scalable items. pItemSales->lQTY / PLU_BASE_UNIT gives actual quantity.
// See also usage of define ITM_SL_QTY.
//    Richard Chambers, Sep-28-2017
#define PLU_BASE_UNIT           1000L  /* item quantity base unit value to convert non-scalable item quantity to weight in thousandths. Same as ITM_SL_QTY  */

#define STOREGNO_UNIT           1000L  /* used with ulStoreregNo, et. al. CLASS_PARASTOREGNO, SREG_NO_ADR. See ItemCountCons() - store number * 1000L + register number */

/*--------------------------------------------------------------------------
*       ITEM MODULE interface data
--------------------------------------------------------------------------*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


/*--------------------------------------------------------------------------
*       operator open data
---------------------------------------------------------------------------*/

    typedef  struct {
        UCHAR       uchMajorClass;          /* 1:major class */
        UCHAR       uchMinorClass;          /* 2:minor class */
        ULONG       ulID;                   /* 3:cashier/waiter ID */
        ULONG       ulPin;                  /* 4:cashier/waiter secret code or PIN */
        UCHAR       fbInputStatus;          /* 5:input status */
 		ULONG       ulStatusFlags;          // status flags to indicate various conditions from UIFDIAEMPLOYEE
   }UIFREGOPEOPEN;



/*--------------------------------------------------------------------------
*       transaction open data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;          /* major class */
        UCHAR       uchMinorClass;          /* minor class */
        USHORT      usNoPerson;             /* No of person */
        USHORT      fbModifierTransOpen;    /* fbModifier modifier flags such as VOID_MODIFIER */
        ULONG       ulAmountTransOpen;      /* ulAmount previous amount. also used to hold guest check number */
        UCHAR       fbInputStatus;          /* input status such as INPUT_0_ONLY */
    }UIFREGTRANSOPEN;



/*--------------------------------------------------------------------------
*       sales data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;          /* 1:major class */
        UCHAR       uchMinorClass;          /* 2:minor class */
        UCHAR       uchSeatNo;                  /* Seat#,     R3.1 */
        TCHAR       aszPLUNo[NUM_PLU_SCAN_LEN]; /*  PLU No, 2172        */
        USHORT      usDeptNo;               /* 5:dept No(extend FSC) */
        LONG        lQTY;                   /* 6:quantity */
		DUNITPRICE  lUnitPrice;            /* 7:unit price, also @/For storage of total price (SR 143 cwunn)
												Ex: If pricing is 3 for $1.00, and 4 items purchased,
													the total price is $1.33.*/
        UCHAR       uchFsc;                 /* 4:FSC */
        UCHAR       fbInputStatus;          /* 7-1:input status */
        UCHAR       uchAdjective;           /* 8:adjective */
        USHORT      fbModifier;             /* 10:void, tax modifier */
        UCHAR       fbModifier2;            /* Price Check, E-Version */
        TCHAR       aszNumber[NUM_NUMBER];  /* 11:number */
        UCHAR       uchPrintModifier;       /* 16:print modifier */
        DCURRENCY   lDiscountAmount;        /* 22:discount amount */
        UCHAR       uchMajorFsc;            /* 2172 */
		USHORT		usFor;					/* @/FOR storage of package price (SR 143 cwunn)
												Ex: If pricing is 3 for $1.00, the package price is $1.00 */
		USHORT		usForQty;				/* @/For storage of package size (SR 143 cwunn)
												Ex: If pricing is 3 for $1.00, the package size is 3.*/
		USHORT		usKdsLineNo;			// line number on the KDS display
    }UIFREGSALES;




/*--------------------------------------------------------------------------
*       discount data
--------------------------------------------------------------------------*/

   typedef  struct {
        UCHAR       uchMajorClass;          /* 1:major class */
        UCHAR       uchMinorClass;          /* 2:minor class */
        UCHAR       uchSeatNo;              /* Seat Number, V3.3 */
        UCHAR       uchDiscountNo;          /* indicates the sub-type for instance which item discount */
        DCURRENCY   lAmount;                /* 6:discount amount */
        TCHAR       aszNumber[NUM_NUMBER];  /* 7:number */
        USHORT      fbModifier;             /* 9:void, tax modifier */
    }UIFREGDISC;



/*--------------------------------------------------------------------------
*       coupon data
--------------------------------------------------------------------------*/

   typedef  struct {
        UCHAR       uchMajorClass;          /* major class */
        UCHAR       uchMinorClass;          /* minor class */
        USHORT       uchFSC;                 /* FSC  UCHAR to USHORT coupon max 100->300 JHHJ*/
        TCHAR       auchUPCCouponNo[NUM_PLU_LEN]; /*  UPC No, 2172    */
        TCHAR       aszNumber[NUM_NUMBER];  /* number */
        USHORT      fbModifier;             /* void, tax modifier */
    }UIFREGCOUPON;



/*--------------------------------------------------------------------------
*       total key data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;          /* major class */
        UCHAR       uchMinorClass;          /* minor class */
        USHORT      fbModifier;             /* tax modifier */
        TCHAR       aszNumber[NUM_NUMBER];  /* number */
        UCHAR       uchSeatNo;              /* seat#,          R3.1 */
        UCHAR       fbInputStatus;          /* input status,   R3.1 */
    }UIFREGTOTAL;



/*--------------------------------------------------------------------------
*       tender data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;          /* 1:major class */
        UCHAR       uchMinorClass;          /* 2:minor class */
        USHORT      fbModifier;             /* 3:void */
        DCURRENCY   lTenderAmount;          /* 7:tender amount or Quantity if Preset Tender */
        TCHAR       aszNumber[NUM_NUMBER];  /* 11:number */
		SHORT		GiftCard;				/* Gift Card type such as IS_GIFT_CARD_GENERIC or IS_GIFT_CARD_FREEDOMPAY */
		SHORT		GiftCardOperation;		/* Gift Card operation such as GIFT_CARD_TENDER or GIFT_CARD_BALANCE or GIFT_CARD_LOOKUPNAMESS4 */
		UCHAR		uchSpecialProcessingFlags;      // indicates if special process is needed or can be done.
    }UIFREGTENDER;




/*--------------------------------------------------------------------------
*       misc. transaction data
--------------------------------------------------------------------------*/
#define  UIFREGMISC_STATUS_METHOD_MASK    0x00000007        // Mask to use to find out the type of Sign-in/Time-in method, same as UIFDIAEMPLOYEE_STATUS_METHOD_MASK
#define  UIFREGMISC_STATUS_METHOD_SWIPE   0x00000001        // indicates employee used an MSR card
#define  UIFREGMISC_STATUS_METHOD_MANUAL  0x00000002        // indicates employee manually entered employee number and PIN
#define  UIFREGMISC_STATUS_METHOD_BIOMET  0x00000003        // indicates employee used biometrics finger scan
#define  UIFREGMISC_STATUS_METHOD_AUTO    0x00000004        // indicates employee was signed out by Auto Sign Out
#define  UIFREGMISC_STATUS_METHOD_FINAL   0x00000005        // indicates employee was signed out by finalize sign out
#define  UIFREGMISC_STATUS_METHOD_MODE    0x00000006        // indicates employee was signed out by mode change
#define  UIFREGMISC_STATUS_FIRST          0x00000010        // indicates this is the first employee
#define  UIFREGMISC_STATUS_BALANCEDUE     0x00000020        // indicates in balance due state from a partial tender
#define  UIFREGMISC_STATUS_ASKPREDATA     0x00000040        // indicates the Ask #20 key was used to enter Pre Data such as return type

#define  UIFREGMISC_IS_METHOD_MANUAL(x)  (((x) & UIFREGMISC_STATUS_METHOD_MASK) == UIFREGMISC_STATUS_METHOD_MANUAL)
#define  UIFREGMISC_IS_METHOD_SWIPED(x)  (((x) & UIFREGMISC_STATUS_METHOD_MASK) == UIFREGMISC_STATUS_METHOD_SWIPE)
#define  UIFREGMISC_IS_METHOD_BIOSCAN(x)  (((x) & UIFREGMISC_STATUS_METHOD_MASK) == UIFREGMISC_STATUS_METHOD_BIOMET)

    typedef struct {
        UCHAR       uchMajorClass;          /* 1:major class */
        UCHAR       uchMinorClass;          /* 2:minor class */
        TCHAR       aszID[NUM_ID];          /* 3:cashier/waiter ID */
        TCHAR       aszNumber[NUM_NUMBER];  /* 5:number */
        TCHAR       aszGuestNo[NUM_GUEST];  /* 7:guest check No */
        DCURRENCY   lAmount;                /* 8:amount */
        LONG        lQTY;                   /*  quantity,   Saratoga */
        UCHAR       uchTendMinor;           /* for Money,   Saratoga */
        USHORT      fbModifier;             /* 10:void */
        ULONG       ulEmployeeNo;           /* Employee Number,      R3.1 */
        UCHAR       uchJobCode;             /* Job Code to be time in */
		SHORT       sTimeInStatus;          // Time in status used with Connection Engine message such as ETK_NOT_IN_FILE
		ULONG       ulStatusFlags;          // status flags to indicate various conditions
    }UIFREGMISC;



/*--------------------------------------------------------------------------
*       modifier data
--------------------------------------------------------------------------*/
   typedef  struct {
        UCHAR       uchMajorClass;          /* 1:major class */
        UCHAR       uchMinorClass;          /* 2:minor class */
        USHORT      usTableNo;              /* 3:table No */
        USHORT      usNoPerson;             /* 4:No of person */
        UCHAR       uchLineNo;              /* 5:line No */
        UCHAR       uchMenuPage;            /* 7:menu page */
        DCURRENCY   lAmount;                /* 9:amount */
        TCHAR       aszNumber[NUM_NUMBER];  /* 10:number */
        USHORT      fbModifier;             /* Modifier,    Saratoga Nov/27/2000 */
        UCHAR       fbInputStatus;          /* Input status,Saratoga Nov/27/2000 */
   }UIFREGMODIFIER;



/*--------------------------------------------------------------------------
*       other data
--------------------------------------------------------------------------*/
    typedef struct {
        UCHAR       uchMajorClass;          /* 1:major class */
        UCHAR       uchMinorClass;          /* 2:minor class */
        DCURRENCY   lAmount;                /* 4:amount */
        UCHAR       uchFsc;                 /*  FSC (Ask)    2172 */
        TCHAR       aszLabelNo[NUM_PLU_SCAN_LEN];   /* Label No, before analysis 2172        */
        TCHAR       aszNumber[NUM_NUMBER];  /* after analaysis */
    }UIFREGOTHER;




/*--------------------------------------------------------------------------
*       operator close data
--------------------------------------------------------------------------*/
    typedef struct {
        UCHAR       uchMajorClass;          /* 1:major class */
        UCHAR       uchMinorClass;          /* 2:minor class */
        ULONG       ulID;                   /* 3:cashier/waiter ID */
        UCHAR       fbInputStatus;          /* 4:input status, R3.3 */
		ULONG       ulStatusFlags;          // status flags to indicate various conditions from UIFDIAEMPLOYEE
    }UIFREGOPECLOSE;



/*--------------------------------------------------------------------------
*       mode-in data
--------------------------------------------------------------------------*/
    typedef struct {
        UCHAR       uchMajorClass;          /* 1:major class */
        UCHAR       uchMinorClass;          /* 2:minor class */
    }UIFREGMODEIN;

/*--------------------------------------------------------------------------
*       TRANSACTION MODULE interface data
--------------------------------------------------------------------------*/
// helper macro to replace using *(UCHAR *)pData to access the uchMajorClass and
// and to instead use ITEMSTORAGENONPTR(pData)->uchMajorClass instead.
// see also other help macros TRANSTORAGENONPTR() and TRANSTORAGEDATAPTR() for
// raw transaction record data that is still compressed.
#define ITEMSTORAGENONPTR(x) ((ITEMGENERICHEADER *)(x))

	//
	//           WARNING: Post Guest Check System with seat number requires that the first
	//           three struct items for ITEMSALES, ITEMDISC, and ITEMCOUPON be the
	//           major class, minor class, and seat number since Post Guest Check allows
	//           assigning seat numbers and splitting a transaction by seat number.
	//           See function TrnSplGetSeat().
	typedef struct {
        UCHAR           uchMajorClass;              /* major class, shared by all */
        UCHAR           uchMinorClass;              /* minor class, shared by all */
        UCHAR           uchSeatNo;                  /* Seat#, shared by item, discount, coupon    R3.1 */
        UCHAR           fbReduceStatus;             /* Reduce Status, shared by discount, coupon R3.1 */
		UCHAR           uchData[1];
	} ITEMGENERICHEADER;

/*--------------------------------------------------------------------------
*       operator open data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;              /* 1:major class */
        UCHAR       uchMinorClass;              /* 2:minor class */
        ULONG       ulID;                       /* 3:cashier/waiter ID */
        ULONG       ulSecret;                   /* 4:secret code */
        TCHAR       aszMnemonic[NUM_OPERATOR];  /* 5:cashier/waiter mnemonic */
        UCHAR       fbCashierStatus;            /* 6:cashier status. See also TRANMODEQUAL */
        USHORT      usFromTo[2];                /* 7:guest check No range */
        UCHAR       auchStatus[PARA_LEN_CASHIER_STATUS];              /* 8:cashier status, V3.3 sized to CAS_NUMBER_OF_CASHIER_STATUS */
        UCHAR       uchChgTipRate;              /* 9:charge tips rate, V3.3 */
        UCHAR       uchTeamNo;                  /* 10:team no, V3.3 */
		USHORT      usSupervisorID;
		USHORT      usCtrlStrKickoff;
		USHORT      usStartupWindow;
		ULONG		ulGroupAssociations;
    }ITEMOPEOPEN;



/*--------------------------------------------------------------------------
*       transaction open data
--------------------------------------------------------------------------*/
// handle special guest check number generation with Cashier Id by only using the least significant 4 digits
// this special guest check number seems to be used with cashier interrupt and is not used with Store Recall
// but with Post Guest Check and possibly Pre Guest Check.
#define TRANSOPEN_GCNO(ulId) ((USHORT)((ulId) % 10000) + 10000)
// provide conversion to create Guest Check Number combined with Check Digit.  see usage of MDC 12 Bit C.
// see also function RflPerformGcfCd() which separates the two values into a pair.
#define TRANSOPEN_GCNCDV(usGuest,uchCDV) ((uchCDV) ? ((usGuest) * 100 + (uchCDV)) : (usGuest))

	// changes to this struct must be coordinated with changes to structs TRANSTORAGETRANSOPENNON and TRANSTORAGETRANSOPEN.
	// see function TrnStoOpen() for storing into a transaction file.
    typedef struct {
        UCHAR       uchMajorClass;              /* 1:major class */
        UCHAR       uchMinorClass;              /* 2:minor class */
        UCHAR       uchSeatNo;                  /* Seat#,     R3.1 */
        ULONG       ulCashierID;                /* 3: cashier ID */
        ULONG       ulWaiterID;                 /* 4: waiter ID */
        USHORT      usGuestNo;                  /* 6:guest check No */
        UCHAR       uchCdv;                     /* 7:Check Digit Value for guest check No., MDC 12 Bit C */
        USHORT      usTableNo;                  /* 8:table No or unique transaction number MDC 234 Bit D, see ItemModSeatNo1() */
        USHORT      usNoPerson;                 /* 9:No of person */
        UCHAR       uchLineNo;                  /* 10:line No */
        USHORT      fbModifier;                 /* 11:tax modifier, TAX_MODIFIER1 or FOOD_MODIFIER */
        DCURRENCY   lAmount;                    /* 13:previous amount, ITEMTRANSOPEN */
        UCHAR       auchCheckOpen[6];           /* 15:time (HH:MM:SS) and date (YY/MM/DD) check open,  R3.1 */
        USHORT      fsPrintStatus;              /* print status ITEMTRANSOPEN, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        UCHAR       fbStorageStatus;            /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        UCHAR       uchStatus;                  /* 18:MDC status */
        UCHAR       uchTeamNo;                  /* 19:Team No, V3.3 */
        UCHAR		uchHourlyOffset;            /* Hourly total block offset, ITEMTRANSOPEN */
        UCHAR       auchCheckHashKey[6];        /* date and time as mm.dd.hh.mm.ss.0, */
    }ITEMTRANSOPEN;



/*--------------------------------------------------------------------------
*       sales data
--------------------------------------------------------------------------*/

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


#define     SALES_MAX_PPI           30          /* sales module ppi calc. buffer */
#define     SALES_MAX_PM            30
#define		SALES_MAX_PLU			200			//  Max ammount of different PLU items in a transaction   ***PDINU

/* ppi calcuration burrer R3.1 */

typedef struct {
    USHORT   uchPPICode; //ESMITH
	//UCHAR   uchPPICode;
    LONG    lPPIPrice;
    SHORT   sPPIQty;
} PPIDATA;
typedef struct {
    PPIDATA PpiData[SALES_MAX_PPI];
} ITEMSALESPPI;                             /* ppi calc. buffer R3.1 */

/* 2172 */
typedef struct {
    TCHAR   auchPLUNo[STD_PLU_NUMBER_LEN+1];
    UCHAR   uchAdjective;
    LONG    lPMPrice;
    SHORT   sPMQty;
} PMDATA;
typedef struct {
    PMDATA PMData[SALES_MAX_PM];
} ITEMSALESPM;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif


#define ITM_PLU_CONT_OTHER      12                   /* R3.1 for kp control */
#define ITM_PLU_CONT_SIZE       8

#define ITM_PLU_FREEDOMPAY_MIN      8001        // define for PLU range for FreedomPay PLUs
#define ITM_PLU_FREEDOMPAY_MAX      8099        // define for PLU range for FreedomPay PLUs
#define ITM_PLU_DEBIT_CASHBACK_MIN  8101        // define for PLU to use for Debit card cash back
#define ITM_PLU_DEBIT_CASHBACK_MAX  8199        // define for PLU to use for Debit card cash back

#define ITM_CONDIMENT_SIZE      15                  /* size of condiment */

/* for random weight label control status, 2172 */
#define     ITM_TYPE_RANDOM         0x01        /* Random Weight Label */
#define     ITM_TYPE_ARTNO          0x02        /* article # is on Label */
#define     ITM_TYPE_PRICE          0x04        /* price is on Label */
#define     ITM_TYPE_WEIGHT         0x08        /* weight is on Label */
#define     ITM_TYPE_QTY            0x10        /* quantity is on Label */
#define     ITM_TYPE_DEPT           0x20        /* department # is on Label */
#define     ITM_TYPE_NUMBER         0x40        /* number # is on Label */

/* for control status
	Used with member fsLabelStatus of the following structs:
	TRANSTORAGESALESNON, ITEMSALES
**/
#define     ITM_CONTROL_LINK        0x0001      /* sold as link PLU                */
#define     ITM_CONTROL_PCHG        0x0002      /* price change                    */
#define     ITM_CONTROL_NOTINFILE   0x0004      /* not in file build PLU           */
#define     ITM_CONTROL_LINKCOND    0x0008      /* w/ link PLU as condiment        */
#define     ITM_CONTROL_VOIDITEM    0x0010      // void item, fbModifier & VOID_MODIFIER
#define     ITM_CONTROL_ZERO_QTY    0x0020      // pItemSales->lQTY is zero but item in transaction file.  See TrnStoSalesSearch()
#define     ITM_CONTROL_NO_CONSOL   0x0040      // this item should not be consolidate and should be stored unique.  See TrnStoSalesSearch()
#define     ITM_CONTROL_RETURN_OLD  0x0080      // this item is old item in a return and should not be consolidated with new items.  See TrnStoSalesSearch()
#define     ITM_CONTROL_RETURNORIG  0x0100      // return item, fbModifier & VOID_MODIFIER

// Following bitmasks used with the bitmask of blPrecedence to indicate
// various conditions associated with auto coupons and coupone precedence
#define     ITM_PRECEDENCE_APPLIED  0x0001      // indicates that item is being processed as a precedence item
#define     ITM_PRECEDENCE_REMOVE   (~ (ITM_PRECEDENCE_APPLIED) )  // remove the bitmask, must use &= operator
#define     ITM_PPI_APPLIED         0x0002      // indicates that item is being processed has PPI applied to it.same as PRECEDENCEDATA_FLAG_PPI_APPLIED

    typedef struct {
        UCHAR       uchItemType;                    /* item type, upper nibble PLU type, lower nibble Major Dept Number. See PLU_TYPE_MASK_REG 2172 */
        UCHAR       uchReportCode;                      /* 15-2:report code */
        UCHAR       auchPluStatus[ITM_PLU_CONT_OTHER];  /* 15-3:control status[5], see notes with function ItemSalesItemPluTypeInfo(). */
		USHORT		usBonusIndex;
		USHORT		uchPPICode;                    /* condiment's PPI code from the condiment's PLU record */
    }ITEMCONTCODE;

    typedef struct {
        TCHAR           auchPLUNo[NUM_PLU_LEN];     /*  PLU No, 2172    */
        USHORT          usDeptNo;                   /* dept No, 2172 */
        UCHAR           uchPrintPriority;           /* print priority */
		DUNITPRICE      lUnitPrice;                 /* 20-3:price */
        UCHAR           uchAdjective;               /* 20-4:adjective */
        TCHAR           aszMnemonic[NUM_DEPTPLU];   /* 20-6:mnemonic */
        TCHAR           aszMnemonicAlt[NUM_DEPTPLU];   /* alternate mnemonic used for kitchen printing */
        ITEMCONTCODE    ControlCode;                /* 15:dept, report code, status */
		UCHAR			uchCondColorPaletteCode;	/* condiment color palette code */
    }ITEMCOND;

	// WARNING:  There are several places where there is a dependency
	//           on the first few members of this struct being in the
	//           same order and of the same type as some other structs.
	//           See function TrnIsSameZipDataSalesNon() for an example.
	//           Structs that need to be compatible in the first five members
	//           uchMajorClass, uchMinorClass, uchSeatno, auchPLUNo, and usDeptNo
	//           include but are not limited to the following structs:
	//              ITEMSALES, TRANSTORAGESALESNON, 
	//           
	//           WARNING:  Any changes made to the ITEMSALES and TRANSTORAGESALES struct
	//           must be reflected in the compression function, TrnStoSalesComp() as
	//           well as in the uncompression function RflGetStorageItem().
	//
	//           WARNING: Post Guest Check System with seat number requires that the first
	//           three struct items for ITEMSALES, ITEMDISC, and ITEMCOUPON be the
	//           major class, minor class, and seat number since Post Guest Check allows
	//           assigning seat numbers and splitting a transaction by seat number.
	//           See function TrnSplGetSeat().
    typedef struct {
        UCHAR           uchMajorClass;              /* major class */
        UCHAR           uchMinorClass;              /* minor class */
        UCHAR           uchSeatNo;                  /* Seat#, may have a value of 0 or 'B' for Base Transaction     R3.1 */
        TCHAR           auchPLUNo[NUM_PLU_LEN];     /*  PLU No, 2172    */
        USHORT          usDeptNo;                   /* dept No, 2172 or order declaration number (uchOrderDecNum) for CLASS_ITEMORDERDEC */
        LONG            lQTY;                       /* quantity of product sold in thousandths (due to scale units of .001). See PLU_BASE_UNIT */
		DCURRENCY       lProduct;                   /* product or currency amount for product sold */
        SHORT           sCouponQTY;                 /* quantity for coupon */
        SHORT           sCouponCo;                  /* coupon counter */
        USHORT          usItemOffset;               /* Offset of Partner such as voided or non-voided item in transaction, R3.1 */
        UCHAR           uchPrintPriority;           /* print priority */
        USHORT          uchPPICode;                 /* PPI Code   ESMITH */
        UCHAR           fbReduceStatus;             /* Reduce Status,  R3.1 */
        USHORT          fsLabelStatus;              /* label control status from GP, 2172 */
		USHORT			usUniqueID;
		USHORT			usKdsLineNo;			    // line number on the KDS display
		DUNITPRICE      lUnitPrice;                 /* unit price */
        UCHAR           uchAdjective;               /* adjective mnemonic address (see ItemSalesBreakDownAdj()) or order declaration number (uchOrderDecNum) for CLASS_ITEMORDERDEC */
        USHORT          fbModifier;                 /* void, tax modifier, ITEMSALES */
        UCHAR           fbModifier2;                /* price check, e-version, 2172, ITEMSALES */
        TCHAR           aszNumber[NUM_OF_NUMTYPE_ENT][NUM_NUMBER];//increased # of #/Type entries stored US Customs cwunn 4/10/03
        TCHAR           aszMnemonic[NUM_DEPTPLU];   /* mnemonic used for order declare mnemonic*/
        TCHAR           aszMnemonicAlt[NUM_DEPTPLU];   /* alternate mnemonic used for kitchen printing */
        ITEMCONTCODE    ControlCode;                /* dept, report code, status */
        UCHAR           uchPrintModNo;              /* No of print modifiers (P47 mnemonics).  */
        UCHAR           auchPrintModifier[STD_MAX_COND_NUM];      /* list of print modifier mnemonic addresses for this item. */
        USHORT          fsPrintStatus;              /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        UCHAR           uchCondNo;                  /* No of condiments stored in Condiment[]. Condiments list offset by uchChildNo containing count of linked PLUs. */
        UCHAR           uchChildNo;                 /* No of child PLUs such as Linked PLUs stored in Condiment[]. */
        ITEMCOND        Condiment[STD_MAX_COND_NUM];  /* linked PLUs (Condiment[0]) and condiments. Use uchChildNo for offset to first condiment. */
        UCHAR           uchRate;                    /* modifier discount rate */
        DCURRENCY       lDiscountAmount;            /* discount amount, used during PLU totals update to adjust lProduct amount */
        UCHAR           fbStorageStatus;            /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        UCHAR           uchRndType;                    /*  random weight type, 2172 */
        UCHAR           uchPM;                      /*  price multiple, 2172     */
        USHORT          usLinkNo;                   /* link plu no, 2172 */
        USHORT          usFamilyCode;               /* family code, 2172 */
        UCHAR           uchGroupNumber;	/* this is not stored */
        UCHAR           uchTableNumber;	/* this is not stored */
		UCHAR			uchDiscountSignifier; //US Customs cwunn 5/1/03, signifies this item has had a discount applied.
		USHORT			usFor;					/* @/FOR storage of package price (SR 143 cwunn)
												Ex: If pricing is 3 for $1.00, the package price is $1.00 */
		USHORT			usForQty;				/* @/For storage of package size (SR 143 cwunn)
												Ex: If pricing is 3 for $1.00, the package size is 3.*/
		UCHAR			uchColorPaletteCode;	/* ColorPalette Address, see also use of MLD_ATTRIB_REV_VIDEO for reverse video. ITEMSALES */
        UCHAR			uchHourlyOffset;        /* Hourly total block offset, ITEMSALES */
		USHORT			usTareInformation;		//tare information for specific PLU. ITEMSALES
		USHORT			blPrecedence;			// Precedence allowed?
		USHORT			usReasonCode;           //a reason code associated with this operator action. ITEMSALES
}ITEMSALES;

#define  PRECEDENCEDATA_FLAG_PPI_APPLIED    (ITM_PPI_APPLIED)

// Structure added for PPI and Coupon Precedence  ***PDINU
typedef struct {
    TCHAR       auchPLUNo[NUM_PLU_LEN]; // PLU No
	USHORT      fPpiStatus;             // Status of PPI operations
	USHORT      usSalesPpiIndex;        // index into ItemSalesPpi.PpiData[] for the PPI data for this item
	ITEMSALES	ItemSales;				// Stores the item sales information of each PLU
} PRECEDENCEDATA;


/*--------------------------------------------------------------------------
*       discount data
--------------------------------------------------------------------------*/


#define ITM_DISC_HOURLY         0x20        /* hourly affection */

/* print status */
#define ITM_DISC_SINGLE_REC     0x0100      /* issue single receipt */
#define ITM_DISC_DOUBLE_REC     0x0200      /* issue double receipt */

//-------------------------------------------------------------------------------
// Bit masks used with auchDiscStatus of ITEMDISC struct
//
// WARNING: Meaning of MDC bit settings will vary between Item Discount and Transaction Discount.
//          The first two pages are similar but be careful after that.
//
//          Also note that legacy Item Discount and Transaction Discount were both only two keys each.
//          So before GenPOS Rel 2.0.0 back in 2003, with NHPOS 1.4 there were only two Item Discount keys
//          and only two Transaction Discount keys. There were only two Discount Itemizers.
//
//          We have added more Item Discount keys and more Transaction Discount keys however maintained only the
//          original two Discount Itemizers.
//
// following defines used with auchDiscStatus[0] to indicate MDC settings for
// page 1 and page 2 of the Discount Key's MDC settings
#define     DISC_ALLOW_OVERRIDE		0x01    // MDC Bit D of first page, Allow discount amount to be overrident
#define     DISC_RATE_OVERRIDE		0x02    // MDC Bit C of first page, Percent (or Amount) override allowed if Bit D is on (DISC_ALLOW_OVERRIDE)
#define		DISC_AFFECT_TAXABLE1	0x04    // MDC Bit B of first page, Affect US taxble itemizer #1, VAT Int'l tax index bit 0. see also PLU_AFFECT_TAX1
#define		DISC_AFFECT_TAXABLE2	0x08    // MDC Bit A of first page, Affect US taxble itemizer #2. see also PLU_AFFECT_TAX2
#define		DISC_AFFECT_TAXABLE3	0x10    // MDC Bit D of second page, Affect US taxble itemizer #3. see also PLU_AFFECT_TAX3
#define		DISC_NOTAFFECT_HOURLY	0x20    // MDC Bit C of second page, Do not modify hourly totals
#define		DISC_SUP_INTV_REQD		0x40    // MDC Bit B of second page, Supervisor Intevention required
#define		DISC_VALDATION_PRINT	0x80    // MDC Bit A of second page, Compulsory validation print

// following defines used with auchDiscStatus[1] to indicate MDC settings for
// page 3 of the Item Discount Key's MDC settings
#define     DISC_SURCHARGE_KEY		0x01    // MDC Bit D of third page, Item Discount or Transaction Discount, Key is Discount or Surcharge
#define     DISC_AFFECT_TOTAL		0x02    // MDC Bit C of third page, Item Discount ONLY!!, Key affect Dept./PLU totals
#define		DISC_AFFECT_FOODSTAMP	0x04    // MDC Bit B of third page, Item Discount ONLY!!, Key affect Food stamp itemizer
#define		DISC_AFFECT_BONUS_09	0x08    // MDC Bit A of third page, Item Discount ONLY!! Bonus varies, 9 - 14

// following defines used with auchDiscStatus[1] to indicate MDC settings for
// page 3 of the Transaction Discount Key's MDC settings. Some unused for Transaction Discount #1 and #2.
#define     DISC_AFFECT_REGDISC_AFFECT	0x02    // MDC Bit C of third page, Transaction Discount ONLY!!, Key affect Discount Itemizers, not Main Itemizer
#define		DISC_AFFECT_REGDISC_1OR2	0x04    // MDC Bit B of third page, Transaction Discount ONLY!!, Key affect Discount Itemizer #2 if 1, Discount Itemizer #1 if 0
#define		DISC_AFFECT_REGDISC_FOOD	0x08    // MDC Bit A of third page, Transaction Discount ONLY!!, Key affect Food Stamp Itemizer if US Tax System.

// following defines used with auchDiscStatus[1] to indicate MDC settings for
// page 4 of the Item Discount Key and the Transaction Discount Key's MDC settings. See MDC_MODID15_ADR, etc.
#define		DISC_SUPINTV_HALO		    0x10    // MDC Bit D of fourth page, Sup Intv. over HALO or HALO setting

/* --- auchDiscStatus[2] for Item Discount --- */
//  Upper nibble of auchDiscStatus[2] (0xf0) used to hold Canadian tax modifier status.
//  Lower nibbile of auchDiscStatus[0] for US and VAT Int'l tax modifier status.
//  See functions ItemDiscItemDiscModif() or PrtGetSISymItemDiscCanada().
#define ITM_DISC_AFFECT_DISC1   0x01        /* Affect Discountable Itemizer #1, indicated by ItemSales.ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC1 */
#define ITM_DISC_AFFECT_DISC2   0x02        /* Affect Discountable Itemizer #2, indicated by ItemSales.ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC2 */
#define ITM_DISC_AFFECT_SERV    0x04        /* Affect Service(Int'l only),   V3.3, indicated by ItemSales.ControlCode.auchPluStatus[1] & 0x08 */

#define     DISC_STATUS_2_REGDISC1  0x01         // regular discountable 1
#define     DISC_STATUS_2_REGDISC2  0x02         // regular discountable 2

/*
* --------------------------------------------------------------------------
    Discount Module's Define Declaration
* --------------------------------------------------------------------------
*/
#define     DISC_MDC_TAX_MASK		0x1c    // auchDiscStatus[0] mask for US Taxable #1, #2, and #3 flags. VAT Int'l tax index.
#define     DISC_TIP_TAX_MASK		0x07    // auchDiscStatus[1] mask for Taxable #1, Taxable #2, and Taxable #3 flags.

#define ITM_DISC_TAXCALCED      0x01        /* tax is already calculated, TAR191625, used with fbVoidDiscStatus of ITEMVOIDDISC union.  */

/*
 *  Charge Tips defines. Charge Tips is a kind of discount/surcharge and uses ITEMDISC struct.
 *  There are three Charge Tips MDC pages, MDC Address 23, 24, and 25. These are available in PEP
 *  P1 MDC dialog in the Internal Environment section. See MDC_CHRGTIP1_ADR, MDC_CHRGTIP2_ADR, and MDC_CHRGTIP3_ADR
 *
 *  Charge Tips page #3, MDC 25, contains Canadian tax for Charge Tips key.
*/
#define  DISC_CHRGTIP_ALLOW_AUTO_MAN    0x01     // MDC 23 Bit D indicates if Charge Tip is auto calculated or auto or manual calculated
#define  DISC_CHRGTIP_ALLOW_OVERRIDE    0x02     // MDC 23 Bit C indicates if override of Charge Tip allowed if D is on
#define  DISC_CHRGTIP_SUP_INTV_REQD     0x04     // MDC 23 Bit B indicates if Charge Tips requires Supervisor Intervention or not
#define  DISC_CHRGTIP_ALLOW_ZRO_ITEM    0x08     // MDC 23 Bit A indicates if Charge Tips Override allowed if zero (0) itemizer
#define  DISC_CHRGTIP_PRNT_ITEM_RECPT   0x10     // MDC 24 Bit D indicates if Charge Tips Print item receipt or not
#define  DISC_CHRGTIP_PRNT_SNGL_DBLE    0x20     // MDC 24 Bit C indicates if Charge Tips Print Single or Double receipt if D is on
#define  DISC_CHRGTIP_AC86_12_RATE      0x40     // MDC 24 Bit B indicates if Charge Tips use AC 86 Address 12 for Bonus #8 rate or for Pre-Auth rate


	typedef struct {
        DCURRENCY   lVoidDiscNonAffectTaxable[STD_TAX_ITEMIZERS_MAX];   /* TAR191625 */
        DCURRENCY   lVoidDiscFSDiscTaxable[STD_TAX_ITEMIZERS_MAX];	    /* TAR191625 */
		UCHAR		fbVoidDiscStatus;
	} ITEMUSVOIDDISC;

	typedef struct {
        DCURRENCY   lVoidDiscTax[STD_PLU_ITEMIZERS_PLUS];         /* TAR191625 */
		UCHAR	    fbVoidDiscStatus;
	} ITEMCANVOIDDISC;

	typedef struct {
        DCURRENCY   lVoidServiceable;                     /* serviceable itemizer */
        DCURRENCY   lVoidVATSer[STD_TAX_ITEMIZERS_MAX];   /* VATable with service */
        DCURRENCY   lVoidVATNon[STD_TAX_ITEMIZERS_MAX];   /* VATable without service */
		UCHAR		fbVoidDiscStatus;
	} ITEMINTVOIDDISC;

	typedef union {
		ITEMUSVOIDDISC  USVoidDisc;
		ITEMCANVOIDDISC CanVoidDisc;
		ITEMINTVOIDDISC IntlVoidDisc;
	} ITEMVOIDDISC;   /* TAR191625 */

	// WARNING:  Any changes made to this struct ITEMDISC should also be made to
	//           structs TRANSTORAGEDISCNON and TRANSTORAGEDISC which are used during the storing of a transaction.
    typedef struct {
        UCHAR       uchMajorClass;              /* major class */
        UCHAR       uchMinorClass;              /* minor class */
        UCHAR       uchSeatNo;                  /* Seat#, may have a value of 0 or 'B' for Base Transaction  R3.1 */
        UCHAR       fbReduceStatus;             /* Reduce Status,  R3.1 */
        DCURRENCY   lAmount;                    /* discount amount */
        LONG        lQTY;                       /* quantity of product sold in thousandths (due to scale units of .001). See PLU_BASE_UNIT */
        TCHAR       auchPLUNo[NUM_PLU_LEN];     /* PLU No, 2172, TRANSTORAGEDISC begins here   */
        UCHAR       uchAdjective;               /* adjective, ITEMDISC */
        USHORT      usDeptNo;                  /* dept No, 2172, ITEMDISC */
        UCHAR       uchRate;                    /* discount rate. multiply by RFL_DISC_RATE_MUL for use with RflRateCalc1() */
        TCHAR       aszNumber[NUM_NUMBER];      /* number */
        USHORT      fbDiscModifier;             /* void, tax modifier */
        UCHAR       auchDiscStatus[3];          /* status - WARNING these bit masks vary depending on Item Discount or Transaction Discount and tax system in use */
        USHORT      fsPrintStatus;              /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        ULONG       ulID;                       /* waiter ID */
        UCHAR       fbStorageStatus;            /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        ITEMVOIDDISC VoidDisc;				/* TAR191625, this member is not included in same discount comparisons such as void check */
		UCHAR        uchDiscountNo;         // item discount number or other identifier as to sub-type of discount, ITEMDISC
        UCHAR        uchHourlyOffset;       /* Hourly total block offset, ITEMDISC */
		USHORT       usReasonCode;          //a reason code associated with this operator action for GenPOS Rel 2.2.0. ITEMDISC
        TCHAR        aszMnemonic[NUM_DEPTPLU];     /* discount mnemonic   */
    }ITEMDISC;




/*--------------------------------------------------------------------------
*       coupon data
--------------------------------------------------------------------------*/

/* coupon status ( fbStatus[0] ) */
/* bit 7 6 5 4 3 2 1 0
       | | | | | | | +--- void modifier
       | | | | | | +----- affect US taxable modifier #1 or Canadian tax index bit 0 or VAT Int'l tax index bit 0
       | | | | | +------- affect US taxable modifier #2 or Canadian tax index bit 1 or VAT Int'l tax index bit 1
       | | | | +--------- affect US taxable modifier #3 or Canadian tax index bit 2
       | | | +----------- for Int'l VAT (Affect Service due to discountable), V3.3 or Canadian tax index bit 3
       +-+-+------------- not used
*/

// Canadian tax type index is a 4 bit value from 0 to 9 indicating which of the Canadian tax types
// (meal, carbonated beverage, etc.) is to be applied with the coupon.

/* coupon status - taxable itemizers affected ( fbStatus[0] ) */
/* fbStatus[0]  0x01 - void modifier,  V3.3, same as VOID_MODIFIER */
/* fbStatus[0]  0x02 - affect US taxable itemizer #1,  V3.3, same as TAX_MODIFIER1 */
/* fbStatus[0]  0x04 - affect US taxable itemizer #2,  V3.3, same as TAX_MODIFIER2 */
/* fbStatus[0]  0x08 - affect US taxable itemizer #3,  V3.3, same as TAX_MODIFIER3 */
/* fbStatus[0]  0x10 - affect US food stamp itemizer,  V3.3, same as FOOD_MODIFIER */
#define ITM_COUPON_AFFECT_SERV        0x10        /* fbStatus[0] affect service - International VAT only,  V3.3 */

/* coupon status ( fbStatus[1] ) */
#define ITM_COUPON_AFFECT_DISC1 0x01        /* fbStatus[1] affect discount itemizer # 1 */
#define ITM_COUPON_AFFECT_DISC2 0x02        /* fbStatus[1] affect discount itemizer # 2 */
#define ITM_COUPON_SINGLE_TYPE  0x04        /* fbStatus[1] single type coupon */
#define ITM_COUPON_HOURLY       0x20        /* fbStatus[1] affect hourly itemizer */

#define AUTO_CPN_NUM			  50		/* Max Number of Auto Coupons, see function ItemAutoCouponCheck() */

	// Changes to this struct, ITEMCOUPON, must be synchronized with the same changes to the
	// structs TRANSTORAGECOUPONNON, the non-compressed part, and TRANSTORAGECOUPON, the compressed part.
	//
	// Also will be required changes to the uncompress function RflGetCouponItem() and the
	// compress function TrnStoCouponComp() which handle the compress/uncompress transformation.
	// See also struct COMCOUPONPARA used in file PARAMCPN to store AC161 coupon provisioning data.
    typedef struct {
        UCHAR   uchMajorClass;              /* major class */
        UCHAR   uchMinorClass;              /* minor class */
        UCHAR   uchSeatNo;                  /* Seat#, may have a value of 0 or 'B' for Base Transaction    R3.1 */
        UCHAR   fbReduceStatus;             /* Reduce Status,  R3.1 */
        UCHAR   uchCouponNo;                /* coupon unique number */
        TCHAR   auchUPCCouponNo[NUM_PLU_LEN]; /*  UPC No, 2172    */
        SHORT   sVoidCo;                    /* void counter */
        USHORT  usItemOffset[STD_MAX_NUM_PLUDEPT_CPN];  /* Dept/PLU offset pointer for coupon, R3.1 */
        DCURRENCY    lAmount;                    /* coupon amount */
        DCURRENCY    lDoubleAmount;              /* coupon amount for future use */
        DCURRENCY    lTripleAmount;              /* coupon amount for future use */
        USHORT  fbModifier;                 /* void, tax modifier */
        UCHAR   fbStatus[2];                /* coupon status, sized OP_CPN_STS_SIZE, such as taxable itemizers affected, etc. see COMCOUPONPARA.uchCouponStatus */
        TCHAR   aszMnemonic[NUM_DEPTPLU];   /* mnemonic */
        TCHAR   aszNumber[NUM_NUMBER];      /* number */
        USHORT  fsPrintStatus;              /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        UCHAR   uchNoOfItem;                /* no. of sales items */
        TCHAR   auchPLUNo[NUM_PLU_LEN];     /* target PLU number for affection, 2172 */
        USHORT  usDeptNo;                   /* target Dept number for affection , 2172 */
        UCHAR   uchAdjective;               /* target adjective number for affection, 2172*/
        UCHAR   fbStorageStatus;            /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        UCHAR	uchHourlyOffset;            /* Hourly total block offset, ITEMCOUPON */
		USHORT	usBonusIndex;
		USHORT  usReasonCode;               //a reason code associated with this operator action.
    }ITEMCOUPON;

    /* structure for coupon search function */

// The following flags are used with the fbStatus member of ITEMCOUPONSALES
// to indicate processing status for a coupon.
#define ITM_COUPON_PLU_REC      0x00        /* target sales item is PLU item  WARNING: do no use with bitwise operators such as AND & */
#define ITM_COUPON_DEPT_REC     0x01        /* target sales item is Dept item */
#define ITM_COUPON_REC_MASK     0x01        /* bit mask for record type */
#define ITM_COUPON_NOT_FOUND    0x10        /* target item is not found in storage */
#define ITM_COUPON_FIND_MASK    0x10        /* bit mask for found status */
#define ITM_COUPON_RESERVED     0xee        /* coupon status reserved area */

    typedef struct {
        TCHAR   auchPLUNo[NUM_PLU_LEN];     /* target PLU number, 2172 */
        USHORT  usDeptNo;                   /* target Dept number, 2172 */
        UCHAR   uchAdjective;               /* target adjective number */
        USHORT  usItemOffset;               /* offset in transaction storage */
        UCHAR   fbStatus;                   /* target item status */
        UCHAR   uchGroup;                   /* condition group, see TrnStoIsAllItemFound() for usage. */
    } ITEMCOUPONSALES;


/* ItemCouponSearch.fbStatus */
#define ITM_COUPON_SINGLE       0x01        /* Single Type Coupon */

// for auchStatus[] bit usage see description for COMCOUPONPARA.uchCouponStatus[] and
// ITEMCOUPON.fbStatus[] bit fields
// ParaPlu.ContPlu.auchContOther[1] is assigned to ITEMCOUPONSEARCH.auchStatus[0] in TrnStoCpnUPCSearch().
// Use the PLU_AFFECT_ type defines for testing tax itemizers affected, food stamp, etc.

    typedef struct {
        UCHAR   uchMajorClass;              /* major class */
        UCHAR   uchMinorClass;              /* minor class */
        UCHAR   uchCouponNo;                /* coupon number */
        TCHAR   auchUPCCouponNo[NUM_PLU_LEN]; /*  UPC No, 2172    */
        USHORT  fbModifier;                 /* void, tax modifier */
        UCHAR   fbStatus;                   /* status */
        TCHAR   aszNumber[ NUM_NUMBER ];    /* number */
        UCHAR   uchNoOfItem;                /* no. of sales items */
        UCHAR   auchStatus[2];              /* PLU taxable status, sized OP_CPN_STS_SIZE, 2172, same as COMCOUPONPARA.uchCouponStatus[] */
        SHORT   sValue;                     /* UPC face value, 2172 */
        DCURRENCY    lAmount;                    /* UPC discount amount, 2172 */
        ITEMCOUPONSALES aSalesItem[ STD_MAX_NUM_PLUDEPT_CPN ];    /* target sales item for coupon */
    } ITEMCOUPONSEARCH;



/*--------------------------------------------------------------------------
*       total key data
--------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------------
// Flags that are used as part of the total key provisioning.
// These flags are set by the P60 provisioning and specify various total key behaviors
// that are desired when a total key is pressed.

// WARNING:  These macros all assume they are given a pointer to an ITEMTOTAL structure
//           meaning the address of an ITEMTOTAL structure and not an ITEMTOTAL structure
//           variable itself.
//           If using one of these macros with an ITEMTOTAL structure variable (not a pointer)
//           you will need to use the address of operator (&) as in:
//                 ITEMTOTAL   ItemTotal;
//                         <-- do stuff  -->
//                 if (ITM_TTL_FLAG_PRINT_BUFFERING(&ItemTotal)) {
//                          <--- do stuff if we are doing print buffering   --->
//                 }

/*  type of total key ( auchTotalStatus[ 0 ] / 21 ) and total key number ( auchTotalStatus[ 0 ] % 21 ) */
/* The CHECK_TOTAL_TYPE is used as the total key number and total key type are stored in the same byte. */
#define ITM_TTL_GET_TTLTYPE(uch)                   ( UCHAR )( ( ( UCHAR )( uch ) / CHECK_TOTAL_TYPE ) )
#define ITM_TTL_GET_TTLKEY(uch)                    ( UCHAR )( ( ( UCHAR )( uch ) % CHECK_TOTAL_TYPE ) )
#define ITM_TTL_FLAG_TOTAL_TYPE(pItemTotal)        ((pItemTotal)->auchTotalStatus[0] / CHECK_TOTAL_TYPE)
#define ITM_TTL_FLAG_TOTAL_KEY(pItemTotal)         ((pItemTotal)->auchTotalStatus[0] % CHECK_TOTAL_TYPE)
#define ITM_TTL_CALC_TTLKEYTYPE(k,t)               (UCHAR)(((UCHAR)(k) % CHECK_TOTAL_TYPE) + ((UCHAR)(t) * CHECK_TOTAL_TYPE))

// See function ItemTotalSEPreAffect() for examples of using these tax macros.
// See file ittax.c for tax calculation functions:
//     ItemCurTaxWork() and called functions for US tax calculation
//     ItemCanTax() and called functions for Candian tax calculation
// See Also: defined macro constants beginning with CURQUAL_TOTAL_ which require knowledge of the array element.
//           we want to replace those with the following instead to eliminate duplication.

#define TTL_STAT_FLAG_TAX_1(auchTotalStatus)               (auchTotalStatus[1] & ODD_MDC_BIT0)      // calculate US tax #1 or calculate CAN tax GST
#define TTL_STAT_FLAG_TAX_1_1(auchTotalStatus)             (auchTotalStatus[1] & ODD_MDC_BIT1)      // use taxable itemizer #1 for US tax #1 calculation or calculate CAN tax PST1
#define TTL_STAT_FLAG_TAX_1_2(auchTotalStatus)             (auchTotalStatus[1] & ODD_MDC_BIT2)      // use taxable itemizer #2 for US tax #1 calculation or calculate CAN tax PST2
#define TTL_STAT_FLAG_TAX_1_3(auchTotalStatus)             (auchTotalStatus[1] & ODD_MDC_BIT3)      // use taxable itemizer #2 for US tax #1 calculation or calculate CAN tax PST3
#define TTL_STAT_FLAG_TAX_2(auchTotalStatus)               (auchTotalStatus[1] & EVEN_MDC_BIT0)     // calculate US tax #2
#define TTL_STAT_FLAG_TAX_2_1(auchTotalStatus)             (auchTotalStatus[1] & EVEN_MDC_BIT1)     // use taxable itemizer #1 for US tax #2 calculation
#define TTL_STAT_FLAG_TAX_2_2(auchTotalStatus)             (auchTotalStatus[1] & EVEN_MDC_BIT2)     // use taxable itemizer #2 for US tax #2 calculation
#define TTL_STAT_FLAG_TAX_2_3(auchTotalStatus)             (auchTotalStatus[1] & EVEN_MDC_BIT3)     // use taxable itemizer #3 for US tax #2 calculation
#define TTL_STAT_FLAG_TAX_3(auchTotalStatus)               (auchTotalStatus[2] & ODD_MDC_BIT0)      // calculate US tax #3
#define TTL_STAT_FLAG_TAX_3_1(auchTotalStatus)             (auchTotalStatus[2] & ODD_MDC_BIT1)      // use taxable itemizer #1 for US tax #3 calculation
#define TTL_STAT_FLAG_TAX_3_2(auchTotalStatus)             (auchTotalStatus[2] & ODD_MDC_BIT2)      // use taxable itemizer #2 for US tax #3 calculation
#define TTL_STAT_FLAG_TAX_3_3(auchTotalStatus)             (auchTotalStatus[2] & ODD_MDC_BIT3)      // use taxable itemizer #3 for US tax #3 calculation
#define TTL_STAT_FLAG_NOT_CLEAR_TAX_ITEMIZER(auchTotalStatus)  (auchTotalStatus[2] & EVEN_MDC_BIT0)
#define TTL_STAT_FLAG_PRINT_BUFFERING(auchTotalStatus)     (auchTotalStatus[2] & EVEN_MDC_BIT1)     // CURQUAL_TOTAL_PRINT_BUFFERING
#define TTL_STAT_FLAG_ALLOW_AUTO_COUPON(auchTotalStatus)   (auchTotalStatus[2] & EVEN_MDC_BIT2)     // CURQUAL_TOTAL_ALLOW_AUTO_COUPON
#define TTL_STAT_FLAG_DRAWER(auchTotalStatus)              (auchTotalStatus[2] & EVEN_MDC_BIT3)     // CURQUAL_TOTAL_COMPUL_DRAWER
#define TTL_STAT_FLAG_CRT_1_4(auchTotalStatus)             (auchTotalStatus[3] & 0x0F)
#define TTL_STAT_FLAG_PRINT_DOUBLEWIDE(auchTotalStatus)    (auchTotalStatus[3] & EVEN_MDC_BIT1)     // CURQUAL_TOTAL_PRINT_DOUBLE
#define TTL_STAT_FLAG_PRINT_VALIDATION(auchTotalStatus)    (auchTotalStatus[3] & EVEN_MDC_BIT2)     // CURQUAL_TOTAL_COMPULSRY_VALIDATION
#define TTL_STAT_FLAG_COMPULSORY_SLIP(auchTotalStatus)     (auchTotalStatus[3] & EVEN_MDC_BIT3)     // CURQUAL_TOTAL_COMPULSRY_SLIP
#define TTL_STAT_FLAG_DISPLAY_LCD3(auchTotalStatus)        (auchTotalStatus[4] & ODD_MDC_BIT0)      // CURQUAL_TOTAL_DISPLAY_LCD_3 - indicates display order number on LCD #3
#define TTL_STAT_FLAG_ITEMIZER_SERVICE2(auchTotalStatus)   (auchTotalStatus[4] & ODD_MDC_BIT1)      // indicates to use WorkGCF->fsGCFStatus |= GCFQUAL_SERVICE2, default is GCFQUAL_SERVICE1
#define TTL_STAT_FLAG_ITEMIZER_SERVICE3(auchTotalStatus)   (auchTotalStatus[4] & ODD_MDC_BIT2)      // indicates to use WorkGCF->fsGCFStatus |= GCFQUAL_SERVICE3, default is GCFQUAL_SERVICE1
#define TTL_STAT_FLAG_PRE_AUTH(auchTotalStatus)            (auchTotalStatus[4] & EVEN_MDC_BIT0)
#define TTL_STAT_FLAG_NOCONSOLIDATEPRINT(auchTotalStatus)  (auchTotalStatus[4] & EVEN_MDC_BIT1)     // CURQUAL_TOTAL_NOCONSOLIDATE_PRINT - indicates CURQUAL_TOTAL_NOCONSOLIDATE_PRINT
#define TTL_STAT_FLAG_CRT_5_8(auchTotalStatus)             (auchTotalStatus[5] & 0x0F)
#define TTL_STAT_FLAG_DT_PUSH_BACK(auchTotalStatus)        (auchTotalStatus[5] & EVEN_MDC_BIT1)     // CURQUAL_TOTAL_DT_PUSH_BACK - indicates push back for Flexible Drive Thru terminal


#define ITM_TTL_FLAG_TAX_1(pItemTotal)               TTL_STAT_FLAG_TAX_1((pItemTotal)->auchTotalStatus)      // calculate US tax #1 or calculate CAN tax GST
#define ITM_TTL_FLAG_TAX_1_1(pItemTotal)             TTL_STAT_FLAG_TAX_1_1((pItemTotal)->auchTotalStatus)      // use taxable itemizer #1 for US tax #1 calculation or calculate CAN tax PST1
#define ITM_TTL_FLAG_TAX_1_2(pItemTotal)             TTL_STAT_FLAG_TAX_1_2((pItemTotal)->auchTotalStatus)      // use taxable itemizer #2 for US tax #1 calculation or calculate CAN tax PST2
#define ITM_TTL_FLAG_TAX_1_3(pItemTotal)             TTL_STAT_FLAG_TAX_1_3((pItemTotal)->auchTotalStatus)      // use taxable itemizer #2 for US tax #1 calculation or calculate CAN tax PST3
#define ITM_TTL_FLAG_TAX_2(pItemTotal)               TTL_STAT_FLAG_TAX_2((pItemTotal)->auchTotalStatus)     // calculate US tax #2
#define ITM_TTL_FLAG_TAX_2_1(pItemTotal)             TTL_STAT_FLAG_TAX_2_1((pItemTotal)->auchTotalStatus)     // use taxable itemizer #1 for US tax #2 calculation
#define ITM_TTL_FLAG_TAX_2_2(pItemTotal)             TTL_STAT_FLAG_TAX_2_2((pItemTotal)->auchTotalStatus)     // use taxable itemizer #2 for US tax #2 calculation
#define ITM_TTL_FLAG_TAX_2_3(pItemTotal)             TTL_STAT_FLAG_TAX_2_3((pItemTotal)->auchTotalStatus)     // use taxable itemizer #3 for US tax #2 calculation
#define ITM_TTL_FLAG_TAX_3(pItemTotal)               TTL_STAT_FLAG_TAX_3((pItemTotal)->auchTotalStatus)      // calculate US tax #3
#define ITM_TTL_FLAG_TAX_3_1(pItemTotal)             TTL_STAT_FLAG_TAX_3_1((pItemTotal)->auchTotalStatus)      // use taxable itemizer #1 for US tax #3 calculation
#define ITM_TTL_FLAG_TAX_3_2(pItemTotal)             TTL_STAT_FLAG_TAX_3_2((pItemTotal)->auchTotalStatus)      // use taxable itemizer #2 for US tax #3 calculation
#define ITM_TTL_FLAG_TAX_3_3(pItemTotal)             TTL_STAT_FLAG_TAX_3_3((pItemTotal)->auchTotalStatus)      // use taxable itemizer #3 for US tax #3 calculation
#define ITM_TTL_FLAG_NOT_CLEAR_TAX_ITEMIZER(pItemTotal)  TTL_STAT_FLAG_NOT_CLEAR_TAX_ITEMIZER((pItemTotal)->auchTotalStatus)
#define ITM_TTL_FLAG_PRINT_BUFFERING(pItemTotal)     TTL_STAT_FLAG_PRINT_BUFFERING((pItemTotal)->auchTotalStatus)
#define ITM_TTL_FLAG_ALLOW_AUTO_COUPON(pItemTotal)   TTL_STAT_FLAG_ALLOW_AUTO_COUPON((pItemTotal)->auchTotalStatus)
#define ITM_TTL_FLAG_DRAWER(pItemTotal)              TTL_STAT_FLAG_DRAWER((pItemTotal)->auchTotalStatus)
#define ITM_TTL_FLAG_CRT_1_4(pItemTotal)             TTL_STAT_FLAG_CRT_1_4((pItemTotal)->auchTotalStatus)
#define ITM_TTL_FLAG_PRINT_DOUBLEWIDE(pItemTotal)    TTL_STAT_FLAG_PRINT_DOUBLEWIDE((pItemTotal)->auchTotalStatus)
#define ITM_TTL_FLAG_PRINT_VALIDATION(pItemTotal)    TTL_STAT_FLAG_PRINT_VALIDATION((pItemTotal)->auchTotalStatus)
#define ITM_TTL_FLAG_COMPULSORY_SLIP(pItemTotal)     TTL_STAT_FLAG_COMPULSORY_SLIP((pItemTotal)->auchTotalStatus)
#define ITM_TTL_FLAG_DISPLAY_LCD3(pItemTotal)        TTL_STAT_FLAG_DISPLAY_LCD3((pItemTotal)->auchTotalStatus)         // indicates display order number on LCD #3
#define ITM_TTL_FLAG_ITEMIZER_SERVICE3(pItemTotal)   TTL_STAT_FLAG_ITEMIZER_SERVICE3((pItemTotal)->auchTotalStatus)    // indicates to use WorkGCF->fsGCFStatus |= GCFQUAL_SERVICE3, default is GCFQUAL_SERVICE1
#define ITM_TTL_FLAG_ITEMIZER_SERVICE2(pItemTotal)   TTL_STAT_FLAG_ITEMIZER_SERVICE2((pItemTotal)->auchTotalStatus)    // indicates to use WorkGCF->fsGCFStatus |= GCFQUAL_SERVICE2, default is GCFQUAL_SERVICE1
#define ITM_TTL_FLAG_PRE_AUTH(pItemTotal)            TTL_STAT_FLAG_PRE_AUTH((pItemTotal)->auchTotalStatus)
#define ITM_TTL_FLAG_NOCONSOLIDATEPRINT(pItemTotal)  TTL_STAT_FLAG_NOCONSOLIDATEPRINT((pItemTotal)->auchTotalStatus)   // indicates CURQUAL_TOTAL_NOCONSOLIDATE_PRINT
#define ITM_TTL_FLAG_CRT_5_8(pItemTotal)             TTL_STAT_FLAG_CRT_5_8((pItemTotal)->auchTotalStatus)
#define ITM_TTL_FLAG_DT_PUSH_BACK(pItemTotal)        TTL_STAT_FLAG_DT_PUSH_BACK((pItemTotal)->auchTotalStatus)         // indicates push back for Flexible Drive Thru terminal

	
//----------------------------------------------------------------------------------
//   WARNING - This struct uses a similar structure as does the
//			struct TRANSTORAGETOTAL in file trans.h beginning with
//			the member fbModifier.  Any changes to this struct should
//			be reviewed against struct TRANSTORAGETOTAL.  See
//			function TrnStoTotalComp () in trnstor.c where a memcpy
//			is used to transfer information between a ITEMTOTAL and
//			a TRANSTORAGETOTAL.
    typedef struct {
        UCHAR   uchMajorClass;                  /* 1:major class */
        UCHAR   uchMinorClass;                  /* 2:minor class */
        USHORT  fbModifier;                     /* 3:tax modifier */
        DCURRENCY    lMI;                            /* 4:MI  */
        DCURRENCY    lService;                       /* 8:service total */
        DCURRENCY    lTax;                           /* 12:whole tax amount */
        TCHAR   aszNumber[NUM_NUMBER];          /* 16:number */
        UCHAR   auchTotalStatus[NUM_TOTAL_STATUS];     /* 36:total status,  R3.1,  auchTotalStatus[0] contains total key number and type */
        USHORT  fsPrintStatus;                  /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        USHORT  usOrderNo;                      /* 44:order No or guest check number */
        UCHAR   uchCdv;                         /* 46:guest check number check digit, MDC 12 Bit C */
        UCHAR   fbStorageStatus;                /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        UCHAR   uchLineNo;                      /* 48:line No */
        USHORT  usConsNo;                       /* 49:consecutive No */
        ULONG   ulID;                           /* 51:ID(cashier/waiter) unless uchMinorClass == CLASS_TOTAL2_FS, see ItemTendTotal() */
        SHORT   sTrayCo;                        /* 53:tray item counter */
        USHORT  usTrayNo;                       /* current tray number when using tray total. Incremented each tray total. */
        TCHAR   aszRoomNo[NUM_ROOM];            /* 55:room number */
        TCHAR   aszGuestID[NUM_GUESTID];        /* 61:guest ID */
        TCHAR   aszFolioNumber[NUM_FOLIO];      /* 64:folio number */
        TCHAR   aszPostTransNo[NUM_POSTTRAN];   /* 71:posted transaction number */
        TCHAR   aszCPMsgText[NUM_CPRSPCO_EPT][NUM_CPRSPTEXT];   /* 77:message text */
        TCHAR   auchExpiraDate[NUM_EXPIRA];     /* 477:expiration date */
		USHORT  usReasonCode;                   //a reason code associated with this operator action.
    }ITEMTOTAL;



/*--------------------------------------------------------------------------
*       tender data
--------------------------------------------------------------------------*/
//----------------------------------------------------------------------------------
//   WARNING - This struct uses a similar structure as does the
//			struct TRANSTORAGETENDER in file trans.h beginning with
//			the member fbModifier.  Any changes to this struct should
//			be reviewed against struct TRANSTORAGETENDER.  See
//			function TrnStoTenderComp () in trnstor.c where a memcpy
//			is used to transfer information between a ITEMTENDER and
//			a TRANSTORAGETENDER.
#define  ITEMTENDER_STATUS_0_DECIMAL_3      0x02       // MDC 367, bit D and bit C, same for Native and Foreign Currency
#define  ITEMTENDER_STATUS_0_DECIMAL_0      0x04       // MDC 367, bit D and bit C, same for Native and Foreign Currency
#define  ITEMTENDER_STATUS_0_VALIDATION     0x10       // MDC page 2, bit D for Validation Require
#define  ITEMTENDER_STATUS_0_DRAWEROPEN     0x20       // MDC page 2, bit C for open or not Drawer on Tender
#define  ITEMTENDER_STATUS_1_ROOMCHARGE     0x01       // MDC page 3, bit D for Perform ROOM Charge Operation
#define  ITEMTENDER_STATUS_1_ACCOUNTCHARGE  0x02       // MDC page 3, bit C for Perform ACCOUNT Charge Operation
#define  ITEMTENDER_STATUS_1_CREDITAUTH     0x04       // MDC page 3, bit B for Perform Credit Authorization Operation
#define  ITEMTENDER_STATUS_1_PERFORMEPT     0x08       // MDC page 3, bit A for Perform Electronic Payment Operation

// following are Foreign Currency tender MDC flags.  Foreign Currency uses different MDC page layouts.
// in some cases the same bits mean the same thing such as number of decimal places.  in other cases
// the same setting is on different MDC pages using different bits so the bit mask will differ between
// foreign currency tender and native currency tender.
// NOTE:  function ItemTendFCFastCheck() does check for Foreign Currency as well.
#define  ITEMTENDER_STATUS_FC0_VALIDATION   0x01       // MDC page 1, bit D for Validation Require for Foreign Currency
#define  ITEMTENDER_STATUS_FC1_DECIMAL_6    0x01       // MDC 278 - 281, Euro Enhancement Number of decimal indicator for AC89 conversion rates

// following defines used to over ride usage of some parts of ITEMTENDER struct
#define  ITEMTENDER_CARDTYPE    tchCheckSeqNo
#define ITEMTENDER_CHARGETIPS(x) ((x)->ChargeTipsInfo)
#define ITEMTENDER_EPDNAMES(x) (*((ITEMTENDEREPDNAMES *)((x)->aszCPMsgText[NUM_CPRSPCO_CARDHOLDER]))) 
#define ITEMTENDER_EPDLAST4SSNO(x) (*((ITEMTENDEREPDLAST4SSNO *)((x)->aszFolioNumber))) 
	typedef struct {
		TCHAR   uchSignature;
		UCHAR   uchMinorClass;
		UCHAR   uchMinorTenderClass;
        DCURRENCY    lGratuity;                  /* tip amount */
        USHORT  fbModifier;                 /* void, tax modifier */
        USHORT  fsPrintStatus;              /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        UCHAR   fbReduceStatus;             /* Reduce Status,  R3.1 */
	} ITEMTENDERCHARGETIPS;
	typedef struct {
		TCHAR   aszLastName[NUM_CPRSPTEXT];
		TCHAR   aszFirstName[NUM_CPRSPTEXT];
	} ITEMTENDEREPDNAMES;
	typedef struct {
		TCHAR   aszLast4SSNo[5];
	} ITEMTENDEREPDLAST4SSNO;

	typedef struct {
		UCHAR	auchRecordNumber[NUM_RECORD_NO];  // RecordNo - from DSI Client XML.  record identifier in current batch.
	} ITEMTENDERSRNO;
	typedef struct {
		UCHAR	auchInvoiceNo[NUM_INVOICE_NUM];   // XEPT Invoice Number - generated for DSI Client XML.
	} ITEMTENDERINVNO;
	typedef struct {
		UCHAR	auchReferncNo[NUM_REFERENCE];   // XEPT Reference Number - generated for DSI Client XML. see also ItemSalesRefEntry()
	} ITEMTENDERREFNO;
	typedef struct {
		UCHAR	auchAuthCode[NUM_AUTHCODE];   // XEPT Authorization Code - from DSI Client XML.
	} ITEMTENDERAUTHCODE;

    typedef struct {
        UCHAR   uchMajorClass;                  /* 1:major class */
        UCHAR   uchMinorClass;                  /* 2:minor class */
        UCHAR   fbReduceStatus;                 /* Reduce Status,  R3.1 */
        USHORT  fbModifier;                     /* 3:void, from this point on is same as struct TRANSTORAGETENDER*/
        DCURRENCY    lRound;                         /* 4:round amount */
        ULONG   ulFCRate;                       /* 8:foreign currency rate */
        DCURRENCY    lTenderAmount;                  /* 12:tender amount */
        DCURRENCY    lForeignAmount;                 /* 16:foreign amount */
        DCURRENCY    lBalanceDue;                    /* 20:balance due */
        DCURRENCY    lChange;                        /* 24:change amount */
        DCURRENCY    lFSChange;                      /* Food Stamp change in currency, see ItemTendFSChange() */
        DCURRENCY    lFoodStamp;                     /* Food Stamp change in coupons, see ItemTendFSChange(), TAR111547 */
        DCURRENCY    lGratuity;                      /* gratutity or charge tip ammount */
        USHORT  fsPrintStatus;                  /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
		USHORT  usCheckTenderId;                // Unique tender id for split checks identifying specific tender
		USHORT  usReasonCode;                   //a reason code associated with this operator action.
        TCHAR   aszRoomNo[NUM_ROOM];            /* 52:room number */
        TCHAR   aszGuestID[NUM_GUESTID];        /* 58:guest ID */
        TCHAR   aszFolioNumber[NUM_FOLIO];      /* 61:folio number */
        TCHAR   aszPostTransNo[NUM_POSTTRAN];   /* 68:posted transaction number */
        TCHAR   aszNumber[NUM_NUMBER];          // 28:number
        TCHAR   auchExpiraDate[NUM_EXPIRA];     /* 316:expiration date */
        TCHAR   auchApproval[NUM_APPROVAL];     /* 320:EPT approval code  */
        TCHAR   auchMSRData[NUM_MSRDATA_PADDED];       /* 327:MSR data */
        TCHAR   aszTendMnem[STD_LEADTHRUMNEMO_LEN + 1];          /* Display Mnem for CP, Saratoga  */
        TCHAR   aszCPMsgText[NUM_CPRSPCO_EPT][NUM_CPRSPTEXT];   /* 75:message text, Saratoga */
		ITEMTENDERCHARGETIPS   ChargeTipsInfo;
		TCHAR	tchRoutingNumber[NUM_ROUTING_NUM];	// XEPT routing number, used for check auth
		TCHAR	tchCheckSeqNo[NUM_SEQUENCE_NUM];	//XEPT Check Sequence number, used for check auth, reused with card type see also ITEMTENDER_CARDTYPE
		UCHAR	tchAcqRefData[NUM_ACQREFDATA];	    // XEPT Acquirer Reference Number - used for pre-authorization
		ITEMTENDERAUTHCODE  authcode;               // XEPT authorization Code - from DSI Client XML.
		ITEMTENDERREFNO     refno;		            // XEPT Reference Number generated for processor of transaction for EEPT
		ITEMTENDERINVNO     invno;                  // XEPT Invoice number generated for processor of transaction for EEPT
		ITEMTENDERSRNO      srno;                   // XEPT RecordNo - from DSI Client XML.  record identifier in current batch.
        UCHAR   auchTendStatus[3];                  /* 48:MDC page status which varies depending on tender key number */
        UCHAR   fbStorageStatus;                /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        UCHAR   uchCPLineNo;                    /* 326:charge posting line number */
        UCHAR   uchPrintSelect;                 /* Print Selection for EPT, Saratoga */
        UCHAR   uchTenderOff;                   /* Tender Offset for EPT,           Saratoga */
        UCHAR   uchBuffOff;                     /* Offset of Saved Buffer for EPT,  Saratoga */
		UCHAR	uchPaymentType;					//used for contactless or swipe types of transactions
		UCHAR   uchTenderYear;
		UCHAR   uchTenderMonth;
		UCHAR   uchTenderDay;
		UCHAR   uchTenderHour;
		UCHAR   uchTenderMinute;
		UCHAR   uchTenderSecond;
    }ITEMTENDER;

// Following are the possible values for pUifTender->GiftCard in struct UIFREGTENDER
// See also file itmlocal.h which has similar defines.
#define IS_GIFT_CARD_IS_NOT				0x0000  // UIFREGTENDER * UifRegTender->GiftCard setting to indicate not a gift card tender
#define IS_GIFT_CARD_GENERIC			0x0001  /* Gift Card Flag  **SSTOLTZ*/
#define IS_GIFT_CARD_EPAYMENT			0x0002  /* Employee Payroll Deduction or E-Meal for VCS */
#define IS_GIFT_CARD_FREEDOMPAY         0x0101  // This is a FreedomPay gift card

// Following are the possible values for pWorkGCF->TrnGiftCard[index].Used in struct ITEMSALESGIFTCARD
#define GIFT_CARD_SLOT_OPEN				0		//No Gift Card has been put there
#define GIFT_CARD_USED					1		//Gift Card has been issued/reloaded
#define GIFT_CARD_NOT_USED				2		//Gift Card has not been issued/reloaded
#define GIFT_CARD_TESTED				3		//Gift Card has not been issued/reloaded but has been tested successfully

// Following are the possible values for pUifTender->GiftCardOperation in struct UIFREGTENDER
#define GIFT_CARD_NONE					0		/* no Gift Card used */
#define GIFT_CARD_VOID					2		/* Gift Card Void Issue */
#define GIFT_CARD_BALANCE				3		// Gift Card Balance
#define GIFT_CARD_RELOAD				4		// Gift Card Reload
#define GIFT_CARD_VOID_RELOAD			5		// Gift Card Void Reload
#define GIFT_CARD_ISSUE					6		// Gift Card Issue
#define GIFT_CARD_TENDER				7		// Gift Card Tender
#define GIFT_CARD_PAYMENT				8		// Gift Card Payment
#define GIFT_CARD_VOID_PAYMENT			9		// Gift Card Void Payment
#define GIFT_CARD_LOOKUPNAMESS4			10		// Gift Card lookup by last name and last 4 digits Social Security No.
#define GIFT_CARD_LOOKUPEPDACCTNO		11		// Gift Card lookup by Employee Payroll Deduction Account Number

// See also struct UIFREGTENDER
typedef struct {
	long	aszPrice;
	USHORT	usCommStatus;
	SHORT	GiftCardOperation;                 /* Gift Card operation such as GIFT_CARD_TENDER or GIFT_CARD_BALANCE or GIFT_CARD_LOOKUPNAMESS4 */
	USHORT	usUniqueID;
	SHORT   GiftCard;                          /* Gift Card type such as IS_GIFT_CARD_GENERIC or IS_GIFT_CARD_FREEDOMPAY */
	SHORT	Used;                              /* Gift Card operational status indicating if Gift Card has been issued/reloaded */
	USHORT  auchPluStatus;
	TCHAR	aszNumber[NUM_NUMBER];
	TCHAR   aszTrack2[NUM_MSRDATA_PADDED];
	ITEMTENDERREFNO     refno;		            // XEPT Reference Number generated for processor of transaction for EEPT
	ITEMTENDERAUTHCODE  authcode;               // XEPT authorization Code - from DSI Client XML.
	ITEMTENDER ItemTender;
}ITEMSALESGIFTCARD;

	//need pif.h for DATE_TIME
	#include "PIF.H"

#define ITMSTOREFORWARD_FLAG_IGNORE_DECLINES     0x0001
#define ITMSTOREFORWARD_FLAG_REPLACE_ERROR_TEXT  0x0002
#define ITMSTOREFORWARD_FLAG_SKIP_FAILED         0x0004

	typedef struct {
        UCHAR		uchMajorClass;                  /* 1:major class */
        UCHAR		uchMinorClass;                  /* 2:minor class */
        USHORT		fbModifier;                     /* 3:void, */
		UCHAR		auchPurchase[10];              // tender purchase amount from transaction
		UCHAR		auchGratuity[10];              // tender gratuity from transaction
		USHORT		usGuestNo;						// guest check No of the transaction
		USHORT		usConsNo;						// consecutive No of the transaction
		DATE_TIME   DT; 
		ULONG       ulProcessingFlags;              // flags for special processing in CDsiClient::ForwardStoredTransactions()
        ULONG		ulStoreregNo;                   // store/reg No of the transaction. See ItemCountCons() - encoded as store number * 1000L + register number
		ULONG		ulCashierID;
        DCURRENCY	lTenderAmount;                 /* 12:tender amount */
        DCURRENCY	lGratuity;                     /* raw, unmodified tip amount, possibly negative for returned tips */
		LONG		lAttempts;
		ITEMDISC		      TranChargeTips;
		ITEMTENDERCHARGETIPS  ReturnsTenderChargeTips;
		TCHAR		aszNumber[NUM_NUMBER];          // 28:number
		SHORT		sErrorCode;
		USHORT      usEeptreqdata_Flag;
		LONG        lRawError;                      // lError value (EEPTINTERFACE_SOCK_ERROR) from InterpretXML() in CDsiClient::ForwardStoredTransactions()
		SHORT       sTranStatus;                    // transaction status such as EEPT_DECLINED, EEPT_NOW_PROCESSED, or EEPT_ERROR_DONOTHING
		UCHAR		uchUniqueIdentifier[24];        // Unique identifer for returns as digits, binary coded decimal
		UCHAR		uchUniqueIdentifierReturn[24];  // Unique identifer for transaction being returned as digits, binary coded decimal
		UCHAR	    uchPaymentType;					    //used for contactless or swipe types of transactions
		USHORT      usCheckTenderId;                // Unique tender id for split checks identifying specific tender
		USHORT		usTranType;                     // Transaction Type (Credit, Debit, EBT, etc)
		USHORT		usTranCode;                     // Transaction Code (Sale, Return, Void, etc)
		TCHAR		aszCardHolder[NUM_CPRSPTEXT];   // Card holder name
		TCHAR		aszCardLabel[NUM_CPRSPTEXT];    // 28:number
		ITEMTENDERREFNO refno;		                // XEPT Reference Number generated for processor of transaction for EEPT
		ITEMTENDERINVNO invno;                      // XEPT Invoice number from processor of transaction for EEPT
		ITEMTENDERSRNO  srno;                       // XEPT RecordNo - from DSI Client XML.  record identifier in current batch.
		TCHAR       AcctNumberEncrypted[384];       // encrypted account number and expiration
	}ITMSTOREFORWARD;

/*--------------------------------------------------------------------------
*       misc. transaction data
--------------------------------------------------------------------------*/
//----------------------------------------------------------------------------------
//   WARNING - This struct uses a similar structure as does the
//			struct TRANSTORAGEMISC in file trans.h beginning with
//			the member usID.  Any changes to this struct should
//			be reviewed against struct TRANSTORAGEMISC.  See
//			function TrnStoMiscComp () in trnstor.c where a memcpy
//			is used to transfer information between a ITEMMISC and
//			a TRANSTORAGEMISC.
    typedef struct {
        UCHAR   uchMajorClass;                  /* 1:major class */
        UCHAR   uchMinorClass;                  /* 2:minor class */
        ULONG   ulID;                           /* 3:cashier/waiter ID */
        TCHAR   aszNumber[NUM_NUMBER];          /* 5:number */
        USHORT  usGuestNo;                      /* 6:guest check number */
        UCHAR   uchCdv;                         /* 8:guest check number check digit, MDC 12 Bit C */
        DCURRENCY    lAmount;                   /* 9:amount, ITEMMISC */
        USHORT  fbModifier;                     /* 10:void */
        USHORT  fsPrintStatus;                  /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        UCHAR   fbStorageStatus;                /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        USHORT  usYear;                         /* 13:Year to be time in */
        USHORT  usMonth;                        /* 15:Month to be time in */
        USHORT  usDay;                          /* 17:Day to be time in */
        ULONG   ulEmployeeNo;                   /* 19:Employee Number,   R3.1 */
        UCHAR   uchJobCode;                     /* 21:Job Code to be time in */
        USHORT  usTimeinTime;                   /* 22:Time to be time in */
        USHORT  usTimeinMinute;                 /* 24:Minute to be time in */
        USHORT  usTimeOutTime;                  /* 26:Time to be time out */
        USHORT  usTimeOutMinute;                /* 28:Minute to be time out */
        TCHAR   aszMnemonic[NUM_ETK_NAME];      /* ETK mnemonic,         R3.1 */
        UCHAR   uchTendMinor;           /* tender minor 2172 */
        LONG         lQTY;                      /* quantity 2172, ITEMMISC    */
		DUNITPRICE   lUnitPrice;                /* unit price 2172, ITEMMISC  */
        UCHAR   fchStatus;                      /* foreign cuurency status 2172 */
		USHORT  usReasonCode;                   //a reason code associated with this operator action.
    }ITEMMISC;

#define ITEMTENDERPREAUTH_NOT_VIABLE    0x80         // used with uchMajor and uchMinor to indicate preauth entry to be ignored
#define ITEMTENDERPREAUTH_SPL_DECLINED  0x00000001   // used with ulProcessingFlags to indicate preauth was Declined
    typedef struct {
        UCHAR   uchMajorClass;                  /* 1:major class */
        UCHAR   uchMinorClass;                  /* 2:minor class */
        USHORT  fbModifier;                     /* 3:void, */
        ULONG   ulCashierID;                    /* cashier ID */
		ULONG   ulProcessingFlags;              // special flags
        USHORT  usConsNo;                       // 49:consecutive No
        USHORT  usGuestNo;                      /* guest check No */
        USHORT  usTableNo;                      /* table No */
        USHORT  usSeatNo;                       /* seat No */
        DCURRENCY    lTenderAmountPreauth;           /* 12:tender amount specified in the Preauth */
        DCURRENCY    lTenderAmount;                  /* 12:tender amount specifed in the Preauth Capture */
        DCURRENCY    lGratuity;						/* 12:tender amount */
		ULONG	ulCurrentBlockOffset;           // updated when a Stored Transaction block is read to provide current position.
        TCHAR   aszCPMsgText[NUM_CPRSPCO_EPT][NUM_CPRSPTEXT];   /* 75:message text, Saratoga */
		UCHAR	auchAcqRefData[NUM_ACQREFDATA];	// XEPT Acquirer Reference Number - used for pre-authorization
		UCHAR   auchAuthorize[10];               // XEPT authorized amount from response
        UCHAR   aszPostTransNo[NUM_POSTTRAN];   /* 68:posted transaction number */
		UCHAR	uchPaymentType;					//used for contactless or swipe types of transactions
		UCHAR   auchCheckOpen[6];               // time stamp of when check was opened
		UCHAR   auchCheckHashKey[6];            // hash key for encrypt and decrypt
		UCHAR	auchCardType[12];				// Card Type Code From Processor for the Approved transaction
		UCHAR	auchStoreNo[4];                 // Store Number (Train Number) at the time the block was created.
		USHORT  usReasonCode;                   //a reason code associated with this operator action.
		USHORT  usCheckTenderId;                // Unique tender id for split checks identifying specific tender
		USHORT  usReturnType;
		ITEMDISC	TranChargeTips;
		ITEMTENDERCHARGETIPS  ReturnsTenderChargeTips;
        TCHAR   aszNumber[NUM_NUMBER];          // 28:number
        TCHAR   auchExpiraDate[NUM_EXPIRA];     /* 316:expiration date */
        TCHAR   auchMSRData[NUM_MSRDATA_PADDED];       /* 327:MSR data */
		UCHAR   uchUniqueIdentifier[24];    // Unique identifer for returns as digits, binary coded decimal
		ITEMTENDERAUTHCODE  authcode;               // XEPT authorization Code - from DSI Client XML.
		ITEMTENDERREFNO     refno;		            // XEPT Reference Number generated for processor of transaction for EEPT
		ITEMTENDERINVNO     invno;                  // XEPT Invoice number generated for processor of transaction for EEPT
		ITEMTENDERSRNO      srno;                   // RecordNo - from DSI Client XML.  record identifier in current batch.
		TCHAR	aszCardHolder[NUM_CPRSPTEXT];   // card holder name from swiped card also aszLastName for EPD
		TCHAR	aszCardLabel[NUM_CPRSPTEXT];    // 28:number also aszFirstName for EPD
		TCHAR   AcctNumberEncrypted[384];       // encrypted account number and expiration
    }ITEMTENDERPREAUTH;


/*--------------------------------------------------------------------------
*       modifier data
*
*  WARNING:  Some of these struct members are used for several different purposes
*            depending on the value of uchMinorClass.
*
*  uchMinorClass == CLASS_PRETURN
*      usTableNo contains the return type or minor class from the FSC used.
*
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR   uchMajorClass;              /* 1:major class */
        UCHAR   uchMinorClass;              /* 2:minor class */
        USHORT  usTableNo;                  /* 3:table No or also used for Return type, usReturnType */
        USHORT  usNoPerson;                 /* 4:No of person or also used for Reason Code, usReasonCode */
        UCHAR   uchLineNo;                  /* 5:line No also used for seat number with CLASS_SEATNO  */
        USHORT  fsPrintStatus;              /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        TCHAR   aszName[NUM_NAME];          /* Customer Name,  R3.0 */
    }ITEMMODIFIER;



/*--------------------------------------------------------------------------
*       other data
*   Any changes to this struct ITEMOTHER should be coordinated with changes
*   to struct TRANSTORAGEOTHER and vice versa.
--------------------------------------------------------------------------*/
#define OTHER_MNEM_SIZE    23       // size of the aszMnemonic member of ITEMOTHER minus one. used with _tcsncpy()
    typedef struct {
        UCHAR   uchMajorClass;                  /* 1:major class */
        UCHAR   uchMinorClass;                  /* 2:minor class */
        DCURRENCY    lAmount;                   /* 3:amount, ITEMOTHER */
        DCURRENCY    lBalanceDue;               /* 3:amount, ITEMOTHER */
        DCURRENCY    lChange;                   /* 3:change amount, ITEMOTHER */
        USHORT  fsPrintStatus;                  /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        /* V3.3 */
        UCHAR   uchAction;                      /* action code  */
        UCHAR   uchDayPtd;                      /* daily/ptd    */
        UCHAR   uchCurSave;                     /* current/saved*/
        UCHAR   uchReset;                       /* issued or not*/
        UCHAR   uchError;                       /* error code   */
        UCHAR   uchYear;                        /* year         */
        UCHAR   uchMonth;                       /* month        */
        UCHAR   uchDay;                         /* day          */
        UCHAR   uchHour;                        /* hour         */
        UCHAR   uchMin;                         /* minute       */
        TCHAR   aszMnemonic[24];                /* mnemonics, sized to OTHER_MNEM_SIZE + 1    */
        TCHAR   auchMSRData[NUM_MSRDATA_PADDED];  /* MSR data,    2172, ITEMOTHER   */
        USHORT  husHandle;                      /* Tmp File Handle, 2172 */
        TCHAR   aszNumber[NUM_NUMBER];          /* number,  Saratoga */
    }ITEMOTHER;



/*--------------------------------------------------------------------------
*       affection data
--------------------------------------------------------------------------*/

    typedef struct {
        ULONG          lVATRate;                       /* VAT rate */
        DCURRENCY      lVATable;                       /* VATable amount */
        DCURRENCY      lVATAmt;                        /* VAT amount */
        DCURRENCY      lSum;                           /* sum of VATable & VAT */
        DCURRENCY      lAppAmt;                        /* VAT applied total amount */
    }ITEMVAT;

    typedef struct {
		// these tax itemizer arrays are used for both US and Canadian tax itemizers.
		// there are four Canadian tax indicators (GST, PST1, PST2, PST3) in each itemizer array.
		// there are three US tax indicators (TAX1, TAX2, TAX3) used in each itemizer array.
        DCURRENCY      lTaxable[STD_TAX_ITEMIZERS_MAX];    /* 7: taxable total */
        DCURRENCY      lTaxAmount[STD_TAX_ITEMIZERS_MAX];  /* 9: tax amount */
        DCURRENCY      lTaxExempt[STD_TAX_ITEMIZERS_MAX];  /*11: tax exempt */
        DCURRENCY      lFSExempt[STD_TAX_ITEMIZERS_MAX];   /* foodstamp tax exempt, V6 */
    } USCANTAX;

    typedef union {
        USCANTAX    USCanTax;
        ITEMVAT     ItemVAT[NUM_VAT];       // NUM_VAT
    } USCANVAT;

	// must be coordinate any changes to ITEMAFFECTION struct with changes to TRANSTORAGEAFFECTION struct.
    typedef struct {
        UCHAR       uchMajorClass;                      /* 1:major class */
        UCHAR       uchMinorClass;                      /* 2:minor class */
        DCURRENCY   lAmount;                            /* 3:amount, VAT Service, V3.3, ITEMAFFECTION */
        SHORT       sItemCounter;                       /* 4:item counter */
        SHORT       sNoPerson;                          /* 5:No of person */
        SHORT       sCheckOpen;                         /*   check open counter */
        UCHAR       uchOffset;                          /* 6:hourly block offset, same as uchHourlyOffset other structs. */
        USHORT      fbModifier;                         /*   taxable modifier, ITEMAFFECTION */
        USCANVAT    USCanVAT;
        DCURRENCY   lService[STD_MAX_SERVICE_TOTAL];    /*13: service total */
        DCURRENCY   lCancel;                            /*14: cancel total */
        ULONG       ulId;                               /*15: cashier/waiter ID/Service Time Border(R3.1) */
        USHORT      usGuestNo;                          /*16: guest check No */
        UCHAR       uchTotalID;                         /*17: totalkey Id */
        USHORT      fsPrintStatus;                      /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        UCHAR       fbStorageStatus;                    /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        UCHAR       uchAddNum;                          /*21: addcheck number */
        UCHAR       uchAffecMDCstatus;                  /* 18:MDC status */
        DCURRENCY   lBonus[STD_NO_BONUS_TTL];           /* bonus total for hourly, V3.3 */
    }ITEMAFFECTION;



/*--------------------------------------------------------------------------
*       print data
*	Any changes to this struct need to also be made in struct TRANSTORAGEPRINT
*   since that struct is used for compression in function TrnStoPrintComp();
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR   uchMajorClass;                  /* 1:major class */
        UCHAR   uchMinorClass;                  /* 2:minor class */
        ULONG   ulID;                           /* 3:waiter/cashier ID, ulCashierID or ulWaiterID */
        ULONG   ulStoreregNo;                   /* 5:store/reg No. See ItemCountCons() - encoded as store number * 1000L + register number */
        USHORT  usConsNo;                       /* 9:consecutive No */
        USHORT  fsPrintStatus;                  /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        UCHAR   fbStorageStatus;                /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
        USHORT  usTableNo;                      /* 13:table No */
        USHORT  usGuestNo;                      /* 15:guest check number */
        UCHAR   uchCdv;                         /* 17:guest check number check digit, MDC 12 Bit C */
        UCHAR   uchLineNo;                      /* 18:line No */
        TCHAR   aszNumber[NUM_OF_NUMTYPE_ENT][NUM_NUMBER];//increased # of #/Type entries stored US Customs cwunn 4/10/03
        TCHAR   aszRoomNo[NUM_ROOM];            /* 19:room number */
        TCHAR   aszGuestID[NUM_GUESTID];        /* 25:guest ID */
        TCHAR   aszCPText[NUM_CPRSPCO_EPT][NUM_CPRSPTEXT];   /* message text, Saratoga */
        UCHAR   uchCPLineNo;                    /* 326:charge posting line number */
        UCHAR   uchPrintSelect;                 /* Print Selection for EPT, Saratoga */
        USHORT  usFuncCode;                     /* 268:function code */
        SHORT   sErrorCode;                     /* 270:error code */
        DCURRENCY    lAmount;                        /* 272:amount        *add EPT*/
        DCURRENCY    lMI;                            /* MI,  DTREE#2 Dec/18/2000 */
        UCHAR   uchStatus;                      /* 278:EPT status    *add EPT*/
        USHORT  fbModifier;                     /* 279:Modifier      *add EPT*/
        TCHAR   auchExpiraDate[NUM_EXPIRA];     /* 300:ExpDate       *add EPT*/
        TCHAR   auchApproval[NUM_APPROVAL];     /* 304:approval code *add EPT*/
		ITEMTENDERAUTHCODE  authcode;           // XEPT authorization Code - from DSI Client XML.
		ITEMTENDERREFNO     refno;		        // XEPT Reference Number generated for processor of transaction for EEPT
		UCHAR   uchUniqueIdentifier[24];        // Transaction Unique Identifier to print as bar code if trailer
    }ITEMPRINT;



/*--------------------------------------------------------------------------
*       operator close data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR   uchMajorClass;              /* 1:major class */
        UCHAR   uchMinorClass;              /* 2:minor class */
        ULONG   ulID;                       /* 3:cashier/waiter ID */
        UCHAR   uchUniqueAdr;               /* 4:unique address */
    }ITEMOPECLOSE;



/*--------------------------------------------------------------------------
*       transaction close data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR   uchMajorClass;              /* 1:major class */
        UCHAR   uchMinorClass;              /* 2:minor class */
    }ITEMTRANSCLOSE;



/*--------------------------------------------------------------------------
*       mode-in data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR   uchMajorClass;              /* 1:major class */
        UCHAR   uchMinorClass;              /* 2:minor class */
    }ITEMREGMODEIN;



/*--------------------------------------------------------------------------
*       multi check payment data
*	Any changes to this struct need to also be made in struct TRANSTORAGEMULTICHECK
*   since that struct is used for compression in function TrnStoMultiComp();
--------------------------------------------------------------------------*/

    typedef struct{
        UCHAR   uchMajorClass;                  /* 1:major class */
        UCHAR   uchMinorClass;                  /* 2:minor class */
        DCURRENCY    lService[STD_MAX_SERVICE_TOTAL];   /* 4: service total, TRANSTORAGEMULTICHECK starts here */
        USHORT  usGuestNo;                      /* 5: guest check No */
        UCHAR   uchCdv;                         /* guest check number check digit, MDC 12 Bit C  */
        DCURRENCY    lMI;                            /*    main itemizer */
        USHORT  fsPrintStatus;                  /* print status, lower byte is printer (PRT_RECEIPT), upper byte is status (PRT_SINGLE_RECPT) */
        UCHAR   fbStorageStatus;                /* storage status, indicators for transaction data storage such as NOT_ITEM_CONS or IS_CONDIMENT_EDIT */
    }ITEMMULTI;

/*--------------------------------------------------------------------------
*       EPT data
--------------------------------------------------------------------------*/

    typedef struct{
        TCHAR   auchSTX[1];                  /* 1: start of text */
        TCHAR   auchTransID[8];              /* 2: transaction ID */
        TCHAR   auchActionCode[3];           /* 10: action code */
        TCHAR   auchAcctNumber[20];          /* 13: account number */
        TCHAR   auchExpiraDate[NUM_EXPIRA];  /* 43: account expiration date */
        TCHAR   auchChargeTtl[8];            /* 51: charge total */
        TCHAR   auchAuthoCode[6];            /* 57: authorization code */
        TCHAR   auchTransType[3];            /* 63: transaction type */
        TCHAR   auchProductCode[10];         /* 66: product code */
        TCHAR   auchETX[1];                  /* 76: end of text */
        TCHAR   auchLRC[1];                  /* 77: LRC data */
    }ITEMEPTREQDATA;

    typedef struct{
        TCHAR   auchSTX[1];                  /* 1: start of text */
        TCHAR   auchTransID[8];              /* 2: transaction ID */
        TCHAR   auchActionCode[3];           /* 10: action code */
        TCHAR   auchAcctNumber[20];          /* 13: account number */
        TCHAR   auchExpiraDate[NUM_EXPIRA];           /* 33: account expiration date */
        TCHAR   auchReplyData[18];           /* 37: reply data from host */
        TCHAR   auchETX[1];                  /* 55: end of text */
        TCHAR   auchLRC[1];                  /* 56: LRC data */
    }ITEMEPTRSPDATA;

/*--------------------------------------------------------------------------
*       Temporary File Definition for CP/EPT,           Saratoga
--------------------------------------------------------------------------*/
    typedef struct {
        USHORT      usFSize;                    /* File Size */
        USHORT      usVli;                      /* File Vli */
        UCHAR       uchWriteCnt;                /* File Write Counter R2.0GCA */
        UCHAR       uchPrintCnt;                /* File Write Counter R2.0GCA */
    } TRN_CPHEADER;

    typedef struct {
        USHORT      usPrintStatus;              /* Print Status */
        UCHAR       uchPrintSelect;             /* Print Selection */
        UCHAR       uchPrintLine;               /* No. of Print Line */
    } TRN_CPRSPHEADER;


// --------------------------------------------------------------------------------------

// The following union is used to provide for a buffer size calculation for the buffer
// used with the RflGetStorageItem() function to unzip or decompress a transaction item into.
// Using this union will help to ensure that transaction item changes can be accomodated
// rather than relying on a specific transaction item struct being the largest of any.
typedef struct {
	// in a number of places in the code there is a buffer that is sized
	// as CHAR   achBuff[sizeof(ITEMSALES) + sizeof(ITEMDISC) + 10];
	// this struct part of the union is to duplicate that size. not sure why the extra 10 bytes.
	ITEMSALES    ItemSalesBuff;   // the sales item data
	ITEMDISC     ItemDiscBuff;    // a possible discount associated with the sales item
	UCHAR        Padding[10];     // not sure why a padding of an extra 10 bytes is used in so many places.
} ITEMSALESDISC;

// following union is a type for memory that may hold a variety of different structs
// containing transaction data. previously the use of ITEMSALES was used as a catch all
// however with EMV the ITEMTENDER struct is really large.
// proper engineering request that rather than using the largest struct as a catch all
// we should instead use a union of all the various structs and allow the compiler to
// figure out the appropriate sizing. we will be moving foward with replacing the
// previous use of ITEMSALES as a catch all to using this union.
//
// This union also used to determine a max size for a number of buffers used in
// various places in receipt printing and MLD receipt display on the screen.
typedef union {
	ITEMGENERICHEADER  ItemHeader;
	ITEMSALESDISC      ItemSalesDiscBuff;
	ITEMTENDER         ItemTenderBuff;
	ITEMTOTAL          ItemTotalBuff;
	ITEMCOUPON         ItemCouponBuff;
	ITEMSALES          ItemSalesBuff;
	ITEMDISC           ItemDiscBuff;
	ITEMMISC           ItemMiscBuff;
	ITEMAFFECTION      ItemAffectionBuff;
	ITEMPRINT          ItemPrintBuff;
    ITEMMODIFIER       ItemModifierBuff;
	ITEMOTHER          ItemOtherBuff;
} ITEMDATASIZEUNION;

// --------------------------------------------------------------------------------------

/*--------------------------------------------------------------------------
*       Create Temporary File for CP/EPT,               Saratoga
--------------------------------------------------------------------------*/
#define ITM_FILE_OVERRIDE           0           /* Override */
#define ITM_FILE_APPEND             1           /* Append   */

#define ITM_MAX_GUEST_REC           25          /* Max Record of Guest Name */

#define ITM_FILE1_BASE              512         // Base size for ITM files
#define ITM_FILE1_SIZE              (512 * 4)   /* Tmp#1 File Size */
#define ITM_FILE2_SIZE              (512 * 3)   /* Tmp#2 File Size */
#define ITM_FILE3_SIZE              (512 * 8)   /* Tmp#3 File Size */

#define RSP_SAVE_MAX                10           /* EPT/COM response saxe max R2.0GCA */


extern  SHORT   ItemTendEPTComm(UCHAR uchFuncCode, UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender, ULONG ulNo);       /* Saratoga */
extern  SHORT   ItemTendEPTCommGiftCard(UCHAR uchFuncCode, UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender, ULONG ulNo);       /* Saratoga */

extern SHORT    CliGusLoadTenderTableInit (DATE_TIME *pBatchCloseDateTime);
extern SHORT    CliGusLoadTenderTableAdd (ITEMTENDER *pItemTender);

SHORT   ItemTendCPCreateFile(VOID);
SHORT   ItemTendCPOpenFile(VOID);


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

#endif
/****** End of Definition ******/