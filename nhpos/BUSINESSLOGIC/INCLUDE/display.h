/*
*===========================================================================
* Title       : Display Module Header                         
* Category    : Display, NCR 2170 US Hospitality Application        
* Program Name: DISPLAY.H
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
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-14-92: J.Ikeda   : initial                                   
* Jun-21-93: K.you     : add CLASS_REGDISP_ETK.                                   
*
** NCR2172 **
*
* Oct-05-99: M.Teraki  : initial (for 2172)
*
*** GenPOS **
* Oct-02-17 : 02.02.02  : R.Chambers  : added new uchMinorClass CLASS_UIFDISP_DESCONLY.
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

#if !defined(DISPLAY_H_INCLUDE)
#define DISPLAY_H_INCLUDE

#include <regstrct.h>

/*------------------------------------------------------------------------*\

            D E F I N I T I O N    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
*       CLASS FOR MODIFY OPERATION 
--------------------------------------------------------------------------*/

/* Major Class -----------------------------------------------------------*/

#define     CLASS_UIFREGDISP        70           /* display module */

/* Minor Class -----------------------------------------------------------*/

#define     CLASS_UIFDISP_DESCONLY  0    /* special minor class to display descriptors only. */
#define     CLASS_UIFDISP_QTY       1    /* QTY key class */
#define     CLASS_UIFDISP_VOID      2    /* void key class */
#define     CLASS_UIFDISP_NUMBER    3    /* number key class */
#define     CLASS_UIFDISP_ADJ       4    /* adjective key class */
#define     CLASS_UIFDISP_MODDISC   5    /* item disc #2 key class */
#define     CLASS_UIFDISP_REMOVEKEY 6    /* remove key class */
#define     CLASS_UIFDISP_LOCK      7    /* lock positon display class */
#define     CLASS_UIFDISP_E_VERSION 8    /* e-version modifier key, 2172 */
#define     CLASS_UIFDISP_PRICECHECK 9   /* price check key, 2172 */
#define     CLASS_UIFDISP_DEPTENTER 10  /* dept no. entry, 2172 */
#define     CLASS_UIFDISP_PRICEENTER 11 /* price entry, 2172 */
#define     CLASS_UIFDISP_SKUNUMBER  12    /* sku number key class, 2172 */
#define     CLASS_UIFDISP_CONSNUMBER 13    /* consumer number key class, 2172 */
#define     CLASS_UIFDISP_POWERDOWN  14    /* power down class, 2172 */
#define     CLASS_UIFDISP_POWERUP    15    /* power up class, 2172 */
#define		CLASS_UIFDISP_FOR		 16		/*@FOR SR281 ESMITH */
#define		CLASS_UIFDISP_TAREENTER	 17		/*Tare Entry JHHJ */

/*--------------------------------------------------------------------------
*       CLASS FOR COMPULSORY OPERATION 
--------------------------------------------------------------------------*/

/* Major Class -----------------------------------------------------------*/

#define     CLASS_REGDISPCOMPLSRY       71   /* display module */

/* Minor Class -----------------------------------------------------------*/

#define     CLASS_REGDISP_SUPERINT      10   /* supr. intervention class */
#define     CLASS_REGDISP_REMOVEKEY     11   /* waiter lock key remove request class */
#define     CLASS_REGDISP_GCNO          12   /* GC No. entry request class */
#define     CLASS_REGDISP_TABLENO       13   /* table No. entry request class */
#define     CLASS_REGDISP_PERSON        14   /* No. of person entry request class */
#define     CLASS_REGDISP_LINENO        15   /* line No. entry request class */
#define     CLASS_REGDISP_PM_COND       16   /* print mod. condiment entry request class */
#define     CLASS_REGDISP_CASHIERNO     17   /* cashier No. entry request class */
#define     CLASS_REGDISP_WAITERNO      18   /* waiter No. entry request class */
#define     CLASS_REGDISP_ETK           19   /* ETK job code entry request class */
#define     CLASS_REGDISP_CPRN          20   /* room no. entry request class */
#define     CLASS_REGDISP_CPGI          21   /* guest id entry request class */
#define     CLASS_REGDISP_CPLN          22   /* guest line no. entry request class */
#define     CLASS_REGDISP_CPAN          23   /* account no. entry request class */
#define     CLASS_REGDISP_OEPNO         24   /* order entry prompt no request class */
#define     CLASS_REGDISP_PAUSE         25   /* macro key pause request class */
#define     CLASS_REGDISP_CUSTOMER      26   /* customer name entry request class */
#define     CLASS_REGDISP_SEATNO        27   /* seat number for split check,  R3.1 */
#define     CLASS_REGDISP_INSERTKEY     28   /* insert server lock,  V3.3 */
#define     CLASS_REGDISP_SECRET        30   /* Secret No. entry request class, V3.3 */
#define     CLASS_REGDISP_DEPTNO        31   /* Secret No. entry request class, 2172 */
#define     CLASS_REGDISP_PCHECK        32   /* price check entry request class, 2172 */
#define     CLASS_REGDISP_BIRTHDAY      33   /* birthday entry request class, 2172 */
#define     CLASS_REGDISP_BUILDING      34   /* start plu building, 2172 */
#define     CLASS_REGDISP_CLEAR         35   /* Clear key Request,  Saratoga */
#define     CLASS_REGDISP_PCHECK_PM     36   /* price entry with PM, 2172 */
#define     CLASS_REGDISP_WICMOD        37   /* compulsory WIC modifier, Saratoga */

#define     CLASS_REGDISP_OEP_LEAD      100  /* order entry prompt display for 2x20 */
#define     CLASS_REGDISP_OEP_PLU       101  /* order entry prompt display for 2x20 */
#define     CLASS_REGDISP_EVENTTRIGGER  102  // event specific key press required

/*--------------------------------------------------------------------------
*       CLASS FOR SCALABLE ITEM DISPLAY FUNCTIONS 
--------------------------------------------------------------------------*/

/* Major Class -----------------------------------------------------------*/

#define     CLASS_REGDISP_SALES         72   

/* Minor Class -----------------------------------------------------------*/
#define     CLASS_REGDISP_SCALE2DIGIT   1   /* scalable item display (2 digits type) */
#define     CLASS_REGDISP_SCALE3DIGIT   2   /* scalable item display (3 digits type) */


/*--------------------------------------------------------------------------
*       CLASS FOR FLEXIBLE MEMORY ASSIGNMENT 
--------------------------------------------------------------------------*/

/* Major Class -----------------------------------------------------------*/

#define     CLASS_PARAFLEXMEM_DISPRECORD    1           /* Indicaded Record of Flexible Memory */
#define     CLASS_PARAFLEXMEM_DISPFLAG      2           /* Indicaded PTD Flag of Flexible Memory */


/*--------------------------------------------------------------------------
*       CLASS FOR DEPT/PLU ASSIGNMENT 
--------------------------------------------------------------------------*/

/* Major Class -----------------------------------------------------------*/

#define     CLASS_PARADEPT_DATA             1           /* Display Not Status Data */
#define     CLASS_PARADEPT_STS              2           /* Display Status Data */

#define     CLASS_PARAPLU_DISPMAINT_STS     3           /* Display Status Data */
#define     CLASS_PARAPLU_DISPPRICE_STS     4           /* Display Status Data */
#define     CLASS_PARAPLU_DISPADD_STS       5           /* Display Status Data */
#define     CLASS_PARAPLU_DISPMNEMO_STS     6           /* Display Status Data */
#define     CLASS_PARAPLU_DISPMAINT_DATA    7           /* Display Not Status Data */
#define     CLASS_PARAPLU_DISPPRICE_DATA    8           /* Display Not Status Data */
#define     CLASS_PARAPLU_DISPADD_DATA      9           /* Display Not Status Data */
#define     CLASS_PARAPLU_DISPMNEMO_DATA    10          /* Display Not Status Data */

/*--------------------------------------------------------------------------
*       CLASS FOR WAITER NO. ASSIGNMENT 
--------------------------------------------------------------------------*/

#define CLASS_PARAWAITERNO_DISPWAI          1
#define CLASS_PARAWAITERNO_DISPSTGC         2
#define CLASS_PARAWAITERNO_DISPENDGC        3

/*
*------------------------------------------------------------------------------
*   MISC DATA DEFINE DECRALATIONS
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
*   DESCRIPTOR CONTROL FLAG DATA DEFINE DECRALATION
*   used with global ULONG  flDispRegDescrControl as part of the descriptor
*   indicator logic.  See functions DispRegSetOpStat() and DispRegOpDscrCntr().
*------------------------------------------------------------------------------
*/
#define FOODSTAMP_CTL   0x00000000L     /* FS Modifier (NOT USED),  Saratoga */

#define RECEIPT_CNTRL   0x00000001L     /* Bit 0 : Receipt Key sets this flag */                

#define VOID_CNTRL      0x00000002L     /* Bit 1 : Void key sets this flag */   
#define EC_CNTRL        0x00000004L     /* Bit 2 : E.C key sets this flag */    

#define TVOID_CNTRL     0x00000008L     /* Bit 3 : Transaction Void key sets this flag */   
#define WIC_CNTRL       0x00000008L     /* Bit 3 : WIC key sets this flag,  Saratoga */   

#define ITEMDISC_CNTRL  0x00000010L     /* Bit 4 : Item Discount Operation sets this flag */    
#define REGDISC_CNTRL   0x00000020L     /* Bit 5 : Regular Discount sets this flag */   
#define CHRGTIP_CNTRL   0x00000040L     /* Bit 6 : Charge Tip sets this flag */ 

#define CRED_CNTRL      0x00000080L     /* Bit 7 : Credit Dept or Coupon PLU sets this flag */  

#define TAXMOD_CNTRL    0x00000100L     /* Bit 8 : Tax Modifier Key sets this flag */
#define TAXEXMPT_CNTRL  0x00000200L     /* Bit 9 : Tax Exempt Key sets this flag */
#define TRANSFR_CNTRL   0x00000400L     /* Bit 10 : Guest Check Transfer operation sets this flag */

#define SUBTTL_CNTRL    0x00000800L     /* Bit 11 : Subtotal Key sets this flag */  
#define TOTAL_CNTRL     0x00001000L     /* Bit 12 : Total Key sets this flag */ 
#define TENDER_CNTRL    0x00002000L     /* Bit 13 : Tender Key sets this flag */    
#define PTEND_CNTRL     0x00004000L     /* Bit 14 : Partial Tender operation sets this flag */  
#define PO_CNTRL        0x00008000L     /* Bit 15 : Paid Out operation sets this flag */    
#define ROA_CNTRL       0x00010000L     /* Bit 16 : R.O.A operation sets this flag */   
#define TIPSPO_CNTRL    0x00020000L     /* Bit 17 : Tips P.O operation sets this flag */    
#define DECTIP_CNTRL    0x00040000L     /* Bit 18 : Declared Tips operation sets this flag */   

#define CHANGE_CNTRL    0x00080000L     /* Bit 19 : Change operation sets this flag */  

#define NEWCHK_CNTRL    0x00100000L     /* Bit 20 : New Check operation sets this flag */   
#define STORE_CNTRL     0x00200000L     /* Bit 21 : Store operation sets this flag */   

#define PBAL_CNTRL      0x00400000L     /* Bit 22 : Previous Balance operation sets this flag */    

#define PAYMENT_CNTRL   0x00800000L     /* Bit 23 : Payment operation sets this flag */ 
#define RECALL_CNTRL    0x01000000L     /* Bit 24 : Recall operation sets this flag */  

#define TRAIN_CNTRL     0x02000000L     /* Bit 25 : Training operator sets this flag */ 

#define SLIP_CNTRL      0x04000000L     /* Bit 26 : Slip Request Function sets this flag */ 

#define COMPLSRY_CNTRL  0x08000000L     /* Bit 27 : Compulsory Key Function sets this flag */   
#define PM_CNTRL        0x10000000L     /* Bit 28 : Print Modifier sets this flag */    
#define ADJ_CNTRL       0x20000000L     /* Bit 29 : KDS down sets this flag. Was used for Adjective Key sets this flag */ 
#define PRT_OFFLINE_CNTRL 0x40000000L     /* Bit 30 : Take-to-Kitchen Opearation sets this flag */    

#define COMM_CNTRL      0x80000000L     /* Bit 31 : Communication Control */    
/*
*------------------------------------------------------------------------------
*   DISPLAY DATA SAVE CONTROL FLAG DEFINITION 
*------------------------------------------------------------------------------
*/

#define DISP_SAVE_DATA          0x01            /* Save display data for Re-write function */   
#define DISP_CLEAR_ALL_DESCRI   0L              /* Save display data for Re-write function */   

/*                 
*------------------------------------------------------------------------------
*   2X20 DISPLAY'S BIT CONTROL IN 'fbSaveControl' ( REGDISPMSG )
*   also applies to descriptor window display with the various descriptor state indicators
*   that can be added to a layout using the Layout Manager screen design utility.
*
*  The following Save Control types manipulate several data structures within the display
*  logic by either clearing saved areas, saving the message in a redisplay area, or
*  saving the current display descriptors in a redisplay area.
*  See function DispRegSave() as well as function DispWriteSpecial20() and DispWriteSpecial().
*
*  See DispRegOpDscrCntr() which iterates over the list of descriptors displaying their
*  current state using function UieDescriptor().
*  See use of function DispRegRewrite() which does a rewrite of the screen, usually with DispRegSaveData.
*
*  Most calls into the display logic using DispWrite() or similar functions will also
*  refresh the descriptor window and the state indicators displayed there. These state
*  indicators are derived from the NCR 2170 2X20 LCD display which had two lines of text
*  and a set of LEDs that displayed various state information through being on or off or
*  blinking. That functionality was replicated with first NHPOS on the NCR 7448 and
*  now GenPOS on standard terminal displays with a line of text with acronyms for the
*  various states. The displayed text has several possible views:
*    - not shown       -> LED is off
*    - shown as gray   -> LED is blinking
*    - show as red     -> LED is on
*------------------------------------------------------------------------------
*/
#define DISP_SAVE_TYPE_MASK           0x07      // mask used to separate the type (lower 4 bits) from the flags (upper 4 bites)
#define DISP_SAVE_TYPE_0              0x00      // display current display descriptors, clear saved display message area
#define DISP_SAVE_TYPE_1              0x01      // TYPE_1 save display data. also save current display descriptors.
#define DISP_SAVE_TYPE_2              0x02      // TYPE_2 save display data, no mnemonic displayed
#define DISP_SAVE_TYPE_3              0x03      // TYPE_3 save display data, display mnemonic LDT_REGMSG_ADR and mnemonic
#define DISP_SAVE_TYPE_4              0x04      // TYPE_4 display mnemonic with the menu page number
#define DISP_SAVE_TYPE_5              0x05      // TYPE_5 same as TYPE_1 but also save
#define DISP_SAVE_ECHO_ONLY           0x20      // Flag causing many lower level display functions to return doing nothing. used to trigger call to DispRegOpEchoBack().

// These follow the bit settings for Foreign Currency Tender keys, MDC #79, #80, etc.
#define DISP_SAVE_FOREIGN_MASK        0xC0      // Upper 2 bits of upper nibble indicates assumed decimal point for foreign currency
#define DISP_SAVE_DECIMAL_0           0xC0      // foreign currency entry with no decimal point
#define DISP_SAVE_DECIMAL_2           0x00      // foreign currency entry with 2 decimal point
#define DISP_SAVE_DECIMAL_3           0x40      // foreign currency entry with 3 decimal point

/*
*------------------------------------------------------------------------------
*   FLAG DEFINITION FOR FOREIGN CURRENCY TOTAL  
*------------------------------------------------------------------------------
*/
#define FC_INTL_OR_JPN          0x40            /* for Inter or Japan */
#define FC_JPN                  0x80            /* for Japan */
        
/*------------------------------------------------------------------------*\

  I N T E R F A C E   D A T A    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


#define NO_OF_ORDER             7

typedef struct {
        UCHAR       uchMajorClass;
        UCHAR       uchMinorClass;
        LONG        lQTY;
        DCURRENCY   lAmount;
        TCHAR       aszMnemonic[STD_TRANSMNEM_LEN + 1];
        TCHAR       aszAdjMnem[STD_ADJMNEMO_LEN + 1];
        TCHAR       aszStringData[STD_NUMBER_LEN + 1];
        UCHAR       fbSaveControl;                         /* whether to save data or not, see DISP_SAVE_TYPE_0, DISP_SAVE_TYPE_1, etc. */
        UCHAR       uchFsc;                                /* interface data with U.I */
        USHORT      auchOrderNo[NO_OF_ORDER];              /* Order No.s */
        TCHAR       aszSPMnemonic[STD_SPEMNEMO_LEN + 1];
        UCHAR       uchArrow;                              /* arrow data for scroll */
        TCHAR       aszAdjMnem2[STD_ADJMNEMO_LEN + 1];     /* use for oep scroll */
}REGDISPMSG;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*------------------------------------------------------------------------*\

        E X T E R N  V A L I A B L E S    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

extern  REGDISPMSG RegDispMsg;            /* display data structure */
extern  REGDISPMSG DispRegSaveData;       /* saved data structure for re-display */
extern	REGDISPMSG DispRegPrevData;		  /* saved data structure for re-display SR 155*/
extern	REGDISPMSG DispRegRedisplayData;  /* saved data structure for re-display JHHJ*/

extern  UCHAR   uchDispCurrMenuPage;      /* current menu page 1 to 5 */
extern  UCHAR   uchDispRepeatCo;          /* repeat counter */
extern  ULONG   flDispRegDescrControl;    /* descriptor control flag */
extern  ULONG   flDispRegKeepControl;     /* descriptor keep control flag */
extern  ULONG   ulDispRegOpOldStat;       /* save current descriptor status for next timing */  
extern  UCHAR   uchDispRegCuOldStat;      /* save current descriptor status for next timing */  
extern  ULONG   ulDispRegSaveDsc;         /* save descriptor status for clear key entry */  
extern  ULONG   ulDispRegOpBackStat;      /* save descriptor status in echo back */

/*------------------------------------------------------------------------*\

            P R O T O T Y P E    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

VOID DispWrite( VOID *pData );                  /* make data & output display device */
VOID DispWriteSpecial( REGDISPMSG *pData1, REGDISPMSG *pData2 );    
                                                /* make data & output display device */
VOID DispWrite20( VOID *pData );                     /* disp20.c */
VOID DispWriteSpecial20( REGDISPMSG *pData1, REGDISPMSG *pData2 );    
                                                    /*  disp20.c */
VOID DispWrite10( VOID *pData );                     /* disp10.c */
VOID DispWriteSpecial10( REGDISPMSG *pData1, REGDISPMSG *pData2 );    
                                                    /*  disp10.c */

#endif                    
