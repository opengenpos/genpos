/*
* ---------------------------------------------------------------------------
* Title         :   File Migration Module Table File
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILEMIGR.C
* Copyright (C) :   1995, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Apr-25-95 : 03.00.00 : H.Ishida   : Initial (Migrate from HP US Model)
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
*       Include File Declarations
* ===========================================================================
*/

/*
* ===========================================================================
*       Compiler Message Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Public Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Static Work Area Declarations
* ===========================================================================
*/
static  HANDLE     iFileHandle;

/* reference Uniniram.asm */
static  long   nFileMigrNewLen[FILE_MIGRHGV10_NUM_PARA] = {400, /* 1, (P1)  MDC */
                                                       70,  /* 2, (P2)  FLEXMEM */                  
                                                       6600,/* 3, (P3)  FSC 3630->6600*/
                                                       2,   /* 4, (P6)  Security Number */
                                                       4,   /* 5, (P7)  Store/Register No */
                                                       24,  /* 6, (P8)  Supervisor Number Assignment */
                                                       127, /* 7, (P9)  Action Code Security */
                                                       36,  /* 8, (P10) Transaction Halo */         
                                                       98,  /* 9, (P17) Hourly Activity Time */     
                                                       6,   /* 10,(P18) Slip Printer Feed Control */
                                                       3200,/* 11,(P20) Transaction Mnemonics */    /* 1600 -> 3200 */
                                                       2880,/* 12,(P21) Lead Through Table */       /* 2400 -> 4800 */
                                                       288, /* 13,(P22) Report Name */	//144 -> 288
                                                       400, /* 14,(P23) Special Mnemonics */	// 200 -> 400
                                                       20,  /* 15,(P39) PC.IF Size */
                                                       160,  /* 16,(P46) Adjective Mnemonics */		//80 -> 160
                                                       160,  /* 17,(P47) Print Mnemonics */		//80 -> 160
                                                       480, /* 18,(P48) Major Dept. Mnemonics */	//240 -> 480
                                                       8,   /* 19,(P49) Auto Alternative Kitchen Ptr */
                                                       58,  /* 20,(P54) CPM, EPT */
                                                       960, /* 21,(P57) 24 Character Mnemonics */	// 480 -960 
                                                       6,   /* 22,(P60) Total Key Type Assignment */
                                                       54,  /* 23,(P61) Total Key Control *//* ### MOD Saratoga (48 --> 54)(060600) */
                                                       20,  /* 24,(P62) TENDER KEY PARAMETER , V3.3 */
                                                      /* 2970,/* 25,(A4)  Plu Key Size */
                                                       47520,/* 25,(A4)  Plu Key Size 23760 ->47520*/
                                                       23,  /* 26,(A5)  Set Control Table Of Menu Page */
                                                       8,   /* 27,(A6)  Manual Alternative Kitchen Printer */
                                                       4,   /* 28,(A7)  Casher A/B Keys Assignment */
                                                       45,  /* 29,(A84) Set Rounding Table */
                                                       20,  /* 30,(A86) Set Discount Bonus % Rates */ /* V3.3 18->20 */
                                                       144,  /* 31,(A88) Set Sales Promotion Message */
                                                       32,   /* 32,(A89) Set Currency Conversion Rate */
                                                       1,   /* 33,(A169) Set Tone Volume */
                                                      /* 1600, / 34,(A116) Assign Set Menu */
                                                       3840, /* 34,(A116) Assign Set Menu */
                                                       208, /* 35,(A124,130,126) Tax Table Progarmming */
                                                       20,  /* 36,(A127) Tax/Service/Reciprocal Rates */
                                                       /* ### CAUTION!!! Saratoga (Misc Para???)(060600) */
                                                       20,  /* 37,(A111) Trae, Table */
                                                       16,  /* 38,(P15) Preset Amount preset cash key */
                                                       48,  /* 39,(P50) Assignment shared printer */
                                                       840, /* 40,(A87) Soft check */ 
                                                       40,  /* 41,(A130) Pig Rule */
                                                       990, /* 42,(A160) Order Entry Prompt */        /* V3.3 30->99 */
                                                       36,  /* 43,(A162) FLEXIBLE DRIVE THROUGH Parameter */
                                                       4,   /* 44,(A133) Set Service Time Parameter Table */
                                                       200, /* 45,(A150) SET LABOR COST VALUE */
                                                       777,  /* 46,(A137) SET DISPENSER PARAMETER */
														64,  /* 47. Prog, 51, 2172 */
														36,	 /* 48. AC 170, 2172 */
														 3,  /* 49. A/C 208, 2172 */
													   900,  /* 50. A/C #4, 2172 */
														 8  /* 51. SPECIAL COUNTER */
															};

/*  NHPOS 1.x data file */

static  short   nFileMigrHGV10Len[FILE_MIGRHGV10_NUM_PARA] = {400, /* 1, (P1)  MDC */
                                                       70,  /* 2, (P2)  FLEXMEM */                   /* V3.3 Add MLD File */
                                                       2970,/* 3, (P3)  FSC */
                                                       2,   /* 4, (P6)  Security Number */
                                                       4,   /* 5, (P7)  Store/Register No */
                                                       24,  /* 6, (P8)  Supervisor Number Assignment */
                                                       127, /* 7, (P9)  Action Code Security */
                                                       36,  /* 8, (P10) Transaction Halo */         /* V3.3 27->29 */
                                                       98,  /* 9, (P17) Hourly Activity Time */     /* V3.3 25->49 */
                                                       6,   /* 10,(P18) Slip Printer Feed Control */
                                                       1600,/* 11,(P20) Transaction Mnemonics */    /* V3.3 135->150 */
                                                       2400,/* 12,(P21) Lead Through Table */       /* V3.3 120->124 */
                                                       144, /* 13,(P22) Report Name */
                                                       200, /* 14,(P23) Special Mnemonics */
                                                       10,  /* 15,(P39) PC.IF Size */
                                                       80,  /* 16,(P46) Adjective Mnemonics */
                                                       80,  /* 17,(P47) Print Mnemonics */
                                                       240, /* 18,(P48) Major Dept. Mnemonics */
                                                       8,   /* 19,(P49) Auto Alternative Kitchen Ptr */
                                                       29,  /* 20,(P54) CPM, EPT */
                                                       480, /* 21,(P57) 24 Character Mnemonics *//* ### MOD Saratoga (576 --> 480)(060600) */
                                                       6,   /* 22,(P60) Total Key Type Assignment */
                                                       54,  /* 23,(P61) Total Key Control *//* ### MOD Saratoga (48 --> 54)(060600) */
                                                       11,  /* 24,(P62) TENDER KEY PARAMETER , V3.3 */
                                                      /* 2970,/* 25,(A4)  Plu Key Size */
                                                       23760,/* 25,(A4)  Plu Key Size */
                                                       19,  /* 26,(A5)  Set Control Table Of Menu Page */
                                                       8,   /* 27,(A6)  Manual Alternative Kitchen Printer */
                                                       4,   /* 28,(A7)  Casher A/B Keys Assignment */
                                                       45,  /* 29,(A84) Set Rounding Table */
                                                       20,  /* 30,(A86) Set Discount Bonus % Rates */ /* V3.3 18->20 */
                                                       72,  /* 31,(A88) Set Sales Promotion Message */
                                                       32,   /* 32,(A89) Set Currency Conversion Rate */
                                                       1,   /* 33,(A169) Set Tone Volume */
                                                      /* 1600, / 34,(A116) Assign Set Menu */
                                                       1920, /* 34,(A116) Assign Set Menu */
                                                       208, /* 35,(A124,130,126) Tax Table Progarmming */
                                                       20,  /* 36,(A127) Tax/Service/Reciprocal Rates */
                                                       /* ### CAUTION!!! Saratoga (Misc Para???)(060600) */
                                                       20,  /* 37,(A111) Trae, Table */
                                                       16,  /* 38,(P15) Preset Amount preset cash key */
                                                       48,  /* 39,(P50) Assignment shared printer */
                                                       420, /* 40,(A87) Soft check */ 
                                                       40,  /* 41,(A130) Pig Rule */
                                                       990, /* 42,(A160) Order Entry Prompt */        /* V3.3 30->99 */
                                                       36,  /* 43,(A162) FLEXIBLE DRIVE THROUGH Parameter */
                                                       4,   /* 44,(A133) Set Service Time Parameter Table */
                                                       200, /* 45,(A150) SET LABOR COST VALUE */
                                                       777,  /* 46,(A137) SET DISPENSER PARAMETER */
														64,  /* Prog, 51, 2172 */
														36,	 /* AC 170, 2172 */
														 3,  /* A/C 208, 2172 */
													   900,  /* A/C #4, 2172 */
														 8  /* SPECIAL COUNTER */
                                                          };

/* Saratoga Start */

static  short   nFileMigr34Len[FILE_MIGR_NUM_PARA] = {150, /* 1, (P1)  MDC */
                                                       42,  /* 2, (P2)  FLEXMEM */                   /* V3.3 Add MLD File */
                                                       2970,/* 3, (P3)  FSC */
                                                       2,   /* 4, (P6)  Security Number */
                                                       4,   /* 5, (P7)  Store/Register No */
                                                       24,  /* 6, (P8)  Supervisor Number Assignment */
                                                       127, /* 7, (P9)  Action Code Security */
                                                       29,  /* 8, (P10) Transaction Halo */         /* V3.3 27->29 */
                                                       98,  /* 9, (P17) Hourly Activity Time */     /* V3.3 25->49 */
                                                       6,   /* 10,(P18) Slip Printer Feed Control */
                                                       1200,/* 11,(P20) Transaction Mnemonics */    /* V3.3 135->150 */
                                                       1984,/* 12,(P21) Lead Through Table */       /* V3.3 120->124 */
                                                       144, /* 13,(P22) Report Name */
                                                       128, /* 14,(P23) Special Mnemonics */
                                                       10,  /* 15,(P39) PC.IF Size */
                                                       80,  /* 16,(P46) Adjective Mnemonics */
                                                       80,  /* 17,(P47) Print Mnemonics */
                                                       80,  /* 18,(P48) Major Dept. Mnemonics */
                                                       8,   /* 19,(P49) Auto Alternative Kitchen Ptr */
                                                       29,  /* 20,(P54) CPM, EPT */
                                                       288, /* 21,(P57) 24 Character Mnemonics */
                                                       6,   /* 22,(P60) Total Key Type Assignment */
                                                       48,  /* 23,(P61) Total Key Control */
                                                       11,  /* 24,(P62) TENDER KEY PARAMETER , V3.3 */
                                                       2970,/* 25,(A4)  Plu Key Size */
                                                       10,  /* 26,(A5)  Set Control Table Of Menu Page */
                                                       8,   /* 27,(A6)  Manual Alternative Kitchen Printer */
                                                       4,   /* 28,(A7)  Casher A/B Keys Assignment */
                                                       18,  /* 29,(A84) Set Rounding Table */
                                                       20,  /* 30,(A86) Set Discount Bonus % Rates */ /* V3.3 18->20 */
                                                       72,  /* 31,(A88) Set Sales Promotion Message */
                                                       8,   /* 32,(A89) Set Currency Conversion Rate */
                                                       1,   /* 33,(A169) Set Tone Volume */
                                                       360, /* 34,(A116) Assign Set Menu */
                                                       208, /* 35,(A124,130,126) Tax Table Progarmming */
                                                       20,  /* 36,(A127) Tax/Service/Reciprocal Rates */
                                                       20,  /* 37,(A111) Trae, Table */
                                                       16,  /* 38,(P15) Preset Amount preset cash key */
                                                       40,  /* 39,(P50) Assignment shared printer */
                                                       420, /* 40,(A87) Soft check */ 
                                                       40,  /* 41,(A130) Pig Rule */
                                                       990, /* 42,(A160) Order Entry Prompt */        /* V3.3 30->99 */
                                                       36,  /* 43,(A162) FLEXIBLE DRIVE THROUGH Parameter */
                                                       4,   /* 44,(A133) Set Service Time Parameter Table */
                                                       200, /* 45,(A150) SET LABOR COST VALUE */
                                                       777  /* 46,(A137) SET DISPENSER PARAMETER */
                                                          };
/* Saratoga End */

static  short   nFileMigr31Len[FILE_MIGR_NUM_PARA]  = {150, /* 1, MDC */
                                                       39,  /* 2, FLEXMEM */
                                                       2970,/* 3, FSC */
                                                       2,   /* 4, Security Number */
                                                       4,   /* 5, Store/Register No */
                                                       24,  /* 6, Supervisor Number Assignment */
                                                       127, /* 7, Action Code Security */
                                                       27,  /* 8, Transaction Halo */
                                                       50,  /* 9, Hourly Activity Time */
                                                       6,   /* 10,Slip Printer Feed Control */
                                                       1080,/* 11,Transaction Mnemonics */
                                                       1920,/* 12,Lead Through Table */
                                                       144, /* 13,Report Name */
                                                       128, /* 14,Special Mnemonics */
                                                       10,  /* 15,PC.IF Size */
                                                       80,  /* 16,Adjective Mnemonics */
                                                       80,  /* 17,Print Mnemonics */
                                                       80,  /* 18,Major Dept. Mnemonics */
                                                       8,   /* 19,Auto Alternative Kitchen Ptr */
                                                       29,  /* 20,CPM, EPT */
                                                       288, /* 21,24 Character Mnemonics */
                                                       6,   /* 22,Total Key Type Assignment */
                                                       48,  /* 23,Total Key Control */
                                                       0,   /* 24, P62 TENDER KEY PARAMETER , V3.3 */
                                                       2970,/* 25,Plu Key Size */
                                                       10,  /* 26,Set Control Table Of Menu Page */
                                                       8,   /* 27,Manual Alternative Kitchen Printer */
                                                       4,   /* 28,Casher A/B Keys Assignment */
                                                       18,  /* 29,Set Rounding Table */
                                                       18,  /* 30,Set Discount Bonus % Rates */
                                                       72,  /* 31,Set Sales Promotion Message */
                                                       8,   /* 32,Set Currency Conversion Rate */
                                                       1,   /* 33,Set Tone Volume */
                                                       360, /* 34,Assign Set Menu */
                                                       208, /* 35,Tax Table Progarmming */
                                                       20,  /* 36,Tax/Service/Reciprocal Rates */
                                                       20,  /* 37,Trae, Table */
                                                       16,  /* 38, P15 Preset Amount preset cash key */
                                                       40,  /* 39, P50 Assignment shared printer */
                                                       420, /* 40, A/C87 Soft check */ 
                                                       40,  /* 41, A/C 130 Pig Rule */
                                                       300, /* 42, A/C 160 Order Entry Prompt */
                                                       36,  /* 43, A/C 162 FLEXIBLE DRIVE THROUGH Parameter */
                                                       4,   /* 44, A/C 133 Set Service Time Parameter Table */
                                                       200, /* 45, A/C 150 SET LABOR COST VALUE */
                                                       777  /* 46, A/C 137 SET DISPENSER PARAMETER */
                                                          };
/* Saratoga Start */
static  short   nFileMigrGPLen[FILE_MIGRGP_NUM_PARA] = {184, /* 1, MDC */
                                                       27   /* 2, FLEXMEM */
                                                      };
/* Saratoga End   */

static  short   nFileMigr30Len[FILE_MIGR_NUM_PARA] = {127, /* 1, MDC */
                                                       33,  /* 2, FLEXMEM */
                                                       1650,/* 3, FSC */
                                                       2,   /* 4, Security Number */
                                                       4,   /* 5, Store/Register No */
                                                       24,  /* 6, Supervisor Number Assignment */
                                                       100, /* 7, Action Code Security */
                                                       23,  /* 8, Transaction Halo */
                                                       50,  /* 9, Hourly Activity Time */
                                                       6,   /* 10,Slip Printer Feed Control */
                                                       992, /* 11,Transaction Mnemonics */
                                                       1600,/* 12,Lead Through Table */
                                                       138, /* 13,Report Name */
                                                       108, /* 14,Special Mnemonics */
                                                       10,  /* 15,PC.IF Size */
                                                       80,  /* 16,Adjective Mnemonics */
                                                       80,  /* 17,Print Mnemonics */
                                                       80,  /* 18,Major Dept. Mnemonics */
                                                       4,   /* 19,Auto Alternative Kitchen Ptr */
                                                       29,  /* 20,CPM, EPT */
                                                       288, /* 21,24 Character Mnemonics */
                                                       6,   /* 22,Total Key Type Assignment */
                                                       40,  /* 23,Total Key Control */
                                                       0,   /* 24, P62 TENDER KEY PARAMETER , V3.3 */
                                                       1500,/* 25,Plu Key Size */
                                                       6,   /* 26,Set Control Table Of Menu Page */
                                                       4,   /* 27,Manual Alternative Kitchen Printer */
                                                       4,   /* 28,Casher A/B Keys Assignment */
                                                       18,  /* 29,Set Rounding Table */
                                                       14,  /* 30,Set Discount Bonus % Rates */
                                                       72,  /* 31,Set Sales Promotion Message */
                                                       8,   /* 32,Set Currency Conversion Rate */
                                                       1,   /* 33,Set Tone Volume */
                                                       360, /* 34,Assign Set Menu */
                                                       208, /* 35,Tax Table Progarmming */
                                                       20,  /* 36,Tax/Service/Reciprocal Rates */
                                                       20,  /* 37,Trae, Table */
                                                       16,  /* 38, P15 Preset Amount preset cash key */
                                                       36,  /* 39, P50 Assignment shared printer */
                                                       420, /* 40, A/C87 Soft check */ 
                                                       0,   /* 41, A/C 130 Pig Rule */
                                                       300, /* 42, A/C 160 Order Entry Prompt */
                                                       36,  /* 43, A/C 162 FLEXIBLE DRIVE THROUGH Parameter */
                                                       0,   /* 44, A/C 133 Set Service Time Parameter Table */
                                                       0,   /* 45, A/C 150 SET LABOR COST VALUE */
                                                       0    /* 46, A/C 137 SET DISPENSER PARAMETER */
                                                          };
static  short   nFileMigr25Len[FILE_MIGR_NUM_PARA] = {127,  /* 1, MDC */
                                                      27,   /* 2, FLEXMEM */
                                                      330,  /* 3, FSC */
                                                      2,    /* 4, Security Number */
                                                      4,    /* 5, Store/Register No */
                                                      24,   /* 6, Supervisor Number Assignment */
                                                      100,  /* 7, Action Code Security */
                                                      23,   /* 8, Transaction Halo */
                                                      50,   /* 9, Hourly Activity Time */
                                                      6,    /* 10,Slip Printer Feed Control */
                                                      968,  /* 11,Transaction Mnemonics */
                                                      1360, /* 12,Lead Through Table */
                                                      132,  /* 13,Report Name */
                                                      108,  /* 14,Special Mnemonics */
                                                      10,   /* 15,PC.IF Size */
                                                      80,   /* 16,Adjective Mnemonics */
                                                      80,   /* 17,Print Mnemonics */
                                                      80,   /* 18,Major Dept. Mnemonics */
                                                      4,    /* 19,Auto Alternative Kitchen Ptr */
                                                      29,   /* 20,CPM, EPT */
                                                      288,  /* 21,24 Character Mnemonics */
                                                      6,    /* 22,Total Key Type Assignment */
                                                      40,   /* 23,Total Key Control */
                                                      0,    /* 24, P62 TENDER KEY PARAMETER , V3.3 */
                                                      1500, /* 25,Plu Key Size */
                                                      6,    /* 26,Set Control Table Of Menu Page */
                                                      4,    /* 27,Manual Alternative Kitchen Printer */
                                                      4,    /* 28,Casher A/B Keys Assignment */
                                                      18,   /* 29,Set Rounding Table */
                                                      14,   /* 30,Set Discount Bonus % Rates */
                                                      72,   /* 31,Set Sales Promotion Message */
                                                      8,    /* 32,Set Currency Conversion Rate */
                                                      1,    /* 33,Set Tone Volume */
                                                      360,  /* 34,Assign Set Menu */
                                                      208,  /* 35,Tax Table Progarmming */
                                                      20,   /* 36,Tax/Service/Reciprocal Rates */
                                                      20,   /* 37,Trae, Table */
                                                      16,   /* 38, P15 Preset Amount preset cash key */
                                                      32,   /* 39, P50 Assignment shared printer */
                                                      420,  /* 40, A/C87 Soft check */ 
                                                      0,    /* 41, A/C 130 Pig Rule */
                                                      0,    /* 42, A/C 160 Order Entry Prompt */
                                                      0,    /* 43, A/C 162 FLEXIBLE DRIVE THROUGH Parameter */
                                                      0,    /* 44, A/C 133 Set Service Time Parameter Table */
                                                      0,    /* 45, A/C 150 SET LABOR COST VALUE */
                                                      0    /* 46, A/C 137 SET DISPENSER PARAMETER */
                                                        };
static  short   nFileMigrEnhLen[FILE_MIGR_NUM_PARA] = { 100, /*1,  MDC */
                                                        21,  /*2,  FLEXMEM */
                                                        330, /*3,  FSC */
                                                        2,   /*4,  Security Number */
                                                        4,   /*5,  Store/Register No */
                                                        24,  /*6,  Supervisor Number Assignment */
                                                        100, /*7,  Action Code Security */
                                                        20,  /*8,  Transaction Halo */
                                                        50,  /*9,  Hourly Activity Time */
                                                        6,   /*10, Slip Printer Feed Control */
                                                        912, /*11, Transactin Mnemonics */
                                                        1024,/*12, Lead Through Table */
                                                        120, /*13, Report Name */
                                                        104, /*14, Special Mnemonics */
                                                        10,  /*15, PC.IF Size */
                                                        80,  /*16, Adjective Mnemonics */
                                                        80,  /*17, Print Mnemonics */
                                                        80,  /*18, Major Dept. Mnemonics */
                                                        4,   /*19, Auto Alternative Kitchen Ptr */
                                                        0,   /*20, */
                                                        288, /*21, 23 Character Mnemonics */
                                                        6,   /*22, Total Key Type Assignment */
                                                        40,  /*23, Total Key Control */
                                                        0,   /*24, P62 TENDER KEY PARAMETER , V3.3 */
                                                        1500,/*25, Plu Key Size */
                                                        6,   /*26, Set Control Table Of Menu Page */
                                                        4,   /*27, Manual Alternative Kitchen Printer */
                                                        4,   /*28, Casher A/B Keys Assignment */
                                                        18,  /*29, Set Rounding Table */
                                                        14,  /*30, Set discount Bonus % Rates */
                                                        72,  /*31, Set Sales Promotion Message */
                                                        8,   /*32, Set Courrency Conversion Rate */
                                                        1,   /*33, Set Tone Volume */
                                                        360, /*34, Assign Set Menu */
                                                        208, /*35, Tax Table Programming */
                                                        20,  /*36, Tax/Service/Reciprocal Rates */
                                                        20,  /*37, Tare Table */
                                                        16,  /*38, P15 Preset Amount preset cash key */
                                                        32,  /*39, P50 Assignment shared printer */
                                                        210, /*40, A/C87 Soft check */ 
                                                        40,  /*41, A/C 130 Pig Rule */
                                                        0,   /*42, A/C 160 Order Entry Prompt */
                                                        0,   /*43, A/C 162 FLEXIBLE DRIVE THROUGH Parameter */
                                                        0,   /*44, A/C 133 Set Service Time Parameter Table */
                                                        0,   /*45, A/C 150 SET LABOR COST VALUE */
                                                        0   /*46, A/C 137 SET DISPENSER PARAMETER */
                                                           };


static  short   nFileMigrOldLen[FILE_MIGR_NUM_PARA] = {100,   15,  330,    2,    4,
                                                        24,  100,   20,   50,    3,
                                                       960, 1040,  108,  104,   10,
                                                        80,   80,   80,    4,    0,
                                                       264,    6,   40,    0, 1500,    6,
                                                         4,    4,   18,   14,   72,
                                                         8,    1,  360,  206,   20,
                                                        20,    0,    0,    0,    0,
                                                         0,    0,    0,    0,    0};

static  UINT   uiPep_Mf[FLEX_MLD_ADR]     = {PEP_MF_DEPT,          /* change Dept. file   */
                                             PEP_MF_PLU,           /* change PLU file     */
                                             0,                    /* change Server file  *//* Removed V3.3 */
                                             PEP_MF_CASH,          /* change Cashier file */
                                             0,                    /* change Size of EJ file */
                                             PEP_MF_ETK,           /* change ETK File */
                                             0,                    /* change Item Storage file */
                                             0,                    /* change Cons. Storage file */
                                             0,                    /* change GC file */
                                             PEP_MF_CPN,           /* change Coupon File */
                                             PEP_MF_CSTR,          /* change Control String File */
                                             PEP_MF_RPT,           /* change Programmable Report File */
                                             PEP_MF_PPI,           /* change PPI File */
                                             PEP_MF_MLD};         /* change MLD File */

static  WORD   wPep_Acc[FLEX_MLD_ADR]     = {TRANS_ACCESS_DEPT,    /* change Dept. file   */
                                             TRANS_ACCESS_PLU,     /* change PLU file     */
                                             0,                    /* change Server file  *//* Removed V3.3 */
                                             TRANS_ACCESS_CASHIER, /* change Cashier file */
                                             0,                    /* change Size of EJ file */
                                             TRANS_ACCESS_ETK,     /* change ETK File */
                                             0,                    /* change Item Storage file */
                                             0,                    /* change Cons. Storage file */
                                             0,                    /* change GC file */
                                             TRANS_ACCESS_CPN,     /* change Coupon File */
                                             TRANS_ACCESS_CSTR,    /* change Control String File */
                                             TRANS_ACCESS_RPT,     /* change Programmable Report File */
                                             TRANS_ACCESS_PPI,     /* change PPI File */
                                             TRANS_ACCESS_MLD};   /* change MLD File */

/* ===== End of FILEMGTB.C ===== */

