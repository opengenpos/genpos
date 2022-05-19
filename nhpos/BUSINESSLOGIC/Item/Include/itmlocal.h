/*
*===========================================================================
* Title       : Item Module Local Headrer File
* Category    : Register Mode Item Module, NCR 2170 US Application
* Program Name: ITMLOCAL.H
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
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev.  :  Name      : Description
*   2/14/95: 03.00.00  :  hkato     : R3.0
*  10/21/98: 03.03.00  :  M.Suzuki  : TAR 89859
*
** NCR2172 **
*
*  10/05/99: 01.00.00  :  M.Teraki  : Added #pragma(...)
*  12/07/99: 01.00.00  : hrkato     : Saratoga
*  04/02/15: 02.02.01  : R.Chambers : GenPOS, updated comments, removed prototypes
*  11/07/15: 02.02.01  : R.Chambers : changed fbStatus to fbTareStatus in struct ITEMMODSCALE
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

#if !defined(ITMLOCAL_H_INCLUDE)
#define ITMLOCAL_H_INCLUDE

#include <csetk.h>
#include <display.h>
#include <csop.h>
#include "eept.h"
#include "cpm.h"
#include "uie.h"
#include <uireg.h>


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif
/*
*===========================================================================
*   Define Declarations
*
*  WARNING:  Some functions that use these as return codes will also
*            return other values from a different name space.  These other
*            namespaces may include codes that conflict or that may have
*            a positive value but still indicate an error.  Some of these
*            other defined codes are:
*                UIF_CANCEL
*                LDT_PROHBT_ADR
*                LDT_KEYOVER_ADR
*===========================================================================
*/
#define ITM_BEEP                        500     /* beep time */
#define COMP_VAL                        1       /* compulsory validation print */
/* definition for charge posting */
#define ITM_CR                          0x0d    /* CR */

#define ITM_COND_NUM_GRPTBL				20		/*Condiment OEP Group and Table Size*/

// Various kinds of status return defines follow. these are used under various circumstances
// some of which are separate from each other so we use the same value for different meanings.
#define ITM_SUCCESS                STD_RET_SUCCESS       /* success status */
#define ITM_CONT                        -1      /* continue status */
#define ITM_PRINT                       0       /* print item               */
#define ITM_NOT_PRINT                   1       /* not print item           */
#define ITM_CANCEL                      100     /* cancel status */
#define ITM_COMP_ADJ                    -1      /* compulsory adj */
#define ITM_EPT_TENDER                  200     /* EPT Tender status */
#define ITM_PRICECHANGE                 -1      /* plu place change, 2172 */

#define ITM_PREVIOUS					3

#define ITM_ERROR                       -1      /* Error for Client,    Saratoga */
#define ITM_REJECT_ASK                  -2      /* Reject Itemize for ASK,          Saratoga */
#define ITM_NO_READ_SIZE				-32		// No size was sent to Read 11-10-3 RPH

#define ITM_SEAT_DISC                   -100    /* V3.3 */

#define ITM_PARTTEND                    -100    /* partial tender for charge post,  Saratoga */
#define ITM_OVERTEND                    -101    /* over tender for charge post,     Saratoga */
#define ITM_REJECT_DECLINED             -102    // Electronic Payment response indicated transaction was declined. See also DECLINED_EPT_MODIF


/* definition for charge posting */
#define ITM_CR                          0x0d    /* CR */

/* #define ITM_CPRESP_MAXLINE              6    Response Msg Print Line  */

/*----- Type of Total Key with Discount,  R3.1 -----*/
#define ITM_NOT_UPDATE                  0       /* Not Update */
#define ITM_UPDATE                      1       /* Update */
#define ITM_NOT_UPDATE_SPL              2       /* Not Update */
#define ITM_UPDATE_SPL1                 3       /* Update, LCD Left */
#define ITM_UPDATE_AFFECT               4       /* Update/Affection */
#define ITM_UPDATE_SPL2                 5       /* Update, LCD Middle */

/*----- Type of Canadian Tax Calculation Split/Normal R3.1 -----*/
#define ITM_NOT_SPLIT                   0       /* Normal */
#define ITM_SPLIT                       1       /* Split Check */

/* --- Definition by V3.3 --- */
#define ITM_SPLIT_IND_TAX_PRNIT         0       /* Tax is total of ind. tax */
#define ITM_SPLIT_WHOLE_TAX_PRNIT       99      /* Tax is whole tax */

/*----- Return of "ItemCommonTaxSystem()",  V3.3 -----*/
// see also function PrtCheckTaxSystem() which does the same thing, returns same values, different defines. <sigh>
// see also function TrnTaxSystem() which does the same thing, returns same values, different defines. <sigh>
#define     ITM_TAX_US              0       /* US Tax System, see also PRT_TAX_US and TRN_TAX_US */
#define     ITM_TAX_CANADA          1       /* Canada Tax System, see also PRT_TAX_CANADA and TRN_TAX_CANADA as well flag MODEQUAL_CANADIAN */
#define     ITM_TAX_INTL            2       /* Int'l VAT System, see also PRT_TAX_INTL and TRN_TAX_INTL as well flag MODEQUAL_INTL */

/*----- Return of "ItemCommonCondimentTax()" ------- CSMALL */
#define		ITM_COND_MAIN_TAX		0
#define		ITM_COND_TAX			1

#define     ITM_DISPLAY             0      // display the affect item to the screen, see ItemTendVATCommon()
#define     ITM_NOT_DISPLAY         1      // don't display the affect item to the screen, see ItemTendVATCommon()

#define ITM_SL_00OR05           5L   /* to test Tare value under MDC 29 bit 0,2 - MDC_SCALE_ADR Bit D and B, scale units 0.005 LB/Kg */
#define ITM_SL_2DIGITS         10L   /* to scale weight Tare value under MDC29 Bit 0 = 0 - MDC_SCALE_ADR Bit D, scale units 0.01 LB/Kg */
#define ITM_SL_QTY           1000L   /* quantity units for normalized non-scalable items quantity. See PLU_BASE_UNIT  */

#define ITM_LEN_SKU5            5       /* SKU Number Length              */

#define     ITM_KTCHN_SEND_MASK     0xf0    /* kitchen send status mask data */
#define     ITM_KTCHN_SEND_MASK2    0x0f    /* kitchen send status mask data */


#define	OEP_CONDIMENT_RESET	0          // reset the fbOepCondimentEdit variable to turn off condiment edit/add
#define	OEP_CONDIMENT_EDIT	1
#define OEP_CONDIMENT_ADD	2

// These bits are used to determin the status of the Credit Card floor amount that was
// set in AC128.  The bits are used in ParaMiscPara.ulMiscPara as staus markers.
// They are also contained in address 9 of ParaMiscPara.ulMiscPara, that is also
// defined as MISC_CREDITFLOORSTATUS_ADR.

#define A128_CC_ENABLE		0x01	// Status bit to determine if the properties are enabled	***PDINU
#define A128_CC_CUST		0x02	// Status bit to not print cutomer receipt	***PDINU
#define A128_CC_MERC		0x04	// Status bit to not print merchant receipt	***PDINU
#define A128_CC_CUST_AUTH	0x08	// Status bit to not print card authorization text on cutomer receipt	***PDINU
#define A128_CC_MERC_AUTH	0x10	// Status bit to not print card authorization text on merchant receipt	***PDINU


// Following defines to be used with function ItemTenderCheckTenderMdc() to indicate
// the tender key page number of an MDC bit to check.
#define MDC_TENDER_PAGE_1        1
#define MDC_TENDER_PAGE_2        2
#define MDC_TENDER_PAGE_3        3
#define MDC_TENDER_PAGE_4        4
#define MDC_TENDER_PAGE_5        5
#define MDC_TENDER_PAGE_6        6

// return values for function ItemTotalType () indicating the type
// of the UifRegTotal information.
#define ITM_TYPE_ERROR          (-1)  // Error, class type unknown or inconsistent
#define ITM_TYPE_SUBTOTAL         0  // Subtotal  => CLASS_UITOTAL1
#define ITM_TYPE_CHECK_TOTAL      1  // Check total  => CLASS_UITOTAL2
#define ITM_TYPE_SERVER_KEY_OUT   2  // Server Key out   => CLASS_UIKEYOUT
#define ITM_TYPE_SERVICE_TOTAL    2  // Service total
#define ITM_TYPE_FINALIZE_TOTAL   3  // Finalize total
#define ITM_TYPE_NONFINALIZE      4  // Non-finalize total
#define ITM_TYPE_TRAY_TOTAL       5  // Tray total
#define ITM_TYPE_FS_TOTAL         6  // Food Stamp Total  => CLASS_UITOTAL9
#define ITM_TYPE_CHECK_FS_TOTAL   7  // Check total for Food Stamp  => CLASS_UITOTAL2_FS
#define ITM_TYPE_SPLIT_KEY       11  // Split key # 1  => CLASS_UISPLIT
#define ITM_TYPE_CASHIER_INTRPT  12  // Cashier Interrupt  => CLASS_UICASINT

// defines for ItemTotalCalTax() that determine how the tax is calculated.
// decision is based on whether TranGCFQualPtr->usGuestNo is zero, operator transaction,
// or non-zero, store/recall transaction.
#define ITM_CALTAX_TRANS_OPER    0
#define ITM_CALTAX_TRANS_SR      1


/*
* --------------------------------------------------------------------------
    Trailer/ Header Print Type Definitions
* --------------------------------------------------------------------------
*/

#define TYPE_STORAGE                     0       /* strored print */
#define TYPE_THROUGH                     1       /* real time print */
#define TYPE_SINGLE_TICKET               2       /* trailer for single ticket */
#define TYPE_DOUBLE_TICKET               3       /* trailer for double ticket */
#define TYPE_STB_TRAIL                   4       /* throuph trailer print for stub */
#define TYPE_CANCEL                      5       /* trailer print for cancel */
#define TYPE_POSTCHECK                   6       /* trailer print for postcheck, store/recall */
#define TYPE_TRANSFER                    7       /* trailer print for check transfer */
#define TYPE_TRAY                        8       /* trailer print for tray total */
#define TYPE_THROUGH_BOTH                9       /* real time print receipt/journal */
#define TYPE_SOFTCHECK                  10       /* soft check print */
#define TYPE_EPT                        11       /* EPT trailer with logo   */
#define TYPE_STORAGE2                   12       /* for duplicate receipt   */
#define TYPE_EPT2                       13       /* for duplicate receipt   */
#define TYPE_SPLITA                     14       /* for Split Check,    R3.1 */
#define TYPE_SPLITB                     15       /* for Split Check,    R3.1 */
#define TYPE_SPLIT_EPT                  16       /* for Split Check,    R3.1 */
#define TYPE_SPLIT_EPT2                 17       /* for Split Check,    R3.1 */
#define TYPE_SPLIT_TENDER               18       /* for Split Tender,   R3.1 */
#define TYPE_MONEY                      19       /* for money,  Saratoga */
#define TYPE_SPLITA_EPT                 20       /* for Split Check with ept logo */
#define TYPE_STORAGE3                   21       /* strored print like TYPE_STORAGE except to journal only */

/*
===========================================================================
    EXETERN
===========================================================================
*/



/*
*===========================================================================
    Structure Type Define Declarations          R3.0  R3.1
*===========================================================================
*/
    typedef struct {
		// these tax itemizer arrays are used for both US and Canadian tax itemizers.
		// there are four Canadian tax indicators (GST, PST1, PST2, PST3) in each itemizer array.
		// there are three US tax indicators (TAX1, TAX2, TAX3) used in each itemizer array.
        DCURRENCY    alTaxable[STD_TAX_ITEMIZERS_MAX];               /* affection taxable itemizer */
        DCURRENCY    alTax[STD_TAX_ITEMIZERS_MAX];                   /* tax amount */
        DCURRENCY    alTaxExempt[STD_TAX_ITEMIZERS_MAX];             /* tax exempt */
        DCURRENCY    lMealLimitTaxable;
        DCURRENCY    lMealNonLimitTaxable;
        USHORT       fsLimitable;
        USHORT       fsBaked;
    } ITEMCOMMONTAX;

#if 0
	// This struct is the same as FDTPARA
    typedef struct {
        UCHAR  uchDelivery;                 /* Delivery Terminal Address */
        UCHAR  uchPayment;                  /* Payment Terminal Address */
        UCHAR  uchSysTypeTerm;              /* FDT System */
        UCHAR  uchTypeTerm;                 /* Terminal Type */
    } ITEMFDTPARA;
#endif

/*
* --------------------------------------------------------------------------
    Transaction Open Module Local Data     R3.0
* --------------------------------------------------------------------------
*/

    typedef struct {
		// these tax itemizer arrays are used for both US and Canadian tax itemizers.
		// there are four Canadian tax indicators (GST, PST1, PST2, PST3) in each itemizer array.
		// there are three US tax indicators (TAX1, TAX2, TAX3) used in each itemizer array.
        UCHAR        fbTransStatus;                          /* transaction status */
        UCHAR        uchCheckCounter;                        /* multi check counter */
        DCURRENCY    lWorkMI;                                /* work MI for multi check */
        DCURRENCY    alCorrectTaxable[STD_TAX_ITEMIZERS_MAX];  /* correction taxable for multi check */
        DCURRENCY    alCorrectTax[STD_TAX_ITEMIZERS_MAX];      /* correction tax amount for multi check */
        DCURRENCY    alCorrectExempt[STD_TAX_ITEMIZERS_MAX];   /* correction tax exempt for multi check */
        DCURRENCY    alMI[STD_MAX_MULTI_CHECK];              /* saved MI for multi check */
        USHORT       ausGuestNo[STD_MAX_MULTI_CHECK];        /* saved GCF No */
        UCHAR        auchCdv[STD_MAX_MULTI_CHECK];           /* saved GCF CDV */
        DCURRENCY    alAddcheck[STD_MAX_MULTI_CHECK][STD_MAX_SERVICE_TOTAL];     /* addcheck total for multi check */
        DCURRENCY    lManualAmt;                             /* manual PB/addcheck balance */
        SHORT        sPigRuleCo;                             /* pig rule counter */
        SHORT        sPrePigRuleCo;                          /* previous pig rule counter  10-21-98 */
    }ITEMTRANSLOCAL;


#define     TROP_DURINGADDCHECK     0x04        /* during manual addcheck */
#define     TROP_OPENPOSTCHECK      0x08        /* print transaction header at postcheck */


/*
* --------------------------------------------------------------------------
    Sales Module Local Data
* --------------------------------------------------------------------------
*/
#define     SALES_NUM_GROUP         10          /* no of oep group no */

    typedef struct {
        USHORT  fbSalesStatus;                  /* sales status */
        UCHAR   fbPrintStatus;                  /* print status */
        UCHAR   uchMenuShift;                   /* menu shift (default) */
        UCHAR   auchOepGroupNo[SALES_NUM_GROUP+1];/* oep group no for condiment R3.1 */
        DATE_TIME Tod;                           /* 2172 */
        UCHAR   uchAdjDefShift;                  /* adjective menu shift of default, 2172 */
        UCHAR   uchAdjKeyShift;                  /* adjective menu shift by Key, 2172 */
        UCHAR   uchAdjCurShift;                  /* adjective menu shift of current, 2172 */
        UCHAR   uchAge;                          /* Customer Age            */
    }ITEMSALESLOCAL;
/*
#define     CLASS_ITM_CONDIMENT     99           condiment's class for item module intenal use
*/
// following flags are used with fbSalesStatus of ITEMSALESLOCAL struct
#define     SALES_ITEMIZE           0x0001        /* indicate start itemization, indicates transaction started */
#define     SALES_PM_COMPLSRY       0x0002        /* compulsory print modifier, see also PLU_COMP_MODKEY */
#define     SALES_COND_COMPLSRY     0x0004        /* compulsory condiment, see also PLU_COMP_CODKEY */
#define     SALES_BOTH_COMPLSRY     0x0006        /* mask for compulsory print modifier and/or condiment - (SALES_PM_COMPLSRY | SALES_COND_COMPLSRY) */
#define     SALES_ZERO_AMT          0x0008        /* indicate 0 amount entry was done */
#define     SALES_OEP_POPUP         0x0010        /* Create Popup,  R3.0 */
#define     SALES_DISPENSER_SALES   0x0020        /* Beverage Dispenser Sales,  R3.1 */
#define     SALES_PREAUTH_BATCH     0x0040        // indicates that we are in a Preauth Capture mode for Preauth Batch
#define     SALES_PREAUTH_STORED    0x0080        // indicates that we are in a Preauth Capture mode for Stored Preauth Tender
#define		SALES_TRETURN_ITEM		0x0100		 /* this transaction is a return that contains at least one return item */
#define		SALES_TRANSACTION_DISC	0x0200		 /* this transaction has a transaction discount applied, not further changes allowed */
#define		SALES_TRANSACTION_BDUE	0x0400		 /* this transaction has a balance due, not further changes allowed */
#define		SALES_RETURN_TIPS		0x0800       // Return transaction has had tips returned and only one is allowed.
#define		SALES_PREAUTHCAP_DECLN	0x1000       // Preauth Capture transaction has auto decline.  Preauth was Declined.
#define		SALES_RESETLOG_START	0x2000		 /* this transaction has started a reset log that is not yet complete */

#define		MIN_QTY_ALLOWED			1*1000		//Minimum quantity allowed for Quantity Restriction  (set to 1 QTY currently)


// Sales retriction for restricted item sale based on time of day or
// age of purchasers.  See function ItemSalesSalesRestriction().

#define ITM_SL_RESTRICT_ANYTIME 7       /* restriction at any time */

#define ITM_AGE_TYPE1           1       /* type for A/C#208 (birthday entry) */
#define ITM_AGE_TYPE2           2       /* type for A/C#208 (birthday entry) */
#define ITM_AGE_TYPE3           3       /* type for A/C#208 (birthday entry) */

    typedef struct {
        TCHAR  auchPluNo[NUM_PLU_LEN+1];        /* PLU No. 2172  */
        UCHAR   uchAdj;                         /* adjective No. */
    }ITEMSALESOEPLOCAL;


/*
* --------------------------------------------------------------------------
    Total Key Module Local Data
* --------------------------------------------------------------------------
*/

    typedef struct {
        USHORT       usNoPerson;                 /* tray counter (No of person) */
		USHORT       usUniqueID;                 /* Sales item unique Id from last Tray Total total, ItemSalesGetUniqueId() */
        DCURRENCY    lTrayTaxable[STD_TAX_ITEMIZERS_MAX];     /* taxable itemizer for tray transaction */
        DCURRENCY    lTrayTax[STD_TAX_ITEMIZERS_MAX];         /* tax amount for tray transaction */
        DCURRENCY    lTrayTaxExempt[STD_TAX_ITEMIZERS_MAX];   /* tax exempt for tray transaction */
        DCURRENCY    lTrayTotal;                 /* tray total itemizer */
        SHORT   sTrayCo;                    /* tray counter itemizer */
        UCHAR   uchSaveMajor;               /* previous major class (for fast finalize) */
        UCHAR   uchSaveMinor;               /* previous minor class (for fast finalize) */
        TCHAR   aszNumber[NUM_NUMBER];      /* 5/24/96 number */
        ITEMVAT TrayItemVAT[STD_TAX_ITEMIZERS_VAT];     /* VAT data,    V3.3 */
        DCURRENCY    lTrayVatService;                   /* V3.3 */
        DCURRENCY    lTrayVatPayment;                   /* V3.3 */
        DCURRENCY    lPigTaxExempt;              /* tax exempt for Pig Rule 10-21-98 */
    } ITEMTOTALLOCAL;

#define     ITM_TOTAL_AFFECT        0           /* affect itemizer */
#define     ITM_TOTAL_NOT_AFFECT    1           /* not affect itemizer */


/*
* --------------------------------------------------------------------------
    Tender Module Local Data
* --------------------------------------------------------------------------
*/

    typedef struct {
        UCHAR   fbTenderStatus[3];           /* tender status, See TENDER_ defines below for description   R3.1,   Saratoga */
        DCURRENCY   lTenderizer;             /* tenderizer */
        DCURRENCY   lSplitMI;                /* Split Tender MI,    R3.1 */
        DCURRENCY   lSavedMI;                /* saved MI */
        DCURRENCY   lFCMI;                   /* foreign currency tenderizer */
        USHORT  fbModifier;                  /* Modifier E-069 corr 4/28 */
        TCHAR   aszNumber[NUM_NUMBER];       /* Acct Number  add EPT     */
        TCHAR   aszRoomNo[NUM_ROOM];         /* Room No. E-074 corr 4/28 */
        TCHAR   aszGuestID[NUM_GUESTID];     /* Guest ID E-074 corr 4/28 */
        TCHAR   aszCPMsgText[NUM_CPRSPCO_EPT][NUM_CPRSPTEXT];   /* E-074 4/28   */
        TCHAR   auchExpiraDate[NUM_EXPIRA];  /* Exp Date     add EPT     */
        TCHAR   auchApproval[NUM_APPROVAL];  /* Approval     add EPT     */
        UCHAR   fbTenderStatusSave;          /* tender status */
        DCURRENCY   lPayment;                /* external VAT for Euro Calc. V3.4 */
        UCHAR   uchBuffOff;                  /* Offset of Saved Buffer for EPT, Saratoga */
        DCURRENCY   lFSTenderizer;           /* food stamp tenderizer */
        DCURRENCY   lFSTenderizer2;          /* second food stamp tenderizer, V2.01.04 */
        DCURRENCY   lFSTenderVal;            /* FS Tender Amount for Val.R2.0 */
        DCURRENCY   lWorkTaxable[STD_TAX_ITEMIZERS_MAX];         /* work taxable itemizer */
        DCURRENCY   lWorkTax[STD_TAX_ITEMIZERS_MAX];             /* work tax itemizer */
        DCURRENCY   lWorkExempt[STD_TAX_ITEMIZERS_MAX];          /* work tax exempt itemizer */
        DCURRENCY   lWorkFSExempt[STD_TAX_ITEMIZERS_MAX];        /* work tax exempt itemizer, V6 */
        DCURRENCY   lFS;                     /* food stamp change in coupons, see ItemTendFSChange(), TAR111547 */
        DCURRENCY   lFSChange;               /* food stamp credit (chit) */
        DCURRENCY   lCashChange;             /* food stamp credit (cash) */
        ITEMTOTAL   ItemTotal;               /* total key save area */
        ITEMTOTAL   ItemFSTotal;             /* F.S total save area */
        ITEMTENDER  ItemFSTender;            /* F.S tender save area */
        USHORT      usSlipPageLine;			 /* for split partial tender */
        UCHAR       fbSlibPageStatue;		 /* for split partial tender */
		USHORT      usCheckTenderId;                // Unique tender id for split checks identifying specific tender
		ITEMTENDERAUTHCODE  authcode;               // XEPT authorization Code - from DSI Client XML.
		ITEMTENDERREFNO     refno;		            // XEPT Reference Number generated for processor of transaction for EEPT
		TCHAR	tchRoutingNumber[NUM_ROUTING_NUM];	// XEPT routing number, used for check auth
		TCHAR	tchCheckSeqNo[NUM_SEQUENCE_NUM];	//XEPT Check Sequence number, used for check auth
    } ITEMTENDERLOCAL;


/*-----  fbTenderStatus[0], tender status -----*/
#define     TENDER_PARTIAL              0x01        /* partial tender state */
/*#define     TENDER_FC1                  0x02      * foreign currency total #1 */
/*#define     TENDER_FC2                  0x04      * foreign currency total #2 */
#define     TENDER_CPPARTIAL            0x08        /* partial tender of CP */
#define     TENDER_RC                   0x10        /* room charge tender state */
#define     TENDER_AC                   0x20        /* account charge tender state */
#define     TENDER_EPT                  0x40        /* EPT tender state */
#define     TENDER_EC                   0x80        /* EC for charge post tender state */

/*-----  fbTenderStatus[1], tender status -----*/
#define     TENDER_SEAT_PARTIAL         0x01        /* seat partial tender state */
#define     TENDER_SPLIT_TENDER         0x02        /* split tender state */
#define     TENDER_CP_DUP_REC           0x04        /* Duplicate Receipt for EPT,   Saratoga */
#define     TENDER_FS_PARTIAL           0x08        /* Food Stamp partial tender */
#define     TENDER_FS_PARTIAL2          0x10        /* Second Food Stamp partial tender, V2.01.04 */
#define     TENDER_FS_TOTAL             0x20        /* Food Stamp total staus, V2.01.04 */

/*-----  fbTenderStatus[2], tender status,  Saratoga -----*/
#define     TENDER_FC1                  0x01        /* foreign currency total #1 */
#define     TENDER_FC2                  0x02        /* foreign currency total #2 */
#define     TENDER_FC3                  0x04        /* foreign currency total #3 */
#define     TENDER_FC4                  0x08        /* foreign currency total #4 */
#define     TENDER_FC5                  0x10        /* foreign currency total #5 */
#define     TENDER_FC6                  0x20        /* foreign currency total #6 */
#define     TENDER_FC7                  0x40        /* foreign currency total #7 */
#define     TENDER_FC8                  0x80        /* foreign currency total #8 */

/*
* --------------------------------------------------------------------------
    Modifier Module Local Data
* --------------------------------------------------------------------------
*/

    typedef struct {
        UCHAR   fbTareStatus;                /* tare key copy of MDC 29 settings, MDC_SCALE_ADR  */
        LONG    lTareValue;                  /* tare value preset */
        LONG    lWeightScaled;               /* weight scaled     */
    }ITEMMODSCALE;

    typedef struct {
        USHORT  fsTaxable;                   /* taxable status, affected by FSC_TAX_MODIF key press */
        USHORT  usCurrentPerson;             /* affection No of person */
        UCHAR   fbModifierStatus;            /* modifier status */
        ITEMMODSCALE    ScaleData;           /* scale i/f data  */
    }ITEMMODLOCAL;

// following taxable bit indicators used with ItemModLocal.fsTaxable.
// these indicators are used to maintain various tax modification states which
// are triggered by the use of the FSC_TAX_MODIF key with an extended code.
// See also CLASS_UITAXMODIFIER1, CLASS_UITAXMODIFIER2, ..., CLASS_UITAXEXEMPT1, CLASS_UITAXEXEMPT2
// See function ItemModTax() and the functions it calls: ItemModTaxCanada() and ItemModTaxUS().
#define     MOD_TAXABLE1            0x0001      /* taxable modifier #1 */
#define     MOD_TAXABLE2            0x0002      /* taxable modifier #2 */
#define     MOD_TAXABLE3            0x0004      /* taxable modifier #3 */
#define     MOD_TAXABLE4            0x0008      /* taxable modifier #4 */
#define     MOD_TAXABLE5            0x0010      /* taxable modifier #5 */
#define     MOD_TAXABLE6            0x0020      /* taxable modifier #6 */
#define     MOD_TAXABLE7            0x0040      /* taxable modifier #7 */
#define     MOD_TAXABLE8            0x0080      /* taxable modifier #8 */
#define     MOD_TAXABLE9            0x0100      /* taxable modifier #9 */
#define     MOD_GSTEXEMPT           0x0200      /* GST exempt */
#define     MOD_USEXEMPT            MOD_GSTEXEMPT   /* US Tax exempt, see also GCFQUAL_USEXEMPT and CLASS_UITAXEXEMPT1 */
#define     MOD_PSTEXEMPT           0x0400      /* PST exempt */

// See also CPN_STATUS0_FOOD_MOD and other CPN_STATUS0_aaaa defines in csop.h
// for coupon status as some of these are used with coupons as well with
// COMCOUPONPARA.uchCouponStatus[0] bit fields.
// ParaPlu.ContPlu.auchContOther[1] is assigned to ITEMCOUPONSEARCH.auchStatus[0] in TrnStoCpnUPCSearch().
// See the PLU_AFFECT_ type defines for testing tax itemizers affected, food stamp, etc.
#define     MOD_USTAXMOD            0x0007      /* us taxable modifier mask */
#define     MOD_CANTAXMOD           0x000f      /* Canada taxable modifier mask */
#define     MOD_VATTAXMOD           0x0003      /* VAT Int'l taxable modifier mask */
#define     MOD_CANADAEXEMPT        0x0600      /* Canada tax exempt, GST or PST */
#define     MOD_CAN_TAX_MASK        0x01ff      /* Canada tax mask */

// following modifier status bit indicators used with ItemModLocal.fbModifierStatus
#define     MOD_NORECEIPT           0x01        /* no receipt */
#define     MOD_OFFLINE             0x02        /* E-003 corr. 4/18 */
#define     MOD_FSMOD               0x04        /* Food stamp Modifier */
#define     MOD_WIC                 0x08        /* WIC, Saratoga */
#define     MOD_ZEROPERSON          0x10        /* "0" Person Entry, Saratoga Nov/27/2000 */

// following modifier status bit indicators used with ItemModLocal.ScaleData.fbStatus
// must follow MDC address 29 bit mask of (ODD_MDC_BIT0 | ODD_MDC_BIT1 | ODD_MDC_BIT2 | ODD_MDC_BIT3)
#define     MOD_SCALE_DIV_005_OR_001    0x01        /* min. scale division is 0.005 or 0.001, MDC_SCALE_ADR */
#define     MOD_SCALE_DIV_001           0x04        /* min. scale division is 0.001, MDC_SCALE_ADR */
#define     MOD_READ_SCALE_AT_TARE      0x08        /* read scale at tare key depression, MDC_SCALE_ADR */
#define     MOD_TARE_KEY_DEPRESSED      0x80        /* FSC_TARE or tare key was depressed */

/*
* --------------------------------------------------------------------------
    Other Module Local Data
* --------------------------------------------------------------------------
*/

    typedef struct {
        UCHAR   fbOtherStatus;                  /* other module status */
    }ITEMOTHERLOCAL;


#define     OTHER_SLIP_RELEASED     0x01        /* slip release status */

#define     OTHER_RECEIPTFEED       8           /* default receipt feed line */
#define     OTHER_JOURNALFEED       4           /* default journal feed line */


/*
* --------------------------------------------------------------------------
    Common Module Local Data
* --------------------------------------------------------------------------
*/

    typedef struct {
        UCHAR           fbCommonStatus;          /* COMMON MODULE status */
        UCHAR           uchItemNo;               /* No of buffered item */
        USHORT          usSalesBuffSize;         /* temporary buffer size, size of compressed item data as calculated by TrnCheckSize() */
		union {
			// anonymous union to provide access to various possible data items stored here.
			// the ItemSales structure was being used as a union without the union keyword being used
			// which required that all data structures stored must be no larger than ITEMSALES.
			// With changes for EMV the ITEMTENDER struct may now be larger.
			// the data is stored as an uncompressed ITEMSALES, ITEMTENDER, etc. data struct but the usSalesBuffSize is the
			// size of the compressed item data as calculated by function TrnCheckSize().
			// usSalesBuffSize is used in a number of places to provide the compressed item size when stored into the transaction data file
			// as part of using function ItemCommonCheckSize() to check for a Buffer Full condition.
			ITEMSALES       ItemSales;               /* previous sales item, stored as uncompressed data - was original largest item used as a union */
			ITEMGENERICHEADER  uItemSalesHeader;
			ITEMTENDER         uItemSalesTender;        /* previous tender item */
			ITEMCOUPON         uItemSalesCoupon;        /* previous coupon item */
			ITEMMODIFIER       uItemSalesModifier;      /* previous modifier item */
			ITEMDISC           uItemSalesDisc;          /* previous discount item */
			ITEMTOTAL          uItemSalesTotal;         /* previous total item */
		};
		USHORT	        usCountPrecedencePLU;           // Total number of different PLU's entered in PrecedenceData
		SHORT           sPPIQty[SALES_MAX_PPI];         // Same data as ItemSalesPpi.PpiData[sSalesPpiIndex].sPPIQty
        PRECEDENCEDATA  PrecedenceData[SALES_MAX_PLU];	// Used to store info about PPI items
        USHORT          usDiscBuffSize;          /* discount temporary buffer size, size of compressed item data as calculated by TrnCheckSize() */
        ITEMDISC        ItemDisc;                /* previous discount item, stored as uncompressed data */
        DCURRENCY       lChargeTip;              /* charge tips amount */
		ITEMSALES	    CondimentEdit;
		ITEMTENDERCHARGETIPS  ReturnsTenderChargeTips;
		TCHAR           aszCardLabelReturnsTenderChargeTips[NUM_CPRSPTEXT];    // 28:number
    }ITEMCOMMONLOCAL;

#define     COMMON_SUCCESS          0           /* success status */

/*----- Status Definition, (fbCommonStatus) -----*/
#define     COMMON_SPVINT           0x01        /* already SPV intervention */
#define     COMMON_CONSNO           0x02        /* already count-up consecutive No */
#define     COMMON_VOID_EC          0x04        /* prohibit VOID and E/C */
#define     COMMON_NON_GUEST_CHECK  0x10        /* non-guest check transaction, V3.3 */
#define     COMMON_PROHIBIT_SIGNOUT 0x20        /* we are in a transaction so prohibit sign-out */
#define     COMMON_PAIDOUT_RETURNS  0x40        // indicates that this guest check has been paid out and is being processed for returns/voids only.

/* deleted by not used 08/05/98
#define     COMMON_PREV_PAYMENT  0x10     / Previous Transaction is Payment /
#define     COMMON_WAIT          0x20     / Wait Operation /
#define     COMMON_PAID_RECALL   0x20     / Paid Order Recall Transaction /
*/

#define     COMMON_DRAWER_A       ASN_AKEY_ADR        /* drawer A data for UIE */
#define     COMMON_DRAWER_B       ASN_BKEY_ADR        /* drawer B data for UIE */

#define     COMMON_NO_OF_RETRY_READ    5        /* scale read retry */


/* --- Int'l VAT,   V3.3 --- */
typedef struct {
    ITEMVAT     ItemVAT[STD_TAX_ITEMIZERS_MAX];     /* VAT data */
    DCURRENCY   lService;                           /* service amount */
    DCURRENCY   lPayment;                           /* payment total amount */
}ITEMCOMMONVAT;

typedef struct {
    ITEMVAT     ItemVAT;                            /* VAT data */
    ULONG       lServiceRate;                       /* service rate */
    DCURRENCY   lVATable_Ser;                       /* VATable with service */
    DCURRENCY   lVATable_Non;                       /* VATable without service */
    DCURRENCY   lServiceAmt;                        /* service amount */
    DCURRENCY   lAffectPayVAT;                      /* affect payment VAT amount */
    DCURRENCY   lAffectPaySer;                      /* affect payment service amount */
}ITEMCALCVAT;

/*
* --------------------------------------------------------------------------
    Misc. Transaction Module Local Data
* --------------------------------------------------------------------------
*/

    typedef struct {
        ULONG       ulCashierID;                 /* post receipt cashier id, V3.3 */
        ULONG       ulWaiterID;                  /* post receipt waiter id, V3.3 */
        USHORT      fsStatus;                    /* item misc status */
		USHORT		usMaxReceiptsPost;           // maximum number of receipt reprints
		USHORT		usMaxReceiptsDemand;         // maximum number of receipt print on demand
		USHORT		usMaxReceiptsGift;           // maximum number of receipt reprints or print on demand
		USHORT		usMaxReceiptsPark;           // maximum number of receipt reprints or print on demand
		USHORT      usCountPost;                 // current count of the number of Post Receipts printed
		USHORT      usCountDemand;               // current count of the number of Print on Demand Receipts printed
		USHORT      usCountGift;                 // current count of the number of Gift Receipts printed
		USHORT      usCountPark;                 // current count of the number of Parked Receipts printed
    }ITEMMISCLOCAL;


typedef struct
{
	int         nCouponAppliedCount;
	USHORT      usPriority;
	UCHAR       ucStatusBytes[2];                   /* PLU taxable status, sized OP_CPN_STS_SIZE, 2172, same as COMCOUPONPARA.uchCouponStatus[] */
	ITEMCOUPON  ItemCoupon;
	UCHAR		uchGroups[STD_MAX_NUM_PLUDEPT_CPN]; //6 PLUS/DEPT available in each coupon
} ItemPopUpList, *pItemPopUpList;

// following bit masks are used with ItemMiscLocal.fsStatus to indicate print actions
// that may or may not be allowed to be repeated and/or should only be done once.
#define     MISC_POSTRECPT              0x0001      /* exist data for post receipt */
#define     MISC_PARKING                0x0002      /* exist data for parking,   R3.0 */
#define		MISC_GIFTRECEIPT			0x0004		/* Gift Receipt JHHJ*/
#define		MISC_PRINTDEMAND			0x0008		/* Print on Demand */

/*
*===========================================================================
    Function Prototype Declarations
*===========================================================================
*/
/*
* --------------------------------------------------------------------------
    Operator Open Module's Functions Declaration
* --------------------------------------------------------------------------
*/

SHORT   ItemCasSignIn(UIFREGOPEOPEN *pData1, ITEMOPEOPEN *pData2);
SHORT   ItemWaiNoKeyIn(UIFREGOPEOPEN *pData1, ITEMOPEOPEN *pData2);
SHORT   ItemWaiLockIn(UIFREGOPEOPEN *pData1, ITEMOPEOPEN *pData2);
VOID    ItemSignInPutMiscData(VOID);
VOID    ItemSignInDisplay(UIFREGOPEOPEN *pData1, ITEMOPEOPEN *pData2, USHORT fsModeStatus);
VOID    ItemSignInDiaRemoveKey(VOID);
VOID    ItemSignInDiaRetrieveDisp(VOID);
VOID    ItemSignInIgnoreModifierKey(VOID);

SHORT   ItemBarNoKeyIn(UIFREGOPEOPEN *pData1, ITEMOPEOPEN *pData2);
SHORT   ItemCasIntSignIn(UIFREGOPEOPEN *pData1, ITEMOPEOPEN *pData2);

// WARNING:  A couple of these functions are used in uifacmode.c and a prototype
//           was just copied there rather than including this include file.
SHORT  ItemSetCasSecretCode (ULONG ulUifEmployeeId, ULONG ulUifEmployeePin, ULONG *ulUifEmployeePinOld);
SHORT  ItemCasSecretCode(ULONG *puchSecret);
VOID   ItemCasSignInCancel(VOID *pData);

SHORT  ItemReturnsLoadGuestCheck(KEYMSG *pData);

/*
* --------------------------------------------------------------------------
    Transaction Open Module's Functions Declaration
* --------------------------------------------------------------------------
*/

SHORT   ItemTransOpenNC(UIFREGTRANSOPEN *pData);
SHORT   ItemTransNCKey(UIFREGTRANSOPEN *pData1);
VOID    ItemTransNewCheck(ITEMTRANSOPEN *pData);
VOID    ItemTransNCPara(VOID);

SHORT   ItemTransOpenRO(UIFREGTRANSOPEN *pData);
SHORT   ItemTransROKeyPB( ITEMTRANSOPEN *pData);

SHORT   ItemTransOpenAC(UIFREGTRANSOPEN *pData);
SHORT   ItemTransACNor(UIFREGTRANSOPEN *pData);
SHORT   ItemTransACNorCheck(UIFREGTRANSOPEN *pData);
SHORT   ItemTransACNorKey(ITEMTRANSOPEN *pData);
SHORT   ItemTransACNewKey(UIFREGTRANSOPEN *pOpen, ITEMTRANSOPEN *pTransOpen);
VOID    ItemTransAddcheck(ITEMTRANSOPEN *pData);
VOID    ItemTransACNorAff(VOID);
SHORT   ItemTransACMan(UIFREGTRANSOPEN *pData);
SHORT   ItemTransACManCheck(VOID);
SHORT   ItemTransACKeyMan( ITEMTRANSOPEN *pData);
SHORT   ItemTransACSPVInt(UIFREGTRANSOPEN *pData);
VOID    ItemTransManAdd(ITEMTRANSOPEN *pData);
SHORT   ItemTransACManAff(UIFREGTRANSOPEN *pData1, ITEMAFFECTION *pData2);

SHORT   ItemTransACMul(UIFREGTRANSOPEN *pData1);
SHORT   ItemTransACMulCheck(VOID);
SHORT   ItemTransACMulPreFin(UIFREGTRANSOPEN *pData1, ITEMTRANSOPEN *pData2);
SHORT   ItemTransACMulPreFin1(ITEMTRANSOPEN *pData1, TRANGCFQUAL *pData2);
VOID    ItemTransACMulPreFinError(USHORT usGuestNo);
VOID    ItemTransACMulPreFinSpl(UIFREGTRANSOPEN *pOpen);
VOID    ItemTransACMulPreFin2(VOID);
VOID    ItemTransACMulPreFin3(VOID);
VOID    ItemTransACMulPrint(VOID);
VOID    ItemTransACMulDisp(VOID);
VOID    ItemTransACMulDispBack(VOID);
VOID    ItemTransACMulti(ITEMTRANSOPEN *pData);
SHORT   ItemTransACSto(UIFREGTRANSOPEN *pData);
SHORT   ItemTransACStoGet(UIFREGTRANSOPEN *pData);
VOID    ItemTransACStoRec(ITEMTRANSOPEN *pData);

VOID    ItemTransDispDef(VOID);
SHORT   ItemTransGCFNo(ULONG *pData);
SHORT   ItemTransPostOpen(VOID);
SHORT   ItemTransSeatNo(ITEMTRANSOPEN *pData, SHORT sType);
SHORT   ItemTransNoPerson(ITEMTRANSOPEN *TransOpen);
SHORT   ItemTransTableNo(ITEMTRANSOPEN *TransOpen);
VOID    ItemTransNCCommon(ITEMTRANSOPEN *TransOpen);

SHORT   ItemTransACStoOrderCounterOpen(USHORT uchOrder);
SHORT   ItemTransACStoDeliv(UIFREGTRANSOPEN *pData);
SHORT   ItemTransACStoCheckDeliv(UIFREGTRANSOPEN *pData);
SHORT   ItemTransACStoGetDeliv(VOID);
VOID    ItemTransACStoRecDeliv(VOID);
SHORT   ItemTransACStoGetDelivOpen(VOID);
VOID    ItemTransCalSec(ITEMAFFECTION *pData);
VOID    ItemTransACStoServTime(VOID);

SHORT   ItemTransACCIRecall(UIFREGTRANSOPEN *pData);

SHORT   ItemTransOpenNewKeySeq(UIFREGTRANSOPEN *pOpen);

/*
* --------------------------------------------------------------------------
    Sales Module's Functions Declaration
* --------------------------------------------------------------------------
*/

USHORT  ItemSalesGetUniqueId (VOID);
SHORT   ItemSalesDept(UIFREGSALES *pData1, ITEMSALES *pData2);
SHORT   ItemSalesPLU(UIFREGSALES *pData1, ITEMSALES *pData2);
SHORT   ItemSalesPrintMod(UIFREGSALES *pData);
SHORT   ItemSalesRepeat(CONST UIFREGSALES *pData);
SHORT   ItemSalesRepeatCheck(UIFREGSALES *pData1, ITEMSALES *pData2);
SHORT   ItemSalesCursorRepeatChk(ITEMSALES *pData);
SHORT   ItemSalesPrevRepeatChk(UIFREGSALES *pData, ITEMSALES *pData2);
SHORT   ItemSalesCursorRepeat(UIFREGSALES *pData1, ITEMSALES *pData2);
SHORT   ItemSalesRepeatChkPriceChange(CONST ITEMSALES *pItemSales);

/* --- Dec/13/2000 --- */
/*SHORT   ItemSalesPrepare(UIFREGSALES *pData1, ITEMSALES *pData2, UCHAR *puchRestrict, USHORT *pusLinkNo);*/
SHORT   ItemSalesPrepare(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales,
                         UCHAR *puchRestrict, USHORT *pusLinkNo, UCHAR *puchTable);
/* SHORT   ItemSalesPrepare(UIFREGSALES *pData1, ITEMSALES *pData2); */
VOID    ItemSalesSetupStatus(UCHAR  *pauchData);
SHORT   ItemSalesCommon(CONST UIFREGSALES *pData1, ITEMSALES *pData2);
SHORT   ItemSalesDeptOpen(UIFREGSALES *pData1, ITEMSALES *pData2);
SHORT   ItemSalesNonAdj(UIFREGSALES *pData);
SHORT   ItemSalesAdj(UIFREGSALES *pData1, ITEMSALES *pData2);
SHORT   ItemSalesCondiment(UIFREGSALES *pData1, ITEMSALES *pData2);
SHORT   ItemSalesSetMenu(CONST UIFREGSALES *pData1, ITEMSALES *pData2);
SHORT   ItemSalesSetCheckValidation(UCHAR auchControlStatus, UCHAR uchAdjective);
SHORT   ItemSalesCalculation(ITEMSALES *pData);
DCURRENCY   ItemSalesCalcCondimentPPIOnly(CONST ITEMSALES *pItemSales);
SHORT   ItemSalesCalcPPI(ITEMSALES *pItemSales);        /* R3.1 */
SHORT   ItemSalesCalcPPIEC(ITEMSALES *pItemSales);      /* R3.1 */
VOID    ItemSalesCalcPPIInit(VOID);                     /* R3.1 */
SHORT   ItemSalesModifier(UIFREGSALES *pData1, ITEMSALES *pData2);
//SHORT   ItemSalesCommonIF(UIFREGSALES *pData1, ITEMSALES *pData2); //Function prototype moved to item.h for US Customs
SHORT   ItemSalesComnIFVoidHALO(DCURRENCY lAmount);
SHORT   ItemSalesSpvInt(CONST ITEMSALES *pData);
SHORT   ItemSalesSpvIntForCond(ITEMSALES *pData);
VOID    ItemSalesSetDefltPage( VOID );
VOID    ItemSalesGenerateAdj(ITEMSALES *pData, UCHAR *auchWriteBuff);
SHORT   ItemSalesBreakDownAdj(ITEMSALES *pData, UCHAR *auchWriteBuff);
VOID    ItemSalesDisplay(CONST UIFREGSALES *pData1, CONST ITEMSALES *pData2, REGDISPMSG *pDisplayData);
SHORT   ItemSalesSalesAmtCalc(CONST ITEMSALES *pSalesData, DCURRENCY *plCalProduct);
SHORT   ItemSalesModDiscCheck(CONST UIFREGSALES *pData1, ITEMSALES *pData2);
SHORT   ItemSalesModDiscHALO(CONST UIFREGSALES *pData);
SHORT   ItemSalesModDisc(ITEMCOMMONLOCAL *pData1, ITEMSALES *pData2);
VOID    ItemSalesModDiscModifier(ITEMSALES *pData);
VOID    ItemSalesModDiscCancelStatus(ITEMSALES *pData);
VOID    ItemSalesModDiscValidation(ITEMSALES *pData);
VOID    ItemSalesModDiscAffection(ITEMSALES *pData);
SHORT   ItemSalesCursorModDisc(UIFREGSALES *pData1, ITEMSALES *pData2);
SHORT   ItemSalesCursorModDiscIn(UIFREGSALES *pData1, ITEMSALES *pData2);
VOID    ItemSalesCancelPrev(VOID);
SHORT   ItemSalesSpvIntForModDisc(ITEMCOMMONLOCAL *pData);

SHORT   ItemSalesOEP(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales);
UCHAR   ItemSalesOEPGetAdjNo(PLUIF *pPlu);
UCHAR   ItemSalesOEPGetAdjNoImplied(PLUIF *pPlu);

SHORT   ItemSalesCursorVoid(ITEMSALES *pData, SHORT sQTY);
SHORT   ItemSTenderCursorVoidReturns(ITEMTENDER *pItemTender);
SHORT   ItemSalesPriceCheck(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales);

/* 2172 */
SHORT   ItemSalesLabelKeyedOnlyProc (TCHAR *aszPLUNo);
SHORT   ItemSalesLabelProc(UIFREGSALES *pRegSales, ITEMSALES *pItemSales);
SHORT   ItemSalesCheckKeyedPLU(UIFREGSALES *pUifRegSales);
SHORT   ItemSalesLabelPreCheck(UIFREGSALES *pUifRegSales,
                          ITEMSALES *pItemSales, VOID *pPluLabel);
/*SHORT   ItemSalesLabelPreCheck(UIFREGSALES *pUifRegSales,
                          ITEMSALES *pItemSales, LABELANALYSIS *pPluLabel); */
SHORT   ItemSalesGetDeptNo(USHORT *pusDept, UCHAR *puchMajorFsc, UCHAR *puchMinorFsc);
SHORT   ItemSalesGetDeptNoSans(USHORT *pusDept);
SHORT   ItemSalesPriceChange(ITEMSALES *pItemSales);
SHORT   ItemSalesGetPrice(ITEMSALES *pItemSales, DCURRENCY *plPrice);
SHORT   ItemSalesCalcPM(ITEMSALES *pItemSales);        /* 2172 */
SHORT   ItemSalesCalcPMEC(ITEMSALES *pItemSales);      /* 2172 */
VOID    ItemSalesCalcPMInit(VOID);                     /* 2172 */

SHORT   ItemSalesCheckBirth(ITEMSALES *pItemSales);
CHAR   ItemSalesCalAge(UCHAR uchBthDay,UCHAR uchBthMonth,USHORT usBthYear);
SHORT   ItemSalesGetBirthDay(ULONG *ulBirthDay);
SHORT ItemSalesPLUBuilding(CONST UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales, PLUIF *pPluIf);
/* SHORT ItemSalesPLUBuilding(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales, PLUIF *pPluIf); */
SHORT   ItemSalesGetPriceOnly(ULONG *pulPrice);
SHORT   ItemSalesPLUCreatBuilding(CONST UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales);
SHORT   ItemSalesLinkPLU(ITEMSALES *pItemSales, USHORT fsCheck);
SHORT   ItemSalesLinkPrepare(CONST UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales, UCHAR *puchRestrict);
SHORT   ItemSalesLinkCommonIF(CONST UIFREGSALES *pData1, ITEMSALES *pData2, USHORT fsSend);
//STANZA  *ItemSalesMakeStanza(STANZA *pStanza); //Removed by JHHJ in SR 155
SHORT   ItemSalesCheckWIC(ITEMSALES *pItem);                /* Saratoga */
SHORT	GetNextGCSlot();
SHORT	GetGiftCardAccount2(ITEMSALESGIFTCARD *pWorkGift);
SHORT	SaveGiftCard(ITEMSALESGIFTCARD *pWorkGift, SHORT giftPos);
SHORT	ItemSalesRemoveGiftCard(CONST ITEMSALES *pItemSales);
SHORT	DetermineGiftCardItem (CONST ITEMSALES *pItemSales);


/*
* --------------------------------------------------------------------------
    Discount Module's Functions Declaration
* --------------------------------------------------------------------------
*/

SHORT   ItemDiscMain(UIFREGDISC *pUifRegDisc);
SHORT   ItemDiscCheckQual(CONST UIFREGDISC *pDiscData1);
SHORT   ItemDiscCheckQualSpl(CONST UIFREGDISC *pUifDisc, CONST ITEMDISC *pDisc);
SHORT   ItemDiscCheckNormalAndSeat(CONST ITEMDISC *pDisc);
SHORT   ItemDiscCheckGCFStat(VOID);
SHORT   ItemDiscItemDiscModif(ITEMDISC *pDiscData2);
USHORT  ItemDiscGetMdcCheck_ITEMDISC1 (UCHAR uchDiscountNo, USHORT usAddress);
USHORT  ItemDiscGetMnemonic_ITEMDISC1 (UCHAR uchDiscountNo);
USHORT  ItemDiscGetRate_ITEMDISC1 (UCHAR uchDiscountNo);
VOID    ItemDiscGetTaxMDC(ITEMDISC *pDiscData);
VOID    ItemDiscGetCanTaxMod(VOID *pData);
VOID    ItemDiscCommonTranMnemonic (UCHAR uchMinorClass, UCHAR uchDiscountNo, PARATRANSMNEMO  *pTransMnemoRcvBuff);
SHORT   ItemDiscCommonIF(CONST UIFREGDISC *pDiscData1, ITEMDISC *pDiscData2);
VOID    ItemDiscDisplay(ITEMDISC *pItemDisc2);
SHORT   ItemDiscCursorVoid(ITEMDISC *pData);
SHORT   ItemDiscCursorVoidCheck(ITEMDISC *pData);
SHORT   ItemChargeTipsOpen();  //GSU SR 6 CRW 11/8/2002 Opens a transaction with charge tips as first item
/*
* --------------------------------------------------------------------------
    Coupon Module's Define Declaration
* --------------------------------------------------------------------------
*/

SHORT   ItemCoupon(UIFREGCOUPON  *pUifRegCoupon);
SHORT   ItemCouponCursorVoid(ITEMCOUPON *pData);

SHORT   ItemCouponUPCPrepare(UIFREGCOUPON *pUifRegCoupon, ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch);
SHORT   ItemCouponUPCSearch(ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch);
SHORT   ItemCouponUPCSetupStatus(ITEMCOUPON *pItemCoupon, ITEMCOUPONSEARCH *pTrnSearch);
SHORT	ItemAutoCouponCheck(SHORT sTotalKeyType /* from ItemTotalType() */);

/*
* --------------------------------------------------------------------------
    Total Key Module's Functions Declaration
* --------------------------------------------------------------------------
*/

SHORT   ItemTotalType(UIFREGTOTAL *pData);
VOID    ItemTotalInit(VOID);
SHORT   ItemTotalSPVInt(ITEMTOTAL *pData);
SHORT   ItemTotalPrev(VOID);
SHORT   ItemTotalPrevSpl(VOID);
SHORT   ItemTotalPrevSpl(VOID);
VOID    ItemTotalStub(ITEMTOTAL *pData);
VOID    ItemTotalOrder(ITEMTOTAL *pData);
SHORT   ItemTotalSysType(ITEMTOTAL *pData);
LONG    ItemTotalCalDiscWhole(UIFREGTOTAL *pData1, ITEMCOMMONTAX *pData2, SHORT sType, UCHAR fchStorege);
LONG    ItemTotalCalDiscWholeUS(UIFREGTOTAL *pData1, ITEMCOMMONTAX *pData2, SHORT sType, UCHAR fchStorege);
LONG    ItemTotalCalDiscWholeCan(UIFREGTOTAL *pData1, ITEMCOMMONTAX *pData2, SHORT sType, UCHAR fchStorege);
LONG    ItemTotalCalDiscPart(UIFREGTOTAL *pData1, ITEMCOMMONTAX *pData2, SHORT sType, UCHAR fchStorege);
LONG    ItemTotalCalDiscPartUS(UIFREGTOTAL *pData1, ITEMCOMMONTAX *pData2, SHORT sType, UCHAR fchStorege);
LONG    ItemTotalCalDiscPartCan(UIFREGTOTAL *pData1, ITEMCOMMONTAX *pData2, SHORT sType, UCHAR fchStorege);
VOID    ItemTotalDiscTaxLoad(ITEMTENDERLOCAL *WorkTend, SHORT sType);
VOID    ItemTotalDiscTaxSave(VOID);
VOID    ItemTotalCalDiscItemizer(ITEMDISC *pData1, LONG *plTaxable, SHORT sType);
VOID    ItemTotalDiscTaxLoadCan(ITEMTENDERLOCAL *WorkTend, SHORT sType);
VOID    ItemTotalCalDiscItemizerCan(ITEMDISC *pData1, LONG *plTaxable, SHORT sType);
SHORT   ItemTotalCheckDisc(UIFREGTOTAL *pData, USHORT *usDisc);

SHORT   ItemTotalST(UIFREGTOTAL *pData);
SHORT   ItemTotalSTTax(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);
VOID    ItemTotalSTAmount(ITEMTOTAL *pData);
SHORT   ItemTotalSTItem(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);

SHORT   ItemTotalCT(UIFREGTOTAL *pData);
SHORT   ItemTotalCTotal(UIFREGTOTAL *pData);
SHORT   ItemTotalCTNCheck(VOID);
SHORT   ItemTotalCTTax(UIFREGTOTAL *pData1,
        ITEMTOTAL *pData2, ITEMCOMMONTAX *pData3, ITEMCOMMONVAT *pData4);
VOID    ItemTotalCTAmount(ITEMTOTAL *pData);
SHORT   ItemTotalCTItem(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);
SHORT   ItemTotalCTM( UIFREGTOTAL *pData );
SHORT   ItemTotalCTMCheck(VOID);
SHORT   ItemTotalCTMItem( UIFREGTOTAL *pData1, ITEMTOTAL *pData2 );
SHORT   ItemTotalSplTotal(UIFREGTOTAL *pData);
SHORT   ItemTotalSplCheck(VOID);
SHORT   ItemTotalSplTax(UIFREGTOTAL *pData1,
            ITEMTOTAL *pData2, ITEMCOMMONTAX *pData3, ITEMCOMMONVAT *pData4);
SHORT   ItemTotalSplItem(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);
SHORT   ItemTotalSplitTotal(UIFREGTOTAL *pData);
VOID    ItemTotalSplitItem(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);

SHORT   ItemTotalSE(UIFREGTOTAL *pData);
SHORT   ItemTotalSECheck(VOID);
SHORT   ItemTotalSEPreCheck(UIFREGTOTAL *pData);
VOID    ItemTotalSEPreAffect(UIFREGTOTAL *pData1,
            ITEMTOTAL *pData2, ITEMAFFECTION *pData3, ITEMCOMMONTAX *pData4, USHORT fsLimitable, USHORT fsBaked);
VOID    ItemTotalSEPreItem(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);
VOID    ItemTotalSEPreGCFSave(VOID);
SHORT   ItemTotalSEPostCheck(UIFREGTOTAL *pData);
VOID    ItemTotalSEPostAffectTax(ITEMTOTAL *pData1,
            ITEMAFFECTION *pData2, ITEMCOMMONTAX *pData3, USHORT fsLimitable, USHORT fsBaked);
VOID    ItemTotalSEPostAffect(UIFREGTOTAL *pData1,
            ITEMTOTAL *pData2, ITEMAFFECTION *pData3);
VOID    ItemTotalSEPostItem(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);
VOID    ItemTotalSEPostGCFSave(ITEMTOTAL *pData);
SHORT   ItemTotalSESoftCheck(UIFREGTOTAL *pData);
VOID    ItemTotalSESoftGCFSave(ITEMTOTAL *ItemTotal);
VOID    ItemTotalSESoftTI(ITEMTOTAL *pData1, DCURRENCY *lHourly, SHORT *sItemCounter);
VOID    ItemTotalSERestoreTI(DCURRENCY *lHourly, SHORT *sItemCounter);
VOID    ItemTotalSESoftAffectTax(ITEMTOTAL *pData1,
            ITEMAFFECTION *pData2, DCURRENCY *pData3, USHORT fsLimitable, USHORT fsBaked);
VOID    ItemTotalSESoftPrintTax(ITEMTOTAL *Total, ITEMAFFECTION *Affect);
VOID    ItemTotalSESoftItem(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);
SHORT   ItemTotalSESoftCheckSpl(UIFREGTOTAL *pData);
SHORT   ItemTotalSESoftCheckCheckSpl(UIFREGTOTAL *UifTotal);
SHORT   ItemTotalSESoftMI(UIFREGTOTAL *UifTotal, ITEMTOTAL *pItem);
VOID    ItemTotalSEMakeCPTotal(ITEMTOTAL *pItem);
SHORT   ItemTotalSESoftCheckSplSingle(UIFREGTOTAL *pData1,
            ITEMTOTAL *pData2, ITEMAFFECTION *pData3);
VOID    ItemTotalSESoftCheckTaxSpl(UCHAR *auchTotalStatus, ITEMAFFECTION *pData);
VOID    ItemTotalSESoftCheckVATSpl(UCHAR *auchTotalStatus, ITEMAFFECTION *pData, DCURRENCY *plPayment);
VOID    ItemTotalSESoftItemSpl(UIFREGTOTAL *pData1, ITEMTOTAL *pData2, ITEMAFFECTION *pData3,
                UCHAR uchTrail, ITEMTOTAL *pData4, DCURRENCY lPayment);
VOID    ItemTotalSESplDispTaxTotal(ITEMAFFECTION *pTax, UIFREGTOTAL *pData1, ITEMTOTAL *pTotal);
VOID    ItemTotalSESplDispTaxTotalWhole(ITEMAFFECTION *pTax, UIFREGTOTAL *UifTotal, ITEMTOTAL *pTotal);
VOID    ItemTotalSESoftAffectSpl(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);
VOID    ItemTotalSESoftCheckSplAll(UIFREGTOTAL *pData1, ITEMTOTAL *pData2, ITEMAFFECTION *pData3);
VOID    ItemTotalSESoftCheckSplWhole(UIFREGTOTAL *UifTotal,
                        ITEMTOTAL *pWholeTotal, ITEMAFFECTION *pWholeTax);
VOID    ItemTotalSESoftCheckSplDisp(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);
VOID    ItemTotalSESoftCloseSpl(ITEMTOTAL *ItemTotal);
SHORT   ItemTotalSEStoreRecall(UIFREGTOTAL *pData);
VOID    ItemTotalSEStoRecItem(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);
VOID    ItemTotalSEOpenCheck(VOID);
VOID    ItemTotalSEStoRecGCFSave(ITEMTOTAL *pData);
SHORT   ItemTotalSETaxRef(UIFREGTOTAL *pData1, ITEMTOTAL *pData2,
            ITEMAFFECTION *pData3, ITEMCOMMONTAX *WorkTax);
VOID    ItemTotalSEAmount(ITEMTOTAL *pData);
VOID    ItemTotalSEClose(ITEMTOTAL *pData);
UCHAR   ItemTotalSEGetSerID(ITEMTOTAL *pData);
VOID    ItemTotalDupRec(ITEMTOTAL *pData);
VOID    ItemTotalSEGetGC(ITEMTOTAL *pData);
VOID    ItemTotalSESoftGTtlSpl( ITEMTOTAL *pWholeTotal, ITEMAFFECTION *pWholeTax );
VOID    ItemTotalSEDispCom(UIFREGTOTAL *UifTotal, DCURRENCY lTotal); /* R3.3 */
UCHAR   ItemTotalSESetClass(UCHAR uchMinorClass);               /* R3.3 */

VOID    ItemTotalStubSpl( ITEMTOTAL *ItemTotal, UCHAR uchSeatNo );

SHORT   ItemTotalFT(UIFREGTOTAL *pData);
SHORT   ItemTotalFTCheck(VOID);
SHORT   ItemTotalFTNor(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);
SHORT   ItemTotalFTAmount(ITEMTOTAL *pData);
VOID    ItemTotalFTItem(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);
VOID    ItemTotalFTClose(VOID);
VOID    ItemTotalFTSaved(ITEMTOTAL *pData);
SHORT   ItemTotalFTSpl(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);
VOID    ItemTotalFTItemSpl(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);
SHORT   ItemTotalFTSplitTender(ITEMTOTAL *pData);
VOID    ItemTotalFTAffectTaxSplit(ITEMTOTAL *pData1, ITEMAFFECTION *pData2);
VOID    ItemTotalFTAffectVATSplit(ITEMTOTAL *pData1, ITEMAFFECTION *pData2);
VOID    ItemTotalFTSplitTotal(ITEMTOTAL *pData1, ITEMTOTAL *pData2);
VOID    ItemTotalFTDispSplit(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);

SHORT   ItemTotalNF(UIFREGTOTAL *pData);
SHORT   ItemTotalNFTotal(UIFREGTOTAL *pData);
SHORT   ItemTotalNFItem(UIFREGTOTAL *pData1, ITEMTOTAL *pData2);
SHORT   ItemTotalNFTax(UIFREGTOTAL *pData1, ITEMTOTAL *pData2, ITEMCOMMONTAX *pData3, ITEMCOMMONVAT *pData4);
SHORT   ItemTotalNFM(UIFREGTOTAL *pData);
SHORT   ItemTotalNFMItem( UIFREGTOTAL *pData1, ITEMTOTAL *pData2 );

SHORT   ItemTotalTT( UIFREGTOTAL *pData );
VOID    ItemTotalTTTaxSave( CONST ITEMCOMMONTAX *pData1, ITEMAFFECTION *pData2, ITEMAFFECTION *pData3, CONST ITEMCOMMONVAT *pData4);
VOID    ItemTotalTTAmount( ITEMTOTAL *pData );
VOID    ItemTotalTTSave(ITEMTOTAL *pData1, ITEMCOMMONTAX *pData2, ITEMCOMMONVAT *pData3);

SHORT	ItemTotalDiaAcctChg(ITEMTENDER* pItem, SHORT sLen);
VOID	ItemTendCPConvData(UCHAR *pDesBuff, size_t usDesSize, const TCHAR *pDataFmt, DCURRENCY lSrcData);

VOID    ItemTotalDupRec( ITEMTOTAL *pData );

SHORT   ItemTotalSP(UIFREGTOTAL *pData);

SHORT   ItemTotalCI(UIFREGTOTAL *UifRegTotal);  /* R3.3 */
SHORT   ItemTotalCICheck(VOID);

SHORT   ItemTotalAutoChargeTips(VOID);  /* V3.3 */
VOID    ItemTotalEuroTender(VOID); /* V3.4 */

SHORT   ItemTotalFS( UIFREGTOTAL *pData );
SHORT   ItemTotalFSCheck( VOID );
VOID    ItemTotalFSItem( UIFREGTOTAL *pData1, ITEMTOTAL *pData2 );
VOID    ItemTotalFSSet( ITEMTOTAL *pData );
USHORT  ItemTotalFSCheckTenderKey(VOID);
VOID    ItemTotalCalTax(ITEMAFFECTION *pAffect, SHORT sType);
VOID    ItemTotalTaxNor(ITEMAFFECTION *pAffect);
VOID    ItemTotalTaxTend(ITEMAFFECTION *pAffect, SHORT sType);
VOID    ItemTotalTaxMary(ITEMAFFECTION *pAffect, SHORT sType);
VOID    ItemTotalTaxProp(ITEMAFFECTION *pAffect, SHORT sType);

SHORT   ItemTotalCFS(UIFREGTOTAL *pData);
VOID    ItemTotalCTFSTax(UIFREGTOTAL *pUifTotal, ITEMTOTAL *pItem, ITEMAFFECTION *pTax);
SHORT   ItemTotalCFSItem(UIFREGTOTAL *pUifTotal, ITEMTOTAL *pItem);

/*
* --------------------------------------------------------------------------
    Tender Module's Functions Declaration
* --------------------------------------------------------------------------
*/
VOID ItemEptMsgTextCopyReplace (TCHAR *pDest, TCHAR *pSource, int iMaxDestLen);
VOID  ItemTendSetPrintStatus (CONST ITEMTENDER *pItemTender);
USHORT  ItemTenderMdcToStatus (UCHAR uchMinorClass, UCHAR auchStatus[3]);

SHORT   ItemTendCommon(UIFREGTENDER *pData1, ITEMTENDER *pData2);
SHORT   ItemTendCheck(UIFREGTENDER *pData1, ITEMTENDER *pData2);
VOID    ItemTendAmount(UIFREGTENDER *pData1, ITEMTENDER *pData2);
VOID    ItemTendAmountWO(UIFREGTENDER *pData1, ITEMTENDER *pData2);
VOID    ItemTendAmountWA(UIFREGTENDER *pData1, ITEMTENDER *pData2);
VOID    ItemTendAmountWANor(UIFREGTENDER *pData1, ITEMTENDER *pData2);
SHORT   ItemTendState(UIFREGTENDER *pData1, ITEMTENDER *pData2);
VOID    ItemTendDelGCF(VOID);
VOID    ItemTendSaved(ITEMTENDER *pData);
VOID    ItemTendInit(VOID);
SHORT   ItemTendCalAmount(DCURRENCY *lAmount, CONST UIFREGTENDER *UifTender);
VOID    ItemTendCalAmountSpl(DCURRENCY *pData1, CONST UIFREGTENDER *UifTender);
SHORT   ItemTendCalAmountSplit(DCURRENCY *lAmount, CONST UIFREGTENDER *UifTender);
SHORT   ItemTendCalFSAmount(DCURRENCY *lAmount);    /* Saratoga */
SHORT   ItemTendSplitReceipts (CONST UIFREGTENDER *pUifRegTender, ITEMTENDER *pItemTender);
VOID    ItemTendMulti(VOID);
VOID    ItemTendDispIn(REGDISPMSG  *pDisp);
VOID    ItemTendAudVal(DCURRENCY lAmount);
VOID    ItemTendCalServTime(VOID);
VOID    ItemTendAutoChargeTips(VOID);   /* V3.3 */
VOID    ItemTendVAT(UIFREGTENDER *UifTender);
VOID    ItemTendVATCommon(ITEMAFFECTION *pAffect, SHORT sType, SHORT sDispType);
SHORT   ItemTendCheckWIC(UIFREGTENDER *pUifTender, ITEMTENDER *pItem);  /* Saratoga */
VOID    ItemTendWICExempt(VOID);                                        /* Saratoga */
DCURRENCY  ItemTendWholeTransAmount(VOID);

SHORT   ItemTendFast(UIFREGTENDER *pData1, ITEMTENDER *pData2);
SHORT   ItemTendFastNor(UIFREGTENDER *pData1, ITEMTENDER *pData2);
SHORT   ItemTendFastCheck(UIFREGTENDER *pData1, ITEMTENDER *pData2);
SHORT   ItemTendSPVInt(ITEMTENDER *pData);
VOID    ItemTendAffectTax(UIFREGTENDER *UifRegTender);
SHORT   ItemTendTTTax(UIFREGTOTAL *pData1, ITEMTOTAL *pData2, ITEMCOMMONTAX *pData3, ITEMCOMMONVAT *pData4);
VOID    ItemTendTrayTotal(VOID);
VOID    ItemTendTrayTax(VOID);
VOID    ItemTendCheckTotal(ITEMTOTAL *ItemTotal, UIFREGTENDER *UifTender);
VOID    ItemTendFastTender(UIFREGTENDER *pData1, ITEMTENDER *pData2);
VOID    ItemTendFastDisp(UIFREGTENDER *pData1, ITEMTENDER *pData2);
VOID    ItemTendAffection(VOID);
VOID    ItemTendCloseNoTotals(USHORT usFlags);
VOID    ItemTendMarkAndCloseGuestCheck( USHORT usFlags );
VOID    ItemTendClose(VOID);
VOID    ItemTendDrawer(UCHAR uchMinor);
VOID    ItemTendCoin( ITEMTENDER  *ItemTender );
VOID    ItemTendDupRec( VOID );       /* add EPT '94 4/1  */
VOID    ItemTendSplitRec (ITEMTENDER *pItemTender, USHORT fbCompPrint);
VOID	ItemTendGiftDupRec(UIFREGTENDER *UifRegTender);
SHORT   ItemTendPreTenderEdit( UIFREGTENDER *pData );
SHORT   ItemTendFastSpl(UIFREGTENDER *pData1, ITEMTENDER *pData2);
VOID    ItemTendAffectTaxSpl(CONST UCHAR *puchTotal);
VOID    ItemTendDispSpl(VOID);
VOID    ItemTendCloseSpl(VOID);
BOOL	ItemTendIsDup(VOID);
SHORT	EJSetupTxt( const TCHAR FAR *pszFormat, TCHAR* tmpBuff, ...);

SHORT   ItemTendPart(UIFREGTENDER *pData1, ITEMTENDER *pData2);
SHORT   ItemTendPartNor(UIFREGTENDER *pData1, ITEMTENDER *pData2);
SHORT   ItemTendPartSPVInt(UIFREGTENDER *pData);
SHORT   ItemTendPartTender(UIFREGTENDER *pData1, ITEMTENDER *pData2);
VOID    ItemTendPartDisp(UIFREGTENDER *pData1, ITEMTENDER *pData2);
SHORT   ItemTendPartSpl(UIFREGTENDER *pData1, ITEMTENDER *pData2);

SHORT   ItemTendOver(UIFREGTENDER *pData1, ITEMTENDER *pData2);
VOID    ItemTendOverTender(UIFREGTENDER *pData1, ITEMTENDER *pData2);
VOID    ItemTendOverDisp(UIFREGTENDER *pData1, ITEMTENDER *pData2);

SHORT   ItemTendFCTotal(UIFREGTENDER *pData);
DCURRENCY   ItemTendFCCTotal(VOID);

SHORT   ItemTendFCFast(UIFREGTENDER *pData1, ITEMTENDER *pData2);
SHORT   ItemTendFCFastNor(UIFREGTENDER *pData1, ITEMTENDER *pData2);
SHORT   ItemTendFCFastCheck(UCHAR uchMinorClass);
VOID    ItemTendFCFastTender(UIFREGTENDER *pData1, ITEMTENDER *pData2, USHORT fsSplit);
VOID    ItemTendFCFastDisp(UIFREGTENDER *pData1, ITEMTENDER *pData2);
SHORT   ItemTendFCFastSpl(UIFREGTENDER *pData1, ITEMTENDER *pData2);
VOID    ItemTendFCSplitValidation( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender );

SHORT   ItemTendFCPart(UIFREGTENDER *pData1, ITEMTENDER *pData2);
SHORT   ItemTendFCPartNor(UIFREGTENDER *pData1, ITEMTENDER *pData2);
SHORT   ItemTendFCPartCheck(VOID);
SHORT   ItemTendFCPartTender(UIFREGTENDER *pData1, ITEMTENDER *pData2);
SHORT   ItemTendFCPartSpl(UIFREGTENDER *pData1, ITEMTENDER *pData2);

SHORT   ItemTendFCOver(UIFREGTENDER *pData1, ITEMTENDER *pData2);
SHORT   ItemTendFCOverNor(UIFREGTENDER *pData1, ITEMTENDER *pData2);
VOID    ItemTendFCOverTender(UIFREGTENDER *pData1, ITEMTENDER *pData2, USHORT fsSplit);
SHORT   ItemTendFCOverSpl(UIFREGTENDER *pData1, ITEMTENDER *pData2);

SHORT   ItemTendCP( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender );
SHORT   ItemTendRC( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender );
SHORT   ItemTendAC( UCHAR uchFuncCode, UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender );
SHORT   ItemTendEPT( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender );
SHORT   ItemTendEPTEx(UIFREGTENDER *pUifTender, ITEMTENDER *pItem);     /* Saratoga */
SHORT   ItemTendCPTotal( VOID );
VOID    ItemTendEPTEdit(UCHAR uchCPFlag, UCHAR uchFuncCode, VOID *CPSend,
            UIFREGTENDER *pUifTender, ITEMTENDER *ItemTender, PARATENDERKEYINFO *pTendKeyInfo,
            LONG lChargeTips, UCHAR uchSeat, LONG lTax, ULONG ulNo);
VOID    ItemTendCPCanDisp(UIFREGTENDER *UifTender);
SHORT   ItemTendCPConvErr( UCHAR *pCPMErrorDef );
VOID    ItemTendCPCommDisp(VOID);
SHORT   ItemTendCPTrain( ITEMTENDER *ItemTender, UCHAR uchFuncCode, UIFREGTENDER *UifTender);
SHORT   ItemTendCPECTend( ITEMTENDER *ItemTender );
VOID    ItemTendCPECor( ITEMTENDER *ItemTender );   /* S-017 corr. '94 5/17 */
SHORT   ItemExecuteOffTend(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender);
SHORT   ItemTendCPComm( UCHAR uchFuncCode, UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender );
SHORT   ItemTendCheckEPT(CONST UIFREGTENDER  *UifRegTender);
SHORT   ItemTendCompEntry(TCHAR *pDesBuffer, UCHAR uchMaxLen);
SHORT   ItemTendDiaAcctChg(ITEMTENDER *pItem, SHORT sLen);                  /* Saratoga */
SHORT   ItemTendCollectSignatureOnly(USHORT usFsc, USHORT usMinorClass);
SHORT   ItemTendDiaSignatureCapture(ITEMTENDER *pItem, SHORT sLen);
SHORT   ItemTendReadPadMsr (UIFDIADATA  *pDia, ITEMTENDER *pItem, SHORT sLen, UCHAR* pauchFsc);
SHORT	ItemTendCollectAcct(ITEMTENDER *pItem, SHORT sLen, UCHAR* pauchFsc);
SHORT   ItemTendReferenceAuthEntry(ITEMTENDER *pItem, SHORT sLen, UCHAR uchLeadAddress);
SHORT   ItemTendCheckAcctRoutSeqEntry(ITEMTENDER *pItem, SHORT sLen, UCHAR uchLeadAddress);
SHORT   ItemTendCheckLabel12(TCHAR *aszNumber);                             /* Saratoga */
SHORT   ItemTendCPFuncRsp1(UCHAR uchFCode, VOID *CPRcv, SHORT sResponse);   /* Saratoga */
SHORT   ItemTendEEPTAcceptReport(EEPTRSPDATA *CPRcvData, UIFREGTENDER *pUifTender, ITEMTENDER *pItem);/* Saratoga */
SHORT   ItemTendEEPTAccept(UCHAR uchFCode, ITEMTENDER *pItem, EEPTRSPDATA *CPRcvData);
SHORT   ItemTendCPRejectAsk(EEPTRSPDATA *CPRcv, ITEMTENDER *pItem);
SHORT   ItemTendCPReject(UCHAR uchFCode, EEPTRSPDATA *CPRcvData, ITEMTENDER *pItem);
SHORT   ItemTendCPOverride(UCHAR uchFCode, VOID *CPSendData, VOID *CPRcvData,
            UIFREGTENDER *pUifTender, ITEMTENDER *pItem, USHORT usEventSend);

VOID    ItemTendCPDispRspText(VOID *CPRcvData, TCHAR   *aszNumber);
VOID    ItemTendCPPrintRsp(ITEMTENDER *pItem);
VOID    ItemTendGetEPTMDC(UCHAR uchClass, UCHAR *auchStatus);
SHORT   ItemTendCPReadFileHeader (TRN_CPHEADER *pFileHeader, SHORT hsItemCPHandle);
VOID    ItemTendCPWriteFileHeader (TRN_CPHEADER *pFileHeader, SHORT hsItemCPHandle);
SHORT   ItemTendCPReadResponseHeader (USHORT usPos, TRN_CPRSPHEADER *pResponseHeader, SHORT hsItemCPHandle);
VOID    ItemTendCPWriteResponseHeader (USHORT usPos, TRN_CPRSPHEADER *pResponseHeader, SHORT hsItemCPHandle);
SHORT   ItemTendCPReadResponseMessage (USHORT usPos, WCHAR *pResponseMessage, SHORT hsItemCPHandle);
VOID    ItemTendCPWriteResponseMessage (USHORT usPos, WCHAR *pResponseMessage, SHORT hsItemCPHandle);
SHORT   ItemTendCPReadFile(ULONG ulOffset, VOID *pData, ULONG ulSize, SHORT hsHandle, ULONG *pulActualBytesRead);
VOID    ItemTendCPWriteFile(ULONG ulOffset, VOID *pData, ULONG ulSize, SHORT hsHandle);
VOID    ItemTendCPClearFile(VOID);
VOID    ItemTendCPCloseFile(VOID);


SHORT   ItemTendCheckSplitTender(VOID);
SHORT   ItemTendSplitTender(UIFREGTENDER *UifTender);     /* 6/6/96 */
VOID    ItemTendSplitTenderDup(UCHAR uchPrintType, UIFREGTENDER *pData2);
VOID    ItemTendAffectTaxSplit(UCHAR *puchTotal, ITEMAFFECTION *pData, UIFREGTENDER *pData2, SHORT sType);
VOID    ItemTendSplitCheckTotal(ITEMTOTAL *pData, UIFREGTENDER *pData2);
VOID    ItemTendFastTenderSplit(UIFREGTENDER *pData1, ITEMTENDER *pData2);
VOID    ItemTendDupRecSpl(ITEMTENDER *pData1, UIFREGTENDER *pData2);
VOID    ItemTendOverDupRecSpl(ITEMTENDER *pData1, UIFREGTENDER *pData2);
VOID    ItemTendDupCloseSpl(UCHAR uchPrintType);      /* 6/9/96 */
VOID    ItemTendAffectVATSplit(UCHAR *puchTotal, ITEMAFFECTION *pData, UIFREGTENDER *pData2, DCURRENCY *plData3, SHORT sType);
VOID    ItemTendOverTenderSplit( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender );
VOID    ItemTendSplitValidation(CONST UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender);

VOID    ItemTendEuroTender(CONST ITEMTENDER *ItemTender ); /* V3.4 */

VOID    ItemTendPrtFSChange( ITEMTENDER *pData );
VOID    ItemTendFSTotal( ITEMTOTAL *pData );
VOID    ItemTendSendFS( VOID );
SHORT   ItemTendCheckFS(UIFREGTENDER *pUifTender);
SHORT   ItemTendFS( UIFREGTENDER *pData );
VOID    ItemTendFSChit( VOID );
VOID    ItemTendPrintFSChange(VOID);
SHORT   ItemTendFSNonFoodPart( UIFREGTENDER *pData1, ITEMTENDER *pData2 );
VOID    ItemTendFSNonFoodPartTax( VOID );
VOID    ItemTendFSPart( UIFREGTENDER *pData1, ITEMTENDER *pData2 );
VOID    ItemTendFSDisp( UIFREGTENDER *pData1, ITEMTENDER *pData2 );
SHORT   ItemTendFSOver( UIFREGTENDER *pData1, ITEMTENDER *pData2 );
VOID    ItemTendFSOverTender( UIFREGTENDER  *pData1, ITEMTENDER *pData2 );
VOID    ItemTendFSSetAmount( UIFREGTENDER *pData1, ITEMTENDER *pData2 );
VOID    ItemTendFSOverDisp(UIFREGTENDER *pUifTender, ITEMTENDER *pItem);
VOID    ItemTendFSClose(VOID);

SHORT   ItemTendCPEPT(UIFREGTENDER *pUifTender, ITEMTENDER *pItem);
VOID    ItemTendCPCancel(VOID);
VOID    ItemTendAddTaxSum(DCURRENCY *plTax);
VOID    ItemTendTotalTax(ITEMTOTAL *pItem);
VOID    ItemTendTotal(VOID);
VOID    ItemTendSendTotal(VOID);
VOID    ItemTendTax(VOID);
SHORT   ItemTendECFS(VOID);
SHORT	ItemTendGiftCard(UIFREGTENDER *pUifRegTender);
VOID	ItemTendGiftReload(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender);
SHORT   ItemSalesRefEntry(UCHAR RefNo[NUM_REFERENCE]);
VOID DisplayGiftCard();
VOID EditGiftCardNumDisplay(ITEMTENDER *ItemTender,TRANGCFQUAL    *pWorkGCF, SHORT index);

/*
* --------------------------------------------------------------------------
    Gift Card Function Declarations
* --------------------------------------------------------------------------
*/
SHORT GetGiftCardAccount (SHORT sGiftCard, ITEMTENDER *pItem, UCHAR* pauchFsc);
SHORT ItemSalesGetBalance (UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender);
VOID  ItemSalesVoidIssue();
SHORT ItemSalesReload();
SHORT ItemSalesVoidReload();
VOID  GiftBalPrtPrep(ITEMTENDER *ItemTender);
VOID  GiftPreAuthMaskAcct(ITEMSALES *pItemSales);

/*
* --------------------------------------------------------------------------
    Misc. Transaction Module's Declaration
* --------------------------------------------------------------------------
*/
SHORT   ItemMiscCheckTransGC2GC(ITEMMISC *pItem);
SHORT   ItemMiscCheckTransGC2GCCheck(TRANGCFQUAL *pWorkGCF);
SHORT   ItemMiscCheckTransGC2GCChecker(ULONG *pusCasNo, USHORT usGuestNo);
VOID    ItemMiscCancelDispTotal(VOID);
SHORT   ItemMiscETKIn(UIFREGMISC *UifRegMisc, ETK_FIELD *ETKField);
SHORT   ItemMiscETKOut(UIFREGMISC *UifRegMisc, ETK_FIELD *ETKField);
VOID    ItemMiscETKPrint(UIFREGMISC *UifRegMisc, ETK_FIELD *ETKField);
VOID    ItemMiscPostReceiptAffect(UCHAR uchClass);
SHORT   ItemMiscMoneyEC(ITEMMISC *pTrnMisc);

/*
* --------------------------------------------------------------------------
    Modifier Module's functions Declaration
* --------------------------------------------------------------------------
*/

SHORT   ItemModPvoidSPV(VOID);
SHORT   ItemModTaxUS(CONST UIFREGMODIFIER *pData);
SHORT   ItemModTaxCanada(CONST UIFREGMODIFIER *pData);
SHORT   ItemModTareCheckSeq(CONST UIFREGMODIFIER *pData);
VOID    ItemModTareDispMessage(CONST UIFREGMODIFIER *pData);
BOOL    ItemModCheckLocal(UCHAR fchBit);
VOID    ItemModSetLocal(UCHAR fchBit);
/*
* --------------------------------------------------------------------------
    Other Module's Functions Declaration
* --------------------------------------------------------------------------
*/

VOID    ItemOtherHeader(SHORT sLine);
SHORT   ItemOtherECFS(VOID);                            /* Saratoga */
VOID    ItemOtherECFC(VOID);
SHORT   ItemOtherEC( VOID );
SHORT   ItemOtherECSales( ITEMCOMMONLOCAL *pData );
SHORT   ItemOtherECSPV(VOID);
SHORT   ItemOtherECDisc( ITEMCOMMONLOCAL *pData );
SHORT ItemOtherECCoupon(ITEMCOMMONLOCAL *pData);         /* R3.0 */
SHORT   ItemOtherECTend( ITEMCOMMONLOCAL *pData );
SHORT   ItemOtherECMod( ITEMCOMMONLOCAL *pData );
VOID    ItemOtherECError(VOID);
VOID    ItemOtherEJChgNULL( UCHAR *pData, UCHAR uchLen );
SHORT   ItemOtherForward(VOID);
SHORT   ItemOtherBackward(VOID);
SHORT   ItemOtherScrollUpCons( VOID );

SHORT   ItemOtherSplForward(VOID);
SHORT   ItemOtherSplBackward(VOID);
SHORT   ItemOtherTransferCheck(VOID);
VOID    ItemOtherTransfDisp(UCHAR uchPreSeat, UCHAR uchCurSeat);
SHORT   ItemOtherTransferQty(UIFREGOTHER *pData, ITEMSALES *pItem, SHORT *sQty);
SHORT   ItemOtherSeatNo(UCHAR *puchSeatNo);
VOID    ItemOtherDispGuestNo(ULONG ulCashierNo);   /* V3.3 */
VOID    ItemOtherDispTableNo(ULONG ulCashierNo);
SHORT   ItemOtherAskEPT(UIFREGOTHER *pItem);        /* Saratoga */
SHORT   ItemOtherAskFreq(UIFREGOTHER *pItem);       /* Saratoga */
SHORT   ItemOtherAskReport(UIFREGOTHER *pItem);     /* Saratoga */
SHORT   ItemOtherECFS(VOID);
SHORT   ItemOtherValDisp(VOID);						//SR 47  Manual Validation
VOID    ItemOtherValInDisp(SHORT sType)	;			//SR 47	 Manual Validation
SHORT	ItemCouponCheckMultiple(ITEMCOUPON *ItemCoupon);	//SR 47 Double/Triple Coupon

/*
* --------------------------------------------------------------------------
    Operator Close Module's Functions Declaration
* --------------------------------------------------------------------------
*/

SHORT   ItemCasSignOut(UIFREGOPECLOSE *pData1, ITEMOPECLOSE *pData2);
VOID    ItemCasOutDisplay(VOID);
VOID    ItemCasRelOnFinDisp(VOID);
VOID    ItemCasOutInitModLocal(VOID);
SHORT   ItemCasSignOutFinalize(SHORT *psFinalize);  /* V3.3 */
SHORT   ItemWaiSignOut(UIFREGOPECLOSE *pData1, ITEMOPECLOSE *pData2);
VOID    ItemWaiOutDisplay(VOID);
SHORT   ItemWaiRelOnFinOut(ITEMOPECLOSE *pData);
SHORT   ItemModeKeyOut(ITEMOPECLOSE *pData);
SHORT   ItemWaiRemoveKey(ITEMOPECLOSE *pData);
SHORT   ItemWaiExecTtlFunc(VOID);
SHORT   ItemWaiExecTendFunc(VOID);
VOID    ItemSignOutResetMenuPage(VOID); /* 07/07/98 */

SHORT   ItemBarSignOut(UIFREGOPECLOSE *pData1, ITEMOPECLOSE *pData2);
SHORT   ItemCasIntPreSignOut(UIFREGOPECLOSE *pData);

/*
* --------------------------------------------------------------------------
    Mode-in Module's Functions Declaration
* --------------------------------------------------------------------------
*/
VOID   ItemModeInDisplay(UIFREGMODEIN *pData);

/*
* --------------------------------------------------------------------------
   Common Module's Functions Declaration
* --------------------------------------------------------------------------
*/

/*
* --------------------------------------------------------------------------
    Common Module's Functions Declaration
* --------------------------------------------------------------------------
*/
    typedef struct {
        DCURRENCY    lGSTItemizer;                   /* GST itemizer */
        DCURRENCY    lPST1LimitItemizer;             /* PST1 limit itemizer */
        DCURRENCY    lPST1NonLimitItemizer;          /* PST1 non-limit itemizer */
        DCURRENCY    lPST2Itemizer;                  /* PST2 itemizer */
        DCURRENCY    lPST3Itemizer;                  /* PST3 itemizer */
        DCURRENCY    lGSTPST1lItemizer;              /* GST/PST1 itemizer */
        DCURRENCY    lGSTPST1nlItemizer;             /* GST/PST1 non-limit itemizer */
        DCURRENCY    lGSTPST2Itemizer;               /* GST/PST2 itemizer */
        DCURRENCY    lGSTPST3Itemizer;               /* GST/PST3 itemizer */
    }CANTAX;


    typedef struct {
        DCURRENCY    lTaxable[STD_PLU_ITEMIZERS];                    /* work taxable itemizer */
    }CANTAXWORK;


USHORT  ItemCommonConvErr(SHORT sData);
SHORT   ItemCurTax( CONST UCHAR *puchTotal, ITEMCOMMONTAX *pData2 );
VOID    ItemCurTaxWork( CONST UCHAR *puchTotal, ITEMCOMMONTAX *pData2 );
SHORT   ItemCurTaxCal( CONST UCHAR *puchTotal, ITEMCOMMONTAX *pData2 );
SHORT   ItemCurTax1( ITEMCOMMONTAX *pData );
SHORT   ItemCurTax2( ITEMCOMMONTAX *pData );
SHORT   ItemCurTax3( ITEMCOMMONTAX *pData );
VOID    ItemCurTaxCor( CONST UCHAR *puchTotal, ITEMCOMMONTAX *pData2 );

VOID	ItemCanTaxSpecBakedCorr(ITEMCOMMONTAX *WorkTax);
VOID    ItemCanTax(CONST UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax, SHORT sType);
VOID    ItemCanTaxWork1( CANTAXWORK *CanTaxWork,
            CANTAX *CanTax, ITEMCOMMONTAX *WorkTax, SHORT sType,  USHORT *pfsLimitable, USHORT *pfsBaked );
VOID    ItemCanTaxDisc(CANTAX *CanTax, SHORT sType,  USHORT fsLimitable, USHORT fsBaked);
VOID    ItemCanTaxWork2( CONST UCHAR *puchTotal, CANTAX *CanTax, SHORT sType, USHORT fsLimitable, USHORT fsBaked );
VOID    ItemCanTaxWork3( CONST CANTAX *CanTax, ITEMCOMMONTAX *WorkTax, SHORT sType );
VOID    ItemCanTaxWork4( ITEMCOMMONTAX *WorkTax, SHORT sType );
VOID    ItemCanTaxCal( CONST UCHAR *puchTotal,ITEMCOMMONTAX *WorkTax );
VOID    ItemCanTaxCalGST( ITEMCOMMONTAX *WorkTax );
VOID    ItemCanTaxCalPST1( ITEMCOMMONTAX *WorkTax );
VOID    ItemCanTaxCalPST2( ITEMCOMMONTAX *WorkTax );
VOID    ItemCanTaxCalPST3( ITEMCOMMONTAX *WorkTax );
VOID    ItemCanTaxCor( CONST UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax, SHORT sType );
VOID    ItemUSTaxCopy1(ITEMAFFECTION *pAffect, CONST ITEMCOMMONTAX *pTax);
VOID    ItemCanTaxCopy1(ITEMAFFECTION *pAffect, CONST ITEMCOMMONTAX *pTax);
VOID    ItemCanTaxCopy2(ITEMAFFECTION *pAffect);

SHORT   ItemSPVIntDialog( UCHAR uchLeadAddr, UCHAR uchDialogLeadAddr );
SHORT   ItemSPVInt( UCHAR uchLeadAddr );
VOID    ItemCurPrintStatus( VOID );
VOID    ItemCheckAndSetTransDate( VOID );
VOID    ItemCountCons( VOID );
VOID    ItemDrawer( VOID );
VOID    ItemTrailer( SHORT sType );
VOID    ItemNormalTrailer( SHORT sType );
VOID    ItemTicketTrailer( SHORT sType );
VOID    ItemPromotion(USHORT fsPrintStatus, UCHAR uchPrintType);
VOID    ItemHeader(UCHAR uchPrintType);
VOID    ItemInit( VOID );
ITEMCOND  ItemCondimentData(CONST ITEMSALES *pItemSales);
SHORT   ItemPreviousItemClearStore (SHORT bClearStore, VOID *pReceiveItem, USHORT usTempBuffSize);
SHORT   ItemPreviousItem( VOID *pData, USHORT SizeData);
SHORT   ItemPreviousItemSpl(CONST VOID *pItem, SHORT sType);
VOID    ItemPrevSalesValPrint( VOID );
VOID    ItemPrevSalesTicketPrint( VOID );
VOID    ItemPrevTicketHeader(USHORT fsPrintStatus);
VOID    ItemPrevChargeTipTicketPrint( ITEMDISC *pData );
SHORT   ItemPrevSendSales( VOID );
SHORT   ItemPrevSendDisc( VOID );
SHORT   ItemPrevSendSalesAndDiscount(VOID);
VOID    ItemPrevCancelAutoTips(VOID);

USHORT ItemReturnRegularDiscountInit (VOID);
USHORT ItemReturnRegularDiscountAdd (ITEMDISC  *pItemDisc);

SHORT   ItemPModCompare(UCHAR *uchCurPMod, UCHAR *uchPrevPMod);
SHORT   ItemCondCompare(USHORT *usCurCondiment, ITEMCOND *pPrevCondiment);
SHORT   ItemCommonCheckComp( VOID );
SHORT   ItemPrevModDisc(VOID);
SHORT   ItemFinalize(VOID);
VOID    ItemDispRel(VOID);
SHORT   ItemPrevTotalProc(VOID *pData, USHORT usTempBuffSize);
VOID    ItemPrintStart( UCHAR uchType );
SHORT   ItemPrevChargeTipAffection(ITEMDISC *pDisc);
SHORT   ItemCommonCheckSplit(VOID);
SHORT   ItemCommonCheckExempt(VOID);
VOID    ItemCommonCancelExempt(VOID);
VOID    ItemCommonSetDisplayFlagExempt (int iKeepIndic);
SHORT   ItemCommonCheckScaleDiv(ITEMMODSCALE *pData, LONG lScaleData);
SHORT   ItemCommonGetScaleData(ITEMMODSCALE *pData);
SHORT   ItemCommonReadScale(UIE_SCALE *pData);
USHORT  ItemCommonUniqueTransNo( VOID );                            /* R3.0 */
SHORT   ItemCommonSort(ITEMSALES *pData1, ITEMSALES *pData2, UCHAR **pData3);
VOID    ItemCommonMldInit( VOID );
VOID    ItemCommonShiftPage(UCHAR uchPage);
//VOID    ItemCommonFDTPara(ITEMFDTPARA *pData);
VOID    ItemCommonDispSubTotal(CONST VOID *pData);
VOID    ItemCommonSubTotalVoid(VOID *pData);
VOID    ItemCommonDispECSubTotal(CONST VOID *pData);  /* add R3.0 */
SHORT   ItemCommonCheckSize(VOID *pData, USHORT usSize);
VOID    ItemCommonCancelGC(TRANGCFQUAL *pData);
VOID    ItemCommonScrollPause( VOID );          /* add R3.1 */
SHORT   ItemCommonCheckNonGCTrans( VOID );
SHORT   ItemCommonCheckCashDrawerOpen (VOID);
SHORT   ItemCommonCheckCloseCheck( VOID );
VOID    ItemCommonCheckEndorse(UCHAR uchMinorClass, DCURRENCY lAmount, DCURRENCY lSalesAmount, SHORT *psFlag);    /* DTREE#2 */
SHORT   ItemCalAmount(LONG lQty, USHORT usFor, LONG lUnitPrice, DCURRENCY *plAmount); //SR 143 cwunn @/For key

SHORT   ItemCurTaxSpl(CONST UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax);
VOID    ItemCurTaxWorkSpl(CONST UCHAR *puchTotal, ITEMCOMMONTAX *WorkTax);

SHORT   ItemCommonGetSRAutoNo(USHORT *pusGuestNo);  /* R3.3 */
SHORT   ItemCommonTransOpen(USHORT usGuestNo);      /* R3.3 */
void    ItemCommonResetLocalCounts (VOID);
SHORT   ItemCommonTaxSystem(VOID);
VOID    ItemCommonDiscStatus(ITEMDISC *pItem, SHORT sType);
SHORT   ItemCommonIncExcVAT(VOID);

SHORT   ItemCurVAT(CONST UCHAR *puchTotal, ITEMCOMMONVAT *pItemCommonVAT, SHORT sType);

SHORT   ItemCommonLabelPosi(TCHAR *pszPluNumber);
TCHAR   ItemCommonCDV(TCHAR *aszPluNo, SHORT sLen);
VOID    ItemCommonOpenAsk(EEPTRSPDATA *CPRcvData);             /* Saratoga */
VOID    ItemWICRelOnFin(ULONG fsStatus);                /* Saratoga */

/* enhanced KDS functions, 2172 */
SHORT   ItemSendKds(VOID *pItem, USHORT fsEC);
SHORT   ItemSendKdsSales(ITEMSALES *pItem, USHORT fsEC);
SHORT	ItemSendKdsOrderDeclaration(ITEMSALES *pItem);
USHORT	ItemSendKdsItemSetLineNumber(CONST ITEMSALES *pItem);


SHORT   ItemSalesOEPUpdateGroupNoCondiment(TCHAR *pauchPlu, UCHAR uchStaus,
                                          UCHAR *pszGroup,
                                          USHORT usGroupOff,
                                          USHORT *pfsType,
                                          USHORT usContinue,
                                          USHORT usNoOfGroup,
                                          UCHAR  uchTableNumber);

SHORT   ItemSalesOEPNextPluCondiment(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales, UCHAR *pszGroups);

SHORT ItemSalesOEPAdjective(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales, UCHAR *pszGroups);

/*
* --------------------------------------------------------------------------
    Local Data Manipulate Functions Declaration
* --------------------------------------------------------------------------
*/
ITEMTRANSLOCAL *ItemTransGetLocalPointer (VOID);
VOID   ItemTransGetLocal(ITEMTRANSLOCAL *pTransLocal);
VOID   ItemTransPutLocal(ITEMTRANSLOCAL *pTransLocal);
ITEMSALESLOCAL *ItemSalesGetLocalPointer (VOID);
VOID   ItemSalesGetLocal(ITEMSALESLOCAL *pData);
VOID   ItemSalesPutLocal(ITEMSALESLOCAL *pData);
SHORT  ItemTenderGetSetTipReturn (SHORT bReturnTipState);
ITEMTOTALLOCAL *ItemTotalGetLocalPointer (VOID);
VOID   ItemTotalGetLocal(ITEMTOTALLOCAL *pData);
VOID   ItemTotalPutLocal(ITEMTOTALLOCAL *pData);
ITEMTENDERLOCAL *ItemTenderGetLocalPointer (VOID);
VOID   ItemTenderGetLocal(ITEMTENDERLOCAL *pData);
VOID   ItemTenderPutLocal(ITEMTENDERLOCAL *pData);
ITEMMODLOCAL *ItemModGetLocalPtr(VOID);
ITEMMODSCALE *ItemModGetLocalScaleDataPtr(VOID);
VOID   ItemModGetLocal(ITEMMODLOCAL *pData);
VOID   ItemModPutLocal(ITEMMODLOCAL *pData);
ITEMOTHERLOCAL *ItemOtherGetLocalPointer (VOID);
ITEMCOMMONLOCAL *ItemCommonGetLocalPointer(VOID);
VOID   *ItemCommonGetLocalItemUnionPointer(VOID);
VOID   ItemCommonGetLocal(ITEMCOMMONLOCAL **pData);
VOID   ItemCommonPutLocal(ITEMCOMMONLOCAL *pData);

SHORT  ItemComonCheckReturnCardLabel (TCHAR *aszCardLabel);

VOID   ItemMiscSetStatus( USHORT usStatusBit );
VOID   ItemMiscResetStatus( USHORT usStatusBit );
VOID   ItemMiscSetNonGCStatus( VOID );

USHORT ItemPreviousCondiment(ITEMCOND *CondimentData, USHORT usAffect); //SR 192 JHHJ
USHORT ItemSalesMoveCondiment(ITEMSALES *ItemSales,USHORT usCondNumber,USHORT usType); //SR 192 JHHJ

/*
*===========================================================================
    External Valiables Definition
*===========================================================================
*/

extern ITEMTRANSLOCAL   ItemTransLocal;     /* Current Transaction global data such as itemizers */
extern ITEMSALESLOCAL   ItemSalesLocal;     /* Current Sales state data such as OEP group, menu shift state */
extern ITEMTENDERLOCAL  ItemTenderLocal;    /* Current Tender state and calculation data */
extern ITEMMODLOCAL     ItemModLocal;       /* Current Modifier state data such as modifier key press, scale data  */
extern ITEMOTHERLOCAL   ItemOtherLocal;     /* Other module local data */
extern ITEMCOMMONLOCAL  ItemCommonLocal;    /* Common module local data */
extern ITEMTOTALLOCAL   ItemTotalLocal;     /* Total Key module local data */
extern ITEMMISCLOCAL    ItemMiscLocal;      /* Misc. transaction module local data */

extern CONST volatile ITEMTRANSLOCAL * CONST ItemTransLocalPtr;
extern CONST volatile ITEMSALESLOCAL * CONST ItemSalesLocalPtr;
extern CONST volatile ITEMTENDERLOCAL * CONST ItemTenderLocalPtr;
extern CONST volatile ITEMMODLOCAL * CONST ItemModLocalPtr;
extern CONST volatile ITEMCOMMONLOCAL * CONST ItemCommonLocalPtr;
extern CONST volatile ITEMSALES       * CONST ItemCommonLocalItemUnionSalesPtr;
extern CONST volatile ITEMTOTALLOCAL * CONST ItemTotalLocalPtr;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

#endif
/****** end of definition ******/