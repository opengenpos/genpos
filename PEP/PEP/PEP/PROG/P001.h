/*
* ---------------------------------------------------------------------------
* Title         :   Machine Definition Code Header File (Prog. 1)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P001.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here since.
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
* ---------------------------------------------------------------------------
* Abstract :
*
* --------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Dec-22-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Jan-09-94 : 00.00.02 : M.Yamamoto : Add New Requirement.
* Feb-16-95 : 03.00.00 : H.ISHIDA   :
* Sep-14-98 : 03.03.00 : A.Mitsui   : V3.3
* Jun-16-99 : 03.04.01 : hrkato     : Euro Enhancements
* Nov-24-99 :          : K.Yanagida : NCR2172
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
*       Dialog ID definition
* ===========================================================================
*
*  The dialog definitions are used to determine the kind of dialog box that
*  will be presented to the user when a particular MDC is selected in the
*  P01 MDC Descriptions dialog of PEP.
*
*  For a new MDC address you will need to create a new MDC dialog box.  To do
*  so you will need to decide what data will be accessed through the MDC and
*  the type of the data.  There are several different kinds of data using
*  one or more Radio Buttons each of which represents one bit in the data value::
*    - On/Off or Yes/No setting using a Radio Button (RB)
*        . these are grouped in twos such as in Type A Sub-DialogBox Style
*    - range of values from 0 to N where N depends on the number of Radio Buttons
*         . these are grouped in fours, eights, or sixteens
*         . 4 Radio Buttons range is 0 to 15
*         . 8 Radio Buttons range is 0 to 255
*         . you may have to use shifts and bitwise operations to transform the
*           bits in the byte to the correct value when accessing the MDC
*           as a value in the terminal application source code.
*
*  If you are creating a new MDC you will need to select the dialog box
*  style from the various styles A through H below.
*
*  Next you will to select the Category that the new MDC will be grouped with.
*  Search for IDS_P01_CATEGO_01 to find the first one.  The new MDC will then
*  be assigned a Category Offset within the Category so just look for the last MDC in
*  the selected category and just use the next offset available for that category.
*
*  With this information about the dialog box and category you can then create the
*  dialog box style information as a string value.  This string indicates:
*    - the dialog box style (A through H)
*    - the description
*    - the Category (a number 1 through the number of categories)
*    - the Category Offset (a number)
*
*       Sub-DialogBox Style
* +---Type A---+    +---Type B---+      +---Type C---+      +---Type D---+
* |   Desc-1   |    |   Desc-1   |      |   Desc-1   |      |   Desc-1   |
* | +--------+ |    | +--------+ |      | +--------+ |      | +--------+ |
* | | O RB-1 | |    | | O RB-1 | |      | | O RB-1 | |      | | O RB-1 | |
* | | O RB-2 | |    | | O RB-2 | |      | | O RB-2 | |      | | O RB-2 | |
* | +--------+ |    | | O RB-3 | |      | +--------+ |      | | O RB-3 | |
* |   Desc-2   |    | | O RB-4 | |      |   Desc-2   |      | | O RB-4 | |
* | +--------+ |    | +--------+ |      | +--------+ |      | +--------+ |
* | | O RB-3 | |    |   Desc-2   |      | | O RB-3 | |      |   Desc-2   |
* | | O RB-4 | |    | +--------+ |      | | O RB-4 | |      | +--------+ |
* | +--------+ |    | | O RB-5 | |      | +--------+ |      | | O RB-5 | |
* |   Desc-3   |    | | O RB-6 | |      |   Desc-3   |      | | O RB-6 | |
* | +--------+ |    | | O RB-7 | |      | +--------+ |      | +--------+ |
* | | O RB-5 | |    | | O RB-8 | |      | | O RB-5 | |      |   Desc-3   |
* | | O RB-6 | |    | +--------+ |      | | O RB-6 | |      | +--------+ |
* | +--------+ |    +------------+      | | O RB-7 | |      | | O RB-7 | |
* |   Desc-4   |                        | | O RB-8 | |      | | O RB-8 | |
* | +--------+ |                        | +--------+ |      | +--------+ |
* | | O RB-7 | |                        +------------+      +------------+
* | | O RB-8 | |
* | +--------+ |
* +------------+
*
* +---Type E---+    +---Type F---+      +---Type G---+     (+---Type H---+
* |   Desc-1   |    |   Desc-1   |      |   Desc-1   |      |   Desc-1   |
* | +--------+ |    | +--------+ |      | +--------+ |      | +--------+ |
* | | O RB-1 | |    | | O RB-1 | |      | | O RB-1 | |      | | O RB-1 | |
* | | O RB-2 | |    | | O RB-2 | |      | | O RB-2 | |      | | O RB-2 | |
* | +--------+ |    | | O RB-3 | |      | +--------+ |      | | O RB-3 | |
* |   Desc-2   |    | | O RB-4 | |      |   Desc-2   |      | | O RB-4 | |
* | +--------+ |    | | O RB-5 | |      | +--------+ |      | | O RB-5 | |
* | | O RB-3 | |    | | O RB-6 | |      | | O RB-3 | |      | | O RB-6 | |
* | | O RB-4 | |    | | O RB-7 | |      | | O RB-4 | |      | | O RB-7 | |
* | | O RB-5 | |    | | O RB-8 | |      | | O RB-5 | |      | | O RB-8 | |
* | | O RB-6 | |    | +--------+ |      | | O RB-6 | |      | | O RB-9 | |
* | +--------+ |    |   Desc-2   |      | | O RB-7 | |      | | O RB-10| |
* |   Desc-3   |    | +--------+ |      | | O RB-8 | |      | | O RB-11| |
* | +--------+ |    | | O RB-9 | |      | | O RB-9 | |      | | O RB-12| |
* | | O RB-7 | |    | | O RB-10| |      | | O RB-10| |      | | O RB-13| |
* | | O RB-8 | |    | +--------+ |      | +--------+ |      | | O RB-14| |
* | +--------+ |    +------------+      +------------+      | | O RB-15| |
* +------------+                                            | | O RB-16| |
*                                                           | +--------+ |
*                                                           +------------+
 * ===========================================================================
*/
/***
*  ===========================================================================
*	Strings are used to determine the dialog arrangment
*	an example is "A6.1-9.1-10.1-11"
*	the A6 is for the sub-dialogbox type A and style 6
*	1-9 means the 1st string category string number 9
*	the dialog types are above
*	the styles determine the title arrangement ie which title is used
*	each string to be loaded has to be listed
*	each component is seperated by a period.
*
*  The source for the styles are in P001.c and in the following functions:
*    P01SetGrpStr() sets the description text for a group of radio buttons.
*    P01SetRadioDesc() sets the description text for a series of radio buttons in a group.
*
*  The dialog box type (letters A through H) indicates the basic layout of the
*  dialog and how the radio buttons for the various options will be presented.
*
*  The style (digits 1 through 8) indicates the number of descriptions and the
*  order of the descriptions.
*
*  WARNING:  If you should decide to reorder the MDC pages for a particular section,
*            you must make the changes in two places: (1) the category list for the
*            particular category of MDC pages and (2) the style description digits
*            which must correspond to the numeric order of the defines for the styles.
*            In other words, "A1.6-65" must correspond to the page header for the
*            65th item in category 6 which would be (IDS_P01_CATEGO_06 + 65).
*  ===========================================================================
***/
#define IDD_P01_DLG_A       2301                    /* Sub-DialogBox Type A */
#define IDD_P01_DLG_B       2302                    /* Sub-DialogBox Type B */
#define IDD_P01_DLG_C       2303                    /* Sub-DialogBox Type C */
#define IDD_P01_DLG_D       2304                    /* Sub-DialogBox Type D */
#define IDD_P01_DLG_E       2305                    /* Sub-DialogBox Type E */
#define IDD_P01_DLG_F       2306                    /* Sub-DialogBox Type F */
#define IDD_P01_DLG_G       2307                    /* Sub-DialogBox Type G */
#define IDD_P01_DLG_H       2308                    /* Sub-DialogBox Type H */
#define IDD_P01_DLG_16      2309                    /* Sub-DialogBox Addr.16*/
#define IDD_P01_DLG_25      2310                    /* Sub-DialgoBox Addr.25*/
#define IDD_P01_DLG_EDIT    2311                    /* Sub-DialogBox 175/176*/
#define IDD_P01_DLG_259     2312                    /* Sub-DialogBox Addr.259, V3.3 */

/*** Saratoga ***/
#define IDD_P01_DLG_367     2313                    /* Sub-DialogBox Addr.367 */

#define IDD_P01_LIST        (IDD_P01_DLG_A + 14)    /* Category ListBox     */

#define IDD_P01_DESC_1      (IDD_P01_DLG_A + 15)    /* Address Description  */
#define IDD_P01_DESC_2      (IDD_P01_DLG_A + 16)    /* Address Description  */
#define IDD_P01_DESC_3      (IDD_P01_DLG_A + 17)    /* Address Description  */
#define IDD_P01_DESC_4      (IDD_P01_DLG_A + 18)    /* Address Description  */

#define IDD_P01_BUTTON_01   (IDD_P01_DLG_A + 19)    /* RadioButton No. 1    */
#define IDD_P01_BUTTON_02   (IDD_P01_DLG_A + 20)    /* RadioButton No. 2    */
#define IDD_P01_BUTTON_03   (IDD_P01_DLG_A + 21)    /* RadioButton No. 3    */
#define IDD_P01_BUTTON_04   (IDD_P01_DLG_A + 22)    /* RadioButton No. 4    */
#define IDD_P01_BUTTON_05   (IDD_P01_DLG_A + 23)    /* RadioButton No. 5    */
#define IDD_P01_BUTTON_06   (IDD_P01_DLG_A + 24)    /* RadioButton No. 6    */
#define IDD_P01_BUTTON_07   (IDD_P01_DLG_A + 25)    /* RadioButton No. 7    */
#define IDD_P01_BUTTON_08   (IDD_P01_DLG_A + 26)    /* RadioButton No. 8    */
#define IDD_P01_BUTTON_09   (IDD_P01_DLG_A + 27)    /* RadioButton No. 9    */
#define IDD_P01_BUTTON_10   (IDD_P01_DLG_A + 28)    /* RadioButton No. 10   */
#define IDD_P01_BUTTON_11   (IDD_P01_DLG_A + 29)    /* RadioButton No. 11   */
#define IDD_P01_BUTTON_12   (IDD_P01_DLG_A + 30)    /* RadioButton No. 12   */
#define IDD_P01_BUTTON_13   (IDD_P01_DLG_A + 31)    /* RadioButton No. 13   */
#define IDD_P01_BUTTON_14   (IDD_P01_DLG_A + 32)    /* RadioButton No. 14   */
#define IDD_P01_BUTTON_15   (IDD_P01_DLG_A + 33)    /* RadioButton No. 15   */
#define IDD_P01_BUTTON_16   (IDD_P01_DLG_A + 34)    /* RadioButton No. 16   */

#define IDD_P01_EDIT        (IDD_P01_BUTTON_16 + 1)
#define IDD_P01_SPIN        (IDD_P01_BUTTON_16 + 2)
#define IDD_P01_MDC			2337	//RPH 4/21/03 Mulitlingual PEP


/*
* ===========================================================================
*       Resource ID definition
* ===========================================================================
*/
/* ----- MDC Category Strings Definision ----- */

/*** NCR2172
#define P01_CATE_OFFSET     60
 ***/
#define P01_CATE_OFFSET     200
#define IDS_P01_CATEGO_01   IDS_P01                                      // P01_CATE_01_NO indicates number of entries
#define IDS_P01_CATEGO_02   (IDS_P01_CATEGO_01 + P01_CATE_OFFSET)        // P01_CATE_02_NO indicates number of entries
#define IDS_P01_CATEGO_03   (IDS_P01_CATEGO_02 + P01_CATE_OFFSET)        // P01_CATE_03_NO indicates number of entries
#define IDS_P01_CATEGO_04   (IDS_P01_CATEGO_03 + P01_CATE_OFFSET)        // P01_CATE_04_NO indicates number of entries
#define IDS_P01_CATEGO_05   (IDS_P01_CATEGO_04 + P01_CATE_OFFSET)        // P01_CATE_05_NO indicates number of entries
#define IDS_P01_CATEGO_06   (IDS_P01_CATEGO_05 + P01_CATE_OFFSET)        // P01_CATE_06_NO indicates number of entries
#define IDS_P01_CATEGO_07   (IDS_P01_CATEGO_06 + P01_CATE_OFFSET)        // P01_CATE_07_NO indicates number of entries
#define IDS_P01_CATEGO_08   (IDS_P01_CATEGO_07 + P01_CATE_OFFSET)        // P01_CATE_08_NO indicates number of entries
#define IDS_P01_CATEGO_09   (IDS_P01_CATEGO_08 + P01_CATE_OFFSET)        // P01_CATE_09_NO indicates number of entries
/* V3.3 Category 10 is removed and add Category 12. ID sequence is changed */
#define IDS_P01_CATEGO_11   (IDS_P01_CATEGO_09 + P01_CATE_OFFSET)        // P01_CATE_11_NO indicates number of entries
#define IDS_P01_CATEGO_12   (IDS_P01_CATEGO_11 + P01_CATE_OFFSET)        // P01_CATE_12_NO indicates number of entries
#define IDS_P01_CATEGO_13   (IDS_P01_CATEGO_12 + P01_CATE_OFFSET)        // P01_CATE_13_NO indicates number of entries

// At one time predecessors to NHPOS had table service functionality for Waiters or Servers.
// With NHPOS 1.4 on old NCR 7448 donated to Georgia Southern University and maybe earlier Waiter/Server functionality
// was removed. Probably what happened was when the various types of NHPOS were combined into a single product for
// the old NCR 7448 by NCR-Oiso, Japan they did not bother to include Waiter/Server.
//    Richard Chambers, Dec-08-2017
#define IDS_P01_CATEGO_10   (IDS_P01_CATEGO_13 + P01_CATE_OFFSET)        // category commented out as servers/waiters not supported.

/* ----- MDC Address Description Strings Definision ----- */
// See also define for IDS_P01_SUBCAP below which may need changes if number of categories changes.
#define P01_CATE_MAX    12              /* No. of Categories                , V3.3 */


    /* ------------------------------------------------ */
    /* ===== CATEGORY NO. 1 (System Configulation) =====*/
//  Offset used for address space for defines in this category begin at IDS_P01_CATEGO_01
//  Number of items set with define P01_CATE_01_NO and if items added it needs to be updated.
#define IDS_P01_ADDR_365    (IDS_P01_CATEGO_01 + 1) /* Addr.365 - No. of Terminal   */
#define IDS_P01_ADDR_367    (IDS_P01_CATEGO_01 + 2) /* Addr.367 - Decimal Point     */
#define IDS_P01_ADDR_368    (IDS_P01_CATEGO_01 + 3) /* Addr.368 - Back-up System    */
#define IDS_P01_ADDR_013    (IDS_P01_CATEGO_01 + 4) /* Addr.013 - Remote Printer    */
#define IDS_P01_ADDR_014_1  (IDS_P01_CATEGO_01 + 5) /* Addr.014 - Drawer            */
#define IDS_P01_ADDR_014_2  (IDS_P01_CATEGO_01 + 6) /* Addr.014 - Date & Time       */
#define IDS_P01_ADDR_020_1  (IDS_P01_CATEGO_01 + 7) /* Addr.020 - Slip Print        */
#define IDS_P01_ADDR_020_2  (IDS_P01_CATEGO_01 + 8) /* Addr.020 - Sales Promotion.. */
#define IDS_P01_ADDR_021_1  (IDS_P01_CATEGO_01 + 9) /* Addr.021 - c/d Supervisor Inter. */
#define IDS_P01_ADDR_021_2  (IDS_P01_CATEGO_01 +10) /* Addr.021 - b #/Type Entry. US CUSTOMS SCER 4/30/03*/
#define IDS_P01_ADDR_021_3  (IDS_P01_CATEGO_01 +11) /* Addr.021 - a Cancel Receipt. US CUSTOMS SCER 4/30/03 */
#define IDS_P01_ADDR_081    (IDS_P01_CATEGO_01 +12) /* Addr.081 - G.C. Transfer     */
#define IDS_P01_ADDR_082    (IDS_P01_CATEGO_01 +13) /* Addr.082 - Drawer, MDC_DRAWER2_ADR */
#define IDS_P01_ADDR_175    (IDS_P01_CATEGO_01 +14) /* Addr.175 - Display on Fly    */
#define IDS_P01_ADDR_176    (IDS_P01_CATEGO_01 +15) /* Addr.176 - Display on Fly    */
#define IDS_P01_ADDR_363    (IDS_P01_CATEGO_01 +16) /* Addr.363 - Display on the Fly(KDS) */
#define IDS_P01_ADDR_363_1	(IDS_P01_CATEGO_01 +17) /* Addr.363 - Display SR JHHJ */
#define IDS_P01_ADDR_177    (IDS_P01_CATEGO_01 +18) /* Addr.177 - Remote CRT        */
#define IDS_P01_ADDR_178    (IDS_P01_CATEGO_01 +19) /* Addr.178 - Remote CRT        */
#define IDS_P01_ADDR_179    (IDS_P01_CATEGO_01 +20) /* Addr.179 - Shared Prt. Timer */
#define IDS_P01_ADDR_275    (IDS_P01_CATEGO_01 +21) /* Addr.275 - Electronic Journal, MDC_EJ_ADR */
#define IDS_P01_ADDR_276	(IDS_P01_CATEGO_01 +22) /* Addr.276 - Store and Forward, MDC_STR_FWD_ADR */
#define IDS_P01_ADDR_477	(IDS_P01_CATEGO_01 +23) /* Addr.477 - Auto Sign Out, MDC_AUTO_SIGNOUT_ADR */
#define IDS_P01_ADDR_481	(IDS_P01_CATEGO_01 +24) /* Addr.481 - Open Drawer Audible Limit, MDC_DRAWEROPEN_TIME */
#define IDS_P01_ADDR_535	(IDS_P01_CATEGO_01 +25) /* Addr.535 - Storing/Forwarding processing rules, MDC_SF_EXEC_RULES_01 */

#define P01_CATE_01_NO  25              /* No. of Address in Category 1, IDS_P01_CATEGO_01     , Saratoga */

//#define IDS_P01_ADDR_476	(IDS_P01_CATEGO_01 +23) /* Addr.476 - OPEN*/
//used for 277 and 478 auto signout
//#define IDS_P01_ADDR_478	(IDS_P01_CATEGO_01 +23) /* Addr.477 - Auto Sign Out*/

    /* ------------------------------------------ */
    /* ===== CATEGORY NO. 2 (Sign In / Out) ===== */
//  Offset used for address space for defines in this category begin at IDS_P01_CATEGO_02
//  Number of items set with define P01_CATE_02_NO and if items added it needs to be updated.
#define IDS_P01_ADDR_017    (IDS_P01_CATEGO_02 + 1) /* Addr.017 - Operator Sign-In, MDC_CASIN_ADR   */
#define IDS_P01_ADDR_018    (IDS_P01_CATEGO_02 + 2) /* Addr.018 - Server Sign-In, MDC_WTIN_ADR  */
#define IDS_P01_ADDR_019_1  (IDS_P01_CATEGO_02 + 3) /* Addr.019 - Sign-Out, MDC_SIGNOUT_ADR  */
#define IDS_P01_ADDR_085_1  (IDS_P01_CATEGO_02 + 4) /* Addr.085 - Supervisor Sign-In, MDC_SUPER_ADR . MDC_PARAM_BIT_D */
#define IDS_P01_ADDR_085_3	(IDS_P01_CATEGO_02 + 5) /* Addr.085 - Require MSR on Sign-in, MDC_SUPER_ADR . MDC_PARAM_BIT_A */
#define IDS_P01_ADDR_479_AB (IDS_P01_CATEGO_02 + 6) /* Addr.479 - Employee ID Masking 0 - 3 times 2,  MDC_EMPLOYEE_ID_ADR . MDC_PARAM_2_BIT_A */

#define P01_CATE_02_NO  6               /* No. of Address in Category 2, IDS_P01_CATEGO_02     */

    /* -------------------------------------------------- */
    /* ===== CATEGORY NO. 3 (Internal Environmnent) ===== */
	/* Number of items in this category specified by defined constant P01_CATE_03_NO */
#define IDS_P01_ADDR_012    (IDS_P01_CATEGO_03 + 1) /* Addr.012 - Guest Check No.   */
#define IDS_P01_ADDR_011    (IDS_P01_CATEGO_03 + 2) /* Addr.011 - International VAT, V3.3  */
#define IDS_P01_ADDR_011_2	(IDS_P01_CATEGO_03 + 3) /* Addr.011 - Tax Calculation */
#define IDS_P01_ADDR_015_1  (IDS_P01_CATEGO_03 + 4) /*  Addr.015 - Tax, MDC_TAX_ADR  */
#define IDS_P01_ADDR_015_2  (IDS_P01_CATEGO_03 + 5) /*  Addr.015 - Canadian Tax      */
#define IDS_P01_ADDR_016    (IDS_P01_CATEGO_03 + 6) /*  Addr.016 - Canadian Tax      */
#define IDS_P01_ADDR_259    (IDS_P01_CATEGO_03 + 7) /* Addr.259 - VAT, V3.3         */
#define IDS_P01_ADDR_260    (IDS_P01_CATEGO_03 + 8) /* Addr.260 - VAT, V3.3         */
#define IDS_P01_ADDR_019_2  (IDS_P01_CATEGO_03 + 9) /* Addr.019 - Negative or Zero  */
#define IDS_P01_ADDR_022    (IDS_P01_CATEGO_03 +10) /* Addr.022 - Cancel / Abort    */
#define IDS_P01_ADDR_023    (IDS_P01_CATEGO_03 +11) /* Addr.023 - Charge Tips       */
#define IDS_P01_ADDR_024    (IDS_P01_CATEGO_03 +12) /* Addr.024 - Charge Tips       */
#define IDS_P01_ADDR_025    (IDS_P01_CATEGO_03 +13) /* Addr.025 - Charge Tips       */
#define IDS_P01_ADDR_027    (IDS_P01_CATEGO_03 +14) /* Addr.027 - Department / PLU, MDC_DEPT1_ADR  */
#define IDS_P01_ADDR_028    (IDS_P01_CATEGO_03 +15) /* Addr.028 - Department / PLU, MDC_DEPT2_ADR  */
#define IDS_P01_ADDR_235_1    (IDS_P01_CATEGO_03 +16) /* Addr.235 - Department / PLU  */
/*** NCR2172 ***/
#define IDS_P01_ADDR_364    (IDS_P01_CATEGO_03 +17) /* Addr.364 - Adj PLU Enhancement   */
#define IDS_P01_ADDR_029    (IDS_P01_CATEGO_03 +18) /* Addr.029 - Scale, MDC_SCALE_ADR  */
#define IDS_P01_ADDR_030_1  (IDS_P01_CATEGO_03 +19) /* Addr.030 - 21 Printer        */
#define IDS_P01_ADDR_030_2  (IDS_P01_CATEGO_03 +20) /* Addr.030 - Slip Validation   */
#define IDS_P01_ADDR_270    (IDS_P01_CATEGO_03 +21) /* Addr.258 - Kitchen Printer, V3.3 */
#define IDS_P01_ADDR_047    (IDS_P01_CATEGO_03 +22) /* Addr.047 - Error Correct     */
#define IDS_P01_ADDR_052    (IDS_P01_CATEGO_03 +23) /* Addr.052 - Tax               */
#define IDS_P01_ADDR_053_1  (IDS_P01_CATEGO_03 +24) /* Addr.053 - No Sale           */
#define IDS_P01_ADDR_231    (IDS_P01_CATEGO_03 +25) /* Addr.231 - Display           */
#define IDS_P01_ADDR_232    (IDS_P01_CATEGO_03 +26) /* Addr.232 - Print Priority    */
#define IDS_P01_ADDR_232_2  (IDS_P01_CATEGO_03 +27) /* Addr.232 - Print Priority    */
#define IDS_P01_ADDR_233    (IDS_P01_CATEGO_03 +28) /* Addr.233 - Combination Coupon*/
#define IDS_P01_ADDR_233_2	(IDS_P01_CATEGO_03 +29) /* Addr.233 - Double Sided Printing*/
#define IDS_P01_ADDR_234    (IDS_P01_CATEGO_03 +30) /* Addr.234 - Other             */
#define IDS_P01_ADDR_234_2	(IDS_P01_CATEGO_03 +31) /* Addr.234 - S/R # Enhancement	*/
#define IDS_P01_ADDR_236    (IDS_P01_CATEGO_03 +32) /* Addr.236 - Split Guest Check */
#define IDS_P01_ADDR_257    (IDS_P01_CATEGO_03 +33) /* Addr.257 - New Key Swquence, V3.3 */
#define IDS_P01_ADDR_257_2  (IDS_P01_CATEGO_03 +34) /* Addr.257 - Item Count Option SR 217*/
#define IDS_P01_ADDR_258    (IDS_P01_CATEGO_03 +35) /* Addr.258 - Kitchen Printer, V3.3 */
#define IDS_P01_ADDR_258_2	(IDS_P01_CATEGO_03 +36) /* Addr. 258 - Kitchen Printer Qty Extension SR144*/
#define	IDS_P01_ADDR_445	(IDS_P01_CATEGO_03 +37) /* Addr. 445 - Auto Combinational Coupons */
#define IDS_P01_ADDR_235_2  (IDS_P01_CATEGO_03 +38) /* Addr.235 - Department / PLU  */
#define IDS_P01_ADDR_522    (IDS_P01_CATEGO_03 +39) /* Returns Receipt options, MDC_RECEIPT_RTN_ADR  */
#define IDS_P01_ADDR_523    (IDS_P01_CATEGO_03 +40) /* Returns Receipt options, MDC_SF_ALERT_TIME  */
#define IDS_P01_ADDR_525    (IDS_P01_CATEGO_03 +41) /* Returns Receipt options, MDC_SF_AUTOOFF_TIME  */
#define IDS_P01_ADDR_527    (IDS_P01_CATEGO_03 +42) /* Returns Receipt options, MDC_SF_TRANRULE_TIME  */
#define IDS_P01_ADDR_529    (IDS_P01_CATEGO_03 +43) /* Signature Capture Use Rules, MDC_SIGCAP_USE_RULES */
#define IDS_P01_ADDR_530    (IDS_P01_CATEGO_03 +44) /* Returns Usage Rules, MDC_RTN_RECALL_RULES */
#define IDS_P01_ADDR_531    (IDS_P01_CATEGO_03 +45) /* Control string id when Alert Timer expires, MDC_SF_CNTRL_ALERT */
#define IDS_P01_ADDR_533    (IDS_P01_CATEGO_03 +46) /* Control string id when Auto Off Timer expires, MDC_SF_CNTRL_AUTOOFF */

#define P01_CATE_03_NO  46              /* No. of Address in Category 3, IDS_P01_CATEGO_03     , V3.3 SR144*/

    /* ------------------------------------------ */
    /* ===== CATEGORY NO. 4 (Key(Discount)) ===== */
	/* Number of items in this category specified by defined constant P01_CATE_04_NO */
#define IDS_P01_ADDR_031    (IDS_P01_CATEGO_04 + 1) /* Addr.031 - Item Discount Key */
#define IDS_P01_ADDR_032    (IDS_P01_CATEGO_04 + 2) /* Addr.032 - Item Discount Key */
#define IDS_P01_ADDR_033    (IDS_P01_CATEGO_04 + 3) /* Addr.033 - Item Discount Key */
#define IDS_P01_ADDR_035    (IDS_P01_CATEGO_04 + 4) /* Addr.035 - Item Discount Key */
#define IDS_P01_ADDR_036    (IDS_P01_CATEGO_04 + 5) /* Addr.036 - Item Discount Key */
#define IDS_P01_ADDR_037    (IDS_P01_CATEGO_04 + 6) /* Addr.037 - Item Discount Key */
#define IDS_P01_ADDR_039    (IDS_P01_CATEGO_04 + 7) /* Addr.039 - Regular Discount  */
#define IDS_P01_ADDR_040    (IDS_P01_CATEGO_04 + 8) /* Addr.040 - Regular Discount  */
#define IDS_P01_ADDR_041    (IDS_P01_CATEGO_04 + 9) /* Addr.041 - Regular Discount  */
#define IDS_P01_ADDR_042    (IDS_P01_CATEGO_04 +10) /* Addr.042 - Regular Discount  */
#define IDS_P01_ADDR_043    (IDS_P01_CATEGO_04 +11) /* Addr.043 - Regular Discount  */
#define IDS_P01_ADDR_044    (IDS_P01_CATEGO_04 +12) /* Addr.044 - Regular Discount  */
#define IDS_P01_ADDR_045    (IDS_P01_CATEGO_04 +13) /* Addr.045 - Regular Discount  */
#define IDS_P01_ADDR_046    (IDS_P01_CATEGO_04 +14) /* Addr.046 - Regular Discount  */
#define IDS_P01_ADDR_237    (IDS_P01_CATEGO_04 +15) /* Addr.237 - Regular Discount  */
#define IDS_P01_ADDR_238    (IDS_P01_CATEGO_04 +16) /* Addr.238 - Regular Discount  */
#define IDS_P01_ADDR_239    (IDS_P01_CATEGO_04 +17) /* Addr.239 - Regular Discount  */
#define IDS_P01_ADDR_240    (IDS_P01_CATEGO_04 +18) /* Addr.240 - Regular Discount  */
#define IDS_P01_ADDR_241    (IDS_P01_CATEGO_04 +19) /* Addr.241 - Regular Discount  */
#define IDS_P01_ADDR_242    (IDS_P01_CATEGO_04 +20) /* Addr.242 - Regular Discount  */
#define IDS_P01_ADDR_243    (IDS_P01_CATEGO_04 +21) /* Addr.243 - Regular Discount  */
#define IDS_P01_ADDR_244    (IDS_P01_CATEGO_04 +22) /* Addr.244 - Regular Discount  */
#define IDS_P01_ADDR_245    (IDS_P01_CATEGO_04 +23) /* Addr.245 - Regular Discount  */
#define IDS_P01_ADDR_246    (IDS_P01_CATEGO_04 +24) /* Addr.246 - Regular Discount  */
#define IDS_P01_ADDR_247    (IDS_P01_CATEGO_04 +25) /* Addr.247 - Regular Discount  */
#define IDS_P01_ADDR_248    (IDS_P01_CATEGO_04 +26) /* Addr.248 - Regular Discount  */
#define IDS_P01_ADDR_249    (IDS_P01_CATEGO_04 +27) /* Addr.249 - Regular Discount  */
#define IDS_P01_ADDR_250    (IDS_P01_CATEGO_04 +28) /* Addr.250 - Regular Discount  */
#define IDS_P01_ADDR_251    (IDS_P01_CATEGO_04 +29) /* Addr.251 - Regular Discount  */
#define IDS_P01_ADDR_252    (IDS_P01_CATEGO_04 +30) /* Addr.252 - Regular Discount  */
/* ### ADD Saratoga (052500) */
#define IDS_P01_ADDR_371    (IDS_P01_CATEGO_04 +31) /* Addr.371 - UPC Coupon        */
#define IDS_P01_ADDR_373    (IDS_P01_CATEGO_04 +32) /* Addr.373 - UPC Coupon        */
#define IDS_P01_ADDR_374    (IDS_P01_CATEGO_04 +33) /* Addr.374 - UPC Coupon        */
#define IDS_P01_ADDR_375    (IDS_P01_CATEGO_04 +34) /* Addr.375 - UPC Coupon        */
#define IDS_P01_ADDR_376    (IDS_P01_CATEGO_04 +35) /* Addr.376 - UPC Coupon        */
#define IDS_P01_ADDR_380	(IDS_P01_CATEGO_04 +36) // Addr.380 - Double coupon 1
#define IDS_P01_ADDR_381	(IDS_P01_CATEGO_04 +37) // Addr.381 - Double coupon 2
#define IDS_P01_ADDR_382	(IDS_P01_CATEGO_04 +38)	// Addr.382 - Triple Coupon 1
#define IDS_P01_ADDR_383	(IDS_P01_CATEGO_04 +39)	// Addr.383 - Triple Coupon 2

#define IDS_P01_ADDR_451    (IDS_P01_CATEGO_04 +40)/* Modified Item Discount Key (ID #3) */
#define IDS_P01_ADDR_452    (IDS_P01_CATEGO_04 +41)/* Modified Item Discount Key (ID #3) */
#define IDS_P01_ADDR_453    (IDS_P01_CATEGO_04 +42) /* Modified Item Discount Key (ID #3) */
#define IDS_P01_ADDR_454    (IDS_P01_CATEGO_04 +43)/* Modified Item Discount Key (ID #4) */
#define IDS_P01_ADDR_455    (IDS_P01_CATEGO_04 +44) /* Modified Item Discount Key (ID #4) */
#define IDS_P01_ADDR_456    (IDS_P01_CATEGO_04 +45) /* Modified Item Discount Key (ID #4) */
#define IDS_P01_ADDR_457    (IDS_P01_CATEGO_04 +46) /* Modified Item Discount Key (ID #5) */
#define IDS_P01_ADDR_458    (IDS_P01_CATEGO_04 +47) /* Modified Item Discount Key (ID #5) */
#define IDS_P01_ADDR_459    (IDS_P01_CATEGO_04 +48) /* Modified Item Discount Key (ID #5) */
#define IDS_P01_ADDR_460    (IDS_P01_CATEGO_04 +49) /* Modified Item Discount Key (ID #6) */
#define IDS_P01_ADDR_461    (IDS_P01_CATEGO_04 +50) /* Modified Item Discount Key (ID #6) */
#define IDS_P01_ADDR_462    (IDS_P01_CATEGO_04 +51) /* Modified Item Discount Key (ID #6) */
#define IDS_P01_ADDR_503    (IDS_P01_CATEGO_04 +52) /* Modified Item Discount Key (ID #1), page 5,MDC_MODID15_ADR */
#define IDS_P01_ADDR_504    (IDS_P01_CATEGO_04 +53) /* Modified Item Discount Key (ID #2), page 5,MDC_MODID25_ADR */
#define IDS_P01_ADDR_505    (IDS_P01_CATEGO_04 +54) /* Modified Item Discount Key (ID #3), page 5,MDC_MODID35_ADR */
#define IDS_P01_ADDR_506    (IDS_P01_CATEGO_04 +55) /* Modified Item Discount Key (ID #4), page 5,MDC_MODID45_ADR */
#define IDS_P01_ADDR_507    (IDS_P01_CATEGO_04 +56) /* Modified Item Discount Key (ID #5), page 5,MDC_MODID55_ADR */
#define IDS_P01_ADDR_508    (IDS_P01_CATEGO_04 +57) /* Modified Item Discount Key (ID #6), page 5,MDC_MODID65_ADR */
#define IDS_P01_ADDR_509    (IDS_P01_CATEGO_04 +58) /* Regular Discount Key (ID #1), MDC_RDISC15_ADR, page 5 */
#define IDS_P01_ADDR_510    (IDS_P01_CATEGO_04 +59) /* Regular Discount Key (ID #2), MDC_RDISC25_ADR, page 5 */
#define IDS_P01_ADDR_511    (IDS_P01_CATEGO_04 +60) /* Regular Discount Key (ID #3), MDC_RDISC35_ADR, page 5 */
#define IDS_P01_ADDR_512    (IDS_P01_CATEGO_04 +61) /* Regular Discount Key (ID #4), MDC_RDISC45_ADR, page 5 */
#define IDS_P01_ADDR_513    (IDS_P01_CATEGO_04 +62) /* Regular Discount Key (ID #5), MDC_RDISC55_ADR, page 5 */
#define IDS_P01_ADDR_514    (IDS_P01_CATEGO_04 +63) /* Regular Discount Key (ID #6), MDC_RDISC65_ADR, page 5 */

#define P01_CATE_04_NO  63              /* No. of Address in Category 4, IDS_P01_CATEGO_04    */

    /* --------------------------------------------- */
    /* ===== CATEGORY NO. 5 (Key (Individual)) ===== */
	/* Number of items in this category specified by defined constant P01_CATE_05_NO */
#define IDS_P01_ADDR_048    (IDS_P01_CATEGO_05 + 1) /* Addr.048 - Manual PB Ope...  */
#define IDS_P01_ADDR_049    (IDS_P01_CATEGO_05 + 2) /* Addr.049 - Manual Add Ope... */
#define IDS_P01_ADDR_050    (IDS_P01_CATEGO_05 + 3) /* Addr.050 - Manual Add Ope... */
#define IDS_P01_ADDR_051    (IDS_P01_CATEGO_05 + 4) /* Addr.051 - Manual Add Ope... */
#define IDS_P01_ADDR_053_2  (IDS_P01_CATEGO_05 + 5) /* Addr.053 - Menu Shift Key    */
#define IDS_P01_ADDR_054    (IDS_P01_CATEGO_05 + 6) /* Addr.054 - No. of Person     */
#define IDS_P01_ADDR_055_1  (IDS_P01_CATEGO_05 + 7) /* Addr.055 - Paid Out          */
#define IDS_P01_ADDR_055_2  (IDS_P01_CATEGO_05 + 8) /* Addr.055 - Rcved on Account  */
#define IDS_P01_ADDR_056    (IDS_P01_CATEGO_05 + 9) /* Addr.056 - Void Transaction  */
#define IDS_P01_ADDR_057_1  (IDS_P01_CATEGO_05 +10) /* Addr.057 - (No) Recept Key   */
#define IDS_P01_ADDR_057_2  (IDS_P01_CATEGO_05 +11) /* Addr.057 - Table No. Key     */
#define IDS_P01_ADDR_058    (IDS_P01_CATEGO_05 +12) /* Addr.058 - Tax Exempt Keys */
#define IDS_P01_ADDR_059    (IDS_P01_CATEGO_05 +13) /* Addr.059 - Tips Paid Out Key */
#define IDS_P01_ADDR_060_1  (IDS_P01_CATEGO_05 +14) /* Addr.060 - Tips/Declared Key */
#define IDS_P01_ADDR_060_2  (IDS_P01_CATEGO_05 +15) /* Addr.060 - Total Key Common  */
#define IDS_P01_ADDR_061    (IDS_P01_CATEGO_05 +16) /* Addr.061 - Void Key          */
#define IDS_P01_ADDR_062    (IDS_P01_CATEGO_05 +17) /* Addr.062 - Check Sum MDC_CHKSUM_ADR or MDC_PLU_TICKET */
#define IDS_P01_ADDR_083    (IDS_P01_CATEGO_05 +18) /* Addr.083 - Post Receipt Key  */
#define IDS_P01_ADDR_084    (IDS_P01_CATEGO_05 +19) /* Addr.084 - Preset Cash Key   */
/*** NCR2172 ***/
#define IDS_P01_ADDR_331    (IDS_P01_CATEGO_05 +20) /* Addr.331 - Loan/Pick         */
#define IDS_P01_ADDR_332    (IDS_P01_CATEGO_05 +21) /* Addr.332 - Loan/Pick         */
#define IDS_P01_ADDR_333    (IDS_P01_CATEGO_05 +22) /* Addr.333 - Money             */
/* ### ADD Saratoga (052500) */
#define IDS_P01_ADDR_356    (IDS_P01_CATEGO_05 +23) /* Addr.356 - WIC               */
#define IDS_P01_ADDR_384	(IDS_P01_CATEGO_05 +24) /* SR143 HALO Override			*/
#define IDS_P01_ADDR_515	(IDS_P01_CATEGO_05 +25) /* Numeric Pause Key #1 and #2 settings, MDC_PAUSE_1_2_ADR */
#define IDS_P01_ADDR_516	(IDS_P01_CATEGO_05 +26) /* Numeric Pause Key #3 and #4 settings, MDC_PAUSE_3_4_ADR */
#define IDS_P01_ADDR_517	(IDS_P01_CATEGO_05 +27)	//Returns key (option 1 of void), Amtrak/VCS, page 5, MDC_RETURNS_RTN1_ADR
#define IDS_P01_ADDR_518	(IDS_P01_CATEGO_05 +28)	//Returns key (option 2 of void), Amtrak/VCS, page 5, MDC_RETURNS_RTN1_ADR
#define IDS_P01_ADDR_519	(IDS_P01_CATEGO_05 +29)	//Returns key (option 3 of void), Amtrak/VCS, page 5, MDC_RETURNS_RTN1_ADR
#define IDS_P01_ADDR_536	(IDS_P01_CATEGO_05 +30)	//limit on number of Post Receipt key presses allowed, MDC_RECEIPT_POST_LMT
#define IDS_P01_ADDR_537	(IDS_P01_CATEGO_05 +31)	//limit on number of Post Receipt key presses allowed, MDC_RECEIPT_GIFT_LMT
#define IDS_P01_ADDR_538	(IDS_P01_CATEGO_05 +32)	//limit on number of Post Receipt key presses allowed, MDC_RECEIPT_GIFT_LMT

#define P01_CATE_05_NO  32              /* No. of Address in Category 5, IDS_P01_CATEGO_05     */

    /* ---------------------------------------- */
    /* ===== CATEGORY NO. 6 (Key(Tender)) ===== */
#define IDS_P01_ADDR_063    (IDS_P01_CATEGO_06 + 1)  /* Addr.063 - Tender #1 (Cash)  */
#define IDS_P01_ADDR_064    (IDS_P01_CATEGO_06 + 2)  /* Addr.064 - Tender #1 (Cash)  */
#define IDS_P01_ADDR_180    (IDS_P01_CATEGO_06 + 3)  /* Addr.180 - Tender #1 (Cash)  */
#define IDS_P01_ADDR_201    (IDS_P01_CATEGO_06 + 4)  /* Addr.201 - Tender #1 (Cash)  */
#define IDS_P01_ADDR_202    (IDS_P01_CATEGO_06 + 5)  /* Addr.202 - Tender #1 (Cash)  */
#define IDS_P01_ADDR_065    (IDS_P01_CATEGO_06 + 6)  /* Addr.065 - Tender #2         */
#define IDS_P01_ADDR_066    (IDS_P01_CATEGO_06 + 7)  /* Addr.066 - Tender #2         */
#define IDS_P01_ADDR_181    (IDS_P01_CATEGO_06 + 8)  /* Addr.181 - Tender #2         */
#define IDS_P01_ADDR_203    (IDS_P01_CATEGO_06 + 9)  /* Addr.203 - Tender #2         */
#define IDS_P01_ADDR_204    (IDS_P01_CATEGO_06 + 10) /* Addr.204 - Tender #2        */
#define IDS_P01_ADDR_067    (IDS_P01_CATEGO_06 + 11) /* Addr.067 - Tender #3         */
#define IDS_P01_ADDR_068    (IDS_P01_CATEGO_06 + 12) /* Addr.068 - Tender #3         */
#define IDS_P01_ADDR_182    (IDS_P01_CATEGO_06 + 13) /* Addr.182 - Tender #3         */
#define IDS_P01_ADDR_205    (IDS_P01_CATEGO_06 + 14) /* Addr.205 - Tender #3         */
#define IDS_P01_ADDR_206    (IDS_P01_CATEGO_06 + 15) /* Addr.206 - Tender #3         */
#define IDS_P01_ADDR_069    (IDS_P01_CATEGO_06 + 16) /* Addr.069 - Tender #4         */
#define IDS_P01_ADDR_070    (IDS_P01_CATEGO_06 + 17) /* Addr.070 - Tender #4         */
#define IDS_P01_ADDR_183    (IDS_P01_CATEGO_06 + 18) /* Addr.183 - Tender #4         */
#define IDS_P01_ADDR_207    (IDS_P01_CATEGO_06 + 19) /* Addr.207 - Tender #4         */
#define IDS_P01_ADDR_208    (IDS_P01_CATEGO_06 + 20) /* Addr.208 - Tender #4         */
#define IDS_P01_ADDR_071    (IDS_P01_CATEGO_06 + 21) /* Addr.071 - Tender #5         */
#define IDS_P01_ADDR_072    (IDS_P01_CATEGO_06 + 22) /* Addr.072 - Tender #5         */
#define IDS_P01_ADDR_184    (IDS_P01_CATEGO_06 + 23) /* Addr.184 - Tender #5         */
#define IDS_P01_ADDR_209    (IDS_P01_CATEGO_06 + 24) /* Addr.209 - Tender #5         */
#define IDS_P01_ADDR_210    (IDS_P01_CATEGO_06 + 25) /* Addr.210 - Tender #5         */
#define IDS_P01_ADDR_073    (IDS_P01_CATEGO_06 + 26) /* Addr.074 - Tender #6         */
#define IDS_P01_ADDR_074    (IDS_P01_CATEGO_06 + 27) /* Addr.074 - Tender #6         */
#define IDS_P01_ADDR_185    (IDS_P01_CATEGO_06 + 28) /* Addr.185 - Tender #6         */
#define IDS_P01_ADDR_211    (IDS_P01_CATEGO_06 + 29) /* Addr.211 - Tender #6         */
#define IDS_P01_ADDR_212    (IDS_P01_CATEGO_06 + 30) /* Addr.212 - Tender #6         */
#define IDS_P01_ADDR_075    (IDS_P01_CATEGO_06 + 31) /* Addr.075 - Tender #7         */
#define IDS_P01_ADDR_076    (IDS_P01_CATEGO_06 + 32) /* Addr.076 - Tender #7         */
#define IDS_P01_ADDR_186    (IDS_P01_CATEGO_06 + 33) /* Addr.186 - Tender #7         */
#define IDS_P01_ADDR_213    (IDS_P01_CATEGO_06 + 34) /* Addr.213 - Tender #7         */
#define IDS_P01_ADDR_214    (IDS_P01_CATEGO_06 + 35) /* Addr.214 - Tender #7         */
#define IDS_P01_ADDR_077    (IDS_P01_CATEGO_06 + 36) /* Addr.077 - Tender #8         */
#define IDS_P01_ADDR_078    (IDS_P01_CATEGO_06 + 37) /* Addr.078 - Tender #8         */
#define IDS_P01_ADDR_187    (IDS_P01_CATEGO_06 + 38) /* Addr.187 - Tender #8         */
#define IDS_P01_ADDR_215    (IDS_P01_CATEGO_06 + 39) /* Addr.215 - Tender #8         */
#define IDS_P01_ADDR_216    (IDS_P01_CATEGO_06 + 40) /* Addr.216 - Tender #8         */
#define IDS_P01_ADDR_191    (IDS_P01_CATEGO_06 + 41) /* Addr.191 - Tender #9         */
#define IDS_P01_ADDR_192    (IDS_P01_CATEGO_06 + 42) /* Addr.192 - Tender #9         */
#define IDS_P01_ADDR_188    (IDS_P01_CATEGO_06 + 43) /* Addr.188 - Tender #9         */
#define IDS_P01_ADDR_217    (IDS_P01_CATEGO_06 + 44) /* Addr.217 - Tender #9         */
#define IDS_P01_ADDR_218    (IDS_P01_CATEGO_06 + 45) /* Addr.218 - Tender #9         */
#define IDS_P01_ADDR_193    (IDS_P01_CATEGO_06 + 46) /* Addr.193 - Tender #10        */
#define IDS_P01_ADDR_194    (IDS_P01_CATEGO_06 + 47) /* Addr.194 - Tender #10        */
#define IDS_P01_ADDR_189    (IDS_P01_CATEGO_06 + 48) /* Addr.189 - Tender #10        */
#define IDS_P01_ADDR_219    (IDS_P01_CATEGO_06 + 49) /* Addr.219 - Tender #10        */
#define IDS_P01_ADDR_220    (IDS_P01_CATEGO_06 + 50) /* Addr.220 - Tender #10        */
#define IDS_P01_ADDR_195    (IDS_P01_CATEGO_06 + 51) /* Addr.195 - Tender #11        */
#define IDS_P01_ADDR_196    (IDS_P01_CATEGO_06 + 52) /* Addr.196 - Tender #11        */
#define IDS_P01_ADDR_190    (IDS_P01_CATEGO_06 + 53) /* Addr.190 - Tender #11        */
#define IDS_P01_ADDR_221    (IDS_P01_CATEGO_06 + 54) /* Addr.221 - Tender #11        */
#define IDS_P01_ADDR_222    (IDS_P01_CATEGO_06 + 55) /* Addr.222 - Tender #11        */
#define IDS_P01_ADDR_079    (IDS_P01_CATEGO_06 + 56) /* Addr.079 - Foreign Cur. #1, FSC_FOREIGN, MDC_FC1_ADR   */
#define IDS_P01_ADDR_080    (IDS_P01_CATEGO_06 + 57) /* Addr.080 - Foreign Cur. #2, MDC_FC2_ADR   */
/*** NCR2172 ***/
#define IDS_P01_ADDR_337    (IDS_P01_CATEGO_06 + 58) /* Addr.337 - Foreign Cur. #3, MDC_FC3_ADR   */
#define IDS_P01_ADDR_338    (IDS_P01_CATEGO_06 + 59) /* Addr.338 - Foreign Cur. #4, MDC_FC4_ADR   */
#define IDS_P01_ADDR_339    (IDS_P01_CATEGO_06 + 60) /* Addr.339 - Foreign Cur. #5, MDC_FC5_ADR   */
#define IDS_P01_ADDR_340    (IDS_P01_CATEGO_06 + 61) /* Addr.340 - Foreign Cur. #6, MDC_FC6_ADR   */
#define IDS_P01_ADDR_341    (IDS_P01_CATEGO_06 + 62) /* Addr.341 - Foreign Cur. #7, MDC_FC7_ADR   */
#define IDS_P01_ADDR_342    (IDS_P01_CATEGO_06 + 63) /* Addr.342 - Foreign Cur. #8, MDC_FC8_ADR   */

/*** NCR2172 ***/
/*#define IDS_P01_ADDR_277    (IDS_P01_CATEGO_06 + 58) *//* Addr.277 - Euro,        V3.4 */
/*#define IDS_P01_ADDR_278    (IDS_P01_CATEGO_06 + 59) *//* Addr.278 - Euro FC#1/2, V3.4 */
#define IDS_P01_ADDR_277    (IDS_P01_CATEGO_06 + 64) /* Addr.277 - Euro,        V3.4 */
#define IDS_P01_ADDR_278    (IDS_P01_CATEGO_06 + 65) /* Addr.278 - Euro FC#1/2, V3.4 */

#define IDS_P01_ADDR_279    (IDS_P01_CATEGO_06 + 66) /* Addr.279 - Euro Enhancement */
#define IDS_P01_ADDR_280    (IDS_P01_CATEGO_06 + 67) /* Addr.280 - Euro Enhancement */
#define IDS_P01_ADDR_281    (IDS_P01_CATEGO_06 + 68) /* Addr.281 - Euro Enhancement */
/*** Saratoga ***/
#define IDS_P01_ADDR_369    (IDS_P01_CATEGO_06 + 69) /* Addr.369 - Rounding */
#define IDS_P01_ADDR_370    (IDS_P01_CATEGO_06 + 70) /* Addr.370 - Rounding */

/* ### ADD Saratoga (052500) */
#define IDS_P01_ADDR_353    (IDS_P01_CATEGO_06 + 71) /* Addr.353 - Food Stamp */
#define IDS_P01_ADDR_354    (IDS_P01_CATEGO_06 + 72) /* Addr.354 - Food Stamp */

//SR#131
#define IDS_P01_ADDR_377_1	(IDS_P01_CATEGO_06 + 73) // Addr.377 - CP Account Security
#define IDS_P01_ADDR_377_2	(IDS_P01_CATEGO_06 + 74) // Addr.377 - EPT Account Security
#define IDS_P01_ADDR_378_1	(IDS_P01_CATEGO_06 + 75) // Addr.378 - CP Account # Masking
#define IDS_P01_ADDR_378_2	(IDS_P01_CATEGO_06 + 76) // Addr.378 - EPT Account # Masking
#define IDS_P01_ADDR_443	(IDS_P01_CATEGO_06 + 77) // Addr.378 - EPT Account # Masking

//SR#18
#define IDS_P01_ADDR_379	(IDS_P01_CATEGO_06 + 78) // Addr.379 - Check No Purchase

#define IDS_P01_ADDR_385    (IDS_P01_CATEGO_06 + 79) /* Addr.386 - Tender #12        */
#define IDS_P01_ADDR_386    (IDS_P01_CATEGO_06 + 80) /* Addr.387 - Tender #12        */
#define IDS_P01_ADDR_403    (IDS_P01_CATEGO_06 + 81) /* Addr.404 - Tender #12        */
#define IDS_P01_ADDR_412    (IDS_P01_CATEGO_06 + 82) /* Addr.405 - Tender #12        */
#define IDS_P01_ADDR_413    (IDS_P01_CATEGO_06 + 83) /* Addr.422 - Tender #12       */
#define IDS_P01_ADDR_387    (IDS_P01_CATEGO_06 + 84) /* Addr.388 - Tender #13        */
#define IDS_P01_ADDR_388    (IDS_P01_CATEGO_06 + 85) /* Addr.389 - Tender #13        */
#define IDS_P01_ADDR_404    (IDS_P01_CATEGO_06 + 86) /* Addr.406 - Tender #13        */
#define IDS_P01_ADDR_414    (IDS_P01_CATEGO_06 + 87) /* Addr.407 - Tender #13        */
#define IDS_P01_ADDR_415    (IDS_P01_CATEGO_06 + 88) /* Addr.423 - Tender #13        */
#define IDS_P01_ADDR_389    (IDS_P01_CATEGO_06 + 89) /* Addr.390 - Tender #14        */
#define IDS_P01_ADDR_390    (IDS_P01_CATEGO_06 + 90) /* Addr.391 - Tender #14        */
#define IDS_P01_ADDR_405    (IDS_P01_CATEGO_06 + 91) /* Addr.408 - Tender #14        */
#define IDS_P01_ADDR_416    (IDS_P01_CATEGO_06 + 92) /* Addr.409 - Tender #14        */
#define IDS_P01_ADDR_417    (IDS_P01_CATEGO_06 + 93) /* Addr.424 - Tender #14        */
#define IDS_P01_ADDR_391    (IDS_P01_CATEGO_06 + 94) /* Addr.392 - Tender #15        */
#define IDS_P01_ADDR_392    (IDS_P01_CATEGO_06 + 95) /* Addr.393 - Tender #15        */
#define IDS_P01_ADDR_406    (IDS_P01_CATEGO_06 + 96) /* Addr.410 - Tender #15        */
#define IDS_P01_ADDR_418    (IDS_P01_CATEGO_06 + 97) /* Addr.411 - Tender #15        */
#define IDS_P01_ADDR_419    (IDS_P01_CATEGO_06 + 98) /* Addr.425 - Tender #15        */
#define IDS_P01_ADDR_393    (IDS_P01_CATEGO_06 + 99) /* Addr.394 - Tender #16        */
#define IDS_P01_ADDR_394    (IDS_P01_CATEGO_06 + 100) /* Addr.395 - Tender #16        */
#define IDS_P01_ADDR_407    (IDS_P01_CATEGO_06 + 101) /* Addr.412 - Tender #16        */
#define IDS_P01_ADDR_420    (IDS_P01_CATEGO_06 + 102) /* Addr.413 - Tender #16        */
#define IDS_P01_ADDR_421    (IDS_P01_CATEGO_06 + 103) /* Addr.426 - Tender #16        */
#define IDS_P01_ADDR_395    (IDS_P01_CATEGO_06 + 104) /* Addr.396 - Tender #17        */
#define IDS_P01_ADDR_396    (IDS_P01_CATEGO_06 + 105) /* Addr.397 - Tender #17        */
#define IDS_P01_ADDR_408    (IDS_P01_CATEGO_06 + 106) /* Addr.414 - Tender #17        */
#define IDS_P01_ADDR_422    (IDS_P01_CATEGO_06 + 107) /* Addr.415 - Tender #17        */
#define IDS_P01_ADDR_423    (IDS_P01_CATEGO_06 + 108) /* Addr.427 - Tender #17        */
#define IDS_P01_ADDR_397    (IDS_P01_CATEGO_06 + 109) /* Addr.398 - Tender #18        */
#define IDS_P01_ADDR_398    (IDS_P01_CATEGO_06 + 110) /* Addr.399 - Tender #18        */
#define IDS_P01_ADDR_409    (IDS_P01_CATEGO_06 + 111) /* Addr.416 - Tender #18        */
#define IDS_P01_ADDR_424    (IDS_P01_CATEGO_06 + 112) /* Addr.417 - Tender #18        */
#define IDS_P01_ADDR_425    (IDS_P01_CATEGO_06 + 113) /* Addr.428 - Tender #18        */
#define IDS_P01_ADDR_399    (IDS_P01_CATEGO_06 + 114) /* Addr.400 - Tender #19        */
#define IDS_P01_ADDR_400    (IDS_P01_CATEGO_06 + 115) /* Addr.401 - Tender #19        */
#define IDS_P01_ADDR_410    (IDS_P01_CATEGO_06 + 116) /* Addr.418 - Tender #19        */
#define IDS_P01_ADDR_426    (IDS_P01_CATEGO_06 + 117) /* Addr.419 - Tender #19        */
#define IDS_P01_ADDR_427    (IDS_P01_CATEGO_06 + 118) /* Addr.429 - Tender #19        */
#define IDS_P01_ADDR_401    (IDS_P01_CATEGO_06 + 119) /* Addr.402 - Tender #20        */
#define IDS_P01_ADDR_402    (IDS_P01_CATEGO_06 + 120) /* Addr.403 - Tender #20        */
#define IDS_P01_ADDR_411    (IDS_P01_CATEGO_06 + 121) /* Addr.420 - Tender #20        */
#define IDS_P01_ADDR_428    (IDS_P01_CATEGO_06 + 122) /* Addr.421 - Tender #20        */
#define IDS_P01_ADDR_429    (IDS_P01_CATEGO_06 + 123) /* Addr.430 - Tender #20, style IDS_P01_STYLE_429 */
#define IDS_P01_ADDR_483    (IDS_P01_CATEGO_06 + 124) /* Addr.483 - Tender #1 (Cash)  */
#define IDS_P01_ADDR_484    (IDS_P01_CATEGO_06 + 125) /* Addr.484 - Tender #2, style IDS_P01_STYLE_484 */
#define IDS_P01_ADDR_485    (IDS_P01_CATEGO_06 + 126) /* Addr.485 - Tender #3, style IDS_P01_STYLE_485 */
#define IDS_P01_ADDR_486    (IDS_P01_CATEGO_06 + 127) /* Addr.486 - Tender #4, style IDS_P01_STYLE_486 */
#define IDS_P01_ADDR_487    (IDS_P01_CATEGO_06 + 128) /* Addr.487 - Tender #5, style IDS_P01_STYLE_487 */
#define IDS_P01_ADDR_488    (IDS_P01_CATEGO_06 + 129) /* Addr.488 - Tender #6, style IDS_P01_STYLE_488 */
#define IDS_P01_ADDR_489    (IDS_P01_CATEGO_06 + 130) /* Addr.489 - Tender #7, style IDS_P01_STYLE_489 */
#define IDS_P01_ADDR_490    (IDS_P01_CATEGO_06 + 131) /* Addr.490 - Tender #8, style IDS_P01_STYLE_490 */
#define IDS_P01_ADDR_491    (IDS_P01_CATEGO_06 + 132) /* Addr.491 - Tender #9, style IDS_P01_STYLE_491 */
#define IDS_P01_ADDR_492    (IDS_P01_CATEGO_06 + 133) /* Addr.492 - Tender #10, style IDS_P01_STYLE_492 */
#define IDS_P01_ADDR_493    (IDS_P01_CATEGO_06 + 134) /* Addr.493 - Tender #11, style IDS_P01_STYLE_493 */
#define IDS_P01_ADDR_494    (IDS_P01_CATEGO_06 + 135) /* Addr.494 - Tender #12, style IDS_P01_STYLE_494 */
#define IDS_P01_ADDR_495    (IDS_P01_CATEGO_06 + 136) /* Addr.495 - Tender #13, style IDS_P01_STYLE_495 */
#define IDS_P01_ADDR_496    (IDS_P01_CATEGO_06 + 137) /* Addr.496 - Tender #14, style IDS_P01_STYLE_496 */
#define IDS_P01_ADDR_497    (IDS_P01_CATEGO_06 + 138) /* Addr.497 - Tender #15, style IDS_P01_STYLE_497 */
#define IDS_P01_ADDR_498    (IDS_P01_CATEGO_06 + 139) /* Addr.498 - Tender #16, style IDS_P01_STYLE_498 */
#define IDS_P01_ADDR_499    (IDS_P01_CATEGO_06 + 140) /* Addr.499 - Tender #17, style IDS_P01_STYLE_499 */
#define IDS_P01_ADDR_500    (IDS_P01_CATEGO_06 + 141) /* Addr.500 - Tender #18, style IDS_P01_STYLE_500 */
#define IDS_P01_ADDR_501    (IDS_P01_CATEGO_06 + 142) /* Addr.501 - Tender #19, style IDS_P01_STYLE_501 */
#define IDS_P01_ADDR_502    (IDS_P01_CATEGO_06 + 143) /* Addr.502 - Tender #20, style IDS_P01_STYLE_502 */

#define P01_CATE_06_NO      143              /* No. of Address in Category 6     , Saratoga*/

// special defines for MDC pages with multiple sections to provide the text
#define IDS_P01_ADDR_443_2	(IDS_P01_CATEGO_06 + 151) // Addr.443 - EPT Account # Masking, used in IDS_P01_STYLE_443


    /* ----------------------------------- */
    /* ===== CATEGORY NO. 7 (Report) ===== */
#define IDS_P01_ADDR_085_2  (IDS_P01_CATEGO_07 + 1) /* Addr.085 - (Non) Resettable  */
#define IDS_P01_ADDR_086_1  (IDS_P01_CATEGO_07 + 2) /* Addr.086 - Read Report Ctrl. */
#define IDS_P01_ADDR_086_2  (IDS_P01_CATEGO_07 + 3) /* Addr.086 - Reset Report Ctl. */
#define IDS_P01_ADDR_087    (IDS_P01_CATEGO_07 + 4) /* Addr.087 - Parameter Report  */
#define IDS_P01_ADDR_088    (IDS_P01_CATEGO_07 + 5) /* Addr.088 - Coupon Report     */
#define IDS_P01_ADDR_089_1  (IDS_P01_CATEGO_07 + 6) /* Addr.089 - Department Report */
#define IDS_P01_ADDR_089_2  (IDS_P01_CATEGO_07 + 7) /* Addr.089 - PLU Report        */
#define IDS_P01_ADDR_090_1  (IDS_P01_CATEGO_07 + 8) /* Addr.090 - Hourly Report     */
#define IDS_P01_ADDR_090_2  (IDS_P01_CATEGO_07 + 9) /* Addr.090 - Cas/Ser Reset Rep.*/
#define IDS_P01_ADDR_091    (IDS_P01_CATEGO_07 +10) /* Addr.091 - EOD Report        */
#define IDS_P01_ADDR_092    (IDS_P01_CATEGO_07 +11) /* Addr.092 - EOD Report        */
#define IDS_P01_ADDR_253    (IDS_P01_CATEGO_07 +12) /* Addr.253 - EOD Report        */
#define IDS_P01_ADDR_254    (IDS_P01_CATEGO_07 +13) /* Addr.253 - EOD Report        */
#define IDS_P01_ADDR_093    (IDS_P01_CATEGO_07 +14) /* Addr.093 - PTD Report        */
#define IDS_P01_ADDR_094    (IDS_P01_CATEGO_07 +15) /* Addr.094 - PTD Report        */
#define IDS_P01_ADDR_255    (IDS_P01_CATEGO_07 +16) /* Addr.255 - PTD Report        */
#define IDS_P01_ADDR_256    (IDS_P01_CATEGO_07 +17) /* Addr.255 - PTD Report        */
#define IDS_P01_ADDR_095    (IDS_P01_CATEGO_07 +18) /* Addr.095 - ETK Report        */
#define IDS_P01_ADDR_096    (IDS_P01_CATEGO_07 +19) /* Addr.096 - PTD / EOD Report  */
#define IDS_P01_ADDR_097    (IDS_P01_CATEGO_07 +20) /* Addr.097 - Server Report     */
#define IDS_P01_ADDR_173    (IDS_P01_CATEGO_07 +21) /* Addr.173 - G.C. Report       */
#define IDS_P01_ADDR_174    (IDS_P01_CATEGO_07 +22) /* Addr.174 - G.C. Report       */

#define P01_CATE_07_NO  22              /* No. of Address in Category 7, IDS_P01_CATEGO_07     */

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
     /* ###DEL Saratoga */
/*** NCR2172 ***/
#define IDS_P01_ADDR_345    (IDS_P01_CATEGO_07 +21) /* Addr.345 - Operator Report   */
#define IDS_P01_ADDR_346    (IDS_P01_CATEGO_07 +22) /* Addr.346 - Operator Report   */
#define IDS_P01_ADDR_347    (IDS_P01_CATEGO_07 +23) /* Addr.347 - Operator Report   */
#define IDS_P01_ADDR_348    (IDS_P01_CATEGO_07 +24) /* Addr.348 - Operator Report   */
#define IDS_P01_ADDR_349    (IDS_P01_CATEGO_07 +25) /* Addr.349 - Operator Report   */
#define IDS_P01_ADDR_350    (IDS_P01_CATEGO_07 +26) /* Addr.350 - Operator Report   */
#define IDS_P01_ADDR_351    (IDS_P01_CATEGO_07 +27) /* Addr.351 - Operator Report   */
#define IDS_P01_ADDR_352    (IDS_P01_CATEGO_07 +28) /* Addr.352 - Operator Report   */
#endif

    /* ===== CATEGORY NO. 8 (Report (Financial)) ===== */
#define IDS_P01_ADDR_101    (IDS_P01_CATEGO_08 + 1) /* Addr.101 - Financial Report  */
#define IDS_P01_ADDR_102    (IDS_P01_CATEGO_08 + 2) /* Addr.102 - Financial Report  */
#define IDS_P01_ADDR_103    (IDS_P01_CATEGO_08 + 3) /* Addr.103 - Financial Report  */
#define IDS_P01_ADDR_104    (IDS_P01_CATEGO_08 + 4) /* Addr.104 - Financial Report  */
#define IDS_P01_ADDR_105    (IDS_P01_CATEGO_08 + 5) /* Addr.105 - Financial Report  */
#define IDS_P01_ADDR_106    (IDS_P01_CATEGO_08 + 6) /* Addr.106 - Financial Report  */
#define IDS_P01_ADDR_107    (IDS_P01_CATEGO_08 + 7) /* Addr.107 - Financial Report  */
#define IDS_P01_ADDR_108    (IDS_P01_CATEGO_08 + 8) /* Addr.108 - Financial Report  */
#define IDS_P01_ADDR_109    (IDS_P01_CATEGO_08 + 9) /* Addr.109 - Financial Report  */
#define IDS_P01_ADDR_110    (IDS_P01_CATEGO_08 +10) /* Addr.110 - Financial Report  */
#define IDS_P01_ADDR_111    (IDS_P01_CATEGO_08 +11) /* Addr.111 - Financial Report  */
#define IDS_P01_ADDR_112    (IDS_P01_CATEGO_08 +12) /* Addr.112 - Financial Report  */
#define IDS_P01_ADDR_113    (IDS_P01_CATEGO_08 +13) /* Addr.113 - Financial Report  */
#define IDS_P01_ADDR_114    (IDS_P01_CATEGO_08 +14) /* Addr.114 - Financial Report  */
#define IDS_P01_ADDR_115    (IDS_P01_CATEGO_08 +15) /* Addr.115 - Financial Report  */
#define IDS_P01_ADDR_116    (IDS_P01_CATEGO_08 +16) /* Addr.116 - Financial Report  */
#define IDS_P01_ADDR_117    (IDS_P01_CATEGO_08 +17) /* Addr.117 - Financial Report  */
#define IDS_P01_ADDR_118    (IDS_P01_CATEGO_08 +18) /* Addr.118 - Financial Report  */
#define IDS_P01_ADDR_119    (IDS_P01_CATEGO_08 +19) /* Addr.119 - Financial Report  */
#define IDS_P01_ADDR_120    (IDS_P01_CATEGO_08 +20) /* Addr.120 - Financial Report  */
#define IDS_P01_ADDR_121    (IDS_P01_CATEGO_08 +21) /* Addr.121 - Financial Report  */
#define IDS_P01_ADDR_122    (IDS_P01_CATEGO_08 +22) /* Addr.122 - Financial Report  */
#define IDS_P01_ADDR_123    (IDS_P01_CATEGO_08 +23) /* Addr.123 - Financial Report  */
#define IDS_P01_ADDR_124    (IDS_P01_CATEGO_08 +24) /* Addr.124 - Financial Report  */
#define IDS_P01_ADDR_125    (IDS_P01_CATEGO_08 +25) /* Addr.125 - Financial Report  */
#define IDS_P01_ADDR_126    (IDS_P01_CATEGO_08 +26) /* Addr.126 - Financial Report  */
#define IDS_P01_ADDR_127    (IDS_P01_CATEGO_08 +27) /* Addr.127 - Financial Report  */
#define IDS_P01_ADDR_128    (IDS_P01_CATEGO_08 +28) /* Addr.128 - Financial Report  */
#define IDS_P01_ADDR_129    (IDS_P01_CATEGO_08 +29) /* Addr.129 - Financial Report  */
#define IDS_P01_ADDR_130    (IDS_P01_CATEGO_08 +30) /* Addr.130 - Financial Report  */
#define IDS_P01_ADDR_274    (IDS_P01_CATEGO_08 +31) /* Addr.274 - Financial Report, V3.3 */
/*** NCR2172 ***/
#define IDS_P01_ADDR_343    (IDS_P01_CATEGO_08 +32) /* Addr.343 - Registor Financial Report */
#define IDS_P01_ADDR_344    (IDS_P01_CATEGO_08 +33) /* Addr.344 - Registor Financial Report */
/* ### ADD Saratog (052500) */
#define IDS_P01_ADDR_357    (IDS_P01_CATEGO_08 +34) /* Addr.357 - Registor Financial Report */
#define IDS_P01_ADDR_358    (IDS_P01_CATEGO_08 +35) /* Addr.358 - Registor Financial Report */
#define IDS_P01_ADDR_430    (IDS_P01_CATEGO_08 +36) /* Addr.430 - Registor Financial Report */
#define IDS_P01_ADDR_431    (IDS_P01_CATEGO_08 +37) /* Addr.431 - Registor Financial Report */
#define IDS_P01_ADDR_434    (IDS_P01_CATEGO_08 +38) /* Addr.434 - Registor Financial Report */
#define IDS_P01_ADDR_435    (IDS_P01_CATEGO_08 +39) /* Addr.435 - Registor Financial Report */
#define IDS_P01_ADDR_440    (IDS_P01_CATEGO_08 +40) /* Addr.440 - Registor Financial Report */
#define IDS_P01_ADDR_441    (IDS_P01_CATEGO_08 +41) /* Addr.441 - Registor Financial Report */

#define IDS_P01_ADDR_467	(IDS_P01_CATEGO_08 +42) //Financial Reports (for bonus totals)
#define IDS_P01_ADDR_468	(IDS_P01_CATEGO_08 +43)	//Financial Reports (for bonus totals)
#define IDS_P01_ADDR_469	(IDS_P01_CATEGO_08 +44)	//Financial Reports (for bonus totals)
#define IDS_P01_ADDR_470	(IDS_P01_CATEGO_08 +45)	//Financial Reports (for bonus totals)
#define IDS_P01_ADDR_520	(IDS_P01_CATEGO_08 +46)	//Financial Reports (for MDC_RETURNS_RTN1_ADR), MDC_RPTFIN46_ADR

#define P01_CATE_08_NO  46              /* No. of Address in Category 8, IDS_P01_CATEGO_08     , V3.3 */

    /* ---------------------------------------------- */
    /* ===== CATEGORY NO. 9 (Report (Operator)) ===== */
#define IDS_P01_ADDR_131    (IDS_P01_CATEGO_09 + 1) /* Addr.131 - Operator Report    */
#define IDS_P01_ADDR_132    (IDS_P01_CATEGO_09 + 2) /* Addr.132 - Operator Report    */
#define IDS_P01_ADDR_133    (IDS_P01_CATEGO_09 + 3) /* Addr.133 - Operator Report    */
#define IDS_P01_ADDR_134    (IDS_P01_CATEGO_09 + 4) /* Addr.134 - Operator Report    */
#define IDS_P01_ADDR_135    (IDS_P01_CATEGO_09 + 5) /* Addr.135 - Operator Report    */
#define IDS_P01_ADDR_136    (IDS_P01_CATEGO_09 + 6) /* Addr.136 - Operator Report    */
#define IDS_P01_ADDR_137    (IDS_P01_CATEGO_09 + 7) /* Addr.137 - Operator Report    */
#define IDS_P01_ADDR_138    (IDS_P01_CATEGO_09 + 8) /* Addr.138 - Operator Report    */
#define IDS_P01_ADDR_139    (IDS_P01_CATEGO_09 + 9) /* Addr.139 - Operator Report    */
#define IDS_P01_ADDR_140    (IDS_P01_CATEGO_09 +10) /* Addr.140 - Operator Report    */
#define IDS_P01_ADDR_141    (IDS_P01_CATEGO_09 +11) /* Addr.141 - Operator Report    */
#define IDS_P01_ADDR_142    (IDS_P01_CATEGO_09 +12) /* Addr.142 - Operator Report    */
#define IDS_P01_ADDR_143    (IDS_P01_CATEGO_09 +13) /* Addr.143 - Operator Report    */
#define IDS_P01_ADDR_144    (IDS_P01_CATEGO_09 +14) /* Addr.144 - Operator Report    */
#define IDS_P01_ADDR_145    (IDS_P01_CATEGO_09 +15) /* Addr.145 - Operator Report    */
#define IDS_P01_ADDR_146    (IDS_P01_CATEGO_09 +16) /* Addr.146 - Operator Report    */
#define IDS_P01_ADDR_147    (IDS_P01_CATEGO_09 +17) /* Addr.147 - Operator Report    */
#define IDS_P01_ADDR_148    (IDS_P01_CATEGO_09 +18) /* Addr.148 - Operator Report    */
#define IDS_P01_ADDR_149    (IDS_P01_CATEGO_09 +19) /* Addr.149 - Operator Report    */
#define IDS_P01_ADDR_150    (IDS_P01_CATEGO_09 +20) /* Addr.150 - Operator Report    */
#define IDS_P01_ADDR_151    (IDS_P01_CATEGO_09 +21) /* Addr.151 - Operator Report    */
#define IDS_P01_ADDR_152    (IDS_P01_CATEGO_09 +22) /* Addr.152 - Operator Report    */
#define IDS_P01_ADDR_153    (IDS_P01_CATEGO_09 +23) /* Addr.153 - Operator Report    */
#define IDS_P01_ADDR_154    (IDS_P01_CATEGO_09 +24) /* Addr.154 - Operator Report, V3.3 */
#define IDS_P01_ADDR_155    (IDS_P01_CATEGO_09 +25) /* Addr.155 - Operator Report, V3.3 */
#define IDS_P01_ADDR_156    (IDS_P01_CATEGO_09 +26) /* Addr.156 - Operator Report, V3.3 */

/* ###ADD Saratoga */
#define IDS_P01_ADDR_345    (IDS_P01_CATEGO_09 +27) /* Addr.345 - Operator Report   */
#define IDS_P01_ADDR_346    (IDS_P01_CATEGO_09 +28) /* Addr.346 - Operator Report   */
#define IDS_P01_ADDR_347    (IDS_P01_CATEGO_09 +29) /* Addr.347 - Operator Report   */
#define IDS_P01_ADDR_348    (IDS_P01_CATEGO_09 +30) /* Addr.348 - Operator Report   */
#define IDS_P01_ADDR_349    (IDS_P01_CATEGO_09 +31) /* Addr.349 - Operator Report   */
#define IDS_P01_ADDR_350    (IDS_P01_CATEGO_09 +32) /* Addr.350 - Operator Report   */
#define IDS_P01_ADDR_351    (IDS_P01_CATEGO_09 +33) /* Addr.351 - Operator Report   */
#define IDS_P01_ADDR_352    (IDS_P01_CATEGO_09 +34) /* Addr.352 - Operator Report   */
#define IDS_P01_ADDR_352    (IDS_P01_CATEGO_09 +34) /* Addr.352 - Operator Report   */
#define IDS_P01_ADDR_352    (IDS_P01_CATEGO_09 +34) /* Addr.352 - Operator Report   */
#define IDS_P01_ADDR_432    (IDS_P01_CATEGO_09 +35) /* Addr.432 - Operator Report   */
#define IDS_P01_ADDR_433    (IDS_P01_CATEGO_09 +36) /* Addr.433 - Operator Report   */
#define IDS_P01_ADDR_436    (IDS_P01_CATEGO_09 +37) /* Addr.436 - Operator Report   */
#define IDS_P01_ADDR_437    (IDS_P01_CATEGO_09 +38) /* Addr.437 - Operator Report   */
#define IDS_P01_ADDR_438	(IDS_P01_CATEGO_09 +39) /* Addr.438 - Register Financial Report */
#define IDS_P01_ADDR_439	(IDS_P01_CATEGO_09 +40) /* Addr.439 - Register Financial Report */

//addition for Release 2.1 JHHJ operator reporting taxable information
#define IDS_P01_ADDR_446    (IDS_P01_CATEGO_09 +41) /* Addr.446 - Operator Report   */
#define IDS_P01_ADDR_447    (IDS_P01_CATEGO_09 +42) /* Addr.447 - Operator Report   */
#define IDS_P01_ADDR_448    (IDS_P01_CATEGO_09 +43) /* Addr.448 - Operator Report   */
#define IDS_P01_ADDR_449    (IDS_P01_CATEGO_09 +44) /* Addr.449 - Operator Report   */
#define IDS_P01_ADDR_450    (IDS_P01_CATEGO_09 +45) /* Addr.450 - Operator Report   */

#define IDS_P01_ADDR_463	(IDS_P01_CATEGO_09 +46) //Operator Reports (for bonus totals)
#define IDS_P01_ADDR_464	(IDS_P01_CATEGO_09 +47) //Operator Reports (for bonus totals)
#define IDS_P01_ADDR_465	(IDS_P01_CATEGO_09 +48) //Operator Reports (for bonus totals)
#define IDS_P01_ADDR_466	(IDS_P01_CATEGO_09 +49) //Operator Reports (for bonus totals), MDC_RPTCAS45_ADR, IDS_P01_STYLE_466
#define IDS_P01_ADDR_521	(IDS_P01_CATEGO_09 +50) //Operator Reports (for returns), MDC_RPTCAS50_ADR, IDS_P01_STYLE_521

#define P01_CATE_09_NO  50              /* No. of Address in Category 9, IDS_P01_CATEGO_09,  Saratoga */

    /* --------------------------------------------- */
    /* ===== CATEGORY NO. 10 (Report (Server)) ===== */
#define IDS_P01_ADDR_157    (IDS_P01_CATEGO_10 + 1) /* Addr.157 - Server Report     */
#define IDS_P01_ADDR_158    (IDS_P01_CATEGO_10 + 2) /* Addr.158 - Server Report     */
#define IDS_P01_ADDR_159    (IDS_P01_CATEGO_10 + 3) /* Addr.159 - Server Report     */
#define IDS_P01_ADDR_160    (IDS_P01_CATEGO_10 + 4) /* Addr.160 - Server Report     */
#define IDS_P01_ADDR_161    (IDS_P01_CATEGO_10 + 5) /* Addr.161 - Server Report     */
#define IDS_P01_ADDR_162    (IDS_P01_CATEGO_10 + 6) /* Addr.162 - Server Report     */
#define IDS_P01_ADDR_163    (IDS_P01_CATEGO_10 + 7) /* Addr.163 - Server Report     */
#define IDS_P01_ADDR_164    (IDS_P01_CATEGO_10 + 8) /* Addr.164 - Server Report     */
#define IDS_P01_ADDR_165    (IDS_P01_CATEGO_10 + 9) /* Addr.165 - Server Report     */
#define IDS_P01_ADDR_166    (IDS_P01_CATEGO_10 +10) /* Addr.166 - Server Report     */
#define IDS_P01_ADDR_167    (IDS_P01_CATEGO_10 +11) /* Addr.167 - Server Report     */
#define IDS_P01_ADDR_168    (IDS_P01_CATEGO_10 +12) /* Addr.168 - Server Report     */
#define IDS_P01_ADDR_169    (IDS_P01_CATEGO_10 +13) /* Addr.169 - Server Report     */
#define IDS_P01_ADDR_170    (IDS_P01_CATEGO_10 +14) /* Addr.170 - Server Report     */
#define IDS_P01_ADDR_171    (IDS_P01_CATEGO_10 +15) /* Addr.171 - Server Report     */
#define IDS_P01_ADDR_172    (IDS_P01_CATEGO_10 +16) /* Addr.172 - Server Report     */
#define IDS_P01_ADDR_269    (IDS_P01_CATEGO_10 +17) /* Addr.269 - Server Report     */

#define P01_CATE_10_NO  17              /* No. of Address in Category 10, IDS_P01_CATEGO_10    */

    /* -------------------------------------------- */
    /* ===== CATEGORY NO. 11 (Charge Posting) ===== */
#define IDS_P01_ADDR_99     (IDS_P01_CATEGO_11 + 1) /* Addr.99  - Server Report     */
#define IDS_P01_ADDR_100    (IDS_P01_CATEGO_11 + 2) /* Addr.100 - Server Report     */
#define IDS_P01_ADDR_197    (IDS_P01_CATEGO_11 + 3) /* Addr.197 - Server Report     */
#define IDS_P01_ADDR_198    (IDS_P01_CATEGO_11 + 4) /* Addr.198 - Server Report     */
#define IDS_P01_ADDR_199    (IDS_P01_CATEGO_11 + 5) /* Addr.199 - Server Report     */
#define IDS_P01_ADDR_200    (IDS_P01_CATEGO_11 + 6) /* Addr.200 - Server Report     */

/*** NCR2172 ***/
#define IDS_P01_ADDR_361    (IDS_P01_CATEGO_11 + 7) /* Addr.361 - Enhanced EPT      */
#define IDS_P01_ADDR_361_2	(IDS_P01_CATEGO_11 + 8)/* Addr.361 - Gift Card			*/
#define IDS_P01_ADDR_442	(IDS_P01_CATEGO_11 + 9) /* Addr.442 - EPT Masking Pre-Auth */
#define IDS_P01_ADDR_444	(IDS_P01_CATEGO_11 + 10) /* Addr.444 - EPT Masking Pre-Auth */
#define IDS_P01_ADDR_362	(IDS_P01_CATEGO_11 + 11) /* Addr. 362 - Gift # Masking */
#define IDS_P01_ADDR_475	(IDS_P01_CATEGO_11 + 12) /* MDC_RCT_PRT_ADR, Addr. 475 -order of the reciepts for merchant and customer copy */

#define P01_CATE_11_NO  12              /* No. of Address in Category 11, IDS_P01_CATEGO_11    */
// special defines for MDC pages with multiple sections to provide the text
#define IDS_P01_ADDR_442_2	(IDS_P01_CATEGO_11 + 23) /* Addr. 442 - Expirate Date masking Preauth, IDS_P01_STYLE_442 */

    /* -------------------------------------------- */
    /* ===== CATEGORY NO. 12 (Report(Hourly)) ===== */
#define IDS_P01_ADDR_271    (IDS_P01_CATEGO_12 + 1) /* MDC_HOUR2_ADR, Addr. 271 - Hourly Report, V3.3 */
#define IDS_P01_ADDR_272    (IDS_P01_CATEGO_12 + 2) /* MDC_HOUR3_ADR, Addr. 272 - Hourly Report, V3.3 */
#define IDS_P01_ADDR_273    (IDS_P01_CATEGO_12 + 3) /* MDC_HOUR4_ADR, Addr. 273 - Hourly Report, V3.3 */
#define IDS_P01_ADDR_471	(IDS_P01_CATEGO_12 + 4) /* MDC_HOUR5_ADR, Addr. 471 - Hourly Report, 2.1.X*/
#define IDS_P01_ADDR_472	(IDS_P01_CATEGO_12 + 5) /* MDC_HOUR6_ADR, Addr. 472 - Hourly Report, 2.1.X*/
#define IDS_P01_ADDR_473	(IDS_P01_CATEGO_12 + 6) /* MDC_HOUR7_ADR, Addr. 473 - Hourly Report, 2.1.X*/
#define IDS_P01_ADDR_474	(IDS_P01_CATEGO_12 + 7) /* MDC_HOUR8_ADR, Addr. 474 - Hourly Report, 2.1.X*/

#define P01_CATE_12_NO   7              /* No. of Address in Category 12, IDS_P01_CATEGO_12    , V3.3 */

    /* -------------------------------------------- */
    /* ===== CATEGORY NO. 13 (Internal Environment - PLU Labels) ===== */
#define IDS_P01_ADDR_295    (IDS_P01_CATEGO_13 + 1) /* Addr.295 - PLU(LABEL)    */
#define IDS_P01_ADDR_296    (IDS_P01_CATEGO_13 + 2) /* Addr.296 - PLU(LABEL)    */
#define IDS_P01_ADDR_297    (IDS_P01_CATEGO_13 + 3) /* Addr.297 - PLU(LABEL)/GP */
#define IDS_P01_ADDR_298    (IDS_P01_CATEGO_13 + 4) /* Addr.298 - PLU(LABEL)    */
#define IDS_P01_ADDR_299    (IDS_P01_CATEGO_13 + 5) /* Addr.299 - PLU(LABEL  2) */
#define IDS_P01_ADDR_300    (IDS_P01_CATEGO_13 + 6) /* Addr.300 - PLU(LABEL  2) */
#define IDS_P01_ADDR_301    (IDS_P01_CATEGO_13 + 7) /* Addr.301 - PLU(LABEL 20) */
#define IDS_P01_ADDR_302    (IDS_P01_CATEGO_13 + 8) /* Addr.302 - PLU(LABEL 20) */
#define IDS_P01_ADDR_303    (IDS_P01_CATEGO_13 + 9) /* Addr.303 - PLU(LABEL 21) */
#define IDS_P01_ADDR_304    (IDS_P01_CATEGO_13 +10) /* Addr.304 - PLU(LABEL 21) */
#define IDS_P01_ADDR_305    (IDS_P01_CATEGO_13 +11) /* Addr.305 - PLU(LABEL 22) */
#define IDS_P01_ADDR_306    (IDS_P01_CATEGO_13 +12) /* Addr.306 - PLU(LABEL 22) */
#define IDS_P01_ADDR_307    (IDS_P01_CATEGO_13 +13) /* Addr.307 - PLU(LABEL 23) */
#define IDS_P01_ADDR_308    (IDS_P01_CATEGO_13 +14) /* Addr.308 - PLU(LABEL 23) */
#define IDS_P01_ADDR_309    (IDS_P01_CATEGO_13 +15) /* Addr.309 - PLU(LABEL 24) */
#define IDS_P01_ADDR_310    (IDS_P01_CATEGO_13 +16) /* Addr.310 - PLU(LABEL 24) */
#define IDS_P01_ADDR_311    (IDS_P01_CATEGO_13 +17) /* Addr.311 - PLU(LABEL 25) */
#define IDS_P01_ADDR_312    (IDS_P01_CATEGO_13 +18) /* Addr.312 - PLU(LABEL 25) */
#define IDS_P01_ADDR_313    (IDS_P01_CATEGO_13 +19) /* Addr.313 - PLU(LABEL 26) */
#define IDS_P01_ADDR_314    (IDS_P01_CATEGO_13 +20) /* Addr.314 - PLU(LABEL 26) */
#define IDS_P01_ADDR_315    (IDS_P01_CATEGO_13 +21) /* Addr.315 - PLU(LABEL 27) */
#define IDS_P01_ADDR_316    (IDS_P01_CATEGO_13 +22) /* Addr.316 - PLU(LABEL 27) */
#define IDS_P01_ADDR_317    (IDS_P01_CATEGO_13 +23) /* Addr.317 - PLU(LABEL 28) */
#define IDS_P01_ADDR_318    (IDS_P01_CATEGO_13 +24) /* Addr.318 - PLU(LABEL 28) */
#define IDS_P01_ADDR_319    (IDS_P01_CATEGO_13 +25) /* Addr.319 - PLU(LABEL 29) */
#define IDS_P01_ADDR_320    (IDS_P01_CATEGO_13 +26) /* Addr.320 - PLU(LABEL 29) */
#define IDS_P01_ADDR_321    (IDS_P01_CATEGO_13 +27) /* Addr.321 - PLU(LABEL  4) */
#define IDS_P01_ADDR_322    (IDS_P01_CATEGO_13 +28) /* Addr.322 - PLU(LABEL  4) */

#define P01_CATE_13_NO   28              /* No. of Address in Category 13, IDS_P01_CATEGO_13    , V3.3 */

// =================================================================


#define IDS_P01_SUBCAP      (IDS_P01_CATEGO_10 + P01_CATE_10_NO + 1) /* V3.3 , Category 10 is last */

/* ----- MDC Sub-DialogBox Style Table Definision ----- */

#define IDS_P01_STYLE       IDS_P01_SUBCAP
#define IDS_P01_STYLE_001   (IDS_P01_STYLE + 1)
#define IDS_P01_STYLE_002   (IDS_P01_STYLE + 2)
#define IDS_P01_STYLE_003   (IDS_P01_STYLE + 3)
#define IDS_P01_STYLE_004   (IDS_P01_STYLE + 4)
#define IDS_P01_STYLE_005   (IDS_P01_STYLE + 5)
#define IDS_P01_STYLE_006   (IDS_P01_STYLE + 6)
#define IDS_P01_STYLE_007   (IDS_P01_STYLE + 7)
#define IDS_P01_STYLE_008   (IDS_P01_STYLE + 8)
#define IDS_P01_STYLE_009   (IDS_P01_STYLE + 9)
#define IDS_P01_STYLE_010   (IDS_P01_STYLE + 10)
#define IDS_P01_STYLE_011   (IDS_P01_STYLE + 11)
#define IDS_P01_STYLE_012   (IDS_P01_STYLE + 12)
#define IDS_P01_STYLE_013   (IDS_P01_STYLE + 13)
#define IDS_P01_STYLE_014   (IDS_P01_STYLE + 14)
#define IDS_P01_STYLE_015   (IDS_P01_STYLE + 15)    // MDC dialog display style  MDC_TAX_ADR
#define IDS_P01_STYLE_016   (IDS_P01_STYLE + 16)
#define IDS_P01_STYLE_017   (IDS_P01_STYLE + 17)    // MDC dialog display style  MDC_CASIN_ADR
#define IDS_P01_STYLE_018   (IDS_P01_STYLE + 18)    // MDC dialog display style  MDC_WTIN_ADR
#define IDS_P01_STYLE_019   (IDS_P01_STYLE + 19)    // MDC dialog display style  MDC_SIGNOUT_ADR
#define IDS_P01_STYLE_020   (IDS_P01_STYLE + 20)
#define IDS_P01_STYLE_021   (IDS_P01_STYLE + 21)
#define IDS_P01_STYLE_022   (IDS_P01_STYLE + 22)
#define IDS_P01_STYLE_023   (IDS_P01_STYLE + 23)
#define IDS_P01_STYLE_024   (IDS_P01_STYLE + 24)
#define IDS_P01_STYLE_025   (IDS_P01_STYLE + 25)
#define IDS_P01_STYLE_026   (IDS_P01_STYLE + 26)
#define IDS_P01_STYLE_027   (IDS_P01_STYLE + 27)
#define IDS_P01_STYLE_028   (IDS_P01_STYLE + 28)
#define IDS_P01_STYLE_029   (IDS_P01_STYLE + 29)
#define IDS_P01_STYLE_030   (IDS_P01_STYLE + 30)
#define IDS_P01_STYLE_031   (IDS_P01_STYLE + 31)
#define IDS_P01_STYLE_032   (IDS_P01_STYLE + 32)
#define IDS_P01_STYLE_033   (IDS_P01_STYLE + 33)
#define IDS_P01_STYLE_034   (IDS_P01_STYLE + 34)
#define IDS_P01_STYLE_035   (IDS_P01_STYLE + 35)
#define IDS_P01_STYLE_036   (IDS_P01_STYLE + 36)
#define IDS_P01_STYLE_037   (IDS_P01_STYLE + 37)
#define IDS_P01_STYLE_038   (IDS_P01_STYLE + 38)
#define IDS_P01_STYLE_039   (IDS_P01_STYLE + 39)
#define IDS_P01_STYLE_040   (IDS_P01_STYLE + 40)
#define IDS_P01_STYLE_041   (IDS_P01_STYLE + 41)
#define IDS_P01_STYLE_042   (IDS_P01_STYLE + 42)
#define IDS_P01_STYLE_043   (IDS_P01_STYLE + 43)
#define IDS_P01_STYLE_044   (IDS_P01_STYLE + 44)
#define IDS_P01_STYLE_045   (IDS_P01_STYLE + 45)
#define IDS_P01_STYLE_046   (IDS_P01_STYLE + 46)
#define IDS_P01_STYLE_047   (IDS_P01_STYLE + 47)
#define IDS_P01_STYLE_048   (IDS_P01_STYLE + 48)
#define IDS_P01_STYLE_049   (IDS_P01_STYLE + 49)
#define IDS_P01_STYLE_050   (IDS_P01_STYLE + 50)
#define IDS_P01_STYLE_051   (IDS_P01_STYLE + 51)
#define IDS_P01_STYLE_052   (IDS_P01_STYLE + 52)
#define IDS_P01_STYLE_053   (IDS_P01_STYLE + 53)
#define IDS_P01_STYLE_054   (IDS_P01_STYLE + 54)
#define IDS_P01_STYLE_055   (IDS_P01_STYLE + 55)
#define IDS_P01_STYLE_056   (IDS_P01_STYLE + 56)
#define IDS_P01_STYLE_057   (IDS_P01_STYLE + 57)
#define IDS_P01_STYLE_058   (IDS_P01_STYLE + 58)
#define IDS_P01_STYLE_059   (IDS_P01_STYLE + 59)
#define IDS_P01_STYLE_060   (IDS_P01_STYLE + 60)
#define IDS_P01_STYLE_061   (IDS_P01_STYLE + 61)
#define IDS_P01_STYLE_062   (IDS_P01_STYLE + 62)
#define IDS_P01_STYLE_063   (IDS_P01_STYLE + 63)
#define IDS_P01_STYLE_064   (IDS_P01_STYLE + 64)
#define IDS_P01_STYLE_065   (IDS_P01_STYLE + 65)
#define IDS_P01_STYLE_066   (IDS_P01_STYLE + 66)
#define IDS_P01_STYLE_067   (IDS_P01_STYLE + 67)
#define IDS_P01_STYLE_068   (IDS_P01_STYLE + 68)
#define IDS_P01_STYLE_069   (IDS_P01_STYLE + 69)
#define IDS_P01_STYLE_070   (IDS_P01_STYLE + 70)
#define IDS_P01_STYLE_071   (IDS_P01_STYLE + 71)
#define IDS_P01_STYLE_072   (IDS_P01_STYLE + 72)
#define IDS_P01_STYLE_073   (IDS_P01_STYLE + 73)
#define IDS_P01_STYLE_074   (IDS_P01_STYLE + 74)
#define IDS_P01_STYLE_075   (IDS_P01_STYLE + 75)
#define IDS_P01_STYLE_076   (IDS_P01_STYLE + 76)
#define IDS_P01_STYLE_077   (IDS_P01_STYLE + 77)
#define IDS_P01_STYLE_078   (IDS_P01_STYLE + 78)
#define IDS_P01_STYLE_079   (IDS_P01_STYLE + 79)
#define IDS_P01_STYLE_080   (IDS_P01_STYLE + 80)
#define IDS_P01_STYLE_081   (IDS_P01_STYLE + 81)
#define IDS_P01_STYLE_082   (IDS_P01_STYLE + 82)    // MDC dialog display style  MDC_DRAWER2_ADR
#define IDS_P01_STYLE_083   (IDS_P01_STYLE + 83)
#define IDS_P01_STYLE_084   (IDS_P01_STYLE + 84)
#define IDS_P01_STYLE_085   (IDS_P01_STYLE + 85)
#define IDS_P01_STYLE_086   (IDS_P01_STYLE + 86)
#define IDS_P01_STYLE_087   (IDS_P01_STYLE + 87)
#define IDS_P01_STYLE_088   (IDS_P01_STYLE + 88)
#define IDS_P01_STYLE_089   (IDS_P01_STYLE + 89)
#define IDS_P01_STYLE_090   (IDS_P01_STYLE + 90)
#define IDS_P01_STYLE_091   (IDS_P01_STYLE + 91)
#define IDS_P01_STYLE_092   (IDS_P01_STYLE + 92)
#define IDS_P01_STYLE_093   (IDS_P01_STYLE + 93)
#define IDS_P01_STYLE_094   (IDS_P01_STYLE + 94)
#define IDS_P01_STYLE_095   (IDS_P01_STYLE + 95)
#define IDS_P01_STYLE_096   (IDS_P01_STYLE + 96)
#define IDS_P01_STYLE_097   (IDS_P01_STYLE + 97)
#define IDS_P01_STYLE_098   (IDS_P01_STYLE + 98)
#define IDS_P01_STYLE_099   (IDS_P01_STYLE + 99)
#define IDS_P01_STYLE_100   (IDS_P01_STYLE +100)
#define IDS_P01_STYLE_101   (IDS_P01_STYLE +101)
#define IDS_P01_STYLE_102   (IDS_P01_STYLE +102)
#define IDS_P01_STYLE_103   (IDS_P01_STYLE +103)
#define IDS_P01_STYLE_104   (IDS_P01_STYLE +104)
#define IDS_P01_STYLE_105   (IDS_P01_STYLE +105)
#define IDS_P01_STYLE_106   (IDS_P01_STYLE +106)
#define IDS_P01_STYLE_107   (IDS_P01_STYLE +107)
#define IDS_P01_STYLE_108   (IDS_P01_STYLE +108)
#define IDS_P01_STYLE_109   (IDS_P01_STYLE +109)
#define IDS_P01_STYLE_110   (IDS_P01_STYLE +110)
#define IDS_P01_STYLE_111   (IDS_P01_STYLE +111)
#define IDS_P01_STYLE_112   (IDS_P01_STYLE +112)
#define IDS_P01_STYLE_113   (IDS_P01_STYLE +113)
#define IDS_P01_STYLE_114   (IDS_P01_STYLE +114)
#define IDS_P01_STYLE_115   (IDS_P01_STYLE +115)
#define IDS_P01_STYLE_116   (IDS_P01_STYLE +116)
#define IDS_P01_STYLE_117   (IDS_P01_STYLE +117)
#define IDS_P01_STYLE_118   (IDS_P01_STYLE +118)
#define IDS_P01_STYLE_119   (IDS_P01_STYLE +119)
#define IDS_P01_STYLE_120   (IDS_P01_STYLE +120)
#define IDS_P01_STYLE_121   (IDS_P01_STYLE +121)
#define IDS_P01_STYLE_122   (IDS_P01_STYLE +122)
#define IDS_P01_STYLE_123   (IDS_P01_STYLE +123)
#define IDS_P01_STYLE_124   (IDS_P01_STYLE +124)
#define IDS_P01_STYLE_125   (IDS_P01_STYLE +125)
#define IDS_P01_STYLE_126   (IDS_P01_STYLE +126)
#define IDS_P01_STYLE_127   (IDS_P01_STYLE +127)
#define IDS_P01_STYLE_128   (IDS_P01_STYLE +128)
#define IDS_P01_STYLE_129   (IDS_P01_STYLE +129)
#define IDS_P01_STYLE_130   (IDS_P01_STYLE +130)
#define IDS_P01_STYLE_131   (IDS_P01_STYLE +131)
#define IDS_P01_STYLE_132   (IDS_P01_STYLE +132)
#define IDS_P01_STYLE_133   (IDS_P01_STYLE +133)
#define IDS_P01_STYLE_134   (IDS_P01_STYLE +134)
#define IDS_P01_STYLE_135   (IDS_P01_STYLE +135)
#define IDS_P01_STYLE_136   (IDS_P01_STYLE +136)
#define IDS_P01_STYLE_137   (IDS_P01_STYLE +137)
#define IDS_P01_STYLE_138   (IDS_P01_STYLE +138)
#define IDS_P01_STYLE_139   (IDS_P01_STYLE +139)
#define IDS_P01_STYLE_140   (IDS_P01_STYLE +140)
#define IDS_P01_STYLE_141   (IDS_P01_STYLE +141)
#define IDS_P01_STYLE_142   (IDS_P01_STYLE +142)
#define IDS_P01_STYLE_143   (IDS_P01_STYLE +143)
#define IDS_P01_STYLE_144   (IDS_P01_STYLE +144)
#define IDS_P01_STYLE_145   (IDS_P01_STYLE +145)
#define IDS_P01_STYLE_146   (IDS_P01_STYLE +146)
#define IDS_P01_STYLE_147   (IDS_P01_STYLE +147)
#define IDS_P01_STYLE_148   (IDS_P01_STYLE +148)
#define IDS_P01_STYLE_149   (IDS_P01_STYLE +149)
#define IDS_P01_STYLE_150   (IDS_P01_STYLE +150)
#define IDS_P01_STYLE_151   (IDS_P01_STYLE +151)
#define IDS_P01_STYLE_152   (IDS_P01_STYLE +152)
#define IDS_P01_STYLE_153   (IDS_P01_STYLE +153)
#define IDS_P01_STYLE_154   (IDS_P01_STYLE +154)
#define IDS_P01_STYLE_155   (IDS_P01_STYLE +155)
#define IDS_P01_STYLE_156   (IDS_P01_STYLE +156)
#define IDS_P01_STYLE_157   (IDS_P01_STYLE +157)
#define IDS_P01_STYLE_158   (IDS_P01_STYLE +158)
#define IDS_P01_STYLE_159   (IDS_P01_STYLE +159)
#define IDS_P01_STYLE_160   (IDS_P01_STYLE +160)
#define IDS_P01_STYLE_161   (IDS_P01_STYLE +161)
#define IDS_P01_STYLE_162   (IDS_P01_STYLE +162)
#define IDS_P01_STYLE_163   (IDS_P01_STYLE +163)
#define IDS_P01_STYLE_164   (IDS_P01_STYLE +164)
#define IDS_P01_STYLE_165   (IDS_P01_STYLE +165)
#define IDS_P01_STYLE_166   (IDS_P01_STYLE +166)
#define IDS_P01_STYLE_167   (IDS_P01_STYLE +167)
#define IDS_P01_STYLE_168   (IDS_P01_STYLE +168)
#define IDS_P01_STYLE_169   (IDS_P01_STYLE +169)
#define IDS_P01_STYLE_170   (IDS_P01_STYLE +170)
#define IDS_P01_STYLE_171   (IDS_P01_STYLE +171)
#define IDS_P01_STYLE_172   (IDS_P01_STYLE +172)
#define IDS_P01_STYLE_173   (IDS_P01_STYLE +173)
#define IDS_P01_STYLE_174   (IDS_P01_STYLE +174)
#define IDS_P01_STYLE_175   (IDS_P01_STYLE +175)
#define IDS_P01_STYLE_176   (IDS_P01_STYLE +176)
#define IDS_P01_STYLE_177   (IDS_P01_STYLE +177)
#define IDS_P01_STYLE_178   (IDS_P01_STYLE +178)
#define IDS_P01_STYLE_179   (IDS_P01_STYLE +179)
#define IDS_P01_STYLE_180   (IDS_P01_STYLE +180)
#define IDS_P01_STYLE_181   (IDS_P01_STYLE +181)
#define IDS_P01_STYLE_182   (IDS_P01_STYLE +182)
#define IDS_P01_STYLE_183   (IDS_P01_STYLE +183)
#define IDS_P01_STYLE_184   (IDS_P01_STYLE +184)
#define IDS_P01_STYLE_185   (IDS_P01_STYLE +185)
#define IDS_P01_STYLE_186   (IDS_P01_STYLE +186)
#define IDS_P01_STYLE_187   (IDS_P01_STYLE +187)
#define IDS_P01_STYLE_188   (IDS_P01_STYLE +188)
#define IDS_P01_STYLE_189   (IDS_P01_STYLE +189)
#define IDS_P01_STYLE_190   (IDS_P01_STYLE +190)
#define IDS_P01_STYLE_191   (IDS_P01_STYLE +191)
#define IDS_P01_STYLE_192   (IDS_P01_STYLE +192)
#define IDS_P01_STYLE_193   (IDS_P01_STYLE +193)
#define IDS_P01_STYLE_194   (IDS_P01_STYLE +194)
#define IDS_P01_STYLE_195   (IDS_P01_STYLE +195)
#define IDS_P01_STYLE_196   (IDS_P01_STYLE +196)
#define IDS_P01_STYLE_197   (IDS_P01_STYLE +197)    // MDC dialog display style  MDC_CPM_INTVL_TIME
#define IDS_P01_STYLE_198   (IDS_P01_STYLE +198)    // MDC dialog display style  MDC_CPM_WAIT_TIME
#define IDS_P01_STYLE_199   (IDS_P01_STYLE +199)    // MDC dialog display style  MDC_EPT_SND_TIME
#define IDS_P01_STYLE_200   (IDS_P01_STYLE +200)    // MDC dialog display style  MDC_EPT_RCV_TIME
#define IDS_P01_STYLE_201   (IDS_P01_STYLE +201)
#define IDS_P01_STYLE_202   (IDS_P01_STYLE +202)
#define IDS_P01_STYLE_203   (IDS_P01_STYLE +203)
#define IDS_P01_STYLE_204   (IDS_P01_STYLE +204)
#define IDS_P01_STYLE_205   (IDS_P01_STYLE +205)
#define IDS_P01_STYLE_206   (IDS_P01_STYLE +206)
#define IDS_P01_STYLE_207   (IDS_P01_STYLE +207)
#define IDS_P01_STYLE_208   (IDS_P01_STYLE +208)
#define IDS_P01_STYLE_209   (IDS_P01_STYLE +209)
#define IDS_P01_STYLE_210   (IDS_P01_STYLE +210)
#define IDS_P01_STYLE_211   (IDS_P01_STYLE +211)
#define IDS_P01_STYLE_212   (IDS_P01_STYLE +212)
#define IDS_P01_STYLE_213   (IDS_P01_STYLE +213)
#define IDS_P01_STYLE_214   (IDS_P01_STYLE +214)
#define IDS_P01_STYLE_215   (IDS_P01_STYLE +215)
#define IDS_P01_STYLE_216   (IDS_P01_STYLE +216)
#define IDS_P01_STYLE_217   (IDS_P01_STYLE +217)
#define IDS_P01_STYLE_218   (IDS_P01_STYLE +218)
#define IDS_P01_STYLE_219   (IDS_P01_STYLE +219)
#define IDS_P01_STYLE_220   (IDS_P01_STYLE +220)
#define IDS_P01_STYLE_221   (IDS_P01_STYLE +221)
#define IDS_P01_STYLE_222   (IDS_P01_STYLE +222)
#define IDS_P01_STYLE_223   (IDS_P01_STYLE +223)
#define IDS_P01_STYLE_224   (IDS_P01_STYLE +224)
#define IDS_P01_STYLE_225   (IDS_P01_STYLE +225)
#define IDS_P01_STYLE_226   (IDS_P01_STYLE +226)
#define IDS_P01_STYLE_227   (IDS_P01_STYLE +227)
#define IDS_P01_STYLE_228   (IDS_P01_STYLE +228)
#define IDS_P01_STYLE_229   (IDS_P01_STYLE +229)
#define IDS_P01_STYLE_230   (IDS_P01_STYLE +230)
#define IDS_P01_STYLE_231   (IDS_P01_STYLE +231)
#define IDS_P01_STYLE_232   (IDS_P01_STYLE +232)
#define IDS_P01_STYLE_233   (IDS_P01_STYLE +233)
#define IDS_P01_STYLE_234   (IDS_P01_STYLE +234)
#define IDS_P01_STYLE_235   (IDS_P01_STYLE +235)
#define IDS_P01_STYLE_236   (IDS_P01_STYLE +236)
#define IDS_P01_STYLE_237   (IDS_P01_STYLE +237)
#define IDS_P01_STYLE_238   (IDS_P01_STYLE +238)
#define IDS_P01_STYLE_239   (IDS_P01_STYLE +239)
#define IDS_P01_STYLE_240   (IDS_P01_STYLE +240)
#define IDS_P01_STYLE_241   (IDS_P01_STYLE +241)
#define IDS_P01_STYLE_242   (IDS_P01_STYLE +242)
#define IDS_P01_STYLE_243   (IDS_P01_STYLE +243)
#define IDS_P01_STYLE_244   (IDS_P01_STYLE +244)
#define IDS_P01_STYLE_245   (IDS_P01_STYLE +245)
#define IDS_P01_STYLE_246   (IDS_P01_STYLE +246)
#define IDS_P01_STYLE_247   (IDS_P01_STYLE +247)
#define IDS_P01_STYLE_248   (IDS_P01_STYLE +248)
#define IDS_P01_STYLE_249   (IDS_P01_STYLE +249)
#define IDS_P01_STYLE_250   (IDS_P01_STYLE +250)
#define IDS_P01_STYLE_251   (IDS_P01_STYLE +251)
#define IDS_P01_STYLE_252   (IDS_P01_STYLE +252)
#define IDS_P01_STYLE_253   (IDS_P01_STYLE +253)
#define IDS_P01_STYLE_254   (IDS_P01_STYLE +254)
#define IDS_P01_STYLE_255   (IDS_P01_STYLE +255)
#define IDS_P01_STYLE_256   (IDS_P01_STYLE +256)
#define IDS_P01_STYLE_257   (IDS_P01_STYLE +257)
#define IDS_P01_STYLE_258   (IDS_P01_STYLE +258)
#define IDS_P01_STYLE_259   (IDS_P01_STYLE +259)
#define IDS_P01_STYLE_260   (IDS_P01_STYLE +260)
#define IDS_P01_STYLE_261   (IDS_P01_STYLE +261)
#define IDS_P01_STYLE_262   (IDS_P01_STYLE +262)
#define IDS_P01_STYLE_263   (IDS_P01_STYLE +263)
#define IDS_P01_STYLE_264   (IDS_P01_STYLE +264)
#define IDS_P01_STYLE_265   (IDS_P01_STYLE +265)
#define IDS_P01_STYLE_266   (IDS_P01_STYLE +266)
#define IDS_P01_STYLE_267   (IDS_P01_STYLE +267)
#define IDS_P01_STYLE_268   (IDS_P01_STYLE +268)
#define IDS_P01_STYLE_269   (IDS_P01_STYLE +269)

#define IDS_P01_STYLE_270   (IDS_P01_STYLE +270)    /* V3.3 */
#define IDS_P01_STYLE_271   (IDS_P01_STYLE +271)    /* V3.3 */
#define IDS_P01_STYLE_272   (IDS_P01_STYLE +272)    /* V3.3 */
#define IDS_P01_STYLE_273   (IDS_P01_STYLE +273)    /* V3.3 */
#define IDS_P01_STYLE_274   (IDS_P01_STYLE +274)    /* V3.3 */
#define IDS_P01_STYLE_275   (IDS_P01_STYLE +275)    /* V3.3 */
#define IDS_P01_STYLE_276   (IDS_P01_STYLE +276)    /* V3.3 */
#define IDS_P01_STYLE_277   (IDS_P01_STYLE +277)    /* V3.4 */
#define IDS_P01_STYLE_278   (IDS_P01_STYLE +278)    /* V3.4 */

/* #define IDS_P01_TARE_ERR    (IDS_P01_STYLE +279) *//* NCR2172 */

/*** NCR2172 ***/
#define IDS_P01_STYLE_279   (IDS_P01_STYLE +279)    /* Not Uesd */
#define IDS_P01_STYLE_280   (IDS_P01_STYLE +280)    /* Not Uesd */
#define IDS_P01_STYLE_281   (IDS_P01_STYLE +281)    /* Not Uesd */
#define IDS_P01_STYLE_282   (IDS_P01_STYLE +282)    /* Not Uesd */
#define IDS_P01_STYLE_283   (IDS_P01_STYLE +283)    /* Not Uesd */
#define IDS_P01_STYLE_284   (IDS_P01_STYLE +284)    /* Not Uesd */
#define IDS_P01_STYLE_285   (IDS_P01_STYLE +285)    /* Not Uesd */
#define IDS_P01_STYLE_286   (IDS_P01_STYLE +286)    /* Not Uesd */
#define IDS_P01_STYLE_287   (IDS_P01_STYLE +287)    /* Not Uesd */
#define IDS_P01_STYLE_288   (IDS_P01_STYLE +288)    /* Not Uesd */
#define IDS_P01_STYLE_289   (IDS_P01_STYLE +289)    /* Not Uesd */
#define IDS_P01_STYLE_290   (IDS_P01_STYLE +290)    /* Not Uesd */
#define IDS_P01_STYLE_291   (IDS_P01_STYLE +291)    /* Not Uesd */
#define IDS_P01_STYLE_292   (IDS_P01_STYLE +292)    /* Not Uesd */
#define IDS_P01_STYLE_293   (IDS_P01_STYLE +293)    /* Not Uesd */
#define IDS_P01_STYLE_294   (IDS_P01_STYLE +294)    /* Not Uesd */

#define IDS_P01_STYLE_295   (IDS_P01_STYLE +295)
#define IDS_P01_STYLE_296   (IDS_P01_STYLE +296)
#define IDS_P01_STYLE_297   (IDS_P01_STYLE +297)
#define IDS_P01_STYLE_298   (IDS_P01_STYLE +298)
#define IDS_P01_STYLE_299   (IDS_P01_STYLE +299)
#define IDS_P01_STYLE_300   (IDS_P01_STYLE +300)
#define IDS_P01_STYLE_301   (IDS_P01_STYLE +301)
#define IDS_P01_STYLE_302   (IDS_P01_STYLE +302)
#define IDS_P01_STYLE_303   (IDS_P01_STYLE +303)
#define IDS_P01_STYLE_304   (IDS_P01_STYLE +304)
#define IDS_P01_STYLE_305   (IDS_P01_STYLE +305)
#define IDS_P01_STYLE_306   (IDS_P01_STYLE +306)
#define IDS_P01_STYLE_307   (IDS_P01_STYLE +307)
#define IDS_P01_STYLE_308   (IDS_P01_STYLE +308)
#define IDS_P01_STYLE_309   (IDS_P01_STYLE +309)
#define IDS_P01_STYLE_310   (IDS_P01_STYLE +310)
#define IDS_P01_STYLE_311   (IDS_P01_STYLE +311)
#define IDS_P01_STYLE_312   (IDS_P01_STYLE +312)
#define IDS_P01_STYLE_313   (IDS_P01_STYLE +313)
#define IDS_P01_STYLE_314   (IDS_P01_STYLE +314)
#define IDS_P01_STYLE_315   (IDS_P01_STYLE +315)
#define IDS_P01_STYLE_316   (IDS_P01_STYLE +316)
#define IDS_P01_STYLE_317   (IDS_P01_STYLE +317)
#define IDS_P01_STYLE_318   (IDS_P01_STYLE +318)
#define IDS_P01_STYLE_319   (IDS_P01_STYLE +319)
#define IDS_P01_STYLE_320   (IDS_P01_STYLE +320)
#define IDS_P01_STYLE_321   (IDS_P01_STYLE +321)
#define IDS_P01_STYLE_322   (IDS_P01_STYLE +322)
#define IDS_P01_STYLE_331   (IDS_P01_STYLE +331)
#define IDS_P01_STYLE_332   (IDS_P01_STYLE +332)
#define IDS_P01_STYLE_333   (IDS_P01_STYLE +333)
#define IDS_P01_STYLE_337   (IDS_P01_STYLE +337)
#define IDS_P01_STYLE_338   (IDS_P01_STYLE +338)
#define IDS_P01_STYLE_339   (IDS_P01_STYLE +339)
#define IDS_P01_STYLE_340   (IDS_P01_STYLE +340)
#define IDS_P01_STYLE_341   (IDS_P01_STYLE +341)
#define IDS_P01_STYLE_342   (IDS_P01_STYLE +342)
#define IDS_P01_STYLE_343   (IDS_P01_STYLE +343)
#define IDS_P01_STYLE_344   (IDS_P01_STYLE +344)
#define IDS_P01_STYLE_345   (IDS_P01_STYLE +345)
#define IDS_P01_STYLE_346   (IDS_P01_STYLE +346)
#define IDS_P01_STYLE_347   (IDS_P01_STYLE +347)
#define IDS_P01_STYLE_348   (IDS_P01_STYLE +348)
#define IDS_P01_STYLE_349   (IDS_P01_STYLE +349)
#define IDS_P01_STYLE_350   (IDS_P01_STYLE +350)
#define IDS_P01_STYLE_351   (IDS_P01_STYLE +351)
#define IDS_P01_STYLE_352   (IDS_P01_STYLE +352)

#define IDS_P01_STYLE_353   (IDS_P01_STYLE +353)    /* ### MOD Saratoga (not used --> used)(052500) */
#define IDS_P01_STYLE_354   (IDS_P01_STYLE +354)    /* ### MOD Saratoga (not used --> used)(052500) */
#define IDS_P01_STYLE_355   (IDS_P01_STYLE +355)    /* Not Uesd */
#define IDS_P01_STYLE_356   (IDS_P01_STYLE +356)    /* ### MOD Saratoga (not used --> used)(052500) */
#define IDS_P01_STYLE_357   (IDS_P01_STYLE +357)    /* ### MOD Saratoga (not used --> used)(052500) */
#define IDS_P01_STYLE_358   (IDS_P01_STYLE +358)    /* ### MOD Saratoga (not used --> used)(052500) */
#define IDS_P01_STYLE_359   (IDS_P01_STYLE +359)    /* Not Uesd */
#define IDS_P01_STYLE_360   (IDS_P01_STYLE +360)    /* Not Uesd */

#define IDS_P01_STYLE_361   (IDS_P01_STYLE +361)
#define IDS_P01_STYLE_362   (IDS_P01_STYLE +362)    /* Gift Card */
#define IDS_P01_STYLE_363   (IDS_P01_STYLE +363)
#define IDS_P01_STYLE_364   (IDS_P01_STYLE +364)

/*** Saratoga ***/
#define IDS_P01_STYLE_365   (IDS_P01_STYLE +365)    /* No. of Terminal */
#define IDS_P01_STYLE_367   (IDS_P01_STYLE +367)    /* Decimal Point   */
#define IDS_P01_STYLE_368   (IDS_P01_STYLE +368)    /* Back-up System  */
#define IDS_P01_STYLE_369   (IDS_P01_STYLE +369)    /* Rounding */
#define IDS_P01_STYLE_370   (IDS_P01_STYLE +370)    /* Rounding */
/* ### ADD Saratoga (052500) */
#define IDS_P01_STYLE_371   (IDS_P01_STYLE +371)    /* UPC Coupon      */
#define IDS_P01_STYLE_373   (IDS_P01_STYLE +373)    /* UPC Coupon      */
#define IDS_P01_STYLE_374   (IDS_P01_STYLE +374)    /* UPC Coupon      */
#define IDS_P01_STYLE_375   (IDS_P01_STYLE +375)    /* UPC Coupon      */
#define IDS_P01_STYLE_376   (IDS_P01_STYLE +376)    /* UPC Coupon      */
//SR# 131
#define IDS_P01_STYLE_377	(IDS_P01_STYLE +377)	//Account Security
#define IDS_P01_STYLE_378	(IDS_P01_STYLE +378)	//Account # Masking

//SR# 18
#define IDS_P01_STYLE_379	(IDS_P01_STYLE +379)	//Check no purchase

#define IDS_P01_STYLE_380	(IDS_P01_STYLE +380)	//Double Coupon 1
#define IDS_P01_STYLE_381	(IDS_P01_STYLE +381)	//Double Coupon 2
#define IDS_P01_STYLE_382	(IDS_P01_STYLE +382)	//Triple Coupon 1
#define IDS_P01_STYLE_383	(IDS_P01_STYLE +383)	//Triple Coupon 2

#define IDS_P01_STYLE_384	(IDS_P01_STYLE +384)	//HALO Override SR143
#define IDS_P01_STYLE_385	(IDS_P01_STYLE +385)	//Tender 12
#define IDS_P01_STYLE_386	(IDS_P01_STYLE +386)	//Tender 12
#define IDS_P01_STYLE_387	(IDS_P01_STYLE +387)	//Tender 12
#define IDS_P01_STYLE_388	(IDS_P01_STYLE +388)	//Tender 12
#define IDS_P01_STYLE_389	(IDS_P01_STYLE +389)	//Tender 12
#define IDS_P01_STYLE_390	(IDS_P01_STYLE +390)	//Tender 13
#define IDS_P01_STYLE_391	(IDS_P01_STYLE +391)	//Tender 13
#define IDS_P01_STYLE_392	(IDS_P01_STYLE +392)	//Tender 13
#define IDS_P01_STYLE_393	(IDS_P01_STYLE +393)	//Tender 13
#define IDS_P01_STYLE_394	(IDS_P01_STYLE +394)	//Tender 13
#define IDS_P01_STYLE_395	(IDS_P01_STYLE +395)	//Tender 14
#define IDS_P01_STYLE_396	(IDS_P01_STYLE +396)	//Tender 14
#define IDS_P01_STYLE_397	(IDS_P01_STYLE +397)	//Tender 14
#define IDS_P01_STYLE_398	(IDS_P01_STYLE +398)	//Tender 14
#define IDS_P01_STYLE_399	(IDS_P01_STYLE +399)	//Tender 14
#define IDS_P01_STYLE_400	(IDS_P01_STYLE +400)	//Tender 15
#define IDS_P01_STYLE_401	(IDS_P01_STYLE +401)	//Tender 15
#define IDS_P01_STYLE_402	(IDS_P01_STYLE +402)	//Tender 15
#define IDS_P01_STYLE_403	(IDS_P01_STYLE +403)	//Tender 15
#define IDS_P01_STYLE_404	(IDS_P01_STYLE +404)	//Tender 15
#define IDS_P01_STYLE_405	(IDS_P01_STYLE +405)	//Tender 16
#define IDS_P01_STYLE_406	(IDS_P01_STYLE +406)	//Tender 16
#define IDS_P01_STYLE_407	(IDS_P01_STYLE +407)	//Tender 16
#define IDS_P01_STYLE_408	(IDS_P01_STYLE +408)	//Tender 16
#define IDS_P01_STYLE_409	(IDS_P01_STYLE +409)	//Tender 16
#define IDS_P01_STYLE_410	(IDS_P01_STYLE +410)	//Tender 17
#define IDS_P01_STYLE_411	(IDS_P01_STYLE +411)	//Tender 17
#define IDS_P01_STYLE_412	(IDS_P01_STYLE +412)	//Tender 17
#define IDS_P01_STYLE_413	(IDS_P01_STYLE +413)	//Tender 17
#define IDS_P01_STYLE_414	(IDS_P01_STYLE +414)	//Tender 17
#define IDS_P01_STYLE_415	(IDS_P01_STYLE +415)	//Tender 18
#define IDS_P01_STYLE_416	(IDS_P01_STYLE +416)	//Tender 18
#define IDS_P01_STYLE_417	(IDS_P01_STYLE +417)	//Tender 18
#define IDS_P01_STYLE_418	(IDS_P01_STYLE +418)	//Tender 18
#define IDS_P01_STYLE_419	(IDS_P01_STYLE +419)	//Tender 18
#define IDS_P01_STYLE_420	(IDS_P01_STYLE +420)	//Tender 19
#define IDS_P01_STYLE_421	(IDS_P01_STYLE +421)	//Tender 19
#define IDS_P01_STYLE_422	(IDS_P01_STYLE +422)	//Tender 19
#define IDS_P01_STYLE_423	(IDS_P01_STYLE +423)	//Tender 19
#define IDS_P01_STYLE_424	(IDS_P01_STYLE +424)	//Tender 19
#define IDS_P01_STYLE_425	(IDS_P01_STYLE +425)	//Tender 20
#define IDS_P01_STYLE_426	(IDS_P01_STYLE +426)	//Tender 20
#define IDS_P01_STYLE_427	(IDS_P01_STYLE +427)	//Tender 20
#define IDS_P01_STYLE_428	(IDS_P01_STYLE +428)	//Tender 20
#define IDS_P01_STYLE_429	(IDS_P01_STYLE +429)	//Tender 20
#define IDS_P01_STYLE_430	(IDS_P01_STYLE +430)	//Financial Report
#define IDS_P01_STYLE_431	(IDS_P01_STYLE +431)	//Financial Report
#define IDS_P01_STYLE_432	(IDS_P01_STYLE +432)	//Operator Report
#define IDS_P01_STYLE_433	(IDS_P01_STYLE +433)	//Operator Report
#define IDS_P01_STYLE_434	(IDS_P01_STYLE +434)	//Financial Report
#define IDS_P01_STYLE_435	(IDS_P01_STYLE +435)	//Financial Report
#define IDS_P01_STYLE_436	(IDS_P01_STYLE +436)	//Operator Report
#define IDS_P01_STYLE_437	(IDS_P01_STYLE +437)	//Operator Report
#define IDS_P01_STYLE_438	(IDS_P01_STYLE +438)	//Operator Report
#define IDS_P01_STYLE_439	(IDS_P01_STYLE +439)	//Operator Report
#define IDS_P01_STYLE_440	(IDS_P01_STYLE +440)	//Operator Report
#define IDS_P01_STYLE_441	(IDS_P01_STYLE +441)	//Operator Report
#define IDS_P01_STYLE_442	(IDS_P01_STYLE +442)	//EPT# Masking Totals
#define IDS_P01_STYLE_443	(IDS_P01_STYLE +443)	//Account # Masking Totals 12-20
#define IDS_P01_STYLE_444	(IDS_P01_STYLE +444)	//Account # Masking Totals 12-20
#define IDS_P01_STYLE_445	(IDS_P01_STYLE +445)	//Auto Combinational Coupons

//additions for Release 2.1
#define IDS_P01_STYLE_446	(IDS_P01_STYLE +446)	//Operator Report
#define IDS_P01_STYLE_447	(IDS_P01_STYLE +447)	//Operator Report
#define IDS_P01_STYLE_448	(IDS_P01_STYLE +448)	//Operator Report
#define IDS_P01_STYLE_449	(IDS_P01_STYLE +449)	//Operator Report
#define IDS_P01_STYLE_450	(IDS_P01_STYLE +450)	//Operator Report

#define IDS_P01_STYLE_451   (IDS_P01_STYLE +451)  /* Modified Item Discount Key (ID #3) */
#define IDS_P01_STYLE_452   (IDS_P01_STYLE +452)  /* Modified Item Discount Key (ID #3) */
#define IDS_P01_STYLE_453   (IDS_P01_STYLE +453)  /* Modified Item Discount Key (ID #3) */
#define IDS_P01_STYLE_454   (IDS_P01_STYLE +454)  /* Modified Item Discount Key (ID #4) */
#define IDS_P01_STYLE_455   (IDS_P01_STYLE +455)  /* Modified Item Discount Key (ID #4) */
#define IDS_P01_STYLE_456   (IDS_P01_STYLE +456)  /* Modified Item Discount Key (ID #4) */
#define IDS_P01_STYLE_457   (IDS_P01_STYLE +457)  /* Modified Item Discount Key (ID #5) */
#define IDS_P01_STYLE_458   (IDS_P01_STYLE +458)  /* Modified Item Discount Key (ID #5) */
#define IDS_P01_STYLE_459   (IDS_P01_STYLE +459)  /* Modified Item Discount Key (ID #5) */
#define IDS_P01_STYLE_460   (IDS_P01_STYLE +460)  /* Modified Item Discount Key (ID #6) */
#define IDS_P01_STYLE_461   (IDS_P01_STYLE +461)  /* Modified Item Discount Key (ID #6) */
#define IDS_P01_STYLE_462   (IDS_P01_STYLE +462)  /* Modified Item Discount Key (ID #6) */

#define IDS_P01_STYLE_463	 (IDS_P01_STYLE +463)	//Operator Reports (for bonus totals)
#define IDS_P01_STYLE_464	 (IDS_P01_STYLE +464)	//Operator Reports (for bonus totals)
#define IDS_P01_STYLE_465	 (IDS_P01_STYLE +465)	//Operator Reports (for bonus totals)
#define IDS_P01_STYLE_466	 (IDS_P01_STYLE +466)	//Operator Reports (for bonus totals), MDC_RPTCAS45_ADR, IDS_P01_ADDR_466
#define IDS_P01_STYLE_467	 (IDS_P01_STYLE +467)	//Financial Reports (for bonus totals)
#define IDS_P01_STYLE_468	 (IDS_P01_STYLE +468)	//Financial Reports (for bonus totals)
#define IDS_P01_STYLE_469	 (IDS_P01_STYLE +469)	//Financial Reports (for bonus totals)
#define IDS_P01_STYLE_470	 (IDS_P01_STYLE +470)	//Financial Reports (for bonus totals)


#define IDS_P01_STYLE_471	 (IDS_P01_STYLE +471)	//Financial Reports (for bonus totals)
#define IDS_P01_STYLE_472	 (IDS_P01_STYLE +472)	//Financial Reports (for bonus totals)
#define IDS_P01_STYLE_473	 (IDS_P01_STYLE +473)	//Financial Reports (for bonus totals)
#define IDS_P01_STYLE_474	 (IDS_P01_STYLE +474)	//Financial Reports (for bonus totals)
#define IDS_P01_STYLE_475	 (IDS_P01_STYLE +475)	//order of the reciepts for merchant and customer copy

#define IDS_P01_STYLE_477	 (IDS_P01_STYLE +477)	//Auto Sign Out  MDC_AUTO_SIGNOUT_ADR
//#define IDS_P01_STYLE_478	 (IDS_P01_STYLE +477)	//Auto Sign Out, second nibble used for a byte value.
#define IDS_P01_STYLE_479	 (IDS_P01_STYLE +479)	//Employee ID masking MDC_EMPLOYEE_ID_ADR . MDC_PARAM_2_BIT_A
#define IDS_P01_STYLE_481	 (IDS_P01_STYLE +481)	//Open Drawer Audible Delay, 0 no limit on delay and no audible, otherwise number seconds before audible begins
//#define IDS_P01_STYLE_482	 (IDS_P01_STYLE +481)	//Open Drawer Audible, second nibble used for a byte value.
#define IDS_P01_STYLE_483	 (IDS_P01_STYLE +483)	//Tender key #1, MDC_TEND15_ADR, IDS_P01_ADDR_483
#define IDS_P01_STYLE_484	 (IDS_P01_STYLE +484)	//Tender key #2, MDC_TEND25_ADR, IDS_P01_ADDR_484
#define IDS_P01_STYLE_485	 (IDS_P01_STYLE +485)	//Tender key #3, MDC_TEND35_ADR, IDS_P01_ADDR_485
#define IDS_P01_STYLE_486	 (IDS_P01_STYLE +486)	//Tender key #4, MDC_TEND45_ADR, IDS_P01_ADDR_486
#define IDS_P01_STYLE_487	 (IDS_P01_STYLE +487)	//Tender key #5, MDC_TEND55_ADR, IDS_P01_ADDR_487
#define IDS_P01_STYLE_488	 (IDS_P01_STYLE +488)	//Tender key #6, MDC_TEND65_ADR, IDS_P01_ADDR_488
#define IDS_P01_STYLE_489	 (IDS_P01_STYLE +489)	//Tender key #7, MDC_TEND75_ADR, IDS_P01_ADDR_489
#define IDS_P01_STYLE_490	 (IDS_P01_STYLE +490)	//Tender key #8, MDC_TEND85_ADR, IDS_P01_ADDR_490
#define IDS_P01_STYLE_491	 (IDS_P01_STYLE +491)	//Tender key #9, MDC_TEND95_ADR, IDS_P01_ADDR_491
#define IDS_P01_STYLE_492	 (IDS_P01_STYLE +492)	//Tender key #10, MDC_TEND105_ADR, IDS_P01_ADDR_492
#define IDS_P01_STYLE_493	 (IDS_P01_STYLE +493)	//Tender key #11, MDC_TEND115_ADR, IDS_P01_ADDR_493
#define IDS_P01_STYLE_494	 (IDS_P01_STYLE +494)	//Tender key #12, MDC_TEND1205_ADR, IDS_P01_ADDR_494
#define IDS_P01_STYLE_495	 (IDS_P01_STYLE +495)	//Tender key #13, MDC_TEND1305_ADR, IDS_P01_ADDR_495
#define IDS_P01_STYLE_496	 (IDS_P01_STYLE +496)	//Tender key #14, MDC_TEND1505_ADR, IDS_P01_ADDR_496
#define IDS_P01_STYLE_497	 (IDS_P01_STYLE +497)	//Tender key #15, MDC_TEND1505_ADR, IDS_P01_ADDR_497
#define IDS_P01_STYLE_498	 (IDS_P01_STYLE +498)	//Tender key #16, MDC_TEND1605_ADR, IDS_P01_ADDR_498
#define IDS_P01_STYLE_499	 (IDS_P01_STYLE +499)	//Tender key #17, MDC_TEND1705_ADR, IDS_P01_ADDR_499
#define IDS_P01_STYLE_500	 (IDS_P01_STYLE +500)	//Tender key #18, MDC_TEND1805_ADR, IDS_P01_ADDR_500
#define IDS_P01_STYLE_501	 (IDS_P01_STYLE +501)	//Tender key #19, MDC_TEND1905_ADR, IDS_P01_ADDR_501
#define IDS_P01_STYLE_502	 (IDS_P01_STYLE +502)	//Tender key #20, MDC_TEND2005_ADR, IDS_P01_ADDR_502
#define IDS_P01_STYLE_503	 (IDS_P01_STYLE +503)	//Item Discount key #1, page 5, MDC_MODID15_ADR, IDS_P01_ADDR_503
#define IDS_P01_STYLE_504	 (IDS_P01_STYLE +504)	//Item Discount key #2, page 5, MDC_MODID25_ADR, IDS_P01_ADDR_504
#define IDS_P01_STYLE_505	 (IDS_P01_STYLE +505)	//Item Discount key #3, page 5, MDC_MODID35_ADR, IDS_P01_ADDR_505
#define IDS_P01_STYLE_506	 (IDS_P01_STYLE +506)	//Item Discount key #4, page 5, MDC_MODID45_ADR, IDS_P01_ADDR_506
#define IDS_P01_STYLE_507	 (IDS_P01_STYLE +507)	//Item Discount key #5, page 5, MDC_MODID55_ADR, IDS_P01_ADDR_507
#define IDS_P01_STYLE_508	 (IDS_P01_STYLE +508)	//Item Discount key #6, page 5, MDC_MODID65_ADR, IDS_P01_ADDR_508
#define IDS_P01_STYLE_509	 (IDS_P01_STYLE +509)	//Regular Discount key #1, page 5, MDC_RDISC15_ADR, IDS_P01_ADDR_509
#define IDS_P01_STYLE_510	 (IDS_P01_STYLE +510)	//Regular Discount key #2, page 5, MDC_RDISC25_ADR, IDS_P01_ADDR_510
#define IDS_P01_STYLE_511	 (IDS_P01_STYLE +511)	//Regular Discount key #3, page 5, MDC_RDISC35_ADR, IDS_P01_ADDR_511
#define IDS_P01_STYLE_512	 (IDS_P01_STYLE +512)	//Regular Discount key #4, page 5, MDC_RDISC45_ADR, IDS_P01_ADDR_512
#define IDS_P01_STYLE_513	 (IDS_P01_STYLE +513)	//Regular Discount key #5, page 5, MDC_RDISC55_ADR, IDS_P01_ADDR_513
#define IDS_P01_STYLE_514	 (IDS_P01_STYLE +514)	//Regular Discount key #6, page 5, MDC_RDISC65_ADR, IDS_P01_ADDR_514
#define IDS_P01_STYLE_515	 (IDS_P01_STYLE +515)	//Numeric Pause Key #1 and #2 settings, page 1, MDC_PAUSE_1_2_ADR, IDS_P01_ADDR_515
#define IDS_P01_STYLE_516	 (IDS_P01_STYLE +516)	//Numeric Pause Key #3 and #4 settings, page 1, MDC_PAUSE_3_4_ADR, IDS_P01_ADDR_516
#define IDS_P01_STYLE_517	 (IDS_P01_STYLE +517)	//Returns key (option 1 of void), Amtrak/VCS, page 5, MDC_RETURNS_RTN1_ADR, IDS_P01_ADDR_517
#define IDS_P01_STYLE_518	 (IDS_P01_STYLE +518)	//Returns key (option 2 of void), Amtrak/VCS, page 5, MDC_RETURNS_RTN1_ADR, IDS_P01_ADDR_518
#define IDS_P01_STYLE_519	 (IDS_P01_STYLE +519)	//Returns key (option 3 of void), Amtrak/VCS, page 5, MDC_RETURNS_RTN1_ADR, IDS_P01_ADDR_519
#define IDS_P01_STYLE_520	 (IDS_P01_STYLE +520)	//Financial Reports (for returns totals), Amtrak/VCS, MDC_RPTFIN46_ADR, IDS_P01_ADDR_520
#define IDS_P01_STYLE_521	 (IDS_P01_STYLE +521)	//Operator Reports (for returns totals), Amtrak/VCS, MDC_RPTCAS50_ADR, IDS_P01_ADDR_521
#define IDS_P01_STYLE_522	 (IDS_P01_STYLE +522)	//Receipt optons for returns, Amtrak/VCS, MDC_RECEIPT_RTN_ADR, IDS_P01_ADDR_522
#define IDS_P01_STYLE_523	 (IDS_P01_STYLE +523)	//Store and Forward, Amtrak, MDC_SF_ALERT_TIME, IDS_P01_ADDR_523
#define IDS_P01_STYLE_525	 (IDS_P01_STYLE +525)	//Store and Forward, Amtrak, MDC_SF_AUTOOFF_TIME, IDS_P01_ADDR_525
#define IDS_P01_STYLE_527	 (IDS_P01_STYLE +527)	//Store and Forward, Amtrak, MDC_SF_TRANRULE_TIME, IDS_P01_ADDR_527
#define IDS_P01_STYLE_529	 (IDS_P01_STYLE +529)	//Signature Capture Use Rules, VCS, MDC_SIGCAP_USE_RULES, IDS_P01_ADDR_529
#define IDS_P01_STYLE_530	 (IDS_P01_STYLE +530)	//Returns Usage Rules, VCS, MDC_RTN_RECALL_RULES, IDS_P01_ADDR_530
#define IDS_P01_STYLE_531	 (IDS_P01_STYLE +531)	//Control String for MDC_SF_ALERT_TIME, Amtrak, MDC_SF_CNTRL_ALERT, IDS_P01_ADDR_531
#define IDS_P01_STYLE_533	 (IDS_P01_STYLE +533)	//Control String for MDC_SF_AUTOOFF_TIME, Amtrak, MDC_SF_CNTRL_AUTOOFF, IDS_P01_ADDR_533
#define IDS_P01_STYLE_535	 (IDS_P01_STYLE +535)	//Rules for Store/Forward processing, Amtrak, MDC_SF_EXEC_RULES_01, IDS_P01_ADDR_535
#define IDS_P01_STYLE_536	 (IDS_P01_STYLE +536)	//limit on number of Post Receipt key presses allowed, MDC_RECEIPT_POST_LMT, IDS_P01_ADDR_536
#define IDS_P01_STYLE_537	 (IDS_P01_STYLE +537)	//limit on number of Gift Receipt key presses allowed, MDC_RECEIPT_GIFT_LMT, IDS_P01_ADDR_537
#define IDS_P01_STYLE_538	 (IDS_P01_STYLE +538)	//limit on number of Print on Demand reprint Receipt key presses allowed, MDC_RECEIPT_POD_LMT, IDS_P01_ADDR_538

#define IDS_P01_TARE_ERR    (IDS_P01_STYLE +600)
/* ----- MDC Bit Description Definision ----- */

#define IDS_P01_BIT_175_0   (0xAF0 + 0x6000)
#define IDS_P01_BIT_175_1   (0xAF0 + 0x6001)
#define IDS_P01_BIT_175_2   (0xAF0 + 0x6002)
#define IDS_P01_BIT_175_3   (0xAF0 + 0x6003)

#define IDS_P01_BIT_176_0   (0xB00 + 0x6000)
#define IDS_P01_BIT_176_1   (0xB00 + 0x6001)
#define IDS_P01_BIT_176_2   (0xB00 + 0x6002)
#define IDS_P01_BIT_176_3   (0xB00 + 0x6003)

#define IDS_P01_BIT_179_0   (0xb30 + 0x6000)
#define IDS_P01_BIT_179_1   (0xb30 + 0x6001)
#define IDS_P01_BIT_179_2   (0xb30 + 0x6002)
#define IDS_P01_BIT_179_3   (0xb30 + 0x6003)

#define IDS_P01_BIT_197_0   (0xC50 + 0x6000)    // MDC_CPM_INTVL_TIME  MDC dialog title text
#define IDS_P01_BIT_197_1   (0xC50 + 0x6001)    // MDC_CPM_INTVL_TIME  MDC data entry prompt text
#define IDS_P01_BIT_197_2   (0xC50 + 0x6002)    // MDC_CPM_INTVL_TIME  MDC data entry range text
#define IDS_P01_BIT_197_3   (0xC50 + 0x6003)    // MDC_CPM_INTVL_TIME  MDC data entry description

#define IDS_P01_BIT_198_0   (0xC60 + 0x6000)    // MDC_CPM_WAIT_TIME  MDC dialog title text
#define IDS_P01_BIT_198_1   (0xC60 + 0x6001)    // MDC_CPM_WAIT_TIME  MDC data entry prompt text
#define IDS_P01_BIT_198_2   (0xC60 + 0x6002)    // MDC_CPM_WAIT_TIME  MDC data entry range text
#define IDS_P01_BIT_198_3   (0xC60 + 0x6003)    // MDC_CPM_WAIT_TIME  MDC data entry description

#define IDS_P01_BIT_199_0   (0xC70 + 0x6000)    // MDC_EPT_SND_TIME  MDC dialog title text
#define IDS_P01_BIT_199_1   (0xC70 + 0x6001)    // MDC_EPT_SND_TIME  MDC data entry prompt text
#define IDS_P01_BIT_199_2   (0xC70 + 0x6002)    // MDC_EPT_SND_TIME  MDC data entry range text
#define IDS_P01_BIT_199_3   (0xC70 + 0x6003)    // MDC_EPT_SND_TIME  MDC data entry description

#define IDS_P01_BIT_200_0   (0xC80 + 0x6000)    // MDC_EPT_RCV_TIME  MDC dialog title text
#define IDS_P01_BIT_200_1   (0xC80 + 0x6001)    // MDC_EPT_RCV_TIME  MDC data entry prompt text
#define IDS_P01_BIT_200_2   (0xC80 + 0x6002)    // MDC_EPT_RCV_TIME  MDC data entry range text
#define IDS_P01_BIT_200_3   (0xC80 + 0x6003)    // MDC_EPT_RCV_TIME  MDC data entry description

//Auto Sign Out defines for P01 dialog for MDC_AUTO_SIGNOUT_ADR
#define IDS_P01_BIT_476_0	32196	
#define IDS_P01_BIT_476_1	32197
#define IDS_P01_BIT_476_2	32198
#define IDS_P01_BIT_476_3	32199

// Employee Masking defines for P01 dialog for MDC_EMPLOYEE_ID_ADR
// Uses Sub-dialog box Style A and is in Category 2
#define IDS_P01_BIT_479_0	32240	    // Bit D
#define IDS_P01_BIT_479_1	32241       // Bit D
#define IDS_P01_BIT_479_2	32242       // Bit C
#define IDS_P01_BIT_479_3	32243       // Bit C
#define IDS_P01_BIT_479_4	32244	    // Bit B
#define IDS_P01_BIT_479_5	32245       // Bit B
#define IDS_P01_BIT_479_6	32246       // Bit A
#define IDS_P01_BIT_479_7	32247       // Bit A

// Open Drawer Audible defines for P01 dialog for MDC_DRAWEROPEN_TIME
// Uses Sub-dialog box Style K and is in Category 1
#define IDS_P01_BIT_481_0	32200	
#define IDS_P01_BIT_481_1	32201
#define IDS_P01_BIT_481_2	32202
#define IDS_P01_BIT_481_3	32203

// Open Drawer Audible defines for P01 dialog for MDC_SF_ALERT_TIME
// Uses Sub-dialog box Style K and is in Category 3
#define IDS_P01_BIT_523_0	32944	
#define IDS_P01_BIT_523_1	32945
#define IDS_P01_BIT_523_2	32946
#define IDS_P01_BIT_523_3	32947
//#define IDS_P01_BIT_523_8	32952	

// Open Drawer Audible defines for P01 dialog for MDC_SF_AUTOOFF_TIME
// Uses Sub-dialog box Style K and is in Category 3
#define IDS_P01_BIT_525_0	32970	
#define IDS_P01_BIT_525_1	32271
#define IDS_P01_BIT_525_2	32272
#define IDS_P01_BIT_525_3	32273
//#define IDS_P01_BIT_523_8	32978	

// Open Drawer Audible defines for P01 dialog for MDC_SF_TRANRULE_TIME
// Uses Sub-dialog box Style K and is in Category 3
#define IDS_P01_BIT_527_0	32996	
#define IDS_P01_BIT_527_1	32997
#define IDS_P01_BIT_527_2	32998
#define IDS_P01_BIT_527_3	32999

// Store Forward Alert Timer expires control string defines for P01 dialog for MDC_SF_CNTRL_ALERT
// Uses Sub-dialog box Style K and is in Category 3
#define IDS_P01_BIT_531_0	33072	
#define IDS_P01_BIT_531_1	33073
#define IDS_P01_BIT_531_2	33074
#define IDS_P01_BIT_531_3	33075

// Store Forward Auto Off Timer expires control string defines for P01 dialog for MDC_SF_CNTRL_AUTOOFF
// Uses Sub-dialog box Style K and is in Category 3
#define IDS_P01_BIT_533_0	33088	
#define IDS_P01_BIT_533_1	33089
#define IDS_P01_BIT_533_2	33090
#define IDS_P01_BIT_533_3	33091

#define IDS_P01_BIT_536_0   33152    // MDC_RECEIPT_POST_LMT  MDC dialog title text
#define IDS_P01_BIT_536_1   33153    // MDC_RECEIPT_POST_LMT  MDC data entry prompt text
#define IDS_P01_BIT_536_2   33154    // MDC_RECEIPT_POST_LMT  MDC data entry range text
#define IDS_P01_BIT_536_3   33155    // MDC_RECEIPT_POST_LMT  MDC data entry description

#define IDS_P01_BIT_537_0   33168    // MDC_RECEIPT_GIFT_LMT  MDC dialog title text
#define IDS_P01_BIT_537_1   33169    // MDC_RECEIPT_GIFT_LMT  MDC data entry prompt text
#define IDS_P01_BIT_537_2   33170    // MDC_RECEIPT_GIFT_LMT  MDC data entry range text
#define IDS_P01_BIT_537_3   33171    // MDC_RECEIPT_GIFT_LMT  MDC data entry description

#define IDS_P01_BIT_538_0   33184    // MDC_RECEIPT_POD_LMT  MDC dialog title text
#define IDS_P01_BIT_538_1   33185    // MDC_RECEIPT_POD_LMT  MDC data entry prompt text
#define IDS_P01_BIT_538_2   33186    // MDC_RECEIPT_POD_LMT  MDC data entry range text
#define IDS_P01_BIT_538_3   33187    // MDC_RECEIPT_POD_LMT  MDC data entry description

/*
* ===========================================================================
*       Value definition
* ===========================================================================
*/
#define P01_ADDR_MAX    MAX_MDC_SIZE    /* Max Address Size of 1 Byte       */
#define P01_BIT_MAX     8               /* No. of Bits in 1 Byte            */
#define P01_BIT_NO      4               /* No. of Bits in Program Address   */
#define P01_SUBDLG_NO   4               /* No. of Sub-DialogBox Type        */

#define P01_CAN_TAX_ADDR    16          /* Canadian Tax (Address 16)        */
#define P01_CHAR_TIPS_ADDR  25          /* Charge Tips (Address 25)         */
#define P01_DISP_FLY_ADDR1  175         /* Display on the Fly (Address 175) */
#define P01_DISP_FLY_ADDR2  176         /* Display on the Fly (Address 176) */
#define P01_DISP_PRT_ADDR   179         /* Shared Printer Timer (Addr. 179) */
#define P01_VAT_ADDR        259         /* VAT and Service on net (Addr.259), V3.3 */

#define P01_SPIN_STEP   1               /*                                  */
#define P01_EDIT_LEN    2
#define P01_EDIT_LEN3   3
#define P01_EDIT_MIN    0

#define P01_TARE_ADDR   29              /* Address No. of Tare Control      */
#define P01_TARE_MAX    999             /* Default Max. Value of Tare Data  */
#define P01_TARE_CHK    5               /* L.S.D. Value of Tare Data        */

#define P01_TYPE_LEN    32              /* Length of Sub-DialogBox Type     */
#define P01_TYPE_OFFSET 3               /* Offset of Sub-DialogBox Type     */

#define P01_TABSPACE    8

#define P01_BIT_DESC_8  8
#define P01_BIT_DESC_9  9               /* NCR2172*/
#define P01_BIT_DESC_10 10
#define P01_BIT_DESC_16 16

#define P01_2GROUPS     2
#define P01_4GROUPS     4
#define P01_8GROUPS     8
#define P01_16GROUPS    16

#define P01_1BITSHIFT   1
#define P01_2BITSHIFT   2
#define P01_3BITSHIFT   3

#define P01_CHK_ADDR    2               /* Check Odd / Even Address */
#define P01_CHK_BIT     0x01
#define P01_CHK_2BITS   0x03
#define P01_CHK_3BITS   0x07
#define P01_CHK_4BITS   0x0f
#define P01_CHK_255BITS 0xFF			//used for Auto Sign Out 477 (MDC_AUTO_SIGNOUT_ADR), the max is 255 min.

#define P01_CHK_STYLE   0               /* Check Sub-DlgBox Style Address   */
#define P01_CHK_GROUP   1               /* Check Sub-DlgBox Group Address   */

#define P01_CATE_CLOSED 43              /* '+' Category is Closed   */
#define P01_CATE_OPENED 45              /* '-' Category is Opened   */

#define P01_HYPHEN      45              /* '-' DialogBox Style Hypen    */
#define P01_PERIOD      46              /* '.' DialogBox Style Period   */

#define P01_STYLE_A     65              /* 'A' Sub-Dlgbox Style     */
#define P01_STYLE_B     66              /* 'B' Sub-Dlgbox Style     */
#define P01_STYLE_C     67              /* 'C' Sub-Dlgbox Style     */
#define P01_STYLE_D     68              /* 'D' Sub-Dlgbox Style     */
#define P01_STYLE_E     69              /* 'E' Sub-Dlgbox Style     */
#define P01_STYLE_F     70              /* 'F' Sub-Dlgbox Style     */
#define P01_STYLE_G     71              /* 'G' Sub-Dlgbox Style     */
#define P01_STYLE_H     72              /* 'H' Sub-Dlgbox Style     */
#define P01_STYLE_I     73
#define P01_STYLE_J     74
#define P01_STYLE_K     75
#define P01_STYLE_L     76              /* Addr.256 Sub-Dlgbox Style, V3.3 */
#define P01_STYLE_N     77              /* Addr.367 Sub-Dlgbox Style, Saratoga  */

#define P01_STYLE_1     49              /* '1' Sub-DlgBox Group Style   */
#define P01_STYLE_2     50              /* '2' Sub-DlgBox Group Style   */
#define P01_STYLE_3     51              /* '3' Sub-DlgBox Group Style   */
#define P01_STYLE_4     52              /* '4' Sub-DlgBox Group Style   */
#define P01_STYLE_5     53              /* '5' Sub-DlgBox Group Style   */
#define P01_STYLE_6     54              /* '6' Sub-DlgBox Group Style   */
#define P01_STYLE_7     55              /* '7' Sub-DlgBox Group Style   */
#define P01_STYLE_8     56              /* '8' Sub-DlgBox Group Style   */

/*
* ===========================================================================
*       Macro definition
* ===========================================================================
*/
#define P01_GET_LOBIT(x)            (BYTE)(0x0f & (x))
#define P01_GET_HIBIT(x)            (BYTE)((0xf0 & (x)) >> 4)
#define P01_SET_LOBIT(dest, src)    (BYTE)((0xf0 & (dest)) | (0x0f & (src)))
#define P01_SET_HIBIT(dest, src)    (BYTE)((0x0f & (dest)) | ((0x0f & (src)) << 4))
#define P01_GET_DLG_STYLE(char)     (WORD)((char - P01_STYLE_A) + IDD_P01_DLG_A)
#define P01_GET_GRP_DESC(cate, off) (WORD)(((cate - 1) * P01_CATE_OFFSET) + IDS_P01_CATEGO_01 + off)
#define P01_BITDESC_OFFSET(addr)    (WORD)((addr * 16) + 0x6000)

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct  tagP01CATESTAT {     /* Category in ListBox Status   */
    short   nOpen;                      /* (Not) Open Categories        */
    int     cAddr;                      /* No. of Address in Category   */
} P01CATESTAT, *LPP01CATESTAT;

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
VOID    P01InitDlg(HWND);
VOID    P01FinDlg(HWND, WPARAM);
UINT    P01ChkIndex(HWND, LPUINT);
HWND    P01MakeEditDlg(HWND, UINT);
VOID    P01OpenCtgry(HWND, UINT);
int     P01InitSubDlg(HWND);
VOID    P01GetGrpStr(LPWORD);
VOID    P01SetGrpStr(HWND, LPWORD);
int     P01SetRadioDesc(HWND);
WPARAM  P01SetData(HWND, int);
VOID    P01ChkTypeCDE(HWND, BYTE, BYTE, BYTE);
VOID    P01ChkTypeFG(HWND, BYTE, BYTE);
VOID    P01SetBuff(HWND, WORD, WORD, WPARAM, int, BYTE, BYTE);
BOOL    P01ChkTare(HWND);
VOID    P01SpinProc(HWND, WPARAM);
VOID	P01SpinProc475(HWND, WPARAM);
/* ===== End of File (P001.h) ===== */
