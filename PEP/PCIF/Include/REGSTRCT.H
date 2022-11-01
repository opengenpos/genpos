/*=======================================================================*\
\*=======================================================================*\
||  Erectronic  Cash  Register  -  90                                    ||
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
* Title       : REG MODE HEADER FILE                         
* Category    : REG MODE Application Program - NCR 2170 US Hospitality Application      
* Program Name: regstrct.h
* ----------------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
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
* Jan-26-00: 01.00.00 : K.Yanagida  : Saratoga initial
*
*===================================================================================
*===================================================================================
\*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
*   PVCS ENTRY
*    $Revision$
*    $Date$
*    $Author$
*    $Log$
\*-----------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
*       ITEM MODULE interface data (major class)
--------------------------------------------------------------------------*/

#define     CLASS_UIFREGOPEN         1          /* operator open */
#define     CLASS_UIFREGTRANSOPEN    2          /* transaction open */
#define     CLASS_UIFREGSALES       10          /* sales */
#define     CLASS_UIFREGDISC        11          /* discount */
#define     CLASS_UIFREGTOTAL       12          /* total key */
#define     CLASS_UIFREGTENDER      13          /* tender */
#define     CLASS_UIFREGMISC        14          /* misc. transaction */
#define     CLASS_UIFREGMODIFIER    15          /* modifier */
#define     CLASS_UIFREGOTHER       16          /* other */
#define     CLASS_UIFREGPAMTTENDER  17          /* preset amount cash tender */
#define     CLASS_UIFREGCOUPON      18          /* coupon */
#define     CLASS_UIFREGOPECLOSE    50          /* operator close */
#define     CLASS_UIFREGMODE        60          /* mode-in */



/*--------------------------------------------------------------------------
*       ITEM MODULE interface data (minor class)
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
*       OPERATOR OPEN DATA
--------------------------------------------------------------------------*/

#define     CLASS_UICASHIERIN        1          /* cashier sign-in */
#define     CLASS_UIB_IN             2          /* cashier B sign-in */
#define     CLASS_UIWAITERIN        11          /* waiter sign-in */
#define     CLASS_UIWAITERLOCKIN    12          /* waiter lock sign-in */
#define     CLASS_UIBARTENDERIN     21          /* bartender sign-in,   R3.1 */

#define     CLASS_UICASINTIN        31          /* cashier interrupt sign-in, R3.3 */
#define     CLASS_UICASINTB_IN      32          /* cashier interrupt B sign-in, R3.3 */

/*--------------------------------------------------------------------------
*       TRANSACTION OPEN DATA
--------------------------------------------------------------------------*/

#define     CLASS_UINEWCHECK         2          /* newcheck */
#define     CLASS_UIREORDER          3          /* reorder */
#define     CLASS_UIADDCHECK1        4          /* addcheck 1 */
#define     CLASS_UIADDCHECK2        5          /* addcheck 2 */
#define     CLASS_UIADDCHECK3        6          /* addcheck 3 */
#define     CLASS_UIRECALL           7          /* recall */
#define     CLASS_UICASINTOPEN       8          /* cashier interrupt, R3.3 */
#define     CLASS_UICASINTRECALL     9          /* cashier interrupt recall, R3.3 */
#define     CLASS_UINEWKEYSEQ       11          /* New Key Sequence,    V3.3 */
#define     CLASS_UINEWKEYSEQGC     12          /* New Key Sequence,    V3.3 */



/*--------------------------------------------------------------------------
*       SALES DATA
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
#define		CLASS_ITEMORDERDEC		72			/* Order Declaration JHHJ*/
#define		CLASS_ITEMMODIFICATION	73			/* Adjective Modification JHHJ */


/*--------------------------------------------------------------------------
*       DISCOUNT DATA
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

/*--------------------------------------------------------------------------
*       COUPON DATA
--------------------------------------------------------------------------*/

#define     CLASS_UICOMCOUPON        1          /* combination coupon */
#define     CLASS_UICOMCOUPONREPEAT  2          /* coupon repeat */


/*--------------------------------------------------------------------------
*       TOTAL KEY DATA
--------------------------------------------------------------------------*/

#define     CLASS_UITOTAL1           1          /* total #1(subtotal) */
#define     CLASS_UITOTAL2           2          /* total #2(check total) */
#define     CLASS_UITOTAL3           3          /* total #3 */
#define     CLASS_UITOTAL4           4          /* total #4 */
#define     CLASS_UITOTAL5           5          /* total #5 */
#define     CLASS_UITOTAL6           6          /* total #6 */
#define     CLASS_UITOTAL7           7          /* total #7 */
#define     CLASS_UITOTAL8           8          /* total #8 */

#define     CLASS_UISPLIT           11          /* split key,   R3.1 */

#define     CLASS_UICASINT          21          /* cashier interrupt, R3.3 */
#define     CLASS_UIKEYOUT          22          /* sign-out by server lock, V3.3 */


/*---------------------------------------------------------------------------
*       TENDER DATA
--------------------------------------------------------------------------*/

#define     CLASS_UITENDER1          1          /* tender #1 */
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

#define     CLASS_UIFOREIGNTOTAL1   21          /* FC total #1 */
#define     CLASS_UIFOREIGNTOTAL2   22          /* FC total #2 */

#define     CLASS_UIFOREIGNTOTAL3   23          /* FC total #3, Saratoga */
#define     CLASS_UIFOREIGNTOTAL4   24          /* FC total #4, Saratoga */
#define     CLASS_UIFOREIGNTOTAL5   25          /* FC total #5, Saratoga */
#define     CLASS_UIFOREIGNTOTAL6   26          /* FC total #6, Saratoga */
#define     CLASS_UIFOREIGNTOTAL7   27          /* FC total #7, Saratoga */
#define     CLASS_UIFOREIGNTOTAL8   28          /* FC total #8, Saratoga */
#define     CLASS_UIFOREIGN1        31          /* FC tender #1,Saratoga */
#define     CLASS_UIFOREIGN2        32          /* FC tender #2,Saratoga */
#define     CLASS_UIFOREIGN3        33          /* FC tender #3,Saratoga */
#define     CLASS_UIFOREIGN4        34          /* FC tender #4,Saratoga */
#define     CLASS_UIFOREIGN5        35          /* FC tender #5,Saratoga */
#define     CLASS_UIFOREIGN6        36          /* FC tender #6,Saratoga */
#define     CLASS_UIFOREIGN7        37          /* FC tender #7,Saratoga */
#define     CLASS_UIFOREIGN8        38          /* FC tender #8,Saratoga */



/*--------------------------------------------------------------------------
*       MISC. TRANSACTION DATA
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

#define     CLASS_UIFMONEY_IN           18          /* money declaration-in, 2172        */
#define     CLASS_UIFMONEY              19          /* money declaration, 2172           */
#define     CLASS_UIFMONEY_OUT          20          /* money declaration-out, 2172       */
#define     CLASS_UIFMONEY_FOREIGN      21          /* money declaration (foreign), 2172 */

/*--------------------------------------------------------------------------
*       MODIFIER DATA
--------------------------------------------------------------------------*/

#define     CLASS_UIPVOID            1          /* preselect void */
#define     CLASS_UINORECEIPT        2          /* no receipt */
#define     CLASS_UIMENUSHIFT        3          /* menu shift */
#define     CLASS_UITARE             4          /* scale/tare */
#define     CLASS_UITABLENO         11          /* table No */
#define     CLASS_UINOPERSON        12          /* No of person */
#define     CLASS_UILINENO          13          /* line No */
#define     CLASS_UITAXMODIFIER1    21          /* tax modifier #1 */
#define     CLASS_UITAXMODIFIER2    22          /* tax modifier #2 */
#define     CLASS_UITAXMODIFIER3    23          /* tax modifier #3 */
#define       CLASS_UITAXMODIFIER4      24              /* tax modifier #4 */
#define       CLASS_UITAXMODIFIER5      25              /* tax modifier #5 */
#define     CLASS_UITAXMODIFIER6    26              /* tax modifier #6 */
#define     CLASS_UITAXMODIFIER7    27              /* tax modifier #7 */
#define       CLASS_UITAXMODIFIER8      28              /* tax modifier #8 */
#define     CLASS_UITAXMODIFIER9    29              /* tax modifier #9 */
#define     CLASS_UITAXEXEMPT1        30                /* tax exempt #1 */
#define     CLASS_UITAXEXEMPT2        31                /* tax exempt #2 */
#define     CLASS_UIOFFTENDER       32          /* off line tender */
#define     CLASS_UIALPHANAME       33          /* customer name, R3.0 */

#define     CLASS_UISEATNO1         41          /* seat number,   R3.1 */
#define     CLASS_UISEATNO2         42          /* seat number for read, R3.1 */

#define     CLASS_UIADJSHIFT        43          /* adjective shift, 2172 */
#define     CLASS_UIMODNUMBER       44          /* number, 2172 */

/*--------------------------------------------------------------------------
*       OTHER DATA
--------------------------------------------------------------------------*/

#define     CLASS_UIPRINTDEMAND      1          /* print demand */
#define     CLASS_UISLIPFEED         2          /* slip feed */
#define     CLASS_UISLIPRELEASE      3          /* slip release */
#define     CLASS_UIRECEIPTFEED      4          /* receipt feed */
#define     CLASS_UIJOURNALFEED      5          /* journal feed */
#define     CLASS_UICLEAR           11          /* clear */
#define     CLASS_UIEC              12          /* error correct */
#define     CLASS_UICOMPUTATION     21          /* computation */
#define     CLASS_UIORDERDISPLAY    22          /* order No display */
#define     CLASS_UILOWMNEMO        23          /* lower mnemonics display */
#define     CLASS_UISCROLLUP        31       /* scroll-up */
#define     CLASS_UISCROLLDOWN      32       /* scroll-down */
#define     CLASS_UIRIGHTARROW      33       /* right arrow */
#define     CLASS_UILEFTARROW       34       /* left arrow */
#define     CLASS_UIWAIT            35       /* wait */
#define     CLASS_UIPAIDORDERRECALL 36       /* paid order recall */

#define     CLASS_UICURSORVOID      41       /* cursor void,         R3.1 */
#define     CLASS_UITRANSFER        42       /* transfer,            R3.1 */

#define     CLASS_UIFASK            51      /* ask key,         2172 */
#define     CLASS_UIFLABEL          52      /* label entry at number dialog, 2172 */

/*--------------------------------------------------------------------------
*       OPERATOR CLOSE DATA
--------------------------------------------------------------------------*/

#define     CLASS_UICASHIEROUT       1          /* cashier sign-out */
#define     CLASS_UIB_OUT            2          /* cashier B sign-out */
#define     CLASS_UICASRELONFIN      3          /* release on finalize (cashier) */
#define     CLASS_UIWAITEROUT       11          /* waiter sign-out */
#define     CLASS_UIWAITERLOCKOUT   12          /* waiter lock sign-out */
#define     CLASS_UIWAIKEYRELONFIN  13          /* release finalize (waiter sign-in by waiter lock) */
#define     CLASS_UIWAIREMOVEKEY    14          /* release finalize (waiter sign-in by waiter key) */
#define     CLASS_UIMODELOCK        21          /* mode lock key */

#define     CLASS_UIBARTENDEROUT    31          /* bartender sign-out, R3.1 */

#define     CLASS_UICASINTOUT       41          /* cashier interrupt sign-out, R3.3 */
#define     CLASS_UICASINTB_OUT     42          /* cashier interrupt B sign-out, R3.3 */
#define     CLASS_UICASINTOUT2      43          /* for new cashier sign-in error case */
#define     CLASS_UICASINT_PREOUT   44          /* cashier interrupt pre sign-out, R3.3 */
#define     CLASS_UICASINTB_PREOUT  45          /* cashier interrupt B pre sign-out, R3.3 */


/*--------------------------------------------------------------------------
*       MODE-IN DATA
--------------------------------------------------------------------------*/

#define     CLASS_UIREGMODEIN        1          /* reg mode-in */





/*--------------------------------------------------------------------------
*       TRANSACTION MODULE interface data (MAJOR CLASS)
--------------------------------------------------------------------------*/

#define     CLASS_ITEMOPEOPEN        3          /* operator open */
#define     CLASS_ITEMTRANSOPEN      4          /* transaction open */
#define     CLASS_ITEMSALES         30          /* sales */
#define     CLASS_ITEMDISC          31          /* discount */
#define     CLASS_ITEMCOUPON        41          /* coupon */
#define     CLASS_ITEMTOTAL         32          /* total key */
#define     CLASS_ITEMTENDER        33          /* tender */
#define     CLASS_ITEMMISC          34          /* misc. transaction */
#define     CLASS_ITEMMODIFIER      35          /* modifier */
#define     CLASS_ITEMOTHER         36          /* other */
#define     CLASS_ITEMAFFECTION     37          /* affection */
#define     CLASS_ITEMPRINT         38          /* print */
#define     CLASS_ITEMOPECLOSE      51          /* operator close */
#define     CLASS_ITEMTRANSCLOSE    52          /* transaction close */
#define     CLASS_ITEMMODEIN        61          /* mode-in */
#define     CLASS_ITEMMULTI         39          /* multi check payment */
#define     CLASS_ITEMTENDEREC      40          /* E/C tender for charge post */
#define     CLASS_ITEMPREVCOND		50			/*Condiment Editing, JHHJ*/


/*-------------------------------------------------------------------------
*       TRANSACTION MODULE INTERFACE DATA (minor class)
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
*       OPERATOR OPEN DATA 
--------------------------------------------------------------------------*/

#define     CLASS_CASHIERIN          1          /* cashier sign-in */
#define     CLASS_B_IN               2          /* cashier B sign-in */
#define     CLASS_WAITERIN          11          /* waiter sign-in */
#define     CLASS_WAITERLOCKIN      12          /* waiter lock sign-in */
#define     CLASS_BARTENDERIN       21          /* bartender sign-in,   R3.1 */

#define     CLASS_CASINTIN          31          /* cashier interrupt sign-in, R3.3 */
#define     CLASS_CASINTB_IN        32          /* cashier interrupt B sign-in, R3.3 */


/*--------------------------------------------------------------------------
*       TRANSACTION OPEN DATA
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
#define     CLASS_MANUALPB          21          /* manual PB */
#define     CLASS_OPENSEAT          31          /* Seat# Trans Open, R3.1 */
#define     CLASS_OPENPRINT         41          /* post receipt, R3.1 */

#define     CLASS_OPENMONEY         51          /* money declaration,   2172 */

#define     CLASS_OPEN_LOAN         101         /* loan transaction    2172 */
#define     CLASS_OPEN_PICK         102         /* pick-up transaction 2172 */

/*--------------------------------------------------------------------------
*       SALES DATA
--------------------------------------------------------------------------*/

#define     CLASS_DEPT               1          /* dept */
#define     CLASS_DEPTITEMDISC       2          /* dept w/item discount */
#define     CLASS_DEPTMODDISC        3          /* dept w/modifier discount */
#define     CLASS_PLU               11          /* PLU */
#define     CLASS_PLUITEMDISC       12          /* PLU w/item discount */
#define     CLASS_PLUMODDISC        13          /* PLU w/modifier discount */
#define     CLASS_SETMENU           21          /* set menu */
#define     CLASS_SETITEMDISC       22          /* set menu w/item discount */
#define     CLASS_SETMODDISC        23          /* set menu w/modifier discount */
#define     CLASS_OEPPLU            31          /* order entry promot item */
#define     CLASS_OEPITEMDISC       32          /* OEP item w/item discount */
#define     CLASS_OEPMODDISC        33          /* OEP item w/modifier discount */

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



/*--------------------------------------------------------------------------
*       DISCOUNT DATA 
--------------------------------------------------------------------------*/

#define     CLASS_ITEMDISC1          1          /* item discount */
#define     CLASS_REGDISC1          11          /* regular discount 1 */
#define     CLASS_REGDISC2          12          /* regular discount 2 */
#define     CLASS_REGDISC3          13          /* regular discount 3,  R3.1 */
#define     CLASS_REGDISC4          14          /* regular discount 4,  R3.1 */
#define     CLASS_REGDISC5          15          /* regular discount 5,  R3.1 */
#define     CLASS_REGDISC6          16          /* regular discount 6,  R3.1 */
#define     CLASS_CHARGETIP         21          /* charge tip */
#define     CLASS_CHARGETIP2        22          /* charge tip, V3.3 */
#define     CLASS_CHARGETIP3        23          /* charge tip, V3.3 */

#define     CLASS_AUTOCHARGETIP     31          /* auto charge tip, V3.3 */
#define     CLASS_AUTOCHARGETIP2    32          /* auto charge tip, V3.3 */
#define     CLASS_AUTOCHARGETIP3    33          /* auto charge tip, V3.3 */

/*--------------------------------------------------------------------------
*       COUPON DATA 
--------------------------------------------------------------------------*/

#define     CLASS_COMCOUPON      1          /* combination coupon */


/*--------------------------------------------------------------------------
*       TOTAL KEY DATA
--------------------------------------------------------------------------*/

#define     CLASS_TOTAL1             1          /* total #1 */
#define     CLASS_TOTAL2             2          /* total #2 */
#define     CLASS_TOTAL3             3          /* total #3 */
#define     CLASS_TOTAL4             4          /* total #4 */
#define     CLASS_TOTAL5             5          /* total #5 */
#define     CLASS_TOTAL6             6          /* total #6 */
#define     CLASS_TOTAL7             7          /* total #7 */
#define     CLASS_TOTAL8             8          /* total #8 */
#define     CLASS_STUB3             13          /* stub #3 */
#define     CLASS_STUB4             14          /* stub #4 */
#define     CLASS_STUB5             15          /* stub #5 */
#define     CLASS_STUB6             16          /* stub #6 */
#define     CLASS_STUB7             17          /* stub #7 */
#define     CLASS_STUB8             18          /* stub #8 */
#define     CLASS_ORDER3            23          /* order #3 */
#define     CLASS_ORDER4            24          /* order #4 */
#define     CLASS_ORDER5            25          /* order #5 */
#define     CLASS_ORDER6            26          /* order #6 */
#define     CLASS_ORDER7            27          /* order #7 */
#define     CLASS_ORDER8            28          /* order #8 */
#define     CLASS_TOTALAUDACT       31          /* total for audaction */
#define     CLASS_TOTALPOSTCHECK    41          /* total for postcheck buffering print */
#define     CLASS_STUBPOSTCHECK     51          /* stub for postcheck total */
#define     CLASS_PREAUTHTOTAL      61          /* pre-authorization total */

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

/*--------------------------------------------------------------------------
*       TENDER DATA
--------------------------------------------------------------------------*/

#define     CLASS_TENDER1            1          /* tender #1 */
#define     CLASS_TENDER2            2          /* tender #2 */
#define     CLASS_TENDER3            3          /* tender #3 */
#define     CLASS_TENDER4            4          /* tender #4 */
#define     CLASS_TENDER5            5          /* tender #5 */
#define     CLASS_TENDER6            6          /* tender #6 */
#define     CLASS_TENDER7            7          /* tender #7 */
#define     CLASS_TENDER8            8          /* tender #8 */
#define     CLASS_TENDER9            9          /* tender #9 */
#define     CLASS_TENDER10          10          /* tender #10 */
#define     CLASS_TENDER11          11          /* tender #11 */
#define     CLASS_TENDER12          12          /* tender #12 */
#define     CLASS_TENDER13          13          /* tender #13 */
#define     CLASS_TENDER14          14          /* tender #14 */
#define     CLASS_TENDER15          15          /* tender #15 */
#define     CLASS_TENDER16          16          /* tender #16 */
#define     CLASS_TENDER17          17          /* tender #17 */
#define     CLASS_TENDER18          18          /* tender #18 */
#define     CLASS_TENDER19          19          /* tender #19 */
#define     CLASS_TENDER20          20          /* tender #20 */

#define     CLASS_FOREIGN1          23          /* FC tender #1 */
#define     CLASS_FOREIGN2          24          /* FC tender #2 */
#define     CLASS_FOREIGN3          25          /* FC tender #3, 2172 */
#define     CLASS_FOREIGN4          26          /* FC tender #4, 2172 */
#define     CLASS_FOREIGN5          27          /* FC tender #3, 2172 */
#define     CLASS_FOREIGN6          28          /* FC tender #4, 2172 */
#define     CLASS_FOREIGN7          29          /* FC tender #3, 2172 */
#define     CLASS_FOREIGN8          30          /* FC tender #4, 2172 */

#define     CLASS_CPTENDEC          40          /* error correct for charge posting */

#define     CLASS_EUROTENDER        50          /* Euro amount trailer, V3.4 */

/*--------------------------------------------------------------------------
*       MISC. TRANSACTION DATA
--------------------------------------------------------------------------*/

#define     CLASS_NOSALE             1          /* nosale */
#define     CLASS_PO                 2          /* paid out */
#define     CLASS_RA                 3          /* received on account */
#define     CLASS_TIPSPO             4          /* tips paid out */
#define     CLASS_TIPSDECLARED       5          /* tips declared */
#define     CLASS_CHECKTRANSFER      6          /* check transfer */
#define     CLASS_CANCEL             7          /* cancel */
#define     CLASS_ETKIN              8          /* ETK time in */
#define     CLASS_ETKOUT             9          /* ETK time out */
#define     CLASS_ETKOUT_WOETKIN    10          /* ETK time out w/o ETK in */
#define     CLASS_EC                11          /* error correct */
#define     CLASS_CHECKTRANS_FROM   21          /* check transfer */
#define     CLASS_CHECKTRANS_TO     22          /* check transfer */
#define     CLASS_MONEY             23          /* money declaration, 2172 */

/*--------------------------------------------------------------------------
*       MODIFIER DATA
--------------------------------------------------------------------------*/

#define     CLASS_PVOID              1          /* preselect void */
#define     CLASS_NORECEIPT          2          /* no receipt */
#define     CLASS_TABLENO           11          /* table No */
#define     CLASS_NOPERSON          12          /* No of person */
#define     CLASS_LINENO            13          /* line No */
#define     CLASS_ALPHANAME         14          /* Customer Name,      R3.0 */
#define     CLASS_TAXMODIFIER       15          /* Tax Modifier,       2172 */
#define     CLASS_MODNUMBER         16          /* Number              2172 */

#define     CLASS_SEATNO            21          /* Seat#,              R3.1 */
#define     CLASS_SEAT_SINGLE       31          /* Seat# Tender,       R3.1 */
#define     CLASS_SEAT_MULTI        32          /* Multi Seat# Tender, R3.1 */
#define     CLASS_SEAT_TYPE2        33          /* Type 2,             R3.1 */

/*--------------------------------------------------------------------------
*       OTHER DATA
--------------------------------------------------------------------------*/

#define     CLASS_PRINTDEMAND        1          /* print demand */
#define     CLASS_SLIPRELEASE        3          /* slip release */
#define     CLASS_INSIDEFEED         4          /* inside feed  */
#define     CLASS_OUTSIDEFEED        5          /* outside feed */
#define     CLASS_POSTRECEIPT        6          /* post receipt */
#define     CLASS_POSTRECPREBUF      7          /* post receipt for pre-check buffer */
#define     CLASS_CP_EPT_DUPLI       8          /* Dupli-Receipt for EPT, CP*/
#define     CLASS_PARKING            10         /* parking */
#define     CLASS_CP_EPT_FULLDUP     11         /* 5/30/96 full duplicate receipt for CP */
#define     CLASS_RESETLOG           12         /* PC I/F reset log, V3.3 */
#define     CLASS_POWERDOWNLOG       13         /* Power Down Log,      Saratoga*/
#define     CLASS_PREINQ_EPT         21         /* EPT Pre-Inquiry,     Saratoga */


/*--------------------------------------------------------------------------
*       AFFECTION 
--------------------------------------------------------------------------*/

#define     CLASS_HOURLY             1          /* hourly total */
#define     CLASS_SERVICE            2          /* service total */
#define     CLASS_CANCELTOTAL        4          /* cancel total */
#define     CLASS_CASHIERCHECK       5          /* cashier close check */
#define     CLASS_WAITERCHECK        6          /* waiter close check */
#define     CLASS_NEWWAITER          8          /* check transfer new waiter */
#define     CLASS_OLDWAITER          9          /* check transfer old waiter */
#define     CLASS_CHARGETIPWAITER   10          /* charge tip amount for waiter */
#define     CLASS_CHARGETIPWAITER2  11          /* charge tip#2 amount for waiter, V3.3 */
#define     CLASS_CHARGETIPWAITER3  12          /* charge tip#3 amount for waiter, V3.3 */
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

#define     CLASS_LOANAFFECT        71          /* Loan,    Saratoga */
#define     CLASS_PICKAFFECT        72          /* Pickup,  Saratoga */
#define     CLASS_LOANCLOSE         73          /* Loan End,    Saratoga */
#define     CLASS_PICKCLOSE         74          /* Pickup End,  Saratoga */

/*--------------------------------------------------------------------------
*       PRINT
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
#define     CLASS_EPT_TRAILER       35          /* EPT trailer/header (Logo)*/
#define     CLASS_EPT_STUB          36          /* EPT duplicate print      */
#define     CLASS_ERROR             37          /* EPT error                */
#define     CLASS_CP_STUB           38          /* CP duplicate print       */

#define     CLASS_NUMBER            40          /* number print, 2172 */
#define     CLASS_AGELOG            41          /* Age Verify Log,   2172 */

#define     CLASS_MONEY_HEADER      51          /* Menoy Header,        Saratoga */
#define     CLASS_RSPTEXT           61          /* Response Text Print, Saratoga */
#define		CLASS_SPLITA_EPT		62			/* EPT trailer/Promotional trailer */
#define		CLASS_ORDERDEC			63			/* Print Order Dec*/


/*--------------------------------------------------------------------------
*       ORDER DECLARATION
--------------------------------------------------------------------------*/

#define		CLASS_ORDER_DEC_NEW		1			/* New Order Declaration on Top */
#define		CLASS_ORDER_DEC_MID		2			/* Order Declaration is pressed mid transaction*/
#define		CLASS_ORDER_DEC_FSC		3			/* Order Declaration FSC is pressed new*/


/*--------------------------------------------------------------------------
*       TRANSACTION CLOSE 
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

/*--------------------------------------------------------------------------
*       OPERATOR CLOSE DATA
--------------------------------------------------------------------------*/

#define     CLASS_CASHIEROUT         1          /* cashier sign-out */
#define     CLASS_B_OUT              2          /* cashier B sign-out */
#define     CLASS_WAITEROUT         11          /* waiter sign-out */
#define     CLASS_WAITERLOCKOUT     12          /* waiter lock sign-out */
#define     CLASS_MODELOCK          21          /* mode lock key */

#define     CLASS_BARTENDEROUT      31          /* bartender sign-out, R3.1 */
#define     CLASS_CASHIERMISCOUT    32          /* cashier misc. sign-out */

#define     CLASS_CASINTOUT         41          /* cashier interrupt sign-out, R3.3 */
#define     CLASS_CASINTB_OUT       42          /* cashier interrupt B sign-out, R3.3 */

/*--------------------------------------------------------------------------
*       MODE-IN DATA
--------------------------------------------------------------------------*/

#define     CLASS_REGMODEIN          1          /* reg mode-in */
#define     CLASS_SUPMODEIN         101         /* loan/pick-up,    Saratoga */



/*--------------------------------------------------------------------------
*       MULTI CHECK DATA
--------------------------------------------------------------------------*/
                                        
#define     CLASS_MULTICHECKPAYMENT     1       /* multi check payment */
#define     CLASS_MULTIPOSTCHECK        2       /* multi check, for postcheck */
#define     CLASS_MULTITOTAL            3       /* multi check, grand total */
#define     CLASS_MULTIPOSTTOTAL        4       /* multi check, grand total (postcheck) */




/*--------------------------------------------------------------------------
*       PRINT STATUS
--------------------------------------------------------------------------*/

#define     RECEIPT             0x0001          /* receipt print */
#define     JOURNAL             0x0002          /* journal print */
#define     VALIDATION          0x0004          /* validation print */
#define     SLIP                0x0008          /* slip print */

#define     VOID_VALIDATION     0x8000          /* validation print for void*/

#define     SINGLE_RECPT        0x0100          /* issue single receipt */
#define     DOUBLE_RECPT        0x0200          /* issue double receipt */

#define     HEADER24_PRINT      0x0400          /* 24 char header print status */
#define     SPCL_PRINT          0x0800          /* special print status */

#define     MANUAL_WEIGHT       0x1000          /* manual weight */
#define     NOT_UNBUFFING_PRINT 0x2000          /* do not execute unbuffering print, V3.3 */




/* -------------------------------------------------------------------------
*       TAXABLE/VOID/OFF LINE TENDER MODIFIER STATUS (US/CANADA)
--------------------------------------------------------------------------*/

#define     VOID_MODIFIER       0x01            /* void modifier */
#define     TAX_MODIFIER1       0x02            /* taxable modifier #1 */
#define     TAX_MODIFIER2       0x04            /* taxable modifier #2 */
#define     TAX_MODIFIER3       0x08            /* taxable modifier #3 */

#define     EXCLUDE_VAT_SER     0x02            /* exclude VAT/service, V3.3 */
#define     PRESET_MODIFIER     0x04            /* preset modifier,     V3.3 */
#define     PRICE_LEVEL         0x08            /* price level modifier,V3.3 */

#define     OFFEPTTEND_MODIF    0x10            /* ept off line tender modifier */
#define     OFFCPTEND_MODIF     0x20            /* cp off line tender modifier */
#define     OFFCPAUTTEND_MODIF  0x40            /* credit autho. off line tender modifier */

#define     CANTAX_MEAL			0x01            /* meal item */
#define     CANTAX_CARBON       0x02            /* carbonated beverage item */
#define     CANTAX_SNACK        0x03            /* snack item */
#define     CANTAX_BEER			0x04            /* beer/wine item */
#define     CANTAX_LIQUOR       0x05            /* liquor item */
#define     CANTAX_GROCERY      0x06            /* grocery(non-taxable) item */
#define     CANTAX_TIP			0x07            /* tip item */
#define     CANTAX_PST1			0x08            /* PST1 only item */
#define     CANTAX_BAKED        0x09            /* baked item */
#define		CANTAX_BAKEDCTOT	0x10			/* baked item, NO PST*/

#define     TAX_MOD_MASK        0x1e            /* taxable modifier bit mask data */

#define     INTL_VAT1       0x01            /* VATable #1,  V3.3 */
#define     INTL_VAT2       0x02            /* VATable #2,  V3.3 */
#define     INTL_VAT3       0x03            /* VATable #3,  V3.3 */

/* for fbModifer2, 2172 */

#define     EVER_MODIFIER               0x01                    /* E-version modifier, 2172     */
#define     VELO_MODIFIER               0x02                    /* UPC-Velocity modifier, 2172  */
#define     PCHK_MODIFIER               0x04                    /* price check modifier, 2172   */

#define		VOID_EDITCONDIMENT	0x40


/* -------------------------------------------------------------------------
*       transaction storage status
* ------------------------------------------------------------------------*/

#define     NOT_ITEM_CONS          0x01            /* not item consolidation */
#define     NOT_ITEM_STORAGE       0x02            /* not store item storage */
#define     NOT_CONSOLI_STORAGE    0x04            /* not store consoli storage */
#define     NOT_SEND_KDS           0x08            /* not send to KDS R3.1 */
#define     STORAGE_STATUS_MASK    0x0f            /* storage status mask data */


/* -------------------------------------------------------------------------
*       INPUT STATUS 
--------------------------------------------------------------------------*/

#define     INPUT_0_ONLY        0x01                /* '0' input */
#define     INPUT_MSR           0x02                /* MSR input R3.1 */
#define     INPUT_DEPT          0x04                /* Entered Dept No. Data 2172  */


/* -------------------------------------------------------------------------
*       Split Reduce Status(ITEMSALES, ITEMDISC, ITEMCOUPON: fbReduceStatus)    R3.1
--------------------------------------------------------------------------*/

#define     REDUCE_ITEM         0x01                /* Reduced Item */
#define     REDUCE_NOT_HOURLY   0x02                /* Not Affect Hourly */


/* -------------------------------------------------------------------------
*       number of mnemonic 
--------------------------------------------------------------------------*/

#define     NUM_NUMBER          19+1            /* number */
#define     NUM_NAME            19+1            /* Customer Name, R3.0 */
#define     NUM_GUEST           6+1             /* GCF No */
#define		NUM_TENT			(25+1)			// Max length of Tent mnemonic
#define     NUM_ID              3+1             /* ID */
#define     NUM_OPERATOR        20+1            /* operator, V3.3 */
#define     NUM_SPECMNEM        4+1             /* special mnemonic */
#define     NUM_TRANSMNEM       8+1             /* transaction mnemonic */
#define     NUM_ADJECTIVE       4+1             /* adjective mnemonic */
#define     NUM_DEPTPLU         20+1            /* dept/PLU mnemonic, 2172 */
#define     NUM_HEADER          24+1            /* header mnemonic */
#define     NUM_LEADTHRU        16+1            /* lead through mnemonic */
#define     NUM_ROOM            5+1             /* charge posting room number */
#define     NUM_GUESTID         2+1             /* charge posting guest id */
#define     NUM_FOLIO           6+1             /* charge posting folio number */
#define     NUM_POSTTRAN        6+1             /* charge posting posted transaction number */
#define     NUM_CPRSPTEXT       40              /* (39 + 1)charge posting response text */   
#define     NUM_CPRSPCO         6               /* charge posting response text */   
#define     NUM_CPRSPCO_EPT     10              /* charge posting response text,    Saratoga */   
#define     NUM_EXPIRA          4               /* chareg posting expiration date */
#define     NUM_MSRDATA         40              /* chareg posting msr data */
#define     NUM_APPROVAL        6               /* approval code          */
#define     NUM_ETK_NAME        16+1            /* ETK Name,   R3.1 */
#define     NUM_SCAN_NUMBER     13              /* Account Number,  V3.3 */
#define     NUM_ACCT_NUMBER     10              /* Account Number,  V3.3 */
#define     NUM_BIRTYHDAY        6              /* birthday entry, 2172  */
#define     NUM_PLU_LEN         STD_PLU_NUMBER_LEN              /* length of PLU by BCD, 2172 */
/*#define       NUM_PLU_BCD_LEN     7               / length of PLU by BCD, 2172 */
#define     NUM_PLU_SCAN_LEN    74              /* length of PLU by Scan, 2172 */
#define     NUM_ACCT_LABEL      10              /* Account# R.W.L,  Saratoga */
#define     NUM_GUESTNAME       16              /* Guest Name,      Saratoga */
#define     NUM_REC_1LINE       24              /* Receipt 1 Line,  Saratoga */
#define     NUM_SLIP_1LINE      40              /* Slip 1 Line,     Saratoga */
#define     NUM_MAX_LINE        10              /* Max. Line#,      Saratoga */
#define		NUM_AUTHCODE		20				// XEPT authorization Code
#define		NUM_REFERENCE		24				// XEPT Reference Number
#define     NUM_TENT_MNEM		20						 //length of tent mnemonic

/*--------------------------------------------------------------------------
        number of array
--------------------------------------------------------------------------*/

#define     NUM_VAT              3              /* number VAT */
/*--------------------------------------------------------------------------
        number of #/Type entries stored US Customs cwunn 4/21/03
--------------------------------------------------------------------------*/

#define     NUM_OF_NUMTYPE_ENT   5              /* number of #/Type entries stored in aszNumber matrix US Customs cwunn 4/21/03
													(length of each entry is NUM_NUMBER) */




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
        UCHAR       fbInputStatus;          /* 4:input status */
    }UIFREGOPEOPEN;



/*--------------------------------------------------------------------------
*       transaction open data 
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;          /* 1:major class */
        UCHAR       uchMinorClass;          /* 2:minor class */
        USHORT      usNoPerson;             /* 9:No of person */
        UCHAR       fbModifier;             /* 11:modifier */
        ULONG       ulAmount;               /* 13:previous amount */
        UCHAR       fbInputStatus;          /* 13-1:input status */
    }UIFREGTRANSOPEN;



/*--------------------------------------------------------------------------
*       sales data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;          /* 1:major class */
        UCHAR       uchMinorClass;          /* 2:minor class */
        WCHAR       aszPLUNo[NUM_PLU_SCAN_LEN]; /*  PLU No, 2172        */
        /* USHORT      usPLUNo;                / 3:PLU No */
        UCHAR       uchFsc;                 /* 4:FSC */
        USHORT      usDeptNo;               /* 5:dept No(extend FSC) */
        /* UCHAR       uchDeptNo;              / 5:dept No(extend FSC) */
        LONG        lQTY;                   /* 6:quantity */
        ULONG       ulUnitPrice;            /* 7:unit price */ 
        UCHAR       fbInputStatus;          /* 7-1:input status */
        UCHAR       uchAdjective;           /* 8:adjective */
        UCHAR       fbModifier;             /* 10:void, tax modifier */
        UCHAR       fbModifier2;            /* Price Check, E-Version */
        WCHAR       aszNumber[NUM_NUMBER];  /* 11:number */
        UCHAR       uchPrintModifier;       /* 16:print modifier */
        LONG        lDiscountAmount;        /* 22:discount amount */
        UCHAR       uchMajorFsc;            /* 2172 */
    }UIFREGSALES;



/*--------------------------------------------------------------------------
*       discount data
--------------------------------------------------------------------------*/

   typedef  struct {
        UCHAR       uchMajorClass;          /* 1:major class */
        UCHAR       uchMinorClass;          /* 2:minor class */
        UCHAR       uchSeatNo;              /* Seat Number, V3.3 */
        LONG        lAmount;                /* 6:discount amount */
        UCHAR       aszNumber[NUM_NUMBER];  /* 7:number */
        UCHAR       fbModifier;             /* 9:void, tax modifier */
    }UIFREGDISC;



/*--------------------------------------------------------------------------
*       coupon data
--------------------------------------------------------------------------*/

   typedef  struct {
        UCHAR       uchMajorClass;          /* major class */
        UCHAR       uchMinorClass;          /* minor class */
        UCHAR       uchFSC;                 /* FSC */
        UCHAR       aszNumber[NUM_NUMBER];  /* number */
        UCHAR       fbModifier;             /* void, tax modifier */
    }UIFREGCOUPON;



/*--------------------------------------------------------------------------
*       total key data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;          /* major class */
        UCHAR       uchMinorClass;          /* minor class */
        UCHAR       fbModifier;             /* tax modifier */
        UCHAR       aszNumber[NUM_NUMBER];  /* number */
        UCHAR       uchSeatNo;              /* seat#,          R3.1 */ 
        UCHAR       fbInputStatus;          /* input status,   R3.1 */
    }UIFREGTOTAL;



/*--------------------------------------------------------------------------
*       tender data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;          /* 1:major class */
        UCHAR       uchMinorClass;          /* 2:minor class */
        UCHAR       fbModifier;             /* 3:void */
        LONG        lTenderAmount;          /* 7:tender amount */
        UCHAR       aszNumber[NUM_NUMBER];  /* 11:number */
    }UIFREGTENDER;




/*--------------------------------------------------------------------------
*       misc. transaction data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;          /* 1:major class */
        UCHAR       uchMinorClass;          /* 2:minor class */
        UCHAR       aszID[NUM_ID];          /* 3:cashier/waiter ID */
        UCHAR       aszNumber[NUM_NUMBER];  /* 5:number */
        UCHAR       aszGuestNo[NUM_GUEST];  /* 7:guest check No */
        LONG        lAmount;                /* 8:amount */
        LONG        lQTY;                   /*  quantity,   Saratoga */
        UCHAR       uchTendMinor;           /* for Money,   Saratoga */
        UCHAR       fbModifier;             /* 10:void */
        ULONG       ulEmployeeNo;           /* Employee Number,      R3.1 */
        UCHAR       uchJobCode;             /* Job Code to be time in */
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
        LONG        lAmount;                 /* 9:amount */
        UCHAR       aszNumber[NUM_NUMBER];  /* 10:number */
    }UIFREGMODIFIER;



/*--------------------------------------------------------------------------
*       other data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;                  /* 1:major class */
        UCHAR       uchMinorClass;                  /* 2:minor class */
        LONG        lAmount;                        /* 4:amount */
        UCHAR       uchFsc;                         /*  FSC (Ask)    2172 */
        UCHAR       aszLabelNo[NUM_PLU_SCAN_LEN];   /* Label No, before analysis 2172        */
        UCHAR       aszNumber[NUM_NUMBER];          /* after analaysis */
    }UIFREGOTHER;

/*--------------------------------------------------------------------------
*       operator close data 
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;          /* 1:major class */
        UCHAR       uchMinorClass;          /* 2:minor class */
        USHORT      usID;                   /* 3:cashier/waiter ID */
        UCHAR       fbInputStatus;          /* 4:input status, R3.3 */
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

/*--------------------------------------------------------------------------
*       operator open data 
--------------------------------------------------------------------------*/

    typedef struct {                            
        UCHAR       uchMajorClass;              /* 1:major class */
        UCHAR       uchMinorClass;              /* 2:minor class */
        USHORT      usID;                       /* 3:cashier/waiter ID */
        UCHAR       uchSecret;                  /* 4:secret code */
        UCHAR       aszMnemonic[NUM_OPERATOR];  /* 5:cashier/waiter mnemonic */
        UCHAR       fbStatus;                   /* 6:status */
        USHORT      usFromTo[2];                /* 7:guest check No range */
        UCHAR       auchStatus[3];              /* 8:cashier status, V3.3 */
        UCHAR       uchChgTipRate;              /* 9:charge tips rate, V3.3 */
        UCHAR       uchTeamNo;                  /* 10:team no, V3.3 */
    }ITEMOPEOPEN;



/*--------------------------------------------------------------------------
*       transaction open data 
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR       uchMajorClass;              /* 1:major class */
        UCHAR       uchMinorClass;              /* 2:minor class */
        UCHAR       uchSeatNo;                  /* Seat#,     R3.1 */
        USHORT      usCashierID;                /* 3: cashier ID */
        USHORT      usWaiterID;                 /* 4: waiter ID */
        USHORT      usGuestNo;                  /* 6:guest check No */
        UCHAR       uchCdv;                     /* 7:CDV */
        USHORT      usTableNo;                  /* 8:table No */
        USHORT      usNoPerson;                 /* 9:No of person */
        UCHAR       uchLineNo;                  /* 10:line No */
        UCHAR       fbModifier;                 /* 11:tax modifier */
        LONG        lAmount;                    /* 13:previous amount */
        UCHAR       auchCheckOpen[3];           /* 15:time check open,  R3.1 */
        USHORT      fsPrintStatus;              /* 16:print status */
        UCHAR       fbStorageStatus;            /* 17:storage status */
        UCHAR       uchStatus;                  /* 18:MDC status */
        UCHAR       uchTeamNo;                  /* 19:Team No, V3.3 */
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
#define ITM_MINUS_ITEM          0x01                /* minus item status */
#define ITM_SPECIAL_STATUS      0x02                /* special item status */
#define ITM_SCALABLE_ITEM       0x01                /* scalable item status */
#define ITM_CONDIMENT_ITEM      0x02                /* condiment item status */
#define ITM_PLU_CONT_OTHER      7                   /* R3.1 for kp control */
#define ITM_PLU_CONT_SIZE       8

#define ITM_SALES_HOURLY            0x01            /* hourly affection */
#define ITM_HASH_FINANC             0x02            /* hourly affection */
#define ITM_AFFECT_DEV_BY_10        0x04            /* scalable item affects by 0.01 LB/ Kg */
#define ITM_PLU_LINK_SCALE_DEPT     0x08            /* PLUlinks to scalable dept */

#define ITM_PLU2_BDY_CONTROL1   0x10        /* Birthday Control 1 */
#define ITM_PLU2_BDY_CONTROL2   0x20        /* Birthday Control 2 */
#define ITM_PLU2_PREV_COND		0x30

/* for modified discount */
#define ITM_MOD_DISC_SUR            0x10            /* modified surcharge */
#define ITM_MOD_AFFECT_DEPT_PLU     0x20            /* mod disc affection */

#define ITM_CONDIMENT_SIZE      15                  /* size of condiment */

/* for random weight label control status, 2172 */
#define     ITM_TYPE_RANDOM         0x01        /* Random Weight Label */
#define     ITM_TYPE_ARTNO          0x02        /* article # is on Label */
#define     ITM_TYPE_PRICE          0x04        /* price is on Label */
#define     ITM_TYPE_WEIGHT         0x08        /* weight is on Label */
#define     ITM_TYPE_QTY            0x10        /* quantity is on Label */
#define     ITM_TYPE_DEPT           0x20        /* department # is on Label */
#define     ITM_TYPE_NUMBER         0x40        /* number # is on Label */

/* for control status */
#define     ITM_CONTROL_LINK        0x0001      /* sold as link PLU                */
#define     ITM_CONTROL_PCHG        0x0002      /* price change                    */
#define     ITM_CONTROL_NOTINFILE   0x0004      /* not in file build PLU           */
#define     ITM_CONTROL_LINKCOND    0x0008      /* w/ link PLU as condiment        */


    typedef struct {
        UCHAR       uchItemType;                    /* item type, 2172 */
        /* UCHAR       uchDeptNo;                          / 15-1:dept No */
        UCHAR       uchReportCode;                      /* 15-2:report code */
        UCHAR       auchStatus[ITM_PLU_CONT_OTHER];      /* 15-3:control status[5] */
		USHORT		usBonusIndex;
    }ITEMCONTCODE;

    typedef struct {
        UCHAR           auchPLUNo[NUM_PLU_LEN];     /*  PLU No, 2172    */
        /* USHORT          usPLUNo;                    / 20-1:PLU No */
        USHORT          usDeptNo;                   /* dept No, 2172 */
        /* UCHAR           uchDeptNo;                  / 20-2:dept No */
        UCHAR           uchPrintPriority;           /* print priority */
        LONG            lUnitPrice;                 /* 20-3:price */
        UCHAR           uchAdjective;               /* 20-4:adjective */
        UCHAR           aszMnemonic[NUM_DEPTPLU];   /* 20-6:mnemonic */
        ITEMCONTCODE    ControlCode;                /* 15:dept, report code, status */
		UCHAR			uchCondColorPaletteCode;	/* condiment color palette code */
    }ITEMCOND;

       typedef struct {
        UCHAR           uchMajorClass;              /* major class */
        UCHAR           uchMinorClass;              /* minor class */
        UCHAR           uchSeatNo;                  /* Seat#,     R3.1 */
        TCHAR           auchPLUNo[NUM_PLU_LEN];     /*  PLU No, 2172    */
        /* USHORT          usPLUNo;                    / PLU No */
        USHORT          usDeptNo;                   /* dept No, 2172 */
        /* UCHAR           uchDeptNo;                  / dept No */
        LONG            lQTY;                       /* quantity */
        LONG            lProduct;                   /* product */
        SHORT           sCouponQTY;                 /* quantity for coupon */
        SHORT           sCouponCo;                  /* coupon counter */
        USHORT          usOffset;                   /* Offset of Partner, R3.1 */
        UCHAR           uchPrintPriority;           /* print priority */
        USHORT           uchPPICode;                 /* PPI Code   ESMITH */
		//UCHAR           uchPPICode;
        UCHAR           fbReduceStatus;             /* Reduce Status,  R3.1 */
        USHORT          fsLabelStatus;              /* label control status from GP, 2172 */
		USHORT			usUniqueID;
        LONG            lUnitPrice;                 /* unit price */
        UCHAR           uchAdjective;               /* adjective */
        UCHAR           fbModifier;                 /* void, tax modifier */
        UCHAR           fbModifier2;                /* price check, e-version, 2172 */
        TCHAR           aszNumber[NUM_OF_NUMTYPE_ENT][NUM_NUMBER];//increased # of #/Type entries stored US Customs cwunn 4/10/03
        TCHAR           aszMnemonic[NUM_DEPTPLU];   /* mnemonic */
        ITEMCONTCODE    ControlCode;                /* dept, report code, status */
        UCHAR           uchPrintModNo;              /* No of P.M */
        UCHAR           auchPrintModifier[STD_MAX_COND_NUM];      /* print modifier */
        USHORT          fsPrintStatus;              /* print status */
        UCHAR           uchCondNo;                  /* No of condiment */
        UCHAR           uchChildNo;                 /* No of condiment */
        ITEMCOND        Condiment[STD_MAX_COND_NUM];              /* condiment */
        UCHAR           uchRate;                    /* modifier discount rate */
        LONG            lDiscountAmount;            /* discount amount */
        UCHAR           fbStorageStatus;            /* storage status */
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
		UCHAR			uchColorPaletteCode;			/* ColorPalette Address */
		USHORT			usTareInformation;		//tare information for specific PLU.

}ITEMSALES;




/*--------------------------------------------------------------------------
*       discount data
--------------------------------------------------------------------------*/


#define ITM_DISC_HOURLY         0x20        /* hourly affection */

/* print status */
#define ITM_DISC_SINGLE_REC     0x0100      /* issue single receipt */
#define ITM_DISC_DOUBLE_REC     0x0200      /* issue double receipt */

/* --- auchStatus[2] --- */
#define ITM_DISC_AFFECT_DISC1   0x01        /* Affect Discountable Itemizer #1 */
#define ITM_DISC_AFFECT_DISC2   0x02        /* Affect Discountable Itemizer #2 */
#define ITM_DISC_AFFECT_SERV    0x04        /* Affect Service(Int'l),   V3.3 */

    typedef struct {
        UCHAR   uchMajorClass;              /* major class */
        UCHAR   uchMinorClass;              /* minor class */
        UCHAR   uchSeatNo;                  /* Seat#,          R3.1 */
        UCHAR   fbReduceStatus;             /* Reduce Status,  R3.1 */
        UCHAR   auchPLUNo[NUM_PLU_LEN];     /* PLU No, 2172    */
        /* USHORT  usPLUNo;                    / PLU No */
        UCHAR   uchAdjective;               /* adjective */
        USHORT  usDeptNo;                  /* dept No, 2172 */
        /* UCHAR   uchDeptNo;                  / dept No */
        UCHAR   uchRate;                    /* rate */
        LONG    lAmount;                    /* discount amount */
        UCHAR   aszNumber[NUM_NUMBER];      /* number */
        UCHAR   fbModifier;                 /* void, tax modifier */
        UCHAR   auchStatus[3];              /* status */
        USHORT  fsPrintStatus;              /* print status */
        USHORT  usID;                       /* waiter ID */
        UCHAR   fbStorageStatus;            /* storage status */
    }ITEMDISC;



/*--------------------------------------------------------------------------
*       coupon data
--------------------------------------------------------------------------*/

/* coupon status ( fbStatus[0] ) */
/* bit 7 6 5 4 3 2 1 0
       | | | | | | | +--- void modifier
       | | | | | | +----- affect taxable modifier #1
       | | | | | +------- affect taxable modifier #2
       | | | | +--------- affect taxable modifier #3
       | | | +----------- for Canada/Int'l(Affect Service), V3.3
       +-+-+------------- not used  */

#define ITM_COUPON_AFFECT_SERV  0x10        /* affect service,  V3.3 */

/* coupon status ( fbStatus[1] ) */
#define ITM_COUPON_AFFECT_DISC1 0x01        /* affect disc. itemizer # 1 */
#define ITM_COUPON_AFFECT_DISC2 0x02        /* affect disc. itemizer # 2 */
#define ITM_COUPON_SINGLE_TYPE  0x04        /* single type coupon */
#define ITM_COUPON_HOURLY       0x20        /* affect hourly itemizer */

    typedef struct {
        UCHAR   uchMajorClass;              /* major class */
        UCHAR   uchMinorClass;              /* minor class */
        UCHAR   uchSeatNo;                  /* Seat#,          R3.1 */
        UCHAR   fbReduceStatus;             /* Reduce Status,  R3.1 */
        UCHAR   uchCouponNo;                /* coupon unique number */
        SHORT   sVoidCo;                    /* void counter */
        USHORT  usOffset[7];                /* Dept/PLU offset pointer for coupon, R3.1 */
        LONG    lAmount;                    /* coupon amount */
        UCHAR   fbModifier;                 /* void, tax modifier */
        UCHAR   fbStatus[2];                /* coupon status */
        UCHAR   aszMnemonic[NUM_DEPTPLU];   /* mnemonic */
        UCHAR   aszNumber[NUM_NUMBER];      /* number */
        USHORT  fsPrintStatus;              /* print status */
        UCHAR   uchNoOfItem;                /* no. of sales items */
        UCHAR   fbStorageStatus;            /* storage status */
    }ITEMCOUPON;

    /* structure for coupon search function */

#define ITM_COUPON_PLU_REC      0x00        /* target sales item is PLU item */
#define ITM_COUPON_DEPT_REC     0x01        /* target sales item is Dept item */
#define ITM_COUPON_REC_MASK     0x01        /* bit mask for record type */
#define ITM_COUPON_FOUND        0x00        /* target item is found in storage */
#define ITM_COUPON_NOT_FOUND    0x10        /* target item is not found in storage */
#define ITM_COUPON_FIND_MASK    0x10        /* bit mask for found status */
#define ITM_COUPON_RESERVED     0xee        /* coupon status reserved area */

/* ItemCouponSearch.fbStatus */
#define ITM_COUPON_SINGLE       0x01        /* Single Type Coupon */

    typedef struct {
        UCHAR   auchPLUNo[NUM_PLU_LEN];     /* target PLU number, 2172 */
        USHORT  usDeptNo;                   /* target Dept number, 2172 */
        /* USHORT   usPLUDeptNo;                / target PLU/Dept number */
        UCHAR   uchAdjective;               /* target adjective number */
        USHORT  usItemOffset;               /* offset in transaction storage */
        UCHAR   fbStatus;                   /* target item status */
        UCHAR   uchGroup;                   /* condition group */
    } ITEMCOUPONSALES;
        
    typedef struct {
        UCHAR   uchMajorClass;              /* major class */
        UCHAR   uchMinorClass;              /* minor class */
        UCHAR   uchCouponNo;                /* coupon number */
        UCHAR   fbModifier;                 /* void, tax modifier */
        UCHAR   fbStatus;                   /* status */
        UCHAR   aszNumber[ NUM_NUMBER ];    /* number */
        UCHAR   uchNoOfItem;                /* no. of sales items */
        ITEMCOUPONSALES aSalesItem[ 7 ];    /* target sales item for coupon */
    } ITEMCOUPONSEARCH;



/*--------------------------------------------------------------------------
*       total key data
--------------------------------------------------------------------------*/

    /* ===== type of total key ( auchTotalStatus[ 0 ] / 10 ) ===== */

    /* --- get type of total key --- */
#define ITM_TTL_GET_TTLTYPE( uch )  ( UCHAR )((( UCHAR )( uch ) / 10 ))

#define ITM_TTL_NOFINALIZE  0x01    /* no finalaize total */
#define ITM_TTL_SERVICE1    0x02    /* service total without stub */
#define ITM_TTL_SERVICE2    0x03    /* service total with stub */
#define ITM_TTL_FINALIZE1   0x04    /* finalize total without stub  */
#define ITM_TTL_FINALIZE2   0x05    /* finalize total with stub */
#define ITM_TTL_TRAY1       0x06    /* tray total without stub */
#define ITM_TTL_TRAY2       0x07    /* tray total with stub */

#define ITM_TTL_CASINT1     0x08    /* cashier interrupt total without stub, R3.3 */
#define ITM_TTL_CASINT2     0x09    /* cashier interrupt total with stub, R3.3 */
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

#define ITM_TTL_FLAG_CASHIER_TYPE(pItemTotal)        ((pItemTotal)->auchTotalStatus[0] / 10)

        // See function ItemTotalSEPreAffect() for examples of using these tax macros
#define ITM_TTL_FLAG_TAX_1(pItemTotal)               ((pItemTotal)->auchTotalStatus[1] & ODD_MDC_BIT0)
#define ITM_TTL_FLAG_TAX_2(pItemTotal)               ((pItemTotal)->auchTotalStatus[1] & EVEN_MDC_BIT0)
#define ITM_TTL_FLAG_TAX_1_1(pItemTotal)             ((pItemTotal)->auchTotalStatus[1] & ODD_MDC_BIT1)
#define ITM_TTL_FLAG_TAX_1_2(pItemTotal)             ((pItemTotal)->auchTotalStatus[1] & ODD_MDC_BIT2)
#define ITM_TTL_FLAG_TAX_3(pItemTotal)               ((pItemTotal)->auchTotalStatus[2] & ODD_MDC_BIT0)
#define ITM_TTL_FLAG_TAX_CLEAR_ITEMIZER(pItemTotal)  ((pItemTotal)->auchTotalStatus[2] & EVEN_MDC_BIT0)

#define ITM_TTL_FLAG_PRINT_BUFFERING(pItemTotal)     ((pItemTotal)->auchTotalStatus[2] & EVEN_MDC_BIT1)
#define ITM_TTL_FLAG_DRAWER(pItemTotal)              ((pItemTotal)->auchTotalStatus[2] & EVEN_MDC_BIT3)
#define ITM_TTL_FLAG_CRT_1_4(pItemTotal)             ((pItemTotal)->auchTotalStatus[3] & 0x0F)
#define ITM_TTL_FLAG_PRINT_DOUBLEWIDE(pItemTotal)    ((pItemTotal)->auchTotalStatus[3] & EVEN_MDC_BIT1)
#define ITM_TTL_FLAG_PRINT_VALIDATION(pItemTotal)    ((pItemTotal)->auchTotalStatus[3] & EVEN_MDC_BIT2)
#define ITM_TTL_FLAG_BUFFERED(pItemTotal)            ((pItemTotal)->auchTotalStatus[4] & ODD_MDC_BIT0)
#define ITM_TTL_FLAG_PRE_AUTH(pItemTotal)            ((pItemTotal)->auchTotalStatus[4] & EVEN_MDC_BIT0)
#define ITM_TTL_FLAG_CRT_5_8(pItemTotal)             ((pItemTotal)->auchTotalStatus[5] & 0x0F)
#define ITM_TTL_FLAG_DT_PUSH_BACK(pItemTotal)        ((pItemTotal)->auchTotalStatus[5] & EVEN_MDC_BIT1)

	
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
        UCHAR   fbModifier;                     /* 3:tax modifier */
        LONG    lMI;                            /* 4:MI  */
        LONG    lService;                       /* 8:service total */
        LONG    lTax;                           /* 12:whole tax amount */
        UCHAR   aszNumber[NUM_NUMBER];          /* 16:number */
        UCHAR   auchTotalStatus[7];             /* 36:total status,  R3.1 */
        USHORT  fsPrintStatus;                  /* 42:print status */
        USHORT  usOrderNo;                      /* 44:order No */
        UCHAR   uchCdv;                         /* 46:CDV */
        UCHAR   fbStorageStatus;                /* 47:storage status */
        UCHAR   uchLineNo;                      /* 48:line No */
        USHORT  usConsNo;                       /* 49:consecutive No */
        USHORT  usID;                           /* 51:ID(cashier/waiter) */
        SHORT   sTrayCo;                        /* 53:tray counter */
        UCHAR   aszRoomNo[NUM_ROOM];            /* 55:room number */
        UCHAR   aszGuestID[NUM_GUESTID];        /* 61:guest ID */
        UCHAR   aszFolioNumber[NUM_FOLIO];      /* 64:folio number */
        UCHAR   aszPostTransNo[NUM_POSTTRAN];   /* 71:posted transaction number */
        UCHAR   aszCPMsgText[NUM_CPRSPCO][NUM_CPRSPTEXT];   /* 77:message text */
        UCHAR   auchExpiraDate[NUM_EXPIRA];     /* 477:expiration date */
    }ITEMTOTAL;



/*--------------------------------------------------------------------------
*       tender data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR   uchMajorClass;                  /* 1:major class */
        UCHAR   uchMinorClass;                  /* 2:minor class */
        UCHAR   fbModifier;                     /* 3:void */
        LONG    lRound;                         /* 4:round amount */
        ULONG   ulFCRate;                       /* 8:foreign currency rate */
        LONG    lTenderAmount;                  /* 12:tender amount */
        LONG    lForeignAmount;                 /* 16:foreign amount */
        LONG    lBalanceDue;                    /* 20:balance due */
        LONG    lChange;                        /* 24:change@amount */
        UCHAR   aszNumber[NUM_NUMBER];          /* 28:number */
        UCHAR   auchStatus[3];                  /* 48:status */
        USHORT  fsPrintStatus;                  /* 51:print status */
        UCHAR   aszRoomNo[NUM_ROOM];            /* 52:room number */
        UCHAR   aszGuestID[NUM_GUESTID];        /* 58:guest ID */
        UCHAR   aszFolioNumber[NUM_FOLIO];      /* 61:folio number */
        UCHAR   aszPostTransNo[NUM_POSTTRAN];   /* 68:posted transaction number */
        UCHAR   aszCPMsgText[NUM_CPRSPCO_EPT][NUM_CPRSPTEXT];   /* 75:message text, Saratoga */
        UCHAR   fbStorageStatus;                /* 315:storage status */
        UCHAR   auchExpiraDate[NUM_EXPIRA];     /* 316:expiration date */
        UCHAR   auchApproval[NUM_APPROVAL];     /* 320:EPT approval code  */
        UCHAR   uchCPLineNo;                    /* 326:charge posting line number */
        UCHAR   uchPrintSelect;                 /* Print Selection for EPT, Saratoga */
        UCHAR   auchMSRData[NUM_MSRDATA];       /* 327:MSR data */
        UCHAR   aszMnem[NUM_LEADTHRU];          /* Display Mnem for CP, Saratoga  */
        UCHAR   uchTenderOff;                   /* Tender Offset for EPT,           Saratoga */
        UCHAR   uchBuffOff;                     /* Offset of Saved Buffer for EPT,  Saratoga */
		WCHAR	tchAuthCode[NUM_AUTHCODE];		// XEPT authorization Code
		WCHAR	tchRefNum[NUM_REFERENCE];		// XEPT Reference Number
    }ITEMTENDER;



/*--------------------------------------------------------------------------
*       misc. transaction data
--------------------------------------------------------------------------*/        

    typedef struct {
        UCHAR   uchMajorClass;                  /* 1:major class */
        UCHAR   uchMinorClass;                  /* 2:minor class */
        USHORT  usID;                           /* 3:cashier/waiter ID */
        UCHAR   aszNumber[NUM_NUMBER];          /* 5:number */
        USHORT  usGuestNo;                      /* 6:guest check No */
        UCHAR   uchCdv;                         /* 8: check digit */
        LONG    lAmount;                        /* 9:amount */
        UCHAR   fbModifier;                     /* 10:void */
        USHORT  fsPrintStatus;                  /* 11:print status */
        UCHAR   fbStorageStatus;                /* 12:storage status */
        USHORT  usYear;                         /* 13:Year to be time in */
        USHORT  usMonth;                        /* 15:Month to be time in */
        USHORT  usDay;                          /* 17:Day to be time in */
        ULONG   ulEmployeeNo;                   /* 19:Employee Number,   R3.1 */
        UCHAR   uchJobCode;                     /* 21:Job Code to be time in */
        USHORT  usTimeinTime;                   /* 22:Time to be time in */
        USHORT  usTimeinMinute;                 /* 24:Minute to be time in */
        USHORT  usTimeOutTime;                  /* 26:Time to be time out */
        USHORT  usTimeOutMinute;                /* 28:Minute to be time out */
        UCHAR   aszMnemonic[NUM_ETK_NAME];      /* ETK mnemonic,         R3.1 */
        UCHAR   uchTendMinor;                   /* tender minor 2172 */
        LONG    lQTY;                           /* quantity 2172    */
        LONG    lUnitPrice;                     /* unit price 2172  */
        UCHAR   fchStatus;                      /* foreign cuurency status 2172 */
    }ITEMMISC;



/*--------------------------------------------------------------------------
*       modifier data 
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR   uchMajorClass;              /* 1:major class */
        UCHAR   uchMinorClass;              /* 2:minor class */
        USHORT  usTableNo;                  /* 3:table No */
        USHORT  usNoPerson;                 /* 4:No of person */
        UCHAR   uchLineNo;                  /* 5:line No */
        USHORT  fsPrintStatus;              /* 8:print status */
        UCHAR   aszName[NUM_NAME];          /* Customer Name,  R3.0 */
    }ITEMMODIFIER;



/*--------------------------------------------------------------------------
*       other data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR   uchMajorClass;                  /* 1:major class */             
        UCHAR   uchMinorClass;                  /* 2:minor class */
        LONG    lAmount;                        /* 3:amount */
        /* V3.3 */
        UCHAR   uchAction;                      /* action code  */
        UCHAR   uchDayPtd;                      /* daily/ptd    */
        UCHAR   uchCurSave;                     /* current/saved*/
        UCHAR   uchReset;                       /* issued or not*/
/*        ULONG   ulNumber;                     / ind. number  */
        UCHAR   uchError;                       /* error code   */
        UCHAR   uchYear;                        /* year         */
        UCHAR   uchMonth;                       /* month        */
        UCHAR   uchDay;                         /* day          */
        UCHAR   uchHour;                        /* hour         */
        UCHAR   uchMin;                         /* minute       */
        UCHAR   aszMnemonic[24];                /* mnemonics    */
        USHORT  fsPrintStatus;                  /* 46:print status */
        UCHAR   auchMSRData[NUM_MSRDATA];       /* MSR data,    2172   */
        USHORT  husHandle;                      /* Tmp File Handle, 2172 */
        UCHAR   aszNumber[NUM_NUMBER];          /* number,  Saratoga */
    }ITEMOTHER;



/*--------------------------------------------------------------------------
*       affection data
--------------------------------------------------------------------------*/

    typedef struct {
        LONG        lVATRate;                       /* VAT rate */
        LONG        lVATable;                       /* VATable amount */
        LONG        lVATAmt;                        /* VAT amount */
        LONG        lSum;                           /* sum of VATable & VAT */
        LONG        lAppAmt;                        /* VAT applied total amount */
    }ITEMVAT;

    typedef struct {
        LONG        lTaxable[5];                        /* 7: taxable total */
        LONG        lTaxAmount[4];                      /* 9: tax amount */
        LONG        lTaxExempt[4];                      /*11: tax exempt */
    } USCANTAX;

    typedef union {
        USCANTAX    USCanTax;
        ITEMVAT     ItemVAT[3];
    } USCANVAT;

    typedef struct {
        UCHAR       uchMajorClass;                      /* 1:major class */
        UCHAR       uchMinorClass;                      /* 2:minor class */
        LONG        lAmount;                            /* 3:amount, VAT Service, V3.3 */
        SHORT       sItemCounter;                       /* 4:item counter */
        SHORT       sNoPerson;                          /* 5:No of person */
        SHORT       sCheckOpen;                         /*   check open counter */
        UCHAR       uchOffset;                          /* 6:hourly block offset */
        UCHAR       fbModifier;                         /*   taxable modifier */
        USCANVAT    USCanVAT;
        LONG        lService[3];                        /*13: service total */
        LONG        lCancel;                            /*14: cancel total */
        USHORT      usId;       /*15: cashier/waiter ID/Service Time Border(R3.1) */
        USHORT      usGuestNo;                          /*16: guest check No */
        UCHAR       uchTotalID;                         /*17: totalkey Id */
        USHORT      fsPrintStatus;                      /*18:print status */
        UCHAR       fbStorageStatus;                    /*19: storage status */
        UCHAR       uchAddNum;                          /*21: addcheck number */
        UCHAR       uchStatus;                          /* 18:MDC status */
        LONG        lBonus[STD_NO_BONUS_TTL];                          /* bonus total for hourly, V3.3 */
    }ITEMAFFECTION;



/*--------------------------------------------------------------------------
*       print data
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR   uchMajorClass;                  /* 1:major class */
        UCHAR   uchMinorClass;                  /* 2:minor class */
        USHORT  usID;                           /* 3:waiter/cashier ID */
        ULONG   ulStoreregNo;                   /* 5:store/reg No */
        USHORT  usConsNo;                       /* 9:consecutive No */
        USHORT  fsPrintStatus;                  /* 11:print status */
        UCHAR   fbStorageStatus;                /* 12:storage status */
        USHORT  usTableNo;                      /* 13:table No */
        USHORT  usGuestNo;                      /* 15:GCF No */
        UCHAR   uchCdv;                         /* 17:CDV */
        UCHAR   uchLineNo;                      /* 18:line No */
        UCHAR   aszNumber[NUM_NUMBER];          /* 280:number        *add EPT, saratoga */
        UCHAR   aszRoomNo[NUM_ROOM];            /* 19:room number */
        UCHAR   aszGuestID[NUM_GUESTID];        /* 25:guest ID */
        UCHAR   aszCPText[NUM_CPRSPCO][NUM_CPRSPTEXT];  /* 28:charge posting response text */
        USHORT  usFuncCode;                     /* 268:function code */
        SHORT   sErrorCode;                     /* 270:error code */
        LONG    lAmount;                        /* 272:amount        *add EPT*/
        UCHAR   uchStatus;                      /* 278:EPT status    *add EPT*/
        UCHAR   fbModifier;                     /* 279:Modifier      *add EPT*/
        UCHAR   auchExpiraDate[NUM_EXPIRA];     /* 300:ExpDate       *add EPT*/
        UCHAR   auchApproval[NUM_APPROVAL];     /* 304:approval code *add EPT*/
		WCHAR	tchAuthCode[NUM_AUTHCODE];		// XEPT authorization Code
		WCHAR	tchRefNum[NUM_REFERENCE];		// XEPT Reference Number
    }ITEMPRINT;



/*--------------------------------------------------------------------------
*       operator close data 
--------------------------------------------------------------------------*/

    typedef struct {
        UCHAR   uchMajorClass;              /* 1:major class */
        UCHAR   uchMinorClass;              /* 2:minor class */
        USHORT  usID;                       /* 3:cashier/waiter ID */
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
--------------------------------------------------------------------------*/

    typedef struct{
        UCHAR   uchMajorClass;                  /* 1:major class */
        UCHAR   uchMinorClass;                  /* 2:minor class */
        LONG    lService[3];                    /* 4: service total */
        USHORT  usGuestNo;                      /* 5: guest check No */
        UCHAR   uchCdv;                         /*      CDV     */
        LONG    lMI;                            /*    main itemizer */
        USHORT  fsPrintStatus;                  /* 6:print status */
        UCHAR   fbStorageStatus;                /* 7: storage status */
    }ITEMMULTI;


/*--------------------------------------------------------------------------
*       EPT data
--------------------------------------------------------------------------*/

    typedef struct{
        UCHAR   auchSTX[1];                  /* 1: start of text */
        UCHAR   auchTransID[8];              /* 2: transaction ID */
        UCHAR   auchActionCode[3];           /* 10: action code */
        UCHAR   auchAcctNumber[20];          /* 13: account number */
        UCHAR   auchExpiraDate[4];           /* 43: account expiration date */
        UCHAR   auchChargeTtl[8];            /* 51: charge total */
        UCHAR   auchAuthoCode[6];            /* 57: authorization code */
        UCHAR   auchTransType[3];            /* 63: transaction type */
        UCHAR   auchProductCode[10];         /* 66: product code */
        UCHAR   auchETX[1];                  /* 76: end of text */
        UCHAR   auchLRC[1];                  /* 77: LRC data */
    }ITEMEPTREQDATA;

    typedef struct{
        UCHAR   auchSTX[1];                  /* 1: start of text */
        UCHAR   auchTransID[8];              /* 2: transaction ID */
        UCHAR   auchActionCode[3];           /* 10: action code */
        UCHAR   auchAcctNumber[20];          /* 13: account number */
        UCHAR   auchExpiraDate[4];           /* 33: account expiration date */
        UCHAR   auchReplyData[18];           /* 37: reply data from host */
        UCHAR   auchETX[1];                  /* 55: end of text */
        UCHAR   auchLRC[1];                  /* 56: LRC data */
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

/*--------------------------------------------------------------------------
*       Create Temporary File for CP/EPT,               Saratoga
--------------------------------------------------------------------------*/
#define ITM_FILE_OVERRIDE           0           /* Override */
#define ITM_FILE_APPEND             1           /* Append   */

#define ITM_MAX_GUEST_REC           25          /* Max Record of Guest Name */

#define ITM_FILE1_SIZE              (512 * 4)   /* Tmp#1 File Size */
#define ITM_FILE2_SIZE              (512 * 3)   /* Tmp#2 File Size */
#define ITM_FILE3_SIZE              (512 * 8)   /* Tmp#3 File Size */

#define RSP_SAVE_MAX                3           /* EPT/COM response saxe max R2.0GCA */


extern  UCHAR FARCONST      aszItemCPTmpFile3[];    /* Tmp#3 File Name */
extern  SHORT               hsItemCPHandle2;        /* Temporary File Handle#2 */
extern  SHORT               hsItemCPHandle3;        /* Temporary File Handle#3 */

extern  SHORT   ItemTendEPTComm(UCHAR uchFuncCode, UIFREGTENDER *UifRegTender,
                        ITEMTENDER *ItemTender, USHORT usNo);       /* Saratoga */

SHORT   ItemTendCPCreateFile(VOID);
SHORT   ItemTendCPOpenFile(VOID);

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/****** End of Definition ******/