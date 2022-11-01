/*
* ---------------------------------------------------------------------------
* Title         :   Set PLU No. on Keyboard (AC 4)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A004.H
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
* Mar-02-96 : 03.00.05 : M.Suzuki   : Chg R3.1
* Jan-01-98 : 01.00.08 : M.Ozawa    : Add A04ConvPluKey() for Correct PLU Key Data
* Sep-10-98 : 03.03.00 : A.Mitsui   : V3.3 Include All Strings
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
*       Dialog ID definition
* ===========================================================================
*/
#define IDD_A04_PAGE    10400   /* menu page                            */
#define IDD_A04_PLUKEY  10401   /* PLU key number                       */
#define IDD_A04_KEYSPN  10402   /* spin button (PLU key No.)            */
#define IDD_A04_KEYMAX  10403   /* data range of PLU key No.            */
#define IDD_A04_PLUNO   10404   /* PLU number                           */
#define IDD_A04_NOSPN   10405   /* spin button (PLU No.)                */

#define IDD_A04_FIXED      -1   /* fixed Key code                       */
#define IDD_A04_KEY01       1   /* FSC Key code                         */
#define IDD_A04_KEY02       2   /* FSC Key code                         */
#define IDD_A04_KEY03       3   /* FSC Key code                         */
#define IDD_A04_KEY04       4   /* FSC Key code                         */
#define IDD_A04_KEY05       5   /* FSC Key code                         */
#define IDD_A04_KEY06       6   /* FSC Key code                         */
#define IDD_A04_KEY07       7   /* FSC Key code                         */
#define IDD_A04_KEY08       8   /* FSC Key code                         */
#define IDD_A04_KEY09       9   /* FSC Key code                         */
#define IDD_A04_KEY10      10   /* FSC Key code                         */
#define IDD_A04_KEY11      11   /* FSC Key code                         */
#define IDD_A04_KEY12      12   /* FSC Key code                         */
#define IDD_A04_KEY13      13   /* FSC Key code                         */
#define IDD_A04_KEY14      14   /* FSC Key code                         */
#define IDD_A04_KEY15      15   /* FSC Key code                         */
#define IDD_A04_KEY16      16   /* FSC Key code                         */
#define IDD_A04_KEY17      17   /* FSC Key code                         */
#define IDD_A04_KEY18      18   /* FSC Key code                         */
#define IDD_A04_KEY19      19   /* FSC Key code                         */
#define IDD_A04_KEY20      20   /* FSC Key code                         */
#define IDD_A04_KEY21      21   /* FSC Key code                         */
#define IDD_A04_KEY22      22   /* FSC Key code                         */
#define IDD_A04_KEY23      23   /* FSC Key code                         */
#define IDD_A04_KEY24      24   /* FSC Key code                         */
#define IDD_A04_KEY25      25   /* FSC Key code                         */
#define IDD_A04_KEY26      26   /* FSC Key code                         */
#define IDD_A04_KEY27      27   /* FSC Key code                         */
#define IDD_A04_KEY28      28   /* FSC Key code                         */
#define IDD_A04_KEY29      29   /* FSC Key code                         */
#define IDD_A04_KEY30      30   /* FSC Key code                         */
#define IDD_A04_KEY31      31   /* FSC Key code                         */
#define IDD_A04_KEY32      32   /* FSC Key code                         */
#define IDD_A04_KEY33      33   /* FSC Key code                         */
#define IDD_A04_KEY34      34   /* FSC Key code                         */
#define IDD_A04_KEY35      35   /* FSC Key code                         */
#define IDD_A04_KEY36      36   /* FSC Key code                         */
#define IDD_A04_KEY37      37   /* FSC Key code                         */
#define IDD_A04_KEY38      38   /* FSC Key code                         */
#define IDD_A04_KEY39      39   /* FSC Key code                         */
#define IDD_A04_KEY40      40   /* FSC Key code                         */
#define IDD_A04_KEY41      41   /* FSC Key code                         */
#define IDD_A04_KEY42      42   /* FSC Key code                         */
#define IDD_A04_KEY43      43   /* FSC Key code                         */
#define IDD_A04_KEY44      44   /* FSC Key code                         */
#define IDD_A04_KEY45      45   /* FSC Key code                         */
#define IDD_A04_KEY46      46   /* FSC Key code                         */
#define IDD_A04_KEY47      47   /* FSC Key code                         */
#define IDD_A04_KEY48      48   /* FSC Key code                         */
#define IDD_A04_KEY49      49   /* FSC Key code                         */
#define IDD_A04_KEY50      50   /* FSC Key code                         */
#define IDD_A04_KEY51      51   /* FSC Key code                         */
#define IDD_A04_KEY52      52   /* FSC Key code                         */
#define IDD_A04_KEY53      53   /* FSC Key code                         */
#define IDD_A04_KEY54      54   /* FSC Key code                         */
#define IDD_A04_KEY55      55   /* FSC Key code                         */
#define IDD_A04_KEY56      56   /* FSC Key code                         */
#define IDD_A04_KEY57      57   /* FSC Key code                         */
#define IDD_A04_KEY58      58   /* FSC Key code                         */
#define IDD_A04_KEY59      59   /* FSC Key code                         */
#define IDD_A04_KEY60      60   /* FSC Key code                         */
#define IDD_A04_KEY61      61   /* FSC Key code                         */
#define IDD_A04_KEY62      62   /* FSC Key code                         */
#define IDD_A04_KEY63      63   /* FSC Key code                         */
#define IDD_A04_KEY64      64   /* FSC Key code                         */
#define IDD_A04_KEY65      65   /* FSC Key code                         */
#define IDD_A04_KEY66      66   /* FSC Key code                         */
#define IDD_A04_KEY67      67   /* FSC Key code                         */
#define IDD_A04_KEY68      68   /* FSC Key code                         */
#define IDD_A04_KEY69      69   /* FSC Key code                         */
#define IDD_A04_KEY70      70   /* FSC Key code                         */
#define IDD_A04_KEY71      71   /* FSC Key code                         */
#define IDD_A04_KEY72      72   /* FSC Key code                         */
#define IDD_A04_KEY73      73   /* FSC Key code                         */
#define IDD_A04_KEY74      74   /* FSC Key code                         */
#define IDD_A04_KEY75      75   /* FSC Key code                         */
#define IDD_A04_KEY76      76   /* FSC Key code                         */
#define IDD_A04_KEY77      77   /* FSC Key code                         */
#define IDD_A04_KEY78      78   /* FSC Key code                         */
#define IDD_A04_KEY79      79   /* FSC Key code                         */
#define IDD_A04_KEY80      80   /* FSC Key code                         */
#define IDD_A04_KEY81      81   /* FSC Key code                         */
#define IDD_A04_KEY82      82   /* FSC Key code                         */
#define IDD_A04_KEY83      83   /* FSC Key code                         */
#define IDD_A04_KEY84      84   /* FSC Key code                         */
#define IDD_A04_KEY85      85   /* FSC Key code                         */
#define IDD_A04_KEY86      86   /* FSC Key code                         */
#define IDD_A04_KEY87      87   /* FSC Key code                         */
#define IDD_A04_KEY88      88   /* FSC Key code                         */
#define IDD_A04_KEY89      89   /* FSC Key code                         */
#define IDD_A04_KEY90      90   /* FSC Key code                         */
#define IDD_A04_KEY91      91   /* FSC Key code                         */
#define IDD_A04_KEY92      92   /* FSC Key code                         */
#define IDD_A04_KEY93      93   /* FSC Key code                         */
#define IDD_A04_KEY94      94   /* FSC Key code                         */
#define IDD_A04_KEY95      95   /* FSC Key code                         */
#define IDD_A04_KEY96      96   /* FSC Key code                         */
#define IDD_A04_KEY97      97   /* FSC Key code                         */
#define IDD_A04_KEY98      98   /* FSC Key code                         */
#define IDD_A04_KEY99      99   /* FSC Key code                         */
#define IDD_A04_KEY100    100   /* FSC Key code                         */
#define IDD_A04_KEY101    101   /* FSC Key code                         */
#define IDD_A04_KEY102    102   /* FSC Key code                         */
#define IDD_A04_KEY103    103   /* FSC Key code                         */
#define IDD_A04_KEY104    104   /* FSC Key code                         */
#define IDD_A04_KEY105    105   /* FSC Key code                         */
#define IDD_A04_KEY106    106   /* FSC Key code                         */
#define IDD_A04_KEY107    107   /* FSC Key code                         */
#define IDD_A04_KEY108    108   /* FSC Key code                         */
#define IDD_A04_KEY109    109   /* FSC Key code                         */
#define IDD_A04_KEY110    110   /* FSC Key code                         */
#define IDD_A04_KEY111    111   /* FSC Key code                         */
#define IDD_A04_KEY112    112   /* FSC Key code                         */
#define IDD_A04_KEY113    113   /* FSC Key code                         */
#define IDD_A04_KEY114    114   /* FSC Key code                         */
#define IDD_A04_KEY115    115   /* FSC Key code                         */
#define IDD_A04_KEY116    116   /* FSC Key code                         */
#define IDD_A04_KEY117    117   /* FSC Key code                         */
#define IDD_A04_KEY118    118   /* FSC Key code                         */
#define IDD_A04_KEY119    119   /* FSC Key code                         */
#define IDD_A04_KEY120    120   /* FSC Key code                         */
#define IDD_A04_KEY121    121   /* FSC Key code                         */
#define IDD_A04_KEY122    122   /* FSC Key code                         */
#define IDD_A04_KEY123    123   /* FSC Key code                         */
#define IDD_A04_KEY124    124   /* FSC Key code                         */
#define IDD_A04_KEY125    125   /* FSC Key code                         */
#define IDD_A04_KEY126    126   /* FSC Key code                         */
#define IDD_A04_KEY127    127   /* FSC Key code                         */
#define IDD_A04_KEY128    128   /* FSC Key code                         */
#define IDD_A04_KEY129    129   /* FSC Key code                         */
#define IDD_A04_KEY130    130   /* FSC Key code                         */
#define IDD_A04_KEY131    131   /* FSC Key code                         */
#define IDD_A04_KEY132    132   /* FSC Key code                         */
#define IDD_A04_KEY133    133   /* FSC Key code                         */
#define IDD_A04_KEY134    134   /* FSC Key code                         */
#define IDD_A04_KEY135    135   /* FSC Key code                         */
#define IDD_A04_KEY136    136   /* FSC Key code                         */
#define IDD_A04_KEY137    137   /* FSC Key code                         */
#define IDD_A04_KEY138    138   /* FSC Key code                         */
#define IDD_A04_KEY139    139   /* FSC Key code                         */
#define IDD_A04_KEY140    140   /* FSC Key code                         */
#define IDD_A04_KEY141    141   /* FSC Key code                         */
#define IDD_A04_KEY142    142   /* FSC Key code                         */
#define IDD_A04_KEY143    143   /* FSC Key code                         */
#define IDD_A04_KEY144    144   /* FSC Key code                         */
#define IDD_A04_KEY145    145   /* FSC Key code                         */
#define IDD_A04_KEY146    146   /* FSC Key code                         */
#define IDD_A04_KEY147    147   /* FSC Key code                         */
#define IDD_A04_KEY148    148   /* FSC Key code                         */
#define IDD_A04_KEY149    149   /* FSC Key code                         */
#define IDD_A04_KEY150    150   /* FSC Key code                         */
#define IDD_A04_KEY151    151   /* FSC Key code                         */
#define IDD_A04_KEY152    152   /* FSC Key code                         */
#define IDD_A04_KEY153    153   /* FSC Key code                         */
#define IDD_A04_KEY154    154   /* FSC Key code                         */
#define IDD_A04_KEY155    155   /* FSC Key code                         */
#define IDD_A04_KEY156    156   /* FSC Key code                         */
#define IDD_A04_KEY157    157   /* FSC Key code                         */
#define IDD_A04_KEY158    158   /* FSC Key code                         */
#define IDD_A04_KEY159    159   /* FSC Key code                         */
#define IDD_A04_KEY160    160   /* FSC Key code                         */
#define IDD_A04_KEY161    161   /* FSC Key code                         */
#define IDD_A04_KEY162    162   /* FSC Key code                         */
#define IDD_A04_KEY163    163   /* FSC Key code                         */
#define IDD_A04_KEY164    164   /* FSC Key code                         */
#define IDD_A04_KEY165    165   /* FSC Key code                         */

#define IDD_A04_FIXCLR    200   /* Fix Key "clr"                        */
#define IDD_A04_FIX00     201   /* Fix Key "0"                          */
#define IDD_A04_FIX01     202   /* Fix Key "1"                          */
#define IDD_A04_FIX02     203   /* Fix Key "2"                          */
#define IDD_A04_FIX03     204   /* Fix Key "3"                          */
#define IDD_A04_FIX04     205   /* Fix Key "4"                          */
#define IDD_A04_FIX05     206   /* Fix Key "5"                          */
#define IDD_A04_FIX06     207   /* Fix Key "6"                          */
#define IDD_A04_FIX07     208   /* Fix Key "7"                          */
#define IDD_A04_FIX08     209   /* Fix Key "8"                          */
#define IDD_A04_FIX09     210   /* Fix Key "9"                          */

#define IDD_A04_STRPAGE         10406
#define IDD_A04_STRPAGELEN      10407
#define IDD_A04_STRPLUKEY       10408
#define IDD_A04_STRKEYMAX       10409
#define IDD_A04_STRPLUNO        10410
#define IDD_A04_STRPLUNOLEN     10411
#define IDD_A04_STRDESC         10412
#define IDD_A04_LABEL           10413   /* Label type    Saratoga       */
#define IDD_A04_STRDESC2        10414   /* Saratoga */
#define IDD_A04_ADDCHG          10415
#define IDD_A04_DEL             10416

#define IDD_A04_GROUPBOX1		10417
/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_A04_CONVPLUKEY  (IDS_A04 + 0)   /* convert plu key */

#define IDS_A04_CAPTION     (IDS_A04 + 1)
#define IDS_A04_PAGE        (IDS_A04 + 2)
#define IDS_A04_PAGELEN     (IDS_A04 + 3)
#define IDS_A04_PLUKEY      (IDS_A04 + 4)
#define IDS_A04_PLUNO       (IDS_A04 + 5)
#define IDS_A04_PLUNOLEN    (IDS_A04 + 6)
#define IDS_A04_DESC        (IDS_A04 + 7)
#define IDS_A04_LABEL       (IDS_A04 + 8)   /* Saratoga */
#define IDS_A04_DESC2       (IDS_A04 + 9)   /* Saratoga */

#define IDS_A04_TYPE_NORMAL    (IDS_A04 + 10)  /* Saratoga */
#define IDS_A04_TYPE_EVERSION  (IDS_A04 + 11)  /* Saratoga */
#define IDS_A04_ERR_PLUNO   (IDS_A04 + 12)     /* Saratoga */
#define IDS_A04_ERR_LABEL   (IDS_A04 + 13)     /* Saratoga */
#define IDS_A04_OVERRANGE   (IDS_A04 + 14)     /* Saratoga */

#define IDS_A04_FIXCLR      (IDS_A04 + 100)
#define IDS_A04_FIX00       (IDS_A04 + 101)
#define IDS_A04_FIX01       (IDS_A04 + 102)
#define IDS_A04_FIX02       (IDS_A04 + 103)
#define IDS_A04_FIX03       (IDS_A04 + 104)
#define IDS_A04_FIX04       (IDS_A04 + 105)
#define IDS_A04_FIX05       (IDS_A04 + 106)
#define IDS_A04_FIX06       (IDS_A04 + 107)
#define IDS_A04_FIX07       (IDS_A04 + 108)
#define IDS_A04_FIX08       (IDS_A04 + 109)
#define IDS_A04_FIX09       (IDS_A04 + 110)

/*
* ===========================================================================
*       Value definition
* ===========================================================================
*/
#define A04_PLUKEY              1   /* PLU key Check Code (FSC)         */
#define A04_DEF_PAGENO          1   /* default page no.                 */
#define A04_NORMAL_END          0   /* end codes                        */
#define A04_MEM_ERROR           1   /* end codes                        */
#define A04_MJR_FSC             0   /* FSC data buffer access index     */
#define A04_EXT_FSC             1   /* FSC data buffer access index     */
#define A04_PAGE_MAX            9   /* number of max page        R3.1       */
#define A04_KEYNO_LEN           3   /* length of PLU key No. data       */
/*#define A04_NO_LEN              4    length of PLU No. data           */
#define A04_KEY_LEN            16   /* length of key button             */
#define A04_PAGE_LEN            8   /* length of page number            */
#define A04_PLUKEY_MIN          1   /* minimum value of PLU key No.     */
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#define A04_PLUNO_MAX        9999   /* maximum value of PLU No.         */
#endif
                                    /* maximum value of PLU No.         */
#define A04_PLUNO_MAX      L"999999999999"
#define A04_PLUNO_MIN           1   /* minimum value of PLU No.         */
#define A04_SPIN_STEP           1   /* step of spin button              */
#define A04_SPIN_TURBO         50   /* turbo point of spin button       */
#define A04_SPIN_TURSTEP       50   /* turbo step of spin button        */
#define A04_ERROR_LEN         256   /* error message of conversion      */

/* Saratoga */
#define A04_BUFF_LEN           64   /* temporary buffer length          */
#define A04_NO_LEN             STD_PLU_NUMBER_LEN   /* length of PLU No. data           */
#define A04_TYPE_MAX            2   /* maximum of PLU Label type        */

#define A04_PLUKEY_001          1   /* PLU key code definitions         */
#define A04_PLUKEY_002          2   /* PLU key code definitions         */
#define A04_PLUKEY_003          3   /* PLU key code definitions         */
#define A04_PLUKEY_004          4   /* PLU key code definitions         */
#define A04_PLUKEY_005          5   /* PLU key code definitions         */
#define A04_PLUKEY_006          6   /* PLU key code definitions         */
#define A04_PLUKEY_007          7   /* PLU key code definitions         */
#define A04_PLUKEY_008          8   /* PLU key code definitions         */
#define A04_PLUKEY_009          9   /* PLU key code definitions         */
#define A04_PLUKEY_010         10   /* PLU key code definitions         */
#define A04_PLUKEY_011         11   /* PLU key code definitions         */
#define A04_PLUKEY_012         12   /* PLU key code definitions         */
#define A04_PLUKEY_013         13   /* PLU key code definitions         */
#define A04_PLUKEY_014         14   /* PLU key code definitions         */
#define A04_PLUKEY_015         15   /* PLU key code definitions         */
#define A04_PLUKEY_016         16   /* PLU key code definitions         */
#define A04_PLUKEY_017         17   /* PLU key code definitions         */
#define A04_PLUKEY_018         18   /* PLU key code definitions         */
#define A04_PLUKEY_019         19   /* PLU key code definitions         */
#define A04_PLUKEY_020         20   /* PLU key code definitions         */
#define A04_PLUKEY_021         21   /* PLU key code definitions         */
#define A04_PLUKEY_022         22   /* PLU key code definitions         */
#define A04_PLUKEY_023         23   /* PLU key code definitions         */
#define A04_PLUKEY_024         24   /* PLU key code definitions         */
#define A04_PLUKEY_025         25   /* PLU key code definitions         */
#define A04_PLUKEY_026         26   /* PLU key code definitions         */
#define A04_PLUKEY_027         27   /* PLU key code definitions         */
#define A04_PLUKEY_028         28   /* PLU key code definitions         */
#define A04_PLUKEY_029         29   /* PLU key code definitions         */
#define A04_PLUKEY_030         30   /* PLU key code definitions         */
#define A04_PLUKEY_031         31   /* PLU key code definitions         */
#define A04_PLUKEY_032         32   /* PLU key code definitions         */
#define A04_PLUKEY_033         33   /* PLU key code definitions         */
#define A04_PLUKEY_034         34   /* PLU key code definitions         */
#define A04_PLUKEY_035         35   /* PLU key code definitions         */
#define A04_PLUKEY_036         36   /* PLU key code definitions         */
#define A04_PLUKEY_037         37   /* PLU key code definitions         */
#define A04_PLUKEY_038         38   /* PLU key code definitions         */
#define A04_PLUKEY_039         39   /* PLU key code definitions         */
#define A04_PLUKEY_040         40   /* PLU key code definitions         */
#define A04_PLUKEY_041         41   /* PLU key code definitions         */
#define A04_PLUKEY_042         42   /* PLU key code definitions         */
#define A04_PLUKEY_043         43   /* PLU key code definitions         */
#define A04_PLUKEY_044         44   /* PLU key code definitions         */
#define A04_PLUKEY_045         45   /* PLU key code definitions         */
#define A04_PLUKEY_046         46   /* PLU key code definitions         */
#define A04_PLUKEY_047         47   /* PLU key code definitions         */
#define A04_PLUKEY_048         48   /* PLU key code definitions         */
#define A04_PLUKEY_049         49   /* PLU key code definitions         */
#define A04_PLUKEY_050         50   /* PLU key code definitions         */
#define A04_PLUKEY_051         51   /* PLU key code definitions         */
#define A04_PLUKEY_052         52   /* PLU key code definitions         */
#define A04_PLUKEY_053         53   /* PLU key code definitions         */
#define A04_PLUKEY_054         54   /* PLU key code definitions         */
#define A04_PLUKEY_055         55   /* PLU key code definitions         */
#define A04_PLUKEY_056         56   /* PLU key code definitions         */
#define A04_PLUKEY_057         57   /* PLU key code definitions         */
#define A04_PLUKEY_058         58   /* PLU key code definitions         */
#define A04_PLUKEY_059         59   /* PLU key code definitions         */
#define A04_PLUKEY_060         60   /* PLU key code definitions         */
#define A04_PLUKEY_061         61   /* PLU key code definitions         */
#define A04_PLUKEY_062         62   /* PLU key code definitions         */
#define A04_PLUKEY_063         63   /* PLU key code definitions         */
#define A04_PLUKEY_064         64   /* PLU key code definitions         */
#define A04_PLUKEY_065         65   /* PLU key code definitions         */
#define A04_PLUKEY_066         66   /* PLU key code definitions         */
#define A04_PLUKEY_067         67   /* PLU key code definitions         */
#define A04_PLUKEY_068         68   /* PLU key code definitions         */
#define A04_PLUKEY_069         68   /* PLU key code definitions         */
#define A04_PLUKEY_070         70   /* PLU key code definitions         */
#define A04_PLUKEY_071         71   /* PLU key code definitions         */
#define A04_PLUKEY_072         72   /* PLU key code definitions         */
#define A04_PLUKEY_073         73   /* PLU key code definitions         */
#define A04_PLUKEY_074         74   /* PLU key code definitions         */
#define A04_PLUKEY_075         75   /* PLU key code definitions         */
#define A04_PLUKEY_076         76   /* PLU key code definitions         */
#define A04_PLUKEY_077         77   /* PLU key code definitions         */
#define A04_PLUKEY_078         78   /* PLU key code definitions         */
#define A04_PLUKEY_079         79   /* PLU key code definitions         */
#define A04_PLUKEY_080         80   /* PLU key code definitions         */
#define A04_PLUKEY_081         81   /* PLU key code definitions         */
#define A04_PLUKEY_082         82   /* PLU key code definitions         */
#define A04_PLUKEY_083         83   /* PLU key code definitions         */
#define A04_PLUKEY_084         84   /* PLU key code definitions         */
#define A04_PLUKEY_085         85   /* PLU key code definitions         */
#define A04_PLUKEY_086         86   /* PLU key code definitions         */
#define A04_PLUKEY_087         87   /* PLU key code definitions         */
#define A04_PLUKEY_088         88   /* PLU key code definitions         */
#define A04_PLUKEY_089         89   /* PLU key code definitions         */
#define A04_PLUKEY_090         90   /* PLU key code definitions         */
#define A04_PLUKEY_091         91   /* PLU key code definitions         */
#define A04_PLUKEY_092         92   /* PLU key code definitions         */
#define A04_PLUKEY_093         93   /* PLU key code definitions         */
#define A04_PLUKEY_094         94   /* PLU key code definitions         */
#define A04_PLUKEY_095         95   /* PLU key code definitions         */
#define A04_PLUKEY_096         96   /* PLU key code definitions         */
#define A04_PLUKEY_097         97   /* PLU key code definitions         */
#define A04_PLUKEY_098         98   /* PLU key code definitions         */
#define A04_PLUKEY_099         99   /* PLU key code definitions         */
#define A04_PLUKEY_100        100   /* PLU key code definitions         */
#define A04_PLUKEY_101        101   /* PLU key code definitions         */
#define A04_PLUKEY_102        102   /* PLU key code definitions         */
#define A04_PLUKEY_103        103   /* PLU key code definitions         */
#define A04_PLUKEY_104        104   /* PLU key code definitions         */
#define A04_PLUKEY_105        105   /* PLU key code definitions         */
#define A04_PLUKEY_106        106   /* PLU key code definitions         */
#define A04_PLUKEY_107        107   /* PLU key code definitions         */
#define A04_PLUKEY_108        108   /* PLU key code definitions         */
#define A04_PLUKEY_109        109   /* PLU key code definitions         */
#define A04_PLUKEY_110        110   /* PLU key code definitions         */
#define A04_PLUKEY_111        111   /* PLU key code definitions         */
#define A04_PLUKEY_112        112   /* PLU key code definitions         */
#define A04_PLUKEY_113        113   /* PLU key code definitions         */
#define A04_PLUKEY_114        114   /* PLU key code definitions         */
#define A04_PLUKEY_115        115   /* PLU key code definitions         */
#define A04_PLUKEY_116        116   /* PLU key code definitions         */
#define A04_PLUKEY_117        117   /* PLU key code definitions         */
#define A04_PLUKEY_118        118   /* PLU key code definitions         */
#define A04_PLUKEY_119        119   /* PLU key code definitions         */
#define A04_PLUKEY_120        120   /* PLU key code definitions         */
#define A04_PLUKEY_121        121   /* PLU key code definitions         */
#define A04_PLUKEY_122        122   /* PLU key code definitions         */
#define A04_PLUKEY_123        123   /* PLU key code definitions         */
#define A04_PLUKEY_124        124   /* PLU key code definitions         */
#define A04_PLUKEY_125        125   /* PLU key code definitions         */
#define A04_PLUKEY_126        126   /* PLU key code definitions         */
#define A04_PLUKEY_127        127   /* PLU key code definitions         */
#define A04_PLUKEY_128        128   /* PLU key code definitions         */
#define A04_PLUKEY_129        129   /* PLU key code definitions         */
#define A04_PLUKEY_130        130   /* PLU key code definitions         */
#define A04_PLUKEY_131        131   /* PLU key code definitions         */
#define A04_PLUKEY_132        132   /* PLU key code definitions         */
#define A04_PLUKEY_133        133   /* PLU key code definitions         */
#define A04_PLUKEY_134        134   /* PLU key code definitions         */
#define A04_PLUKEY_135        135   /* PLU key code definitions         */
#define A04_PLUKEY_136        136   /* PLU key code definitions         */
#define A04_PLUKEY_137        137   /* PLU key code definitions         */
#define A04_PLUKEY_138        138   /* PLU key code definitions         */
#define A04_PLUKEY_139        139   /* PLU key code definitions         */
#define A04_PLUKEY_140        140   /* PLU key code definitions         */
#define A04_PLUKEY_141        141   /* PLU key code definitions         */
#define A04_PLUKEY_142        142   /* PLU key code definitions         */
#define A04_PLUKEY_143        143   /* PLU key code definitions         */
#define A04_PLUKEY_144        144   /* PLU key code definitions         */
#define A04_PLUKEY_145        145   /* PLU key code definitions         */
#define A04_PLUKEY_146        146   /* PLU key code definitions         */
#define A04_PLUKEY_147        147   /* PLU key code definitions         */
#define A04_PLUKEY_148        148   /* PLU key code definitions         */
#define A04_PLUKEY_149        149   /* PLU key code definitions         */
#define A04_PLUKEY_150        150   /* PLU key code definitions         */
#define A04_PLUKEY_151        151   /* PLU key code definitions         */
#define A04_PLUKEY_152        152   /* PLU key code definitions         */
#define A04_PLUKEY_153        153   /* PLU key code definitions         */
#define A04_PLUKEY_154        154   /* PLU key code definitions         */
#define A04_PLUKEY_155        155   /* PLU key code definitions         */
#define A04_PLUKEY_156        156   /* PLU key code definitions         */
#define A04_PLUKEY_157        157   /* PLU key code definitions         */
#define A04_PLUKEY_158        158   /* PLU key code definitions         */
#define A04_PLUKEY_159        159   /* PLU key code definitions         */
#define A04_PLUKEY_160        160   /* PLU key code definitions         */
#define A04_PLUKEY_161        161   /* PLU key code definitions         */
#define A04_PLUKEY_162        162   /* PLU key code definitions         */
#define A04_PLUKEY_163        163   /* PLU key code definitions         */
#define A04_PLUKEY_164        164   /* PLU key code definitions         */
#define A04_PLUKEY_165        165   /* PLU key code definitions         */

#define A04_CAP_LEN            32   /* caption length                   */
#define A04_ERR_LEN           128   /* error message length             */
/*
* ===========================================================================
*       Macro difinition
* ===========================================================================
*/
/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
/* ----- PLU key data associated with FSC Code ----- */

/* Saratoga
typedef struct _A04PLU {
    USHORT  usPluNo;
} A04PLU, FAR *LPA04PLU;
*/

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
typedef struct _A04PLU {
    USHORT  usPluNo;
    UCHAR   uchModStat;                         /* Modifier Status  */
} A04PLU, FAR *LPA04PLU;
#endif

typedef struct _A04PLU {
    UCHAR   uchAdjective;                       /* Adjective Number */
    UCHAR   uchModStat;                         /* Modifier Status  */
    WCHAR   szPluNo[PLU_MAX_DIGIT + 1];         /* PLU No.          */
} A04PLU, FAR *LPA04PLU;

typedef struct _A04DATA {
    A04PLU  PluKey[MAX_PAGE_NO][FSC_PLU_MICROMAX];
} A04DATA, FAR *LPA04DATA;

/* ----- FSC data type definition ----- */

typedef struct _A04FSC {
    PARAFSCTBL UNINT Fsc[MAX_FSC_TBL];
} A04FSC, FAR *LPA04FSC;

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
VOID    A004InitDlg(HWND);
VOID    A004SetPluKey(HWND, LPA04FSC);
VOID    A004SetPluNo(HWND, LPA04DATA);
BOOL    A004ChkRng(HWND, WORD wItem, WORD, WORD);
VOID    A004ExecAbort(HWND, short);
/*** Saratoga 
VOID    A004SavePluData(HWND);
***/
VOID    A004SavePluData(HWND, WPARAM);
VOID    A004CtrlMaintButton(HWND);
BOOL    ChkPluNo(HWND hDlg);
VOID    A004ConvPluKey(HWND, LPA04FSC); /* 1/23/98 */
BOOL    A004ZeroChk(HWND hDlg);         /* Saratoga */
BOOL    A004PluKeyChk(HWND hDlg);       /* Saratoga */

/* ===== End of A004.H ===== */

