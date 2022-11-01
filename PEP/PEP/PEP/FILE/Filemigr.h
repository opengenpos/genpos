/*
* ---------------------------------------------------------------------------
* Title         :   Header File of File Migration Module
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILEMIGR.H
* Copyright (C) :   1995 NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : The only File for PEP included this header file. This header
*           declares Structure Type Define Declarations, Define Declarations
*           and Function Prototype Declarations to use the functions of the
*           File for PEP.
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Mame     : Description
* Jun-26-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Apr-25-95 : 03.00.00 : H.Ishida   : R3.0
* Sep-28-98 : 03.03.00 : A.Mitsui   : V3.3
* Jan-18-00 : 01.00.00 : K.Yanagida : Saratoga
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
*       Value definition
* ===========================================================================
*/
#define FILE_MIGR_NUM_PARA         51   /* No. of parameter class       */ /* V3.3 45->46 */
#define FILE_MIGR_NUM_LIST         43   /* No. of list item  */            /* V3.3 40->43 */
#define FILE_MIGR_MULTIPLY          1   /* size of small dialogbox      */
#define FILE_MIGR_DIVIDE            2   /* size of small dialogbox      */
#define FILE_MIGR_ICONX            20   /* x position of question icon  */
#define FILE_MIGR_ICONY            30   /* y position of question icon  */
#define FILE_MIGR_TAXHED_OLD        6   /* old tax table header size    */
#define FILE_MIGR_TAXHED_NEW        8   /* new tax table header size    */
#define FILE_MIGR_BUFF_LEN         64   /* length of buffer             */

#define FLEXMEM_SIZE_GP             3   /* FLEXMEM Struct size of GP, Saratoga */
#define MAX_FLXMEM_SIZE_GP           9   /* MAX_FLXMEM_SIZE of GP, Saratoga     */

#define FILE_MIGR_PARA_MDC          0   /* (P1) Machine Definition Code    */
#define FILE_MIGR_PARA_FLEXMEM      1   /* parameter class offset       */
#define FILE_MIGR_PARA_FSC          2   /* parameter class offset       */
#define FILE_MIGR_PARA_SECNO        3   /* parameter class offset       */
#define FILE_MIGR_PARA_STRNO        4   /* parameter class offset       */
#define FILE_MIGR_PARA_SUPLV        5   /* parameter class offset       */
#define FILE_MIGR_PARA_ACSEC        6   /* parameter class offset       */
#define FILE_MIGR_PARA_HALO         7   /* parameter class offset       */
#define FILE_MIGR_PARA_HOURLY       8   /* parameter class offset       */
#define FILE_MIGR_PARA_SLIPFEED     9   /* parameter class offset       */
#define FILE_MIGR_PARA_TRANSMNE    10   /* parameter class offset       */
#define FILE_MIGR_PARA_LEADTHRU    11   /* parameter class offset       */
#define FILE_MIGR_PARA_RPTNAME     12   /* parameter class offset       */
#define FILE_MIGR_PARA_SPEMNE      13   /* parameter class offset       */
#define FILE_MIGR_PARA_PCIF        14   /* parameter class offset       */
#define FILE_MIGR_PARA_ADJMNE      15   /* parameter class offset       */
#define FILE_MIGR_PARA_PMOD        16   /* parameter class offset       */
#define FILE_MIGR_PARA_MDEPT       17   /* parameter class offset       */
#define FILE_MIGR_PARA_AUTOKTCH    18   /* parameter class offset       */
#define FILE_MIGR_PARA_CPM         19   /* parameter class offset       */
#define FILE_MIGR_PARA_CHAR24      20   /* parameter class offset       */
#define FILE_MIGR_PARA_TKTYP       21   /* parameter class offset       */
#define FILE_MIGR_PARA_TKCTL       22   /* parameter class offset       */
#define FILE_MIGR_PARA_TEND        23   /* parameter class offset       */ /* V3.3 */
#define FILE_MIGR_PARA_PLUKEY      24   /* parameter class offset       */
#define FILE_MIGR_PARA_CTLMENU     25   /* parameter class offset       */
#define FILE_MIGR_PARA_MANUKTCH    26   /* parameter class offset       */
#define FILE_MIGR_PARA_CASHAB      27   /* parameter class offset       */
#define FILE_MIGR_PARA_ROUND       28   /* parameter class offset       */
#define FILE_MIGR_PARA_DISC        29   /* parameter class offset       */
#define FILE_MIGR_PARA_PROMOT      30   /* parameter class offset       */
#define FILE_MIGR_PARA_FC          31   /* parameter class offset       */
#define FILE_MIGR_PARA_TONE        32   /* parameter class offset       */
#define FILE_MIGR_PARA_MENUPLU     33   /* parameter class offset       */
#define FILE_MIGR_PARA_TAXTBL      34   /* parameter class offset       */
#define FILE_MIGR_PARA_TAXRATE     35   /* parameter class offset       */
#define FILE_MIGR_PARA_TARE        36   /* parameter class offset       */
#define FILE_MIGR_PARA_PRECASH     37                        /* (P15) Preset Amount For Preset Cash Key */
#define FILE_MIGR_PARA_SHARED      38                        /* (P50) Assignment Terminal # for Shared Printerset */
#define FILE_MIGR_PARA_SOFT        39                        /* (A87) Set Trailer Print For Soft Check  */
#define FILE_MIGR_PARA_PIGRULE     40                        /* (A130) Set Pig Rule  */
#define FILE_MIGR_PARA_OEP         41                        /* (A160) Set Order Entry Prompt  */
#define FILE_MIGR_PARA_FDT         42                        /* (A162) Set Flexible Drive Through  */
    /* add V3.1 parameters */
#define FILE_MIGR_PARA_SERVICE     43                        /* (A133) Set Service Time Parameter Table */
#define FILE_MIGR_PARA_LABOR       44                        /* (A150) SET LABOR COST VALUE */
#define FILE_MIGR_PARA_DISPEN      45                        /* (A137) SET DISPENSER PARAMETER */

#define FILE_MIGR_PARA_MLDMNE	46		//(P65) Multi-Line Display Mnemonics

/* ----- parameter list offset ----- */

#define FILE_MIGR_LIST_CONFIG       0   /* (File) Configuration            */
#define FILE_MIGR_LIST_MDC          1   /* (P1) Machine Definition Code    */
#define FILE_MIGR_LIST_FLEXMEM      2   /* (P2) Flexible Memory Assignment */
#define FILE_MIGR_LIST_FSC          3   /* (P3) Function Selection Code    */
#define FILE_MIGR_LIST_SECNO        4   /* (P6) Program Mode Security Code */
#define FILE_MIGR_LIST_SUPLV        5   /* (P8) Supervisor Number/Level    */
#define FILE_MIGR_LIST_ACSEC        6   /* (P9) Action Code Security       */
#define FILE_MIGR_LIST_HALO         7   /* (P10) High Amount Lock-Out Limit*/
#define FILE_MIGR_LIST_PRECASH      8   /* (P15) Preset Amount For Preset Cash Key */
#define FILE_MIGR_LIST_HOURLY       9   /* (P17) Hourly Activity Block     */
#define FILE_MIGR_LIST_SLIPFEED    10   /* (P18) Slip/EJ Print Control     */
#define FILE_MIGR_LIST_TRANSMNE    11   /* (P20) Transaction Mnemonics     */
#define FILE_MIGR_LIST_LEADTHRU    12   /* (P21) Lead-Through Messages     */
#define FILE_MIGR_LIST_RPTNAME     13   /* (P22) Report Mnemonics          */
#define FILE_MIGR_LIST_SPEMNE      14   /* (P23) Special Mnemonics         */
#define FILE_MIGR_LIST_ADJMNE      15   /* (P46) Adjective Mnemonics       */
#define FILE_MIGR_LIST_PMOD        16   /* (P47) Print Modifier Mnemonics  */
#define FILE_MIGR_LIST_MDEPT       17   /* (P48) Major Department Mnemonics*/
#define FILE_MIGR_LIST_AUTOKTCH    18   /* (P49) Automatic Alternative Remote Printer */
#define FILE_MIGR_LIST_SHARED      19   /* (P50) Assignment Terminal # for Shared Printerset */
#define FILE_MIGR_LIST_CPMEPT      20   /* (P54) Charge Post/EPT Parameters*/
#define FILE_MIGR_LIST_CHAR24      21   /* (P57) Header/Special Messages   */
#define FILE_MIGR_LIST_TKTYP       22   /* (P60) Total Key Type            */
#define FILE_MIGR_LIST_TKCTL       23   /* (P61) Total Key Function        */
#define FILE_MIGR_LIST_PLUKEY      24   /* (A4) Set PLU No. on Menu Page   */
#define FILE_MIGR_LIST_CTLMENU     25   /* (A5) Set Menu Page Control      */
#define FILE_MIGR_LIST_CASHAB      26   /* (A7) Assign Cashier A/B Key     */
#define FILE_MIGR_LIST_ROUND       27   /* (A84) Set Rounding Table        */
#define FILE_MIGR_LIST_DISC        28   /* (A86) Set Discount/&Bonus % Rates      */
#define FILE_MIGR_LIST_SOFT        29   /* (A87) Set Trailer Print For Soft Check */
#define FILE_MIGR_LIST_PROMOT      30   /* (A88) Set Sales Promotion Message      */
#define FILE_MIGR_LIST_FC          31   /* (A89) Set Currency Conversion Rates    */
#define FILE_MIGR_LIST_TARE        32   /* (A111) Set Tare Table           */
#define FILE_MIGR_LIST_MENUPLU     33   /* (A116) Set Promotional PLU      */
#define FILE_MIGR_LIST_TAXTBL      34   /* (A124,125,126) Set Tax Tables   */
#define FILE_MIGR_LIST_TAXRATE     35   /* (A127) Set Tax Rates            */
#define FILE_MIGR_LIST_PIGRULE     36   /* (A130) Set Pig Rule             */
#define FILE_MIGR_LIST_SERVICE     37   /* (A133) Set Service Time Parameter Table , V3.3*/
#define FILE_MIGR_LIST_DISPEN      38   /* (A137) SET DISPENSER PARAMETER , V3.3 */
#define FILE_MIGR_LIST_LABOR       39   /* (A150) SET LABOR COST VALUE , V3.3 */
#define FILE_MIGR_LIST_OEP         40   /* (A160) Set Order Entry Prompt   */
#define FILE_MIGR_LIST_FDT         41   /* (A162) Set Flexible Drive Through */

#define FILE_MIGR_LIST_DIRECT      42   /* (Transfer) Terminal Directory   */

#define FILE_MIGR_ONE_LINE          1   /* scroll to one line up/down   */
#define FILE_MIGR_MAX_PAGE          7   /* No. of item in one page      */
#define FILE_MIGR_ONE_PAGE    (FILE_MIGR_MAX_PAGE - 1)
                                        /* scroll to one page up/down   */
#define FILE_MIGR_SCRL_MIN          0   /* min. value of scroll range   */
#define FILE_MIGR_SCRL_MAX   (FILE_MIGR_NUM_LIST - FILE_MIGR_MAX_PAGE)
                                        /* max. value of scroll range   */
#define FILE_MIGR_SCROLL            0   /* scroll process expected      */
#define FILE_MIGR_NOT_SCROLL        1   /* default process expected     */

#define FILE_MIGR2_NUM_PARA         FILE_MIGR_NUM_PARA    /* No. of parameter class       */
#define FILE_MIGR2_NUM_LIST         FILE_MIGR_NUM_LIST    /* No. of list item  */
#define FILE_MIGR2_MULTIPLY         FILE_MIGR_MULTIPLY    /* size of small dialogbox      */
#define FILE_MIGR2_DIVIDE           FILE_MIGR_DIVIDE      /* size of small dialogbox      */
#define FILE_MIGR2_ICONX            FILE_MIGR_ICONX       /* x position of question icon  */
#define FILE_MIGR2_ICONY            FILE_MIGR_ICONY       /* y position of question icon  */
#define FILE_MIGR2_TAXHED_OLD       FILE_MIGR_TAXHED_OLD  /* old tax table header size    */
#define FILE_MIGR2_TAXHED_NEW       FILE_MIGR_TAXHED_NEW  /* new tax table header size    */
#define FILE_MIGR2_BUFF_LEN         FILE_MIGR_BUFF_LEN    /* length of buffer             */

/* ----- parameter class offset ----- */

#define FILE_MIGR2_PARA_MDC         FILE_MIGR_PARA_MDC        /* (P1) Machine Definition Code    */
#define FILE_MIGR2_PARA_FLEXMEM     FILE_MIGR_PARA_FLEXMEM    /* (P2) Flexible Memory Assignment */
#define FILE_MIGR2_PARA_FSC         FILE_MIGR_PARA_FSC        /* (P3) Function Selection Code    */
#define FILE_MIGR2_PARA_SECNO       FILE_MIGR_PARA_SECNO      /* (P6) Program Mode Security Code */
#define FILE_MIGR2_PARA_STRNO       FILE_MIGR_PARA_STRNO      /* (P7) Store/Register No          */
#define FILE_MIGR2_PARA_SUPLV       FILE_MIGR_PARA_SUPLV      /* (P8) Supervisor Number/Level    */
#define FILE_MIGR2_PARA_ACSEC       FILE_MIGR_PARA_ACSEC      /* (P9) Action Code Security       */
#define FILE_MIGR2_PARA_HALO        FILE_MIGR_PARA_HALO       /* (P10) High Amount Lock-Out Limit*/
#define FILE_MIGR2_PARA_HOURLY      FILE_MIGR_PARA_HOURLY     /* (P17) Hourly Activity Block     */
#define FILE_MIGR2_PARA_SLIPFEED    FILE_MIGR_PARA_SLIPFEED   /* (P18) Slip/EJ Print Control     */
#define FILE_MIGR2_PARA_TRANSMNE    FILE_MIGR_PARA_TRANSMNE   /* (P20) Transaction Mnemonics     */
#define FILE_MIGR2_PARA_LEADTHRU    FILE_MIGR_PARA_LEADTHRU   /* (P21) Lead-Through Messages     */
#define FILE_MIGR2_PARA_RPTNAME     FILE_MIGR_PARA_RPTNAME    /* (P22) Report Mnemonics          */
#define FILE_MIGR2_PARA_SPEMNE      FILE_MIGR_PARA_SPEMNE     /* (P23) Special Mnemonics         */
#define FILE_MIGR2_PARA_PCIF        FILE_MIGR_PARA_PCIF       /* (P39) PC I/F */
#define FILE_MIGR2_PARA_ADJMNE      FILE_MIGR_PARA_ADJMNE     /* (P46) Adjective Mnemonics       */
#define FILE_MIGR2_PARA_PMOD        FILE_MIGR_PARA_PMOD       /* (P47) Print Modifier Mnemonics  */
#define FILE_MIGR2_PARA_MDEPT       FILE_MIGR_PARA_MDEPT      /* (P48) Major Department Mnemonics*/
#define FILE_MIGR2_PARA_AUTOKTCH    FILE_MIGR_PARA_AUTOKTCH   /* (P49) Automatic Alternative Remote Printer */
#define FILE_MIGR2_PARA_CPMEPT      FILE_MIGR_PARA_CPM        /* (P54) Charge Post/EPT Parameters*/
#define FILE_MIGR2_PARA_CHAR24      FILE_MIGR_PARA_CHAR24     /* (P57) Header/Special Messages   */
#define FILE_MIGR2_PARA_TKTYP       FILE_MIGR_PARA_TKTYP      /* (P60) Total Key Type            */
#define FILE_MIGR2_PARA_TKCTL       FILE_MIGR_PARA_TKCTL      /* (P61) Total Key Function        */
#define FILE_MIGR2_PARA_PLUKEY      FILE_MIGR_PARA_PLUKEY     /* (A4) Set PLU No. on Menu Page   */
#define FILE_MIGR2_PARA_CTLMENU     FILE_MIGR_PARA_CTLMENU    /* (A5) Set Menu Page Control      */
#define FILE_MIGR2_PARA_MANUKTCH    FILE_MIGR_PARA_MANUKTCH   /* (A6) Manual Alternative Kitchen Printer */
#define FILE_MIGR2_PARA_CASHAB      FILE_MIGR_PARA_CASHAB     /* (A7) Assign Cashier A/B Key     */
#define FILE_MIGR2_PARA_ROUND       FILE_MIGR_PARA_ROUND      /* (A84) Set Rounding Table        */
#define FILE_MIGR2_PARA_DISC        FILE_MIGR_PARA_DISC       /* (A86) Set Discount/&Bonus % Rates      */
#define FILE_MIGR2_PARA_PROMOT      FILE_MIGR_PARA_PROMOT     /* (A88) Set Sales Promotion Message      */
#define FILE_MIGR2_PARA_FC          FILE_MIGR_PARA_FC         /* (A89) Set Currency Conversion Rates    */
#define FILE_MIGR2_PARA_TONE        FILE_MIGR_PARA_TONE       /* (A169) Set Tone Volume          */
#define FILE_MIGR2_PARA_MENUPLU     FILE_MIGR_PARA_MENUPLU    /* (A116) Set Promotional PLU      */
#define FILE_MIGR2_PARA_TAXTBL      FILE_MIGR_PARA_TAXTBL     /* (A124,125,126) Set Tax Tables   */
#define FILE_MIGR2_PARA_TAXRATE     FILE_MIGR_PARA_TAXRATE    /* (A127) Set Tax Rates            */
#define FILE_MIGR2_PARA_TARE        FILE_MIGR_PARA_TARE       /* (A111) Set Tare Table           */
#define FILE_MIGR2_PARA_PRECASH     37                        /* (P15) Preset Amount For Preset Cash Key */
#define FILE_MIGR2_PARA_SHARED      38                        /* (P50) Assignment Terminal # for Shared Printerset */
#define FILE_MIGR2_PARA_SOFT        39                        /* (A87) Set Trailer Print For Soft Check  */
#define FILE_MIGR2_PARA_PIGRULE     40                        /* (A130) Set Pig Rule  */
#define FILE_MIGR2_PARA_OEP         41                        /* (A160) Set Order Entry Prompt  */
#define FILE_MIGR2_PARA_FDT         42                        /* (A162) Set Flexible Drive Through  */

/* ----- parameter list offset ----- */

#define FILE_MIGR2_LIST_CONFIG      FILE_MIGR_LIST_CONFIG     /* (File) Configuration            */
#define FILE_MIGR2_LIST_MDC         FILE_MIGR_LIST_MDC        /* (P1) Machine Definition Code    */
#define FILE_MIGR2_LIST_FLEXMEM     FILE_MIGR_LIST_FLEXMEM    /* (P2) Flexible Memory Assignment */
#define FILE_MIGR2_LIST_FSC         FILE_MIGR_LIST_FSC        /* (P3) Function Selection Code    */
#define FILE_MIGR2_LIST_SECNO       FILE_MIGR_LIST_SECNO      /* (P6) Program Mode Security Code */
#define FILE_MIGR2_LIST_SUPLV       FILE_MIGR_LIST_SUPLV      /* (P8) Supervisor Number/Level    */
#define FILE_MIGR2_LIST_ACSEC       FILE_MIGR_LIST_ACSEC      /* (P9) Action Code Security       */
#define FILE_MIGR2_LIST_HALO        FILE_MIGR_LIST_HALO       /* (P10) High Amount Lock-Out Limit*/
#define FILE_MIGR2_LIST_PRECASH     FILE_MIGR_LIST_PRECASH    /* (P15) Preset Amount For Preset Cash Key */
#define FILE_MIGR2_LIST_HOURLY      FILE_MIGR_LIST_HOURLY     /* (P17) Hourly Activity Block     */
#define FILE_MIGR2_LIST_SLIPFEED    FILE_MIGR_LIST_SLIPFEED   /* (P18) Slip/EJ Print Control     */
#define FILE_MIGR2_LIST_TRANSMNE    FILE_MIGR_LIST_TRANSMNE   /* (P20) Transaction Mnemonics     */
#define FILE_MIGR2_LIST_LEADTHRU    FILE_MIGR_LIST_LEADTHRU   /* (P21) Lead-Through Messages     */
#define FILE_MIGR2_LIST_RPTNAME     FILE_MIGR_LIST_RPTNAME    /* (P22) Report Mnemonics          */
#define FILE_MIGR2_LIST_SPEMNE      FILE_MIGR_LIST_SPEMNE     /* (P23) Special Mnemonics         */
#define FILE_MIGR2_LIST_ADJMNE      FILE_MIGR_LIST_ADJMNE     /* (P46) Adjective Mnemonics       */
#define FILE_MIGR2_LIST_PMOD        FILE_MIGR_LIST_PMOD       /* (P47) Print Modifier Mnemonics  */
#define FILE_MIGR2_LIST_MDEPT       FILE_MIGR_LIST_MDEPT      /* (P48) Major Department Mnemonics*/
#define FILE_MIGR2_LIST_AUTOKTCH    FILE_MIGR_LIST_AUTOKTCH   /* (P49) Automatic Alternative Remote Printer */
#define FILE_MIGR2_LIST_SHARED      FILE_MIGR_LIST_SHARED     /* (P50) Assignment Terminal # for Shared Printerset */
#define FILE_MIGR2_LIST_CPMEPT      FILE_MIGR_LIST_CPMEPT     /* (P54) Charge Post/EPT Parameters*/
#define FILE_MIGR2_LIST_CHAR24      FILE_MIGR_LIST_CHAR24     /* (P57) Header/Special Messages   */
#define FILE_MIGR2_LIST_TKTYP       FILE_MIGR_LIST_TKTYP      /* (P60) Total Key Type            */
#define FILE_MIGR2_LIST_TKCTL       FILE_MIGR_LIST_TKCTL      /* (P61) Total Key Function        */
#define FILE_MIGR2_LIST_PLUKEY      FILE_MIGR_LIST_PLUKEY     /* (A4) Set PLU No. on Menu Page   */
#define FILE_MIGR2_LIST_CTLMENU     FILE_MIGR_LIST_CTLMENU    /* (A5) Set Menu Page Control      */
#define FILE_MIGR2_LIST_CASHAB      FILE_MIGR_LIST_CASHAB     /* (A7) Assign Cashier A/B Key     */
#define FILE_MIGR2_LIST_ROUND       FILE_MIGR_LIST_ROUND      /* (A84) Set Rounding Table        */
#define FILE_MIGR2_LIST_DISC        FILE_MIGR_LIST_DISC       /* (A86) Set Discount/&Bonus % Rates      */
#define FILE_MIGR2_LIST_SOFT        FILE_MIGR_LIST_SOFT       /* (A87) Set Trailer Print For Soft Check */
#define FILE_MIGR2_LIST_PROMOT      FILE_MIGR_LIST_PROMOT     /* (A88) Set Sales Promotion Message      */
#define FILE_MIGR2_LIST_FC          FILE_MIGR_LIST_FC         /* (A89) Set Currency Conversion Rates    */
#define FILE_MIGR2_LIST_TARE        FILE_MIGR_LIST_TARE       /* (A111) Set Tare Table           */
#define FILE_MIGR2_LIST_MENUPLU     FILE_MIGR_LIST_MENUPLU    /* (A116) Set Promotional PLU      */
#define FILE_MIGR2_LIST_TAXTBL      FILE_MIGR_LIST_TAXTBL     /* (A124,125,126) Set Tax Tables   */
#define FILE_MIGR2_LIST_TAXRATE     FILE_MIGR_LIST_TAXRATE    /* (A127) Set Tax Rates            */
#define FILE_MIGR2_LIST_PIGRULE     FILE_MIGR_LIST_PIGRULE    /* (A130) Set Pig Rule             */
#define FILE_MIGR2_LIST_OEP         FILE_MIGR_LIST_OEP        /* (A160) Set Order Entry Prompt   */
#define FILE_MIGR2_LIST_FDT         FILE_MIGR_LIST_FDT        /* (A162) Set Flexible Drive Through */
#define FILE_MIGR2_LIST_DIRECT      FILE_MIGR_LIST_DIRECT     /* (Transfer) Terminal Directory   */

#define FILE_MIGR2_ONE_LINE          1   /* scroll to one line up/down   */
#define FILE_MIGR2_MAX_PAGE          7   /* No. of item in one page      */
#define FILE_MIGR2_ONE_PAGE    (FILE_MIGR2_MAX_PAGE - 1)
                                        /* scroll to one page up/down   */
#define FILE_MIGR2_SCRL_MIN          0   /* min. value of scroll range   */
#define FILE_MIGR2_SCRL_MAX   (FILE_MIGR2_NUM_LIST - FILE_MIGR2_MAX_PAGE)
                                        /* max. value of scroll range   */
#define FILE_MIGR2_SCROLL            0   /* scroll process expected      */
#define FILE_MIGR2_NOT_SCROLL        1   /* default process expected     */

#define FILE_MIGR25_NUM_PARA         FILE_MIGR_NUM_PARA    /* No. of parameter class       */
#define FILE_MIGR25_NUM_LIST         FILE_MIGR_NUM_LIST    /* No. of list item  */
#define FILE_MIGR25_MULTIPLY         FILE_MIGR_MULTIPLY    /* size of small dialogbox      */
#define FILE_MIGR25_DIVIDE           FILE_MIGR_DIVIDE      /* size of small dialogbox      */
#define FILE_MIGR25_ICONX            FILE_MIGR_ICONX       /* x position of question icon  */
#define FILE_MIGR25_ICONY            FILE_MIGR_ICONY       /* y position of question icon  */
#define FILE_MIGR25_TAXHED_OLD       FILE_MIGR_TAXHED_OLD  /* old tax table header size    */
#define FILE_MIGR25_TAXHED_NEW       FILE_MIGR_TAXHED_NEW  /* new tax table header size    */
#define FILE_MIGR25_BUFF_LEN         FILE_MIGR_BUFF_LEN    /* length of buffer             */

/* ----- parameter class offset ----- */

#define FILE_MIGR25_PARA_MDC         FILE_MIGR_PARA_MDC        /* (P1) Machine Definition Code    */
#define FILE_MIGR25_PARA_FLEXMEM     FILE_MIGR_PARA_FLEXMEM    /* (P2) Flexible Memory Assignment */
#define FILE_MIGR25_PARA_FSC         FILE_MIGR_PARA_FSC        /* (P3) Function Selection Code    */
#define FILE_MIGR25_PARA_SECNO       FILE_MIGR_PARA_SECNO      /* (P6) Program Mode Security Code */
#define FILE_MIGR25_PARA_STRNO       FILE_MIGR_PARA_STRNO      /* (P7) Store/Register No          */
#define FILE_MIGR25_PARA_SUPLV       FILE_MIGR_PARA_SUPLV      /* (P8) Supervisor Number/Level    */
#define FILE_MIGR25_PARA_ACSEC       FILE_MIGR_PARA_ACSEC      /* (P9) Action Code Security       */
#define FILE_MIGR25_PARA_HALO        FILE_MIGR_PARA_HALO       /* (P10) High Amount Lock-Out Limit*/
#define FILE_MIGR25_PARA_HOURLY      FILE_MIGR_PARA_HOURLY     /* (P17) Hourly Activity Block     */
#define FILE_MIGR25_PARA_SLIPFEED    FILE_MIGR_PARA_SLIPFEED   /* (P18) Slip/EJ Print Control     */
#define FILE_MIGR25_PARA_TRANSMNE    FILE_MIGR_PARA_TRANSMNE   /* (P20) Transaction Mnemonics     */
#define FILE_MIGR25_PARA_LEADTHRU    FILE_MIGR_PARA_LEADTHRU   /* (P21) Lead-Through Messages     */
#define FILE_MIGR25_PARA_RPTNAME     FILE_MIGR_PARA_RPTNAME    /* (P22) Report Mnemonics          */
#define FILE_MIGR25_PARA_SPEMNE      FILE_MIGR_PARA_SPEMNE     /* (P23) Special Mnemonics         */
#define FILE_MIGR25_PARA_PCIF        FILE_MIGR_PARA_PCIF       /* (P39) PC I/F */
#define FILE_MIGR25_PARA_ADJMNE      FILE_MIGR_PARA_ADJMNE     /* (P46) Adjective Mnemonics       */
#define FILE_MIGR25_PARA_PMOD        FILE_MIGR_PARA_PMOD       /* (P47) Print Modifier Mnemonics  */
#define FILE_MIGR25_PARA_MDEPT       FILE_MIGR_PARA_MDEPT      /* (P48) Major Department Mnemonics*/
#define FILE_MIGR25_PARA_AUTOKTCH    FILE_MIGR_PARA_AUTOKTCH   /* (P49) Automatic Alternative Remote Printer */
#define FILE_MIGR25_PARA_CPMEPT      FILE_MIGR_PARA_CPM        /* (P54) Charge Post/EPT Parameters*/
#define FILE_MIGR25_PARA_CHAR24      FILE_MIGR_PARA_CHAR24     /* (P57) Header/Special Messages   */
#define FILE_MIGR25_PARA_TKTYP       FILE_MIGR_PARA_TKTYP      /* (P60) Total Key Type            */
#define FILE_MIGR25_PARA_TKCTL       FILE_MIGR_PARA_TKCTL      /* (P61) Total Key Function        */
#define FILE_MIGR25_PARA_PLUKEY      FILE_MIGR_PARA_PLUKEY     /* (A4) Set PLU No. on Menu Page   */
#define FILE_MIGR25_PARA_CTLMENU     FILE_MIGR_PARA_CTLMENU    /* (A5) Set Menu Page Control      */
#define FILE_MIGR25_PARA_MANUKTCH    FILE_MIGR_PARA_MANUKTCH   /* (A6) Manual Alternative Kitchen Printer */
#define FILE_MIGR25_PARA_CASHAB      FILE_MIGR_PARA_CASHAB     /* (A7) Assign Cashier A/B Key     */
#define FILE_MIGR25_PARA_ROUND       FILE_MIGR_PARA_ROUND      /* (A84) Set Rounding Table        */
#define FILE_MIGR25_PARA_DISC        FILE_MIGR_PARA_DISC       /* (A86) Set Discount/&Bonus % Rates      */
#define FILE_MIGR25_PARA_PROMOT      FILE_MIGR_PARA_PROMOT     /* (A88) Set Sales Promotion Message      */
#define FILE_MIGR25_PARA_FC          FILE_MIGR_PARA_FC         /* (A89) Set Currency Conversion Rates    */
#define FILE_MIGR25_PARA_TONE        FILE_MIGR_PARA_TONE       /* (A169) Set Tone Volume          */
#define FILE_MIGR25_PARA_MENUPLU     FILE_MIGR_PARA_MENUPLU    /* (A116) Set Promotional PLU      */
#define FILE_MIGR25_PARA_TAXTBL      FILE_MIGR_PARA_TAXTBL     /* (A124,125,126) Set Tax Tables   */
#define FILE_MIGR25_PARA_TAXRATE     FILE_MIGR_PARA_TAXRATE    /* (A127) Set Tax Rates            */
#define FILE_MIGR25_PARA_TARE        FILE_MIGR_PARA_TARE       /* (A111) Set Tare Table           */
#define FILE_MIGR25_PARA_PRECASH     37                        /* (P15) Preset Amount For Preset Cash Key */
#define FILE_MIGR25_PARA_SHARED      38                        /* (P50) Assignment Terminal # for Shared Printerset */
#define FILE_MIGR25_PARA_SOFT        39                        /* (A87) Set Trailer Print For Soft Check  */
#define FILE_MIGR25_PARA_PIGRULE     40                        /* (A130) Set Pig Rule  */
#define FILE_MIGR25_PARA_OEP         41                        /* (A160) Set Order Entry Prompt  */
#define FILE_MIGR25_PARA_FDT         42                        /* (A162) Set Flexible Drive Through  */

/* ----- parameter list offset ----- */

#define FILE_MIGR25_LIST_CONFIG      FILE_MIGR_LIST_CONFIG     /* (File) Configuration            */
#define FILE_MIGR25_LIST_MDC         FILE_MIGR_LIST_MDC        /* (P1) Machine Definition Code    */
#define FILE_MIGR25_LIST_FLEXMEM     FILE_MIGR_LIST_FLEXMEM    /* (P2) Flexible Memory Assignment */
#define FILE_MIGR25_LIST_FSC         FILE_MIGR_LIST_FSC        /* (P3) Function Selection Code    */
#define FILE_MIGR25_LIST_SECNO       FILE_MIGR_LIST_SECNO      /* (P6) Program Mode Security Code */
#define FILE_MIGR25_LIST_SUPLV       FILE_MIGR_LIST_SUPLV      /* (P8) Supervisor Number/Level    */
#define FILE_MIGR25_LIST_ACSEC       FILE_MIGR_LIST_ACSEC      /* (P9) Action Code Security       */
#define FILE_MIGR25_LIST_HALO        FILE_MIGR_LIST_HALO       /* (P10) High Amount Lock-Out Limit*/
#define FILE_MIGR25_LIST_PRECASH     FILE_MIGR_LIST_PRECASH    /* (P15) Preset Amount For Preset Cash Key */
#define FILE_MIGR25_LIST_HOURLY      FILE_MIGR_LIST_HOURLY     /* (P17) Hourly Activity Block     */
#define FILE_MIGR25_LIST_SLIPFEED    FILE_MIGR_LIST_SLIPFEED   /* (P18) Slip/EJ Print Control     */
#define FILE_MIGR25_LIST_TRANSMNE    FILE_MIGR_LIST_TRANSMNE   /* (P20) Transaction Mnemonics     */
#define FILE_MIGR25_LIST_LEADTHRU    FILE_MIGR_LIST_LEADTHRU   /* (P21) Lead-Through Messages     */
#define FILE_MIGR25_LIST_RPTNAME     FILE_MIGR_LIST_RPTNAME    /* (P22) Report Mnemonics          */
#define FILE_MIGR25_LIST_SPEMNE      FILE_MIGR_LIST_SPEMNE     /* (P23) Special Mnemonics         */
#define FILE_MIGR25_LIST_ADJMNE      FILE_MIGR_LIST_ADJMNE     /* (P46) Adjective Mnemonics       */
#define FILE_MIGR25_LIST_PMOD        FILE_MIGR_LIST_PMOD       /* (P47) Print Modifier Mnemonics  */
#define FILE_MIGR25_LIST_MDEPT       FILE_MIGR_LIST_MDEPT      /* (P48) Major Department Mnemonics*/
#define FILE_MIGR25_LIST_AUTOKTCH    FILE_MIGR_LIST_AUTOKTCH   /* (P49) Automatic Alternative Remote Printer */
#define FILE_MIGR25_LIST_SHARED      FILE_MIGR_LIST_SHARED     /* (P50) Assignment Terminal # for Shared Printerset */
#define FILE_MIGR25_LIST_CPMEPT      FILE_MIGR_LIST_CPMEPT     /* (P54) Charge Post/EPT Parameters*/
#define FILE_MIGR25_LIST_CHAR24      FILE_MIGR_LIST_CHAR24     /* (P57) Header/Special Messages   */
#define FILE_MIGR25_LIST_TKTYP       FILE_MIGR_LIST_TKTYP      /* (P60) Total Key Type            */
#define FILE_MIGR25_LIST_TKCTL       FILE_MIGR_LIST_TKCTL      /* (P61) Total Key Function        */
#define FILE_MIGR25_LIST_PLUKEY      FILE_MIGR_LIST_PLUKEY     /* (A4) Set PLU No. on Menu Page   */
#define FILE_MIGR25_LIST_CTLMENU     FILE_MIGR_LIST_CTLMENU    /* (A5) Set Menu Page Control      */
#define FILE_MIGR25_LIST_CASHAB      FILE_MIGR_LIST_CASHAB     /* (A7) Assign Cashier A/B Key     */
#define FILE_MIGR25_LIST_ROUND       FILE_MIGR_LIST_ROUND      /* (A84) Set Rounding Table        */
#define FILE_MIGR25_LIST_DISC        FILE_MIGR_LIST_DISC       /* (A86) Set Discount/&Bonus % Rates      */
#define FILE_MIGR25_LIST_SOFT        FILE_MIGR_LIST_SOFT       /* (A87) Set Trailer Print For Soft Check */
#define FILE_MIGR25_LIST_PROMOT      FILE_MIGR_LIST_PROMOT     /* (A88) Set Sales Promotion Message      */
#define FILE_MIGR25_LIST_FC          FILE_MIGR_LIST_FC         /* (A89) Set Currency Conversion Rates    */
#define FILE_MIGR25_LIST_TARE        FILE_MIGR_LIST_TARE       /* (A111) Set Tare Table           */
#define FILE_MIGR25_LIST_MENUPLU     FILE_MIGR_LIST_MENUPLU    /* (A116) Set Promotional PLU      */
#define FILE_MIGR25_LIST_TAXTBL      FILE_MIGR_LIST_TAXTBL     /* (A124,125,126) Set Tax Tables   */
#define FILE_MIGR25_LIST_TAXRATE     FILE_MIGR_LIST_TAXRATE    /* (A127) Set Tax Rates            */
#define FILE_MIGR25_LIST_PIGRULE     FILE_MIGR_LIST_PIGRULE    /* (A130) Set Pig Rule             */
#define FILE_MIGR25_LIST_OEP         FILE_MIGR_LIST_OEP        /* (A160) Set Order Entry Prompt   */
#define FILE_MIGR25_LIST_FDT         FILE_MIGR_LIST_FDT        /* (A162) Set Flexible Drive Through */
#define FILE_MIGR25_LIST_DIRECT      FILE_MIGR_LIST_DIRECT     /* (Transfer) Terminal Directory   */

#define FILE_MIGR30_NUM_PARA         FILE_MIGR_NUM_PARA    /* No. of parameter class       */
#define FILE_MIGR30_NUM_LIST         FILE_MIGR_NUM_LIST    /* No. of list item  */
#define FILE_MIGR30_MULTIPLY         FILE_MIGR_MULTIPLY    /* size of small dialogbox      */
#define FILE_MIGR30_DIVIDE           FILE_MIGR_DIVIDE      /* size of small dialogbox      */
#define FILE_MIGR30_ICONX            FILE_MIGR_ICONX       /* x position of question icon  */
#define FILE_MIGR30_ICONY            FILE_MIGR_ICONY       /* y position of question icon  */
#define FILE_MIGR30_TAXHED_OLD       FILE_MIGR_TAXHED_OLD  /* old tax table header size    */
#define FILE_MIGR30_TAXHED_NEW       FILE_MIGR_TAXHED_NEW  /* new tax table header size    */
#define FILE_MIGR30_BUFF_LEN         FILE_MIGR_BUFF_LEN    /* length of buffer             */

/* ----- parameter class offset ----- */

#define FILE_MIGR30_PARA_MDC         FILE_MIGR_PARA_MDC        /* (P1) Machine Definition Code    */
#define FILE_MIGR30_PARA_FLEXMEM     FILE_MIGR_PARA_FLEXMEM    /* (P2) Flexible Memory Assignment */
#define FILE_MIGR30_PARA_FSC         FILE_MIGR_PARA_FSC        /* (P3) Function Selection Code    */
#define FILE_MIGR30_PARA_SECNO       FILE_MIGR_PARA_SECNO      /* (P6) Program Mode Security Code */
#define FILE_MIGR30_PARA_STRNO       FILE_MIGR_PARA_STRNO      /* (P7) Store/Register No          */
#define FILE_MIGR30_PARA_SUPLV       FILE_MIGR_PARA_SUPLV      /* (P8) Supervisor Number/Level    */
#define FILE_MIGR30_PARA_ACSEC       FILE_MIGR_PARA_ACSEC      /* (P9) Action Code Security       */
#define FILE_MIGR30_PARA_HALO        FILE_MIGR_PARA_HALO       /* (P10) High Amount Lock-Out Limit*/
#define FILE_MIGR30_PARA_HOURLY      FILE_MIGR_PARA_HOURLY     /* (P17) Hourly Activity Block     */
#define FILE_MIGR30_PARA_SLIPFEED    FILE_MIGR_PARA_SLIPFEED   /* (P18) Slip/EJ Print Control     */
#define FILE_MIGR30_PARA_TRANSMNE    FILE_MIGR_PARA_TRANSMNE   /* (P20) Transaction Mnemonics     */
#define FILE_MIGR30_PARA_LEADTHRU    FILE_MIGR_PARA_LEADTHRU   /* (P21) Lead-Through Messages     */
#define FILE_MIGR30_PARA_RPTNAME     FILE_MIGR_PARA_RPTNAME    /* (P22) Report Mnemonics          */
#define FILE_MIGR30_PARA_SPEMNE      FILE_MIGR_PARA_SPEMNE     /* (P23) Special Mnemonics         */
#define FILE_MIGR30_PARA_PCIF        FILE_MIGR_PARA_PCIF       /* (P39) PC I/F */
#define FILE_MIGR30_PARA_ADJMNE      FILE_MIGR_PARA_ADJMNE     /* (P46) Adjective Mnemonics       */
#define FILE_MIGR30_PARA_PMOD        FILE_MIGR_PARA_PMOD       /* (P47) Print Modifier Mnemonics  */
#define FILE_MIGR30_PARA_MDEPT       FILE_MIGR_PARA_MDEPT      /* (P48) Major Department Mnemonics*/
#define FILE_MIGR30_PARA_AUTOKTCH    FILE_MIGR_PARA_AUTOKTCH   /* (P49) Automatic Alternative Remote Printer */
#define FILE_MIGR30_PARA_CPMEPT      FILE_MIGR_PARA_CPM        /* (P54) Charge Post/EPT Parameters*/
#define FILE_MIGR30_PARA_CHAR24      FILE_MIGR_PARA_CHAR24     /* (P57) Header/Special Messages   */
#define FILE_MIGR30_PARA_TKTYP       FILE_MIGR_PARA_TKTYP      /* (P60) Total Key Type            */
#define FILE_MIGR30_PARA_TKCTL       FILE_MIGR_PARA_TKCTL      /* (P61) Total Key Function        */
#define FILE_MIGR30_PARA_PLUKEY      FILE_MIGR_PARA_PLUKEY     /* (A4) Set PLU No. on Menu Page   */
#define FILE_MIGR30_PARA_CTLMENU     FILE_MIGR_PARA_CTLMENU    /* (A5) Set Menu Page Control      */
#define FILE_MIGR30_PARA_MANUKTCH    FILE_MIGR_PARA_MANUKTCH   /* (A6) Manual Alternative Kitchen Printer */
#define FILE_MIGR30_PARA_CASHAB      FILE_MIGR_PARA_CASHAB     /* (A7) Assign Cashier A/B Key     */
#define FILE_MIGR30_PARA_ROUND       FILE_MIGR_PARA_ROUND      /* (A84) Set Rounding Table        */
#define FILE_MIGR30_PARA_DISC        FILE_MIGR_PARA_DISC       /* (A86) Set Discount/&Bonus % Rates      */
#define FILE_MIGR30_PARA_PROMOT      FILE_MIGR_PARA_PROMOT     /* (A88) Set Sales Promotion Message      */
#define FILE_MIGR30_PARA_FC          FILE_MIGR_PARA_FC         /* (A89) Set Currency Conversion Rates    */
#define FILE_MIGR30_PARA_TONE        FILE_MIGR_PARA_TONE       /* (A169) Set Tone Volume          */
#define FILE_MIGR30_PARA_MENUPLU     FILE_MIGR_PARA_MENUPLU    /* (A116) Set Promotional PLU      */
#define FILE_MIGR30_PARA_TAXTBL      FILE_MIGR_PARA_TAXTBL     /* (A124,130,126) Set Tax Tables   */
#define FILE_MIGR30_PARA_TAXRATE     FILE_MIGR_PARA_TAXRATE    /* (A127) Set Tax Rates            */
#define FILE_MIGR30_PARA_TARE        FILE_MIGR_PARA_TARE       /* (A111) Set Tare Table           */
#define FILE_MIGR30_PARA_PRECASH     37                        /* (P15) Preset Amount For Preset Cash Key */
#define FILE_MIGR30_PARA_SHARED      38                        /* (P50) Assignment Terminal # for Shared Printerset */
#define FILE_MIGR30_PARA_SOFT        39                        /* (A87) Set Trailer Print For Soft Check  */
#define FILE_MIGR30_PARA_PIGRULE     40                        /* (A130) Set Pig Rule  */
#define FILE_MIGR30_PARA_OEP         41                        /* (A160) Set Order Entry Prompt  */
#define FILE_MIGR30_PARA_FDT         42                        /* (A162) Set Flexible Drive Through  */

/* ----- parameter list offset ----- */

#define FILE_MIGR30_LIST_CONFIG      FILE_MIGR_LIST_CONFIG     /* (File) Configuration            */
#define FILE_MIGR30_LIST_MDC         FILE_MIGR_LIST_MDC        /* (P1) Machine Definition Code    */
#define FILE_MIGR30_LIST_FLEXMEM     FILE_MIGR_LIST_FLEXMEM    /* (P2) Flexible Memory Assignment */
#define FILE_MIGR30_LIST_FSC         FILE_MIGR_LIST_FSC        /* (P3) Function Selection Code    */
#define FILE_MIGR30_LIST_SECNO       FILE_MIGR_LIST_SECNO      /* (P6) Program Mode Security Code */
#define FILE_MIGR30_LIST_SUPLV       FILE_MIGR_LIST_SUPLV      /* (P8) Supervisor Number/Level    */
#define FILE_MIGR30_LIST_ACSEC       FILE_MIGR_LIST_ACSEC      /* (P9) Action Code Security       */
#define FILE_MIGR30_LIST_HALO        FILE_MIGR_LIST_HALO       /* (P10) High Amount Lock-Out Limit*/
#define FILE_MIGR30_LIST_PRECASH     FILE_MIGR_LIST_PRECASH    /* (P15) Preset Amount For Preset Cash Key */
#define FILE_MIGR30_LIST_HOURLY      FILE_MIGR_LIST_HOURLY     /* (P17) Hourly Activity Block     */
#define FILE_MIGR30_LIST_SLIPFEED    FILE_MIGR_LIST_SLIPFEED   /* (P18) Slip/EJ Print Control     */
#define FILE_MIGR30_LIST_TRANSMNE    FILE_MIGR_LIST_TRANSMNE   /* (P20) Transaction Mnemonics     */
#define FILE_MIGR30_LIST_LEADTHRU    FILE_MIGR_LIST_LEADTHRU   /* (P21) Lead-Through Messages     */
#define FILE_MIGR30_LIST_RPTNAME     FILE_MIGR_LIST_RPTNAME    /* (P22) Report Mnemonics          */
#define FILE_MIGR30_LIST_SPEMNE      FILE_MIGR_LIST_SPEMNE     /* (P23) Special Mnemonics         */
#define FILE_MIGR30_LIST_ADJMNE      FILE_MIGR_LIST_ADJMNE     /* (P46) Adjective Mnemonics       */
#define FILE_MIGR30_LIST_PMOD        FILE_MIGR_LIST_PMOD       /* (P47) Print Modifier Mnemonics  */
#define FILE_MIGR30_LIST_MDEPT       FILE_MIGR_LIST_MDEPT      /* (P48) Major Department Mnemonics*/
#define FILE_MIGR30_LIST_AUTOKTCH    FILE_MIGR_LIST_AUTOKTCH   /* (P49) Automatic Alternative Remote Printer */
#define FILE_MIGR30_LIST_SHARED      FILE_MIGR_LIST_SHARED     /* (P50) Assignment Terminal # for Shared Printerset */
#define FILE_MIGR30_LIST_CPMEPT      FILE_MIGR_LIST_CPMEPT     /* (P54) Charge Post/EPT Parameters*/
#define FILE_MIGR30_LIST_CHAR24      FILE_MIGR_LIST_CHAR24     /* (P57) Header/Special Messages   */
#define FILE_MIGR30_LIST_TKTYP       FILE_MIGR_LIST_TKTYP      /* (P60) Total Key Type            */
#define FILE_MIGR30_LIST_TKCTL       FILE_MIGR_LIST_TKCTL      /* (P61) Total Key Function        */
#define FILE_MIGR30_LIST_PLUKEY      FILE_MIGR_LIST_PLUKEY     /* (A4) Set PLU No. on Menu Page   */
#define FILE_MIGR30_LIST_CTLMENU     FILE_MIGR_LIST_CTLMENU    /* (A5) Set Menu Page Control      */
#define FILE_MIGR30_LIST_CASHAB      FILE_MIGR_LIST_CASHAB     /* (A7) Assign Cashier A/B Key     */
#define FILE_MIGR30_LIST_ROUND       FILE_MIGR_LIST_ROUND      /* (A84) Set Rounding Table        */
#define FILE_MIGR30_LIST_DISC        FILE_MIGR_LIST_DISC       /* (A86) Set Discount/&Bonus % Rates      */
#define FILE_MIGR30_LIST_SOFT        FILE_MIGR_LIST_SOFT       /* (A87) Set Trailer Print For Soft Check */
#define FILE_MIGR30_LIST_PROMOT      FILE_MIGR_LIST_PROMOT     /* (A88) Set Sales Promotion Message      */
#define FILE_MIGR30_LIST_FC          FILE_MIGR_LIST_FC         /* (A89) Set Currency Conversion Rates    */
#define FILE_MIGR30_LIST_TARE        FILE_MIGR_LIST_TARE       /* (A111) Set Tare Table           */
#define FILE_MIGR30_LIST_MENUPLU     FILE_MIGR_LIST_MENUPLU    /* (A116) Set Promotional PLU      */
#define FILE_MIGR30_LIST_TAXTBL      FILE_MIGR_LIST_TAXTBL     /* (A124,125,126) Set Tax Tables   */
#define FILE_MIGR30_LIST_TAXRATE     FILE_MIGR_LIST_TAXRATE    /* (A127) Set Tax Rates            */
#define FILE_MIGR30_LIST_PIGRULE     FILE_MIGR_LIST_PIGRULE    /* (A130) Set Pig Rule             */
#define FILE_MIGR30_LIST_OEP         FILE_MIGR_LIST_OEP        /* (A160) Set Order Entry Prompt   */
#define FILE_MIGR30_LIST_FDT         FILE_MIGR_LIST_FDT        /* (A162) Set Flexible Drive Through */
#define FILE_MIGR30_LIST_DIRECT      FILE_MIGR_LIST_DIRECT     /* (Transfer) Terminal Directory   */



/*********** add V3.3 ************/

#define FILE_MIGR31_NUM_PARA         FILE_MIGR_NUM_PARA         /* No. of parameter class       */
#define FILE_MIGR31_NUM_LIST         FILE_MIGR_NUM_LIST         /* No. of list item  */
#define FILE_MIGR31_MULTIPLY         FILE_MIGR_MULTIPLY         /* size of small dialogbox      */
#define FILE_MIGR31_DIVIDE           FILE_MIGR_DIVIDE           /* size of small dialogbox      */
#define FILE_MIGR31_ICONX            FILE_MIGR_ICONX            /* x position of question icon  */
#define FILE_MIGR31_ICONY            FILE_MIGR_ICONY            /* y position of question icon  */
#define FILE_MIGR31_TAXHED_OLD       FILE_MIGR_TAXHED_OLD       /* old tax table header size    */
#define FILE_MIGR31_TAXHED_NEW       FILE_MIGR_TAXHED_NEW       /* new tax table header size    */
#define FILE_MIGR31_BUFF_LEN         FILE_MIGR_BUFF_LEN         /* length of buffer             */

/* ----- parameter class offset ----- */

#define FILE_MIGR31_PARA_MDC         FILE_MIGR_PARA_MDC        /* (P1) Machine Definition Code    */
#define FILE_MIGR31_PARA_FLEXMEM     FILE_MIGR_PARA_FLEXMEM    /* (P2) Flexible Memory Assignment */
#define FILE_MIGR31_PARA_FSC         FILE_MIGR_PARA_FSC        /* (P3) Function Selection Code    */
#define FILE_MIGR31_PARA_SECNO       FILE_MIGR_PARA_SECNO      /* (P6) Program Mode Security Code */
#define FILE_MIGR31_PARA_STRNO       FILE_MIGR_PARA_STRNO      /* (P7) Store/Register No          */
#define FILE_MIGR31_PARA_SUPLV       FILE_MIGR_PARA_SUPLV      /* (P8) Supervisor Number/Level    */
#define FILE_MIGR31_PARA_ACSEC       FILE_MIGR_PARA_ACSEC      /* (P9) Action Code Security       */
#define FILE_MIGR31_PARA_HALO        FILE_MIGR_PARA_HALO       /* (P10) High Amount Lock-Out Limit*/
#define FILE_MIGR31_PARA_HOURLY      FILE_MIGR_PARA_HOURLY     /* (P17) Hourly Activity Block     */
#define FILE_MIGR31_PARA_SLIPFEED    FILE_MIGR_PARA_SLIPFEED   /* (P18) Slip/EJ Print Control     */
#define FILE_MIGR31_PARA_TRANSMNE    FILE_MIGR_PARA_TRANSMNE   /* (P20) Transaction Mnemonics     */
#define FILE_MIGR31_PARA_LEADTHRU    FILE_MIGR_PARA_LEADTHRU   /* (P21) Lead-Through Messages     */
#define FILE_MIGR31_PARA_RPTNAME     FILE_MIGR_PARA_RPTNAME    /* (P22) Report Mnemonics          */
#define FILE_MIGR31_PARA_SPEMNE      FILE_MIGR_PARA_SPEMNE     /* (P23) Special Mnemonics         */
#define FILE_MIGR31_PARA_PCIF        FILE_MIGR_PARA_PCIF       /* (P39) PC I/F */
#define FILE_MIGR31_PARA_ADJMNE      FILE_MIGR_PARA_ADJMNE     /* (P46) Adjective Mnemonics       */
#define FILE_MIGR31_PARA_PMOD        FILE_MIGR_PARA_PMOD       /* (P47) Print Modifier Mnemonics  */
#define FILE_MIGR31_PARA_MDEPT       FILE_MIGR_PARA_MDEPT      /* (P48) Major Department Mnemonics*/
#define FILE_MIGR31_PARA_AUTOKTCH    FILE_MIGR_PARA_AUTOKTCH   /* (P49) Automatic Alternative Remote Printer */
#define FILE_MIGR31_PARA_CPMEPT      FILE_MIGR_PARA_CPM        /* (P54) Charge Post/EPT Parameters*/
#define FILE_MIGR31_PARA_CHAR24      FILE_MIGR_PARA_CHAR24     /* (P57) Header/Special Messages   */
#define FILE_MIGR31_PARA_TKTYP       FILE_MIGR_PARA_TKTYP      /* (P60) Total Key Type            */
#define FILE_MIGR31_PARA_TKCTL       FILE_MIGR_PARA_TKCTL      /* (P61) Total Key Function        */
#define FILE_MIGR31_PARA_PLUKEY      FILE_MIGR_PARA_PLUKEY     /* (A4) Set PLU No. on Menu Page   */
#define FILE_MIGR31_PARA_CTLMENU     FILE_MIGR_PARA_CTLMENU    /* (A5) Set Menu Page Control      */
#define FILE_MIGR31_PARA_MANUKTCH    FILE_MIGR_PARA_MANUKTCH   /* (A6) Manual Alternative Kitchen Printer */
#define FILE_MIGR31_PARA_CASHAB      FILE_MIGR_PARA_CASHAB     /* (A7) Assign Cashier A/B Key     */
#define FILE_MIGR31_PARA_ROUND       FILE_MIGR_PARA_ROUND      /* (A84) Set Rounding Table        */
#define FILE_MIGR31_PARA_DISC        FILE_MIGR_PARA_DISC       /* (A86) Set Discount/&Bonus % Rates      */
#define FILE_MIGR31_PARA_PROMOT      FILE_MIGR_PARA_PROMOT     /* (A88) Set Sales Promotion Message      */
#define FILE_MIGR31_PARA_FC          FILE_MIGR_PARA_FC         /* (A89) Set Currency Conversion Rates    */
#define FILE_MIGR31_PARA_TONE        FILE_MIGR_PARA_TONE       /* (A169) Set Tone Volume          */
#define FILE_MIGR31_PARA_MENUPLU     FILE_MIGR_PARA_MENUPLU    /* (A116) Set Promotional PLU      */
#define FILE_MIGR31_PARA_TAXTBL      FILE_MIGR_PARA_TAXTBL     /* (A124,130,126) Set Tax Tables   */
#define FILE_MIGR31_PARA_TAXRATE     FILE_MIGR_PARA_TAXRATE    /* (A127) Set Tax Rates            */
#define FILE_MIGR31_PARA_TARE        FILE_MIGR_PARA_TARE       /* (A111) Set Tare Table           */
#define FILE_MIGR31_PARA_PRECASH     37                        /* (P15) Preset Amount For Preset Cash Key */
#define FILE_MIGR31_PARA_SHARED      38                        /* (P50) Assignment Terminal # for Shared Printerset */
#define FILE_MIGR31_PARA_SOFT        39                        /* (A87) Set Trailer Print For Soft Check  */
#define FILE_MIGR31_PARA_PIGRULE     40                        /* (A130) Set Pig Rule  */
#define FILE_MIGR31_PARA_OEP         41                        /* (A160) Set Order Entry Prompt  */
#define FILE_MIGR31_PARA_FDT         42                        /* (A162) Set Flexible Drive Through  */
    /* add V3.1 parameters */
#define FILE_MIGR31_PARA_SERVICE     43                        /* (A133) Set Service Time Parameter Table */
#define FILE_MIGR31_PARA_LABOR       44                        /* (A150) SET LABOR COST VALUE */
#define FILE_MIGR31_PARA_DISPEN      45                        /* (A137) SET DISPENSER PARAMETER */

/* ----- parameter list offset ----- */

#define FILE_MIGR31_LIST_CONFIG      FILE_MIGR_LIST_CONFIG     /* (File) Configuration            */
#define FILE_MIGR31_LIST_MDC         FILE_MIGR_LIST_MDC        /* (P1) Machine Definition Code    */
#define FILE_MIGR31_LIST_FLEXMEM     FILE_MIGR_LIST_FLEXMEM    /* (P2) Flexible Memory Assignment */
#define FILE_MIGR31_LIST_FSC         FILE_MIGR_LIST_FSC        /* (P3) Function Selection Code    */
#define FILE_MIGR31_LIST_SECNO       FILE_MIGR_LIST_SECNO      /* (P6) Program Mode Security Code */
#define FILE_MIGR31_LIST_SUPLV       FILE_MIGR_LIST_SUPLV      /* (P8) Supervisor Number/Level    */
#define FILE_MIGR31_LIST_ACSEC       FILE_MIGR_LIST_ACSEC      /* (P9) Action Code Security       */
#define FILE_MIGR31_LIST_HALO        FILE_MIGR_LIST_HALO       /* (P10) High Amount Lock-Out Limit*/
#define FILE_MIGR31_LIST_PRECASH     FILE_MIGR_LIST_PRECASH    /* (P15) Preset Amount For Preset Cash Key */
#define FILE_MIGR31_LIST_HOURLY      FILE_MIGR_LIST_HOURLY     /* (P17) Hourly Activity Block     */
#define FILE_MIGR31_LIST_SLIPFEED    FILE_MIGR_LIST_SLIPFEED   /* (P18) Slip/EJ Print Control     */
#define FILE_MIGR31_LIST_TRANSMNE    FILE_MIGR_LIST_TRANSMNE   /* (P20) Transaction Mnemonics     */
#define FILE_MIGR31_LIST_LEADTHRU    FILE_MIGR_LIST_LEADTHRU   /* (P21) Lead-Through Messages     */
#define FILE_MIGR31_LIST_RPTNAME     FILE_MIGR_LIST_RPTNAME    /* (P22) Report Mnemonics          */
#define FILE_MIGR31_LIST_SPEMNE      FILE_MIGR_LIST_SPEMNE     /* (P23) Special Mnemonics         */
#define FILE_MIGR31_LIST_ADJMNE      FILE_MIGR_LIST_ADJMNE     /* (P46) Adjective Mnemonics       */
#define FILE_MIGR31_LIST_PMOD        FILE_MIGR_LIST_PMOD       /* (P47) Print Modifier Mnemonics  */
#define FILE_MIGR31_LIST_MDEPT       FILE_MIGR_LIST_MDEPT      /* (P48) Major Department Mnemonics*/
#define FILE_MIGR31_LIST_AUTOKTCH    FILE_MIGR_LIST_AUTOKTCH   /* (P49) Automatic Alternative Remote Printer */
#define FILE_MIGR31_LIST_SHARED      FILE_MIGR_LIST_SHARED     /* (P50) Assignment Terminal # for Shared Printerset */
#define FILE_MIGR31_LIST_CPMEPT      FILE_MIGR_LIST_CPMEPT     /* (P54) Charge Post/EPT Parameters*/
#define FILE_MIGR31_LIST_CHAR24      FILE_MIGR_LIST_CHAR24     /* (P57) Header/Special Messages   */
#define FILE_MIGR31_LIST_TKTYP       FILE_MIGR_LIST_TKTYP      /* (P60) Total Key Type            */
#define FILE_MIGR31_LIST_TKCTL       FILE_MIGR_LIST_TKCTL      /* (P61) Total Key Function        */
#define FILE_MIGR31_LIST_PLUKEY      FILE_MIGR_LIST_PLUKEY     /* (A4) Set PLU No. on Menu Page   */
#define FILE_MIGR31_LIST_CTLMENU     FILE_MIGR_LIST_CTLMENU    /* (A5) Set Menu Page Control      */
#define FILE_MIGR31_LIST_CASHAB      FILE_MIGR_LIST_CASHAB     /* (A7) Assign Cashier A/B Key     */
#define FILE_MIGR31_LIST_ROUND       FILE_MIGR_LIST_ROUND      /* (A84) Set Rounding Table        */
#define FILE_MIGR31_LIST_DISC        FILE_MIGR_LIST_DISC       /* (A86) Set Discount/&Bonus % Rates      */
#define FILE_MIGR31_LIST_SOFT        FILE_MIGR_LIST_SOFT       /* (A87) Set Trailer Print For Soft Check */
#define FILE_MIGR31_LIST_PROMOT      FILE_MIGR_LIST_PROMOT     /* (A88) Set Sales Promotion Message      */
#define FILE_MIGR31_LIST_FC          FILE_MIGR_LIST_FC         /* (A89) Set Currency Conversion Rates    */
#define FILE_MIGR31_LIST_TARE        FILE_MIGR_LIST_TARE       /* (A111) Set Tare Table           */
#define FILE_MIGR31_LIST_MENUPLU     FILE_MIGR_LIST_MENUPLU    /* (A116) Set Promotional PLU      */
#define FILE_MIGR31_LIST_TAXTBL      FILE_MIGR_LIST_TAXTBL     /* (A124,125,126) Set Tax Tables   */
#define FILE_MIGR31_LIST_TAXRATE     FILE_MIGR_LIST_TAXRATE    /* (A127) Set Tax Rates            */
#define FILE_MIGR31_LIST_PIGRULE     FILE_MIGR_LIST_PIGRULE    /* (A130) Set Pig Rule             */
#define FILE_MIGR31_LIST_SERVICE     FILE_MIGR_LIST_SERVICE    /* (A133) Set Service Time Parameter Table , V3.3*/
#define FILE_MIGR31_LIST_DISPEN      FILE_MIGR_LIST_DISPEN     /* (A137) SET DISPENSER PARAMETER , V3.3 */
#define FILE_MIGR31_LIST_LABOR       FILE_MIGR_LIST_LABOR      /* (A150) SET LABOR COST VALUE , V3.3 */
#define FILE_MIGR31_LIST_OEP         FILE_MIGR_LIST_OEP        /* (A160) Set Order Entry Prompt   */
#define FILE_MIGR31_LIST_FDT         FILE_MIGR_LIST_FDT        /* (A162) Set Flexible Drive Through */
#define FILE_MIGR31_LIST_DIRECT      FILE_MIGR_LIST_DIRECT     /* (Transfer) Terminal Directory   */

/* Saratoga Start */

#define FILE_MIGR34_NUM_PARA         FILE_MIGR_NUM_PARA         /* No. of parameter class       */
#define FILE_MIGR34_NUM_LIST         FILE_MIGR_NUM_LIST         /* No. of list item  */
#define FILE_MIGR34_MULTIPLY         FILE_MIGR_MULTIPLY         /* size of small dialogbox      */
#define FILE_MIGR34_DIVIDE           FILE_MIGR_DIVIDE           /* size of small dialogbox      */
#define FILE_MIGR34_ICONX            FILE_MIGR_ICONX            /* x position of question icon  */
#define FILE_MIGR34_ICONY            FILE_MIGR_ICONY            /* y position of question icon  */
#define FILE_MIGR34_TAXHED_OLD       FILE_MIGR_TAXHED_OLD       /* old tax table header size    */
#define FILE_MIGR34_TAXHED_NEW       FILE_MIGR_TAXHED_NEW       /* new tax table header size    */
#define FILE_MIGR34_BUFF_LEN         FILE_MIGR_BUFF_LEN         /* length of buffer             */

/* ----- parameter class offset ----- */

#define FILE_MIGR34_PARA_MDC         FILE_MIGR_PARA_MDC        /* (P1) Machine Definition Code    */
#define FILE_MIGR34_PARA_FLEXMEM     FILE_MIGR_PARA_FLEXMEM    /* (P2) Flexible Memory Assignment */
#define FILE_MIGR34_PARA_FSC         FILE_MIGR_PARA_FSC        /* (P3) Function Selection Code    */
#define FILE_MIGR34_PARA_SECNO       FILE_MIGR_PARA_SECNO      /* (P6) Program Mode Security Code */
#define FILE_MIGR34_PARA_STRNO       FILE_MIGR_PARA_STRNO      /* (P7) Store/Register No          */
#define FILE_MIGR34_PARA_SUPLV       FILE_MIGR_PARA_SUPLV      /* (P8) Supervisor Number/Level    */
#define FILE_MIGR34_PARA_ACSEC       FILE_MIGR_PARA_ACSEC      /* (P9) Action Code Security       */
#define FILE_MIGR34_PARA_HALO        FILE_MIGR_PARA_HALO       /* (P10) High Amount Lock-Out Limit*/
#define FILE_MIGR34_PARA_HOURLY      FILE_MIGR_PARA_HOURLY     /* (P17) Hourly Activity Block     */
#define FILE_MIGR34_PARA_SLIPFEED    FILE_MIGR_PARA_SLIPFEED   /* (P18) Slip/EJ Print Control     */
#define FILE_MIGR34_PARA_TRANSMNE    FILE_MIGR_PARA_TRANSMNE   /* (P20) Transaction Mnemonics     */
#define FILE_MIGR34_PARA_LEADTHRU    FILE_MIGR_PARA_LEADTHRU   /* (P21) Lead-Through Messages     */
#define FILE_MIGR34_PARA_RPTNAME     FILE_MIGR_PARA_RPTNAME    /* (P22) Report Mnemonics          */
#define FILE_MIGR34_PARA_SPEMNE      FILE_MIGR_PARA_SPEMNE     /* (P23) Special Mnemonics         */
#define FILE_MIGR34_PARA_PCIF        FILE_MIGR_PARA_PCIF       /* (P39) PC I/F */
#define FILE_MIGR34_PARA_ADJMNE      FILE_MIGR_PARA_ADJMNE     /* (P46) Adjective Mnemonics       */
#define FILE_MIGR34_PARA_PMOD        FILE_MIGR_PARA_PMOD       /* (P47) Print Modifier Mnemonics  */
#define FILE_MIGR34_PARA_MDEPT       FILE_MIGR_PARA_MDEPT      /* (P48) Major Department Mnemonics*/
#define FILE_MIGR34_PARA_AUTOKTCH    FILE_MIGR_PARA_AUTOKTCH   /* (P49) Automatic Alternative Remote Printer */
#define FILE_MIGR34_PARA_CPMEPT      FILE_MIGR_PARA_CPM        /* (P54) Charge Post/EPT Parameters*/
#define FILE_MIGR34_PARA_CHAR24      FILE_MIGR_PARA_CHAR24     /* (P57) Header/Special Messages   */
#define FILE_MIGR34_PARA_TKTYP       FILE_MIGR_PARA_TKTYP      /* (P60) Total Key Type            */
#define FILE_MIGR34_PARA_TKCTL       FILE_MIGR_PARA_TKCTL      /* (P61) Total Key Function        */
#define FILE_MIGR34_PARA_PLUKEY      FILE_MIGR_PARA_PLUKEY     /* (A4) Set PLU No. on Menu Page   */
#define FILE_MIGR34_PARA_CTLMENU     FILE_MIGR_PARA_CTLMENU    /* (A5) Set Menu Page Control      */
#define FILE_MIGR34_PARA_MANUKTCH    FILE_MIGR_PARA_MANUKTCH   /* (A6) Manual Alternative Kitchen Printer */
#define FILE_MIGR34_PARA_CASHAB      FILE_MIGR_PARA_CASHAB     /* (A7) Assign Cashier A/B Key     */
#define FILE_MIGR34_PARA_ROUND       FILE_MIGR_PARA_ROUND      /* (A84) Set Rounding Table        */
#define FILE_MIGR34_PARA_DISC        FILE_MIGR_PARA_DISC       /* (A86) Set Discount/&Bonus % Rates      */
#define FILE_MIGR34_PARA_PROMOT      FILE_MIGR_PARA_PROMOT     /* (A88) Set Sales Promotion Message      */
#define FILE_MIGR34_PARA_FC          FILE_MIGR_PARA_FC         /* (A89) Set Currency Conversion Rates    */
#define FILE_MIGR34_PARA_TONE        FILE_MIGR_PARA_TONE       /* (A169) Set Tone Volume          */
#define FILE_MIGR34_PARA_MENUPLU     FILE_MIGR_PARA_MENUPLU    /* (A116) Set Promotional PLU      */
#define FILE_MIGR34_PARA_TAXTBL      FILE_MIGR_PARA_TAXTBL     /* (A124,130,126) Set Tax Tables   */
#define FILE_MIGR34_PARA_TAXRATE     FILE_MIGR_PARA_TAXRATE    /* (A127) Set Tax Rates            */
#define FILE_MIGR34_PARA_TARE        FILE_MIGR_PARA_TARE       /* (A111) Set Tare Table           */
#define FILE_MIGR34_PARA_PRECASH     37                        /* (P15) Preset Amount For Preset Cash Key */
#define FILE_MIGR34_PARA_SHARED      38                        /* (P50) Assignment Terminal # for Shared Printerset */
#define FILE_MIGR34_PARA_SOFT        39                        /* (A87) Set Trailer Print For Soft Check  */
#define FILE_MIGR34_PARA_PIGRULE     40                        /* (A130) Set Pig Rule  */
#define FILE_MIGR34_PARA_OEP         41                        /* (A160) Set Order Entry Prompt  */
#define FILE_MIGR34_PARA_FDT         42                        /* (A162) Set Flexible Drive Through  */
    /* add V3.1 parameters */
#define FILE_MIGR34_PARA_SERVICE     43                        /* (A133) Set Service Time Parameter Table */
#define FILE_MIGR34_PARA_LABOR       44                        /* (A150) SET LABOR COST VALUE */
#define FILE_MIGR34_PARA_DISPEN      45                        /* (A137) SET DISPENSER PARAMETER */

/* ----- parameter list offset ----- */

#define FILE_MIGR34_LIST_CONFIG      FILE_MIGR_LIST_CONFIG     /* (File) Configuration            */
#define FILE_MIGR34_LIST_MDC         FILE_MIGR_LIST_MDC        /* (P1) Machine Definition Code    */
#define FILE_MIGR34_LIST_FLEXMEM     FILE_MIGR_LIST_FLEXMEM    /* (P2) Flexible Memory Assignment */
#define FILE_MIGR34_LIST_FSC         FILE_MIGR_LIST_FSC        /* (P3) Function Selection Code    */
#define FILE_MIGR34_LIST_SECNO       FILE_MIGR_LIST_SECNO      /* (P6) Program Mode Security Code */
#define FILE_MIGR34_LIST_SUPLV       FILE_MIGR_LIST_SUPLV      /* (P8) Supervisor Number/Level    */
#define FILE_MIGR34_LIST_ACSEC       FILE_MIGR_LIST_ACSEC      /* (P9) Action Code Security       */
#define FILE_MIGR34_LIST_HALO        FILE_MIGR_LIST_HALO       /* (P10) High Amount Lock-Out Limit*/
#define FILE_MIGR34_LIST_PRECASH     FILE_MIGR_LIST_PRECASH    /* (P15) Preset Amount For Preset Cash Key */
#define FILE_MIGR34_LIST_HOURLY      FILE_MIGR_LIST_HOURLY     /* (P17) Hourly Activity Block     */
#define FILE_MIGR34_LIST_SLIPFEED    FILE_MIGR_LIST_SLIPFEED   /* (P18) Slip/EJ Print Control     */
#define FILE_MIGR34_LIST_TRANSMNE    FILE_MIGR_LIST_TRANSMNE   /* (P20) Transaction Mnemonics     */
#define FILE_MIGR34_LIST_LEADTHRU    FILE_MIGR_LIST_LEADTHRU   /* (P21) Lead-Through Messages     */
#define FILE_MIGR34_LIST_RPTNAME     FILE_MIGR_LIST_RPTNAME    /* (P22) Report Mnemonics          */
#define FILE_MIGR34_LIST_SPEMNE      FILE_MIGR_LIST_SPEMNE     /* (P23) Special Mnemonics         */
#define FILE_MIGR34_LIST_ADJMNE      FILE_MIGR_LIST_ADJMNE     /* (P46) Adjective Mnemonics       */
#define FILE_MIGR34_LIST_PMOD        FILE_MIGR_LIST_PMOD       /* (P47) Print Modifier Mnemonics  */
#define FILE_MIGR34_LIST_MDEPT       FILE_MIGR_LIST_MDEPT      /* (P48) Major Department Mnemonics*/
#define FILE_MIGR34_LIST_AUTOKTCH    FILE_MIGR_LIST_AUTOKTCH   /* (P49) Automatic Alternative Remote Printer */
#define FILE_MIGR34_LIST_SHARED      FILE_MIGR_LIST_SHARED     /* (P50) Assignment Terminal # for Shared Printerset */
#define FILE_MIGR34_LIST_CPMEPT      FILE_MIGR_LIST_CPMEPT     /* (P54) Charge Post/EPT Parameters*/
#define FILE_MIGR34_LIST_CHAR24      FILE_MIGR_LIST_CHAR24     /* (P57) Header/Special Messages   */
#define FILE_MIGR34_LIST_TKTYP       FILE_MIGR_LIST_TKTYP      /* (P60) Total Key Type            */
#define FILE_MIGR34_LIST_TKCTL       FILE_MIGR_LIST_TKCTL      /* (P61) Total Key Function        */
#define FILE_MIGR34_LIST_PLUKEY      FILE_MIGR_LIST_PLUKEY     /* (A4) Set PLU No. on Menu Page   */
#define FILE_MIGR34_LIST_CTLMENU     FILE_MIGR_LIST_CTLMENU    /* (A5) Set Menu Page Control      */
#define FILE_MIGR34_LIST_CASHAB      FILE_MIGR_LIST_CASHAB     /* (A7) Assign Cashier A/B Key     */
#define FILE_MIGR34_LIST_ROUND       FILE_MIGR_LIST_ROUND      /* (A84) Set Rounding Table        */
#define FILE_MIGR34_LIST_DISC        FILE_MIGR_LIST_DISC       /* (A86) Set Discount/&Bonus % Rates      */
#define FILE_MIGR34_LIST_SOFT        FILE_MIGR_LIST_SOFT       /* (A87) Set Trailer Print For Soft Check */
#define FILE_MIGR34_LIST_PROMOT      FILE_MIGR_LIST_PROMOT     /* (A88) Set Sales Promotion Message      */
#define FILE_MIGR34_LIST_FC          FILE_MIGR_LIST_FC         /* (A89) Set Currency Conversion Rates    */
#define FILE_MIGR34_LIST_TARE        FILE_MIGR_LIST_TARE       /* (A111) Set Tare Table           */
#define FILE_MIGR34_LIST_MENUPLU     FILE_MIGR_LIST_MENUPLU    /* (A116) Set Promotional PLU      */
#define FILE_MIGR34_LIST_TAXTBL      FILE_MIGR_LIST_TAXTBL     /* (A124,125,126) Set Tax Tables   */
#define FILE_MIGR34_LIST_TAXRATE     FILE_MIGR_LIST_TAXRATE    /* (A127) Set Tax Rates            */
#define FILE_MIGR34_LIST_PIGRULE     FILE_MIGR_LIST_PIGRULE    /* (A130) Set Pig Rule             */
#define FILE_MIGR34_LIST_SERVICE     FILE_MIGR_LIST_SERVICE    /* (A133) Set Service Time Parameter Table , V3.3*/
#define FILE_MIGR34_LIST_DISPEN      FILE_MIGR_LIST_DISPEN     /* (A137) SET DISPENSER PARAMETER , V3.3 */
#define FILE_MIGR34_LIST_LABOR       FILE_MIGR_LIST_LABOR      /* (A150) SET LABOR COST VALUE , V3.3 */
#define FILE_MIGR34_LIST_OEP         FILE_MIGR_LIST_OEP        /* (A160) Set Order Entry Prompt   */
#define FILE_MIGR34_LIST_FDT         FILE_MIGR_LIST_FDT        /* (A162) Set Flexible Drive Through */
#define FILE_MIGR34_LIST_DIRECT      FILE_MIGR_LIST_DIRECT     /* (Transfer) Terminal Directory   */

/*#define FILE_MIGR34_PARA_TRANSMNE_SIZE     1056  (P20) file size , not include reserved area 132 */
/*#define FILE_MIGR34_PARA_LEADTHRU_SIZE     1824  (P21) file size , not include reserved area 114 */
#define FILE_MIGR34_PARA_TRANSMNE_SIZE     1144 /* (P20) file size , not include reserved area 132 */
#define FILE_MIGR34_PARA_LEADTHRU_SIZE     1920 /* (P21) file size , not include reserved area 114 */


/* NHPOS Release 1.x data file constants */

#define FILE_MIGRHGV10_NUM_PARA         FILE_MIGR_NUM_PARA         /* No. of parameter class       */
#define FILE_MIGRHGV10_NUM_LIST         FILE_MIGR_NUM_LIST         /* No. of list item  */
#define FILE_MIGRHGV10_MULTIPLY         FILE_MIGR_MULTIPLY         /* size of small dialogbox      */
#define FILE_MIGRHGV10_DIVIDE           FILE_MIGR_DIVIDE           /* size of small dialogbox      */
#define FILE_MIGRHGV10_ICONX            FILE_MIGR_ICONX            /* x position of question icon  */
#define FILE_MIGRHGV10_ICONY            FILE_MIGR_ICONY            /* y position of question icon  */
#define FILE_MIGRHGV10_TAXHED_OLD       FILE_MIGR_TAXHED_OLD       /* old tax table header size    */
#define FILE_MIGRHGV10_TAXHED_NEW       FILE_MIGR_TAXHED_NEW       /* new tax table header size    */
#define FILE_MIGRHGV10_BUFF_LEN         FILE_MIGR_BUFF_LEN         /* length of buffer             */

/* ----- parameter class offset ----- */

#define FILE_MIGRHGV10_PARA_MDC         FILE_MIGR_PARA_MDC        /* (P1) Machine Definition Code    */
#define FILE_MIGRHGV10_PARA_FLEXMEM     FILE_MIGR_PARA_FLEXMEM    /* (P2) Flexible Memory Assignment */
#define FILE_MIGRHGV10_PARA_FSC         FILE_MIGR_PARA_FSC        /* (P3) Function Selection Code    */
#define FILE_MIGRHGV10_PARA_SECNO       FILE_MIGR_PARA_SECNO      /* (P6) Program Mode Security Code */
#define FILE_MIGRHGV10_PARA_STRNO       FILE_MIGR_PARA_STRNO      /* (P7) Store/Register No          */
#define FILE_MIGRHGV10_PARA_SUPLV       FILE_MIGR_PARA_SUPLV      /* (P8) Supervisor Number/Level    */
#define FILE_MIGRHGV10_PARA_ACSEC       FILE_MIGR_PARA_ACSEC      /* (P9) Action Code Security       */
#define FILE_MIGRHGV10_PARA_HALO        FILE_MIGR_PARA_HALO       /* (P10) High Amount Lock-Out Limit*/
#define FILE_MIGRHGV10_PARA_HOURLY      FILE_MIGR_PARA_HOURLY     /* (P17) Hourly Activity Block     */
#define FILE_MIGRHGV10_PARA_SLIPFEED    FILE_MIGR_PARA_SLIPFEED   /* (P18) Slip/EJ Print Control     */
#define FILE_MIGRHGV10_PARA_TRANSMNE    FILE_MIGR_PARA_TRANSMNE   /* (P20) Transaction Mnemonics     */
#define FILE_MIGRHGV10_PARA_LEADTHRU    FILE_MIGR_PARA_LEADTHRU   /* (P21) Lead-Through Messages     */
#define FILE_MIGRHGV10_PARA_RPTNAME     FILE_MIGR_PARA_RPTNAME    /* (P22) Report Mnemonics          */
#define FILE_MIGRHGV10_PARA_SPEMNE      FILE_MIGR_PARA_SPEMNE     /* (P23) Special Mnemonics         */
#define FILE_MIGRHGV10_PARA_PCIF        FILE_MIGR_PARA_PCIF       /* (P39) PC I/F */
#define FILE_MIGRHGV10_PARA_ADJMNE      FILE_MIGR_PARA_ADJMNE     /* (P46) Adjective Mnemonics       */
#define FILE_MIGRHGV10_PARA_PMOD        FILE_MIGR_PARA_PMOD       /* (P47) Print Modifier Mnemonics  */
#define FILE_MIGRHGV10_PARA_MDEPT       FILE_MIGR_PARA_MDEPT      /* (P48) Major Department Mnemonics*/
#define FILE_MIGRHGV10_PARA_AUTOKTCH    FILE_MIGR_PARA_AUTOKTCH   /* (P49) Automatic Alternative Remote Printer */
#define FILE_MIGRHGV10_PARA_CPMEPT      FILE_MIGR_PARA_CPM        /* (P54) Charge Post/EPT Parameters*/
#define FILE_MIGRHGV10_PARA_CHAR24      FILE_MIGR_PARA_CHAR24     /* (P57) Header/Special Messages   */
#define FILE_MIGRHGV10_PARA_TKTYP       FILE_MIGR_PARA_TKTYP      /* (P60) Total Key Type            */
#define FILE_MIGRHGV10_PARA_TKCTL       FILE_MIGR_PARA_TKCTL      /* (P61) Total Key Function        */
#define FILE_MIGRHGV10_PARA_PLUKEY      FILE_MIGR_PARA_PLUKEY     /* (A4) Set PLU No. on Menu Page   */
#define FILE_MIGRHGV10_PARA_CTLMENU     FILE_MIGR_PARA_CTLMENU    /* (A5) Set Menu Page Control      */
#define FILE_MIGRHGV10_PARA_MANUKTCH    FILE_MIGR_PARA_MANUKTCH   /* (A6) Manual Alternative Kitchen Printer */
#define FILE_MIGRHGV10_PARA_CASHAB      FILE_MIGR_PARA_CASHAB     /* (A7) Assign Cashier A/B Key     */
#define FILE_MIGRHGV10_PARA_ROUND       FILE_MIGR_PARA_ROUND      /* (A84) Set Rounding Table        */
#define FILE_MIGRHGV10_PARA_DISC        FILE_MIGR_PARA_DISC       /* (A86) Set Discount/&Bonus % Rates      */
#define FILE_MIGRHGV10_PARA_PROMOT      FILE_MIGR_PARA_PROMOT     /* (A88) Set Sales Promotion Message      */
#define FILE_MIGRHGV10_PARA_FC          FILE_MIGR_PARA_FC         /* (A89) Set Currency Conversion Rates    */
#define FILE_MIGRHGV10_PARA_TONE        FILE_MIGR_PARA_TONE       /* (A169) Set Tone Volume          */
#define FILE_MIGRHGV10_PARA_MENUPLU     FILE_MIGR_PARA_MENUPLU    /* (A116) Set Promotional PLU      */
#define FILE_MIGRHGV10_PARA_TAXTBL      FILE_MIGR_PARA_TAXTBL     /* (A124,130,126) Set Tax Tables   */
#define FILE_MIGRHGV10_PARA_TAXRATE     FILE_MIGR_PARA_TAXRATE    /* (A127) Set Tax Rates            */
/* #define FILE_MIGRHGV10_PARA_MISCPARA	36						   MISC PARAMATER				 */
#define FILE_MIGRHGV10_PARA_TARE        36			          /* (A111) Set Tare Table           */
#define FILE_MIGRHGV10_PARA_PRECASH     37                        /* (P15) Preset Amount For Preset Cash Key */
#define FILE_MIGRHGV10_PARA_SHARED      38                        /* (P50) Assignment Terminal # for Shared Printerset */
#define FILE_MIGRHGV10_PARA_SOFT        39                        /* (A87) Set Trailer Print For Soft Check  */
#define FILE_MIGRHGV10_PARA_PIGRULE     40                        /* (A130) Set Pig Rule  */
#define FILE_MIGRHGV10_PARA_OEP         41                        /* (A160) Set Order Entry Prompt  */
#define FILE_MIGRHGV10_PARA_FDT         42                        /* (A162) Set Flexible Drive Through  */
    /* add V3.1 parameters */
#define FILE_MIGRHGV10_PARA_SERVICE     43                        /* (A133) Set Service Time Parameter Table */
#define FILE_MIGRHGV10_PARA_LABOR       44                        /* (A150) SET LABOR COST VALUE */
#define FILE_MIGRHGV10_PARA_DISPEN      45                        /* (A137) SET DISPENSER PARAMETER */
/* Add R20 parameters */
#define FILE_MIGRHGV10_PARA_KDISP		46						  /* Prog, 51, 2172 */
#define FILE_MIGRHGV10_PARA_RESTRICT    47						  /* AC 170, 2172 */
#define FILE_MIGRHGV10_PARA_BNDAGE      48		                  /* A/C 208, 2172 */
#define FILE_MIGRHGV10_PARA_NODEPT		49						  /* A/C #4, 2172 */
#define FILE_MIGRHGV10_PARA_SPCCOUNT	50						  /* SPECIAL COUNTER */
/* ----- parameter list offset ----- */

#define FILE_MIGRHGV10_LIST_CONFIG      FILE_MIGR_LIST_CONFIG     /* (File) Configuration            */
#define FILE_MIGRHGV10_LIST_MDC         FILE_MIGR_LIST_MDC        /* (P1) Machine Definition Code    */
#define FILE_MIGRHGV10_LIST_FLEXMEM     FILE_MIGR_LIST_FLEXMEM    /* (P2) Flexible Memory Assignment */
#define FILE_MIGRHGV10_LIST_FSC         FILE_MIGR_LIST_FSC        /* (P3) Function Selection Code    */
#define FILE_MIGRHGV10_LIST_SECNO       FILE_MIGR_LIST_SECNO      /* (P6) Program Mode Security Code */
#define FILE_MIGRHGV10_LIST_SUPLV       FILE_MIGR_LIST_SUPLV      /* (P8) Supervisor Number/Level    */
#define FILE_MIGRHGV10_LIST_ACSEC       FILE_MIGR_LIST_ACSEC      /* (P9) Action Code Security       */
#define FILE_MIGRHGV10_LIST_HALO        FILE_MIGR_LIST_HALO       /* (P10) High Amount Lock-Out Limit*/
#define FILE_MIGRHGV10_LIST_PRECASH     FILE_MIGR_LIST_PRECASH    /* (P15) Preset Amount For Preset Cash Key */
#define FILE_MIGRHGV10_LIST_HOURLY      FILE_MIGR_LIST_HOURLY     /* (P17) Hourly Activity Block     */
#define FILE_MIGRHGV10_LIST_SLIPFEED    FILE_MIGR_LIST_SLIPFEED   /* (P18) Slip/EJ Print Control     */
#define FILE_MIGRHGV10_LIST_TRANSMNE    FILE_MIGR_LIST_TRANSMNE   /* (P20) Transaction Mnemonics     */
#define FILE_MIGRHGV10_LIST_LEADTHRU    FILE_MIGR_LIST_LEADTHRU   /* (P21) Lead-Through Messages     */
#define FILE_MIGRHGV10_LIST_RPTNAME     FILE_MIGR_LIST_RPTNAME    /* (P22) Report Mnemonics          */
#define FILE_MIGRHGV10_LIST_SPEMNE      FILE_MIGR_LIST_SPEMNE     /* (P23) Special Mnemonics         */
#define FILE_MIGRHGV10_LIST_ADJMNE      FILE_MIGR_LIST_ADJMNE     /* (P46) Adjective Mnemonics       */
#define FILE_MIGRHGV10_LIST_PMOD        FILE_MIGR_LIST_PMOD       /* (P47) Print Modifier Mnemonics  */
#define FILE_MIGRHGV10_LIST_MDEPT       FILE_MIGR_LIST_MDEPT      /* (P48) Major Department Mnemonics*/
#define FILE_MIGRHGV10_LIST_AUTOKTCH    FILE_MIGR_LIST_AUTOKTCH   /* (P49) Automatic Alternative Remote Printer */
#define FILE_MIGRHGV10_LIST_SHARED      FILE_MIGR_LIST_SHARED     /* (P50) Assignment Terminal # for Shared Printerset */
#define FILE_MIGRHGV10_LIST_CPMEPT      FILE_MIGR_LIST_CPMEPT     /* (P54) Charge Post/EPT Parameters*/
#define FILE_MIGRHGV10_LIST_CHAR24      FILE_MIGR_LIST_CHAR24     /* (P57) Header/Special Messages   */
#define FILE_MIGRHGV10_LIST_TKTYP       FILE_MIGR_LIST_TKTYP      /* (P60) Total Key Type            */
#define FILE_MIGRHGV10_LIST_TKCTL       FILE_MIGR_LIST_TKCTL      /* (P61) Total Key Function        */
#define FILE_MIGRHGV10_LIST_PLUKEY      FILE_MIGR_LIST_PLUKEY     /* (A4) Set PLU No. on Menu Page   */
#define FILE_MIGRHGV10_LIST_CTLMENU     FILE_MIGR_LIST_CTLMENU    /* (A5) Set Menu Page Control      */
#define FILE_MIGRHGV10_LIST_CASHAB      FILE_MIGR_LIST_CASHAB     /* (A7) Assign Cashier A/B Key     */
#define FILE_MIGRHGV10_LIST_ROUND       FILE_MIGR_LIST_ROUND      /* (A84) Set Rounding Table        */
#define FILE_MIGRHGV10_LIST_DISC        FILE_MIGR_LIST_DISC       /* (A86) Set Discount/&Bonus % Rates      */
#define FILE_MIGRHGV10_LIST_SOFT        FILE_MIGR_LIST_SOFT       /* (A87) Set Trailer Print For Soft Check */
#define FILE_MIGRHGV10_LIST_PROMOT      FILE_MIGR_LIST_PROMOT     /* (A88) Set Sales Promotion Message      */
#define FILE_MIGRHGV10_LIST_FC          FILE_MIGR_LIST_FC         /* (A89) Set Currency Conversion Rates    */
#define FILE_MIGRHGV10_LIST_TARE        FILE_MIGR_LIST_TARE       /* (A111) Set Tare Table           */
#define FILE_MIGRHGV10_LIST_MENUPLU     FILE_MIGR_LIST_MENUPLU    /* (A116) Set Promotional PLU      */
#define FILE_MIGRHGV10_LIST_TAXTBL      FILE_MIGR_LIST_TAXTBL     /* (A124,125,126) Set Tax Tables   */
#define FILE_MIGRHGV10_LIST_TAXRATE     FILE_MIGR_LIST_TAXRATE    /* (A127) Set Tax Rates            */
#define FILE_MIGRHGV10_LIST_PIGRULE     FILE_MIGR_LIST_PIGRULE    /* (A130) Set Pig Rule             */
#define FILE_MIGRHGV10_LIST_SERVICE     FILE_MIGR_LIST_SERVICE    /* (A133) Set Service Time Parameter Table , V3.3*/
#define FILE_MIGRHGV10_LIST_DISPEN      FILE_MIGR_LIST_DISPEN     /* (A137) SET DISPENSER PARAMETER , V3.3 */
#define FILE_MIGRHGV10_LIST_LABOR       FILE_MIGR_LIST_LABOR      /* (A150) SET LABOR COST VALUE , V3.3 */
#define FILE_MIGRHGV10_LIST_OEP         FILE_MIGR_LIST_OEP        /* (A160) Set Order Entry Prompt   */
#define FILE_MIGRHGV10_LIST_FDT         FILE_MIGR_LIST_FDT        /* (A162) Set Flexible Drive Through */
#define FILE_MIGRHGV10_LIST_DIRECT      FILE_MIGR_LIST_DIRECT     /* (Transfer) Terminal Directory   */

/*#define FILE_MIGRHGV10_PARA_TRANSMNE_SIZE     1056  (P20) file size , not include reserved area 132 */
/*#define FILE_MIGRHGV10_PARA_LEADTHRU_SIZE     1824  (P21) file size , not include reserved area 114 */
#define FILE_MIGRHGV10_PARA_TRANSMNE_SIZE     1144 /* (P20) file size , not include reserved area 132 */
#define FILE_MIGRHGV10_PARA_LEADTHRU_SIZE     1920 /* (P21) file size , not include reserved area 114 */



/***** Add GP Ver 2.0/2.1/2.0 *****/

#define FILE_MIGRGP_NUM_PARA         2                          /* No. of parameter class       */

#define FILE_MIGRGP_NUM_LIST         FILE_MIGR_NUM_LIST         /* No. of list item  */
#define FILE_MIGRGP_MULTIPLY         FILE_MIGR_MULTIPLY         /* size of small dialogbox      */
#define FILE_MIGRGP_DIVIDE           FILE_MIGR_DIVIDE           /* size of small dialogbox      */
#define FILE_MIGRGP_ICONX            FILE_MIGR_ICONX            /* x position of question icon  */
#define FILE_MIGRGP_ICONY            FILE_MIGR_ICONY            /* y position of question icon  */
#define FILE_MIGRGP_TAXHED_OLD       FILE_MIGR_TAXHED_OLD       /* old tax table header size    */
#define FILE_MIGRGP_TAXHED_NEW       FILE_MIGR_TAXHED_NEW       /* new tax table header size    */
#define FILE_MIGRGP_BUFF_LEN         FILE_MIGR_BUFF_LEN         /* length of buffer             */

/* ----- parameter class offset ----- */

#define FILE_MIGRGP_PARA_MDC         FILE_MIGR_PARA_MDC        /* (P1) Machine Definition Code    */
#define FILE_MIGRGP_PARA_FLEXMEM     FILE_MIGR_PARA_FLEXMEM    /* (P2) Flexible Memory Assignment */

/* ----- parameter list offset ----- */

#define FILE_MIGRGP_LIST_CONFIG      FILE_MIGR_LIST_CONFIG     /* (File) Configuration            */
#define FILE_MIGRGP_LIST_MDC         FILE_MIGR_LIST_MDC        /* (P1) Machine Definition Code    */
#define FILE_MIGRGP_LIST_FLEXMEM     FILE_MIGR_LIST_FLEXMEM    /* (P2) Flexible Memory Assignment */

/* Saratoga End   */


/* ----- Common Parameter ----- */
#define FILE_MIGR_PARA_TRANSMNE_ADD  48  /* Overlap area between New and Old */

#define FILE_MIGR_PARA_SPEMNE_1ST    11*4  /* Special Mnemonics Addr.1 - Addr.11 */
#define FILE_MIGR_PARA_SPEMNE_2ND    4*4   /* Special Mnemonics Addr.12 - Addr.15 */
#define FILE_MIGR_PARA_SPEMNE_3RD    11*4  /* Special Mnemonics Addr.16 - Addr.26 */
#define FILE_MIGR_PARA_SPEMNE_4TH    28*4  /* Special Mnemonics Addr.29 - Addr.32 */
#define FILE_MIGR_PARA_SPEMNE_ADD    1*4   /* Addr. 27 */

#define FILE_MIGR_PARA_RPTNAME_RES   2*6  /* Report Mnemonics 2 address */
#define FILE_MIGR_PARA_TRANSMNE_RES  2*8  /* Transaction Mnemonics Reserved */
#define FILE_MIGR_PARA_LEADTHRU_RES  1*16 /* Reserved area */

#define FILE_MIGR30_PARA_TRANSMNE_RES  6*8  /* Transaction Mnemonics Reserved */

#define FILE_MIGR_COPY_TRANSMNE      888 /* (P20) Transaction Mnemonics Copy Length Ver 1.0 */
#define FILE_MIGR_COPY_LEADTHRU      944 /* (P21) Lead-Through Messages Copy Length Ver 1.0 */

#define FILE_MIGR_PLUKEY             1   /* Keyed PLU Key FSC */

#define FILE_MIGR31_PARA_TRANSMNE_SIZE     1056 /* (P20) file size , not include reserved area 132 */
#define FILE_MIGR31_PARA_LEADTHRU_SIZE     1824 /* (P21) file size , not include reserved area 114 */

#define FILE_MIGR_PATH_LEN         128      /* Saratoga */
#define FILE_MIGR_TEMP_LEN          16      /* Saratoga */

#define FILE_MIGR_CONV8BY10          0
#define FILE_MIGR_CONV6BY10          1
#define FILE_MIGR_MICROMOTION        2

/*
* ===========================================================================
*       Dialog ID definition
* ===========================================================================
*/
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
#pragma pack(push, 1)

typedef struct _FILEMIGRIDX {
    WORD    wTop;           /* top index No.    */
    WORD    wBtm;           /* bottom index No. */
} FILEMIGRIDX, FAR* LPFILEMIGRIDX;

typedef struct _FILEMIGRCHK {
    BYTE    bFile;
    BYTE    abPara[FILE_MIGR_NUM_PARA];
    BYTE    bTrans;
} FILEMIGRCHK, FAR* LPFILEMIGRCHK;

/*** Add GP Ver 2.0/2.1/2.2 Migrate ***/
typedef struct _FILEMIGRCHKGP {
    BYTE    bFile;
    BYTE    abPara[FILE_MIGRGP_NUM_PARA];
    BYTE    bTrans;
} FILEMIGRCHKGP, FAR* LPFILEMIGRCHKGP;



#pragma pack(pop)

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
BOOL    WINAPI FileMigrateDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI FileMigrate2DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI FileMigrate25DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI FileMigrate30DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI FileMigrate31DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI FileMigrate34DlgProc(HWND, UINT, WPARAM, LPARAM);   /* Saratoga */
BOOL    WINAPI FileMigrateGPDlgProc(HWND, UINT, WPARAM, LPARAM);   /* Saratoga */
BOOL    WINAPI FileMigrateGP40DlgProc(HWND, UINT, WPARAM, LPARAM);   /* Saratoga */

BOOL    FileMigrCtrlScrl(HWND, WPARAM, LPARAM, LPFILEMIGRCHK, LPFILEMIGRIDX);
VOID    FileMigrCtrlChkbox(HWND, WORD wEditId, LPFILEMIGRCHK, LPFILEMIGRIDX);

VOID    FileMigrSetListDscrb(HWND, LPFILEMIGRIDX);
VOID    FileMigrSetListData(HWND, LPFILEMIGRCHK, LPFILEMIGRIDX);
BOOL    FileMigrMakeListData(WPARAM, LPFILEMIGRCHK);
BOOL    FileMigrCalcListIndex(WPARAM, LPFILEMIGRIDX);

BOOL    FileMigr2CtrlScrl(HWND, WPARAM, LPARAM, LPFILEMIGRCHK, LPFILEMIGRIDX);
VOID    FileMigr2CtrlChkbox(HWND, WORD wEditId, LPFILEMIGRCHK, LPFILEMIGRIDX);

VOID    FileMigr2SetListDscrb(HWND, LPFILEMIGRIDX);
VOID    FileMigr2SetListData(HWND, LPFILEMIGRCHK, LPFILEMIGRIDX);
BOOL    FileMigr2MakeListData(WPARAM, LPFILEMIGRCHK);
BOOL    FileMigr2CalcListIndex(WPARAM, LPFILEMIGRIDX);

BOOL    FileMigr25CtrlScrl(HWND, WPARAM, LPARAM, LPFILEMIGRCHK, LPFILEMIGRIDX);
VOID    FileMigr25CtrlChkbox(HWND, WORD wEditId, LPFILEMIGRCHK, LPFILEMIGRIDX);

VOID    FileMigr25SetListDscrb(HWND, LPFILEMIGRIDX);
VOID    FileMigr25SetListData(HWND, LPFILEMIGRCHK, LPFILEMIGRIDX);
BOOL    FileMigr25MakeListData(WPARAM, LPFILEMIGRCHK);
BOOL    FileMigr25CalcListIndex(WPARAM, LPFILEMIGRIDX);

BOOL    FileMigr30CtrlScrl(HWND, WPARAM, LPARAM, LPFILEMIGRCHK, LPFILEMIGRIDX);
VOID    FileMigr30CtrlChkbox(HWND, WORD wEditId, LPFILEMIGRCHK, LPFILEMIGRIDX);

VOID    FileMigr30SetListDscrb(HWND, LPFILEMIGRIDX);
VOID    FileMigr30SetListData(HWND, LPFILEMIGRCHK, LPFILEMIGRIDX);
BOOL    FileMigr30MakeListData(WPARAM, LPFILEMIGRCHK);
BOOL    FileMigr30CalcListIndex(WPARAM, LPFILEMIGRIDX);

BOOL    FileMigr31CtrlScrl(HWND, WPARAM, LPARAM, LPFILEMIGRCHK, LPFILEMIGRIDX);
VOID    FileMigr31CtrlChkbox(HWND, WORD wEditId, LPFILEMIGRCHK, LPFILEMIGRIDX);

VOID    FileMigr31SetListDscrb(HWND, LPFILEMIGRIDX);
VOID    FileMigr31SetListData(HWND, LPFILEMIGRCHK, LPFILEMIGRIDX);
BOOL    FileMigr31MakeListData(WPARAM, LPFILEMIGRCHK);
BOOL    FileMigr31CalcListIndex(WPARAM, LPFILEMIGRIDX);

/* Saratoga */
BOOL    FileMigr34CtrlScrl(HWND, WPARAM, LPARAM, LPFILEMIGRCHK, LPFILEMIGRIDX);
VOID    FileMigr34CtrlChkbox(HWND, WORD wEditId, LPFILEMIGRCHK, LPFILEMIGRIDX);

VOID    FileMigr34SetListDscrb(HWND, LPFILEMIGRIDX);
VOID    FileMigr34SetListData(HWND, LPFILEMIGRCHK, LPFILEMIGRIDX);
BOOL    FileMigr34MakeListData(WPARAM, LPFILEMIGRCHK);
BOOL    FileMigr34CalcListIndex(WPARAM, LPFILEMIGRIDX);

#if defined(POSSIBLE_DEAD_CODE)
BOOL    FileMigrPLUFile_22(LPWSTR szGpDataFName);   /* plu (2170GP --> Saratoga) */
#endif
VOID    FileMigratPLUConv(WCHAR*);

/* ===== End of FILEMIGR.H ===== */
