/*
* ---------------------------------------------------------------------------
* Title         :   2170 PEP Common Functions Header File
* Category      :   NCR 2170 PEP for Windows    (Hotel US Model)
* Program Name  :   PEPCOMM.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All right reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Dec-07-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Feb-17-95 : 03.00.00 : H.Ishida   : Add Set Order Entry Prompt Table
*           :          :            : Add Coupon Maintnance
* Apr-05-95 : 03.00.00 : H.Ishida   : Add A/C 33
* Jan-30-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-08-98 : 03.03.00 : A.Mitsui   : V3.3
*** NCR2172 ***
* May-19-00 : 01.02.00 : K.Iwata    : Add A/C128
* ===========================================================================
* PVCS Entry
* ---------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
* ===========================================================================
*/
#ifndef PEPCOMM_INCLUDED_DEFINED
#define PEPCOMM_INCLUDED_DEFINED


// Following added to support multi-lingual and Unicode

// define UWCHAR appropriately for database conversion to multi-lingual
// UWCHAR is only used in the structs for the NHPOS database
#if !defined(UWCHAR)
#if defined(_PEPWIDECHAR)
#define UWCHAR WCHAR
#else
#define UWCHAR UCHAR
#endif
#endif

#include "AW_Interfaces.h"

// xLangId is declared in pep.c and contains current language identifier
extern  LANGID      xLangId;			/* current language						*/
extern  HANDLE      hProgPepInst;   /* Instance Handle of PEP.EXE               */
extern	BOOL	bfPepReloadLayout; /*This will be turned on if the layout needs to be re-loaded JHHJ*/


#define PEP_LOADSTRING_LEN     127        // standard value for length of resource strings

#define PEP_STRING_LEN_MAC(x) (sizeof(x)/sizeof((x)[0]))

/*
* ===========================================================================
*       Common User Message
* ===========================================================================
*/
#define PM_PEPMENU      (WM_USER + 100)
#define PM_TRA_ACCESS   (WM_USER + 101)
#define PM_GET_FCONF    (WM_USER + 102)
#define PM_SET_TRAFLAG  (WM_USER + 103)
#define PM_SELECTCHANGE (WM_USER + 104)     /* For Prog 01 */
#define PM_P03HIDETOOL  (WM_USER + 105)     /* For Prog 03 */
#define PM_KEYENT       (WM_USER + 106)     /* For Act 04 */
#define PM_PAGE_DSP     (WM_USER + 107)     /* For Act 04 */
#define PM_PRN_DSPMSG   (WM_USER + 108)     /* For print message */
#define PM_CMNDLINE_OPEN  (WM_USER + 109)     /* For command line file open */

/*
* ===========================================================================
*       Define Declarations
* ===========================================================================
*/
#define PEPCOMM_DECPNT      46      /* File extension '.'               */

/*
* ===========================================================================
*       Value definition for MakeProcInstance
* ===========================================================================
*/
#define PEP_GETPROC        TRUE            /* for MakeProcInstance */
#define PEP_FREEPROC       FALSE           /* for FreeProcInstance */

/*
* ===========================================================================
*       Common Value Definition
* ===========================================================================
*/
#define PEP_CLASSNAME_LEN   32      /* length of window class name          */

//6/24/04 - increased from 48 to 60 to accomodate for larger caption lengths for French language dll - cdh
#define PEP_CAPTION_LEN     /*48*/60      /* length of caption resource           */
#define PEP_ALLOC_LEN       48      /* length of allocation error resource  */
#define PEP_OVER_LEN        72      /* length of over range error resource  */
#define PEP_STD_DESC_LEN   128      // standard or default length for several buffers
#define PEP_STD_MSG_LEN    256      // standard or default length for other butters
/*
* ===========================================================================
*       Common Macro Definition
* ===========================================================================
*/
#define PEP_HIMASK(x)    (BYTE)((BYTE)(x) & 0x0f)       /* mask high-4bits  */
#define PEP_LOMASK(x)    (BYTE)((BYTE)(x) & 0xf0)       /* mask low-4bits   */
#define PEP_REFER(x)     ((x)=(x))                      /* just refer       */

/*
* ===========================================================================
*       Value definition for Menu Control Identifier
* ===========================================================================
*/
#define PEP_MENU_ON             1       /* enable menu(setup, maintenance)  */
#define PEP_MENU_OFF            2       /* disable menu(setup, maintenance) */
#define PEP_MENU_INDIV_ON       3       /* indivually control               */
#define PEP_MENU_INDIV_OFF      4       /* indivually control               */
#define PEP_MENU_ALL_ON         5       /* enable all menu of pep.exe       */
#define PEP_MENU_ALL_OFF        6       /* disable all menu of pep.exe      */

#define PEP_MENU_GRAYED    (MF_BYPOSITION | MF_GRAYED)    /* disable menu */
#define PEP_MENU_ENABLED   (MF_BYPOSITION | MF_ENABLED)   /* enable menu  */
#define PEP_MENU_NOT_ENABLED  (MF_GRAYED | MF_DISABLED)   /* disable menu  */

    /* ----- Popup Menu Offset (PEP - Main) ----- */

#define PEP_MENU_FILES          0       /* file menu item offset            */
#define PEP_MENU_SETUP          1       /* setup menu item offset           */
#define PEP_MENU_MAINT          2       /* maintenance menu item offset     */
#define PEP_MENU_TRANS          3       /* transfer menu item offset        */
#define PEP_MENU_REPORT         4       /* programmable report menu item offset            */
#define PEP_MENU_HELPS          5       /* help menu item offset            */

/*
* ===========================================================================
*       Value definition for Data Convert Procedure
* ===========================================================================
*/
#define PEP_CONV_CL1            1       /* column 1                         */
#define PEP_CONV_CL2            2       /* column 2                         */
#define PEP_CONV_MLT1         256L      /* multiply value for column 1      */
#define PEP_CONV_MLT2       65536L      /* multiply value for column 2      */

/*
* ===========================================================================
*       Value definition for File Backup Procedure
* ===========================================================================
*/
#define PEP_FILE_BACKUP         1       /* file backup                      */
#define PEP_FILE_RESTORE        2       /* file restore                     */
#define PEP_FILE_BUFF_LEN    2048       /* buffer size for PepFileBackup()  */
#define PEP_FILE_BACKUP_LEN   128       /* length of backup file(full path) */
#define PEP_FILE_TEMP_LEN      16       /* length of temporary file         */

/*
* ===========================================================================
*       Value definition for Mnemonic Procedure
* ===========================================================================
*/
#define PEP_MNEMO_HEIGHT       12       /* mnemonic font height             */

#define PEP_MNEMO_DOUBLE     0x0012       /* double width character code      */
#define PEP_MNEMO_CTRL        WIDE('~')       /* double width control character   */
#define PEP_MNEMO_READ          1       /* read mode in replace menmonic    */
#define PEP_MNEMO_WRITE         2       /* write mode in replace mnemonic   */
#define PEP_MNEMO_CHAR        256       /* number of character              */

/*
* ===========================================================================
*       Value definition for SetFileName Procedure
* ===========================================================================
*/
#define PEP_FILENAME_LEN       256       /*  file name size for caption      */
#define PEP_FILEHEADER_UNUSED  256		 /*  Unused space for file header	 */ //ESMITH LAYOUT

/*
* ===========================================================================
*       Value definition for SpinButton Procedure
* ===========================================================================
*/
#define PEP_SPIN_ROLL           1       /* spin button roll mode            */
#define PEP_SPIN_NOROLL         2       /* spin button no roll mode         */
#define PEP_SPIN_LEN           16       /* edittext buffer size             */
#define PEP_SPIN_CONV          10       /* convert to 10 mode (_ltoa)       */

/*
* ===========================================================================
*       Common Menu Control ID definition
* ===========================================================================
*/
    /* ----- File ----- */
#define IDM_NEW         1300        /* File New                         */
#define IDM_OPEN        1301        /* File Open                        */
#define IDM_SAVE        1302        /* File Save                        */
#define IDM_SAVEAS      1303        /* File Save As                     */
#define IDM_CONFIG      1304        /* Set Terminal configulation       */
#define IDM_DELETE      1305        /* File Delete                      */
#define IDM_PRN         1306        /* Print Out                        */
#define IDM_PRN_PAGE    1307        /* Page setup                       */
#define IDM_PRN_PRINTER 1308        /* Printer setup                    */
#define IDM_EXIT        1309        /* Exit PEP                         */
#define IDM_CHANGE_LANGUAGE 1310    /* Change the current language - multi-lingual */
#define IDM_SECRET_CODE 1311        /* Display the Secret Code needed for Reset Totals in Terminal */

    /* ----- Transfer ----- */
#define IDM_SELECT      1331        /* Select Files                         */
#define IDM_TERMINAL    1332        /* Terminal Communication               */
#define IDM_LOADER      1333        /* Loader Board Communication           */

    /* ----- Report ----- */
#define IDM_PROGREPORT  1341        /* Execute Programmable Report          */
#define IDM_REPORTFILE  1342        /* Report File                          */
#define IDM_CURRENTRPTFILE 1343     /* Current Report File Nae              */

    /* ----- Layout ----- */
#define IDM_LAYOUTTOUCH_SET			1344 //ESMITH LAYOUT
#define IDM_LAYOUTTOUCH_LMNGR		1345
#define IDM_LAYOUTKEYBOARD_SET		1346
#define IDM_LAYOUTKEYBOARD_LMNGR	1347

	/* ----- Receipt Logo ---- */
#define IDM_CHOOSELOGO              1348
#define IDM_EDITLOGO                1349

    /* ----- Help ----- */
#define IDM_HLP_ABOUT   1326        /* Help About                           */
#define IDM_HLP_CONTENTS	1327	/* Help Contents						*/

    /* ----- Setup ----- */
#define IDM_P01         1201        /* Machine Definition Code              */
#define IDM_P02         1202        /* Flexible Memory Assignment           */
#define IDM_P03         1203        /* Function Selelction Code             */
#define IDM_P06         1206        /* Program Mode Security Number         */
#define IDM_P08         1208        /* Supervisor Number/Level              */
#define IDM_P09         1209        /* Action Code Security                 */
#define IDM_P10         1210        /* High Amount Lock-out(HALO) Limit     */
#define IDM_P15         1215        /* Preset Amount for Preset Cash Key    */
#define IDM_P17         1217        /* Hourly Activity Time                 */
#define IDM_P18         1218        /* Print Lines on Slip                  */
#define IDM_P20         1220        /* Transaction Mnemonic                 */
#define IDM_P21         1221        /* Lead Trough Message                  */
#define IDM_P22         1222        /* Report Mnemonic                      */
#define IDM_P23         1223        /* Special Mnemonic                     */
#define IDM_P46         1246        /* Adjective Mnemonics                  */
#define IDM_P47         1247        /* Print Modifier Mnemonics             */
#define IDM_P48         1248        /* Major Department Mnemonics           */
#define IDM_P49         1249        /* Automatic Alternative Remote Printer */
#define IDM_P50         1250        /* Assignment Terminal # Installing Shared Printer */

/* NCR2172 */
#define IDM_P51         1251        /* Assignment IP Address for KDS        */

#define IDM_P54         1254        /* Set Hotel Id and SLD                 */
#define IDM_P57         1257        /* Header Message                       */
#define IDM_P60         1260        /* Total Key Type / Function            */

#define IDM_P62         1262        /* Tender Key Parameter , V3.3          */
#define IDM_P65         1265        /* Multi-Line Display Mnemonics , V3.3  */
#define IDM_P67			1266		/* ACC									*/
#define IDM_P97         1267        /* Terminal information dialog box  */
#define IDM_P98         1268        /* Tender Key setup dialog box */
#define IDM_P72			1269		/* Reason Code mnemonics */

    /* ----- Maintenance ----- */
#define IDM_A04         1004        /* Set Department Number on Menu Page   */
#define IDM_A05         1005        /* Set Menu Page Control Table          */
#define IDM_A07         1007        /* Cashier A/B Key Assignment           */
#define IDM_A20         1020        /* Cashier Maintenance                  */
#define IDM_A33         1033        /* Set Control String Table             */
#define IDM_A50         1050        /* Server Maintenance                   */
#define IDM_A68         1068        /* PLU Maintenance                      */
#define IDM_A71         1071        /* Set PPI Table                        */
#define IDM_A84         1084        /* Set Rounding Table                   */
#define IDM_A86         1086        /* Set Discount % Rates                 */
#define IDM_A87         1087        /* Set Slip Promotion Header            */
#define IDM_A88         1088        /* Set Sales Promotion Message          */
#define IDM_A89         1089        /* Set Currency Conversion Rate         */
#define IDM_A111        1111        /* Set Tare Table                       */
#define IDM_A114        1114        /* Department Maintnance                */

/*  NCR2172*/
/*#define IDM_A115        1115      */  /* Set Department No. on Keyed          */
                                    /*        Department Key                */
#define IDM_A116        1116        /* Set Promotional PLU                  */
#define IDM_A124        1124        /* Set Tax Table                        */
#define IDM_A127        1127        /* Set Tax Rate                         */
#define IDM_A130        1130        /* Set Pig Rule Table            R3.1   */
#define IDM_A133        1133        /* Service Time Parameter Maintenance  R3.1 */
#define IDM_A137        1137        /* Set Beverage Dispenser Parameter R3.1 */
#define IDM_A152        1152        /* Employee Number Assignment           */
#define IDM_A154        1154        /* Set Wage Rate for Job Code    R3.1   */
#define IDM_A160        1160        /* Set Order Entry Prompt Table         */
#define IDM_A161        1161        /* Coupon Maintnance                    */
#define IDM_A162        1162        /* Set Flexible Drivethrough Parameter  */

/* NCR2172 */
#define IDM_A170        1170        /* Set Sales Restriction of PLU         */
#define IDM_A208        3208        /* Set Boundary Age                     */
#define IDM_A209		3209		/* Set Color Palette					*/

/* ### ADD Saratoga (051900) */
#define IDM_A128        3128        /* */

/*
* ===========================================================================
*       Common DialogBox Control ID definition
* ===========================================================================
*/
    /* ----- Common ----- */
#define IDD_EXIT          10        /* Exit button ID                       */
#define IDD_CLEAR         11        /* Clear button ID                      */
#define IDD_ENTER         12        /* Enter button ID                      */
#define IDD_SET           13        /* Set button ID                        */

#define IDD_CHANGE_LANGUAGE 21      /* set the language for multi-lingual   */
#define IDD_CHANGE_LANGUAGE_LIST 22  /* list box for the list of languages for multi-lingual */
#define IDD_LANG_CHOOSE		23 //Multilingual PEP
/*
* ===========================================================================
*       Coommon String ID Definision
* ===========================================================================
*/
    /* ----- Window & DialogBox Title ----- */

#define IDS_PEP_TITLE           (10001)

#define IDS_PEP_CAPTION         0

#define IDS_PEP_CAPTION_P01     (IDS_PEP_CAPTION + 1)
#define IDS_PEP_CAPTION_P02     (IDS_PEP_CAPTION + 2)
#define IDS_PEP_CAPTION_P03     (IDS_PEP_CAPTION + 3)
#define IDS_PEP_CAPTION_P06     (IDS_PEP_CAPTION + 4)
#define IDS_PEP_CAPTION_P08     (IDS_PEP_CAPTION + 5)
#define IDS_PEP_CAPTION_P09     (IDS_PEP_CAPTION + 6)
#define IDS_PEP_CAPTION_P10     (IDS_PEP_CAPTION + 7)
#define IDS_PEP_CAPTION_P15     (IDS_PEP_CAPTION + 8)
#define IDS_PEP_CAPTION_P17     (IDS_PEP_CAPTION + 9)
#define IDS_PEP_CAPTION_P18     (IDS_PEP_CAPTION + 10)
#define IDS_PEP_CAPTION_P20     (IDS_PEP_CAPTION + 11)
#define IDS_PEP_CAPTION_P21     (IDS_PEP_CAPTION + 12)
#define IDS_PEP_CAPTION_P22     (IDS_PEP_CAPTION + 13)
#define IDS_PEP_CAPTION_P23     (IDS_PEP_CAPTION + 14)
#define IDS_PEP_CAPTION_P46     (IDS_PEP_CAPTION + 15)
#define IDS_PEP_CAPTION_P47     (IDS_PEP_CAPTION + 16)
#define IDS_PEP_CAPTION_P48     (IDS_PEP_CAPTION + 17)
#define IDS_PEP_CAPTION_P49     (IDS_PEP_CAPTION + 18)
#define IDS_PEP_CAPTION_P50     (IDS_PEP_CAPTION + 19)

#define IDS_PEP_CAPTION_P51     (IDS_PEP_CAPTION + 86)  /* NCR2172 */

#define IDS_PEP_CAPTION_P54     (IDS_PEP_CAPTION + 20)
#define IDS_PEP_CAPTION_P57     (IDS_PEP_CAPTION + 21)
#define IDS_PEP_CAPTION_P60     (IDS_PEP_CAPTION + 22)

#define IDS_PEP_CAPTION_P62     (IDS_PEP_CAPTION + 84)  /* V3.3 */
#define IDS_PEP_CAPTION_P65     (IDS_PEP_CAPTION + 85)  /* V3.3 */
#define	IDS_PEP_CAPTION_P67		(IDS_PEP_CAPTION + 97)  /* ACC */ //RLZ
#define	IDS_PEP_CAPTION_P72		(IDS_PEP_CAPTION + 98)  /* ACC */ //RLZ

#define IDS_PEP_CAPTION_A04     (IDS_PEP_CAPTION + 23)
#define IDS_PEP_CAPTION_A05     (IDS_PEP_CAPTION + 24)
#define IDS_PEP_CAPTION_A07     (IDS_PEP_CAPTION + 25)
#define IDS_PEP_CAPTION_A20     (IDS_PEP_CAPTION + 26)
#define IDS_PEP_CAPTION_A33     (IDS_PEP_CAPTION + 27)
#define IDS_PEP_CAPTION_A50     (IDS_PEP_CAPTION + 28)
#define IDS_PEP_CAPTION_A68     (IDS_PEP_CAPTION + 29)
#define IDS_PEP_CAPTION_A71     (IDS_PEP_CAPTION + 30)
#define IDS_PEP_CAPTION_A84     (IDS_PEP_CAPTION + 31)
#define IDS_PEP_CAPTION_A86     (IDS_PEP_CAPTION + 32)
#define IDS_PEP_CAPTION_A87     (IDS_PEP_CAPTION + 33)
#define IDS_PEP_CAPTION_A88     (IDS_PEP_CAPTION + 34)
#define IDS_PEP_CAPTION_A89     (IDS_PEP_CAPTION + 35)
#define IDS_PEP_CAPTION_A111    (IDS_PEP_CAPTION + 36)
#define IDS_PEP_CAPTION_A114    (IDS_PEP_CAPTION + 37)

/* NCR2172 */
#define IDS_PEP_CAPTION_A115    (IDS_PEP_CAPTION + 87)

#define IDS_PEP_CAPTION_A116    (IDS_PEP_CAPTION + 38)
#define IDS_PEP_CAPTION_A124    (IDS_PEP_CAPTION + 39)
#define IDS_PEP_CAPTION_A127    (IDS_PEP_CAPTION + 40)
#define IDS_PEP_CAPTION_A130    (IDS_PEP_CAPTION + 41)
#define IDS_PEP_CAPTION_A133    (IDS_PEP_CAPTION + 42)
#define IDS_PEP_CAPTION_A137    (IDS_PEP_CAPTION + 43)
#define IDS_PEP_CAPTION_A152    (IDS_PEP_CAPTION + 44)
#define IDS_PEP_CAPTION_A154    (IDS_PEP_CAPTION + 45)
#define IDS_PEP_CAPTION_A160    (IDS_PEP_CAPTION + 46)
#define IDS_PEP_CAPTION_A161    (IDS_PEP_CAPTION + 47)
#define IDS_PEP_CAPTION_A162    (IDS_PEP_CAPTION + 48)

#define IDS_PEP_CAPTION_A170    (IDS_PEP_CAPTION + 88) /* NCR2172 */
#define IDS_PEP_CAPTION_A208    (IDS_PEP_CAPTION + 89) /* NCR2172 */
#define	IDS_PEP_CAPTION_A209	(IDS_PEP_CAPTION + 96) /* Color Palette */
#define IDS_PEP_CAPTION_A128    (IDS_PEP_CAPTION + 90) /* ### ADD Saratoga (051900) */

#define IDS_PEP_OK              (IDS_PEP_CAPTION + 53) /* V3.3 */
#define IDS_PEP_CANCEL          (IDS_PEP_CAPTION + 54) /* V3.3 */
#define IDS_SET                 (IDS_PEP_CAPTION + 55) /* V3.3 */

#define IDS_P03_RCLICK			(IDS_PEP_CAPTION + 91) //RPH SR#90

#define IDS_PEP_LICENSE_CAPTION	(IDS_PEP_CAPTION + 92) //Security
#define IDS_PEP_LICENSE_EXPIRED	(IDS_PEP_CAPTION + 93) //Security
#define IDS_PEP_CONTROL_FAILED	(IDS_PEP_CAPTION + 94) //Security
#define IDS_PEP_CAPTION_CONTROL	(IDS_PEP_CAPTION + 95) //Security

    /* ----- Common Error Messages ----- */

#define IDS_PEP_ALLOC_ERR       (IDS_PEP_CAPTION + 49)
#define IDS_PEP_OVERRANGE       (IDS_PEP_CAPTION + 50)
#define IDS_PEP_NOT_IMPLE       (IDS_PEP_CAPTION + 51)
#define IDS_PEP_ERR_EXE         (IDS_PEP_CAPTION + 52)

    /* ----- Temporary File Name for PepFileBackup ----- */

#define IDS_PEP_TEMPFILE        (IDS_PEP_CAPTION + 64)
#define IDS_PEP_TEMPFILE2       (IDS_PEP_CAPTION + 65) /* ###ADD Saratoga */
#define IDS_PEP_TEMPFILE3       (IDS_PEP_CAPTION + 66) /* ###ADD Saratoga */

    /* ----- Each PEP Execute File Name ----- */

#define IDS_PEP_EXENAME         (IDS_PEP_CAPTION + 80)
#define IDS_PEP_DLLNAME_PROG    (IDS_PEP_CAPTION + 81)
#define IDS_PEP_DLLNAME_ACT     (IDS_PEP_CAPTION + 82)
#define IDS_PEP_DLLNAME_COMM    (IDS_PEP_CAPTION + 83)

    /* ----- default MLD File Mnemonic ----- */

#define IDS_MLDMNEMO001 16500                      /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO002 (IDS_MLDMNEMO001 +  1)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO003 (IDS_MLDMNEMO001 +  2)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO004 (IDS_MLDMNEMO001 +  3)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO005 (IDS_MLDMNEMO001 +  4)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO006 (IDS_MLDMNEMO001 +  5)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO007 (IDS_MLDMNEMO001 +  6)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO008 (IDS_MLDMNEMO001 +  7)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO009 (IDS_MLDMNEMO001 +  8)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO010 (IDS_MLDMNEMO001 +  9)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO011 (IDS_MLDMNEMO001 + 10)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO012 (IDS_MLDMNEMO001 + 11)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO013 (IDS_MLDMNEMO001 + 12)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO014 (IDS_MLDMNEMO001 + 13)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO015 (IDS_MLDMNEMO001 + 14)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO016 (IDS_MLDMNEMO001 + 15)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO017 (IDS_MLDMNEMO001 + 16)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO018 (IDS_MLDMNEMO001 + 17)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO019 (IDS_MLDMNEMO001 + 18)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO020 (IDS_MLDMNEMO001 + 19)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO021 (IDS_MLDMNEMO001 + 20)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO022 (IDS_MLDMNEMO001 + 21)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO023 (IDS_MLDMNEMO001 + 22)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO024 (IDS_MLDMNEMO001 + 23)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO025 (IDS_MLDMNEMO001 + 24)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO026 (IDS_MLDMNEMO001 + 25)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO027 (IDS_MLDMNEMO001 + 26)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO028 (IDS_MLDMNEMO001 + 27)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO029 (IDS_MLDMNEMO001 + 28)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO030 (IDS_MLDMNEMO001 + 29)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO031 (IDS_MLDMNEMO001 + 30)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO032 (IDS_MLDMNEMO001 + 31)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO033 (IDS_MLDMNEMO001 + 32)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO034 (IDS_MLDMNEMO001 + 33)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO035 (IDS_MLDMNEMO001 + 34)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO036 (IDS_MLDMNEMO001 + 35)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO037 (IDS_MLDMNEMO001 + 36)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO038 (IDS_MLDMNEMO001 + 37)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO039 (IDS_MLDMNEMO001 + 38)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO040 (IDS_MLDMNEMO001 + 39)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO041 (IDS_MLDMNEMO001 + 40)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO042 (IDS_MLDMNEMO001 + 41)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO043 (IDS_MLDMNEMO001 + 42)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO044 (IDS_MLDMNEMO001 + 43)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO045 (IDS_MLDMNEMO001 + 44)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO046 (IDS_MLDMNEMO001 + 45)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO047 (IDS_MLDMNEMO001 + 46)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO048 (IDS_MLDMNEMO001 + 47)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO049 (IDS_MLDMNEMO001 + 48)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO050 (IDS_MLDMNEMO001 + 49)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO051 (IDS_MLDMNEMO001 + 50)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO052 (IDS_MLDMNEMO001 + 51)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO053 (IDS_MLDMNEMO001 + 52)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO054 (IDS_MLDMNEMO001 + 53)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO055 (IDS_MLDMNEMO001 + 54)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO056 (IDS_MLDMNEMO001 + 55)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO057 (IDS_MLDMNEMO001 + 56)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO058 (IDS_MLDMNEMO001 + 57)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO059 (IDS_MLDMNEMO001 + 58)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO060 (IDS_MLDMNEMO001 + 59)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO061 (IDS_MLDMNEMO001 + 60)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO062 (IDS_MLDMNEMO001 + 61)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO063 (IDS_MLDMNEMO001 + 62)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO064 (IDS_MLDMNEMO001 + 63)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO065 (IDS_MLDMNEMO001 + 64)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO066 (IDS_MLDMNEMO001 + 65)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO067 (IDS_MLDMNEMO001 + 66)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO068 (IDS_MLDMNEMO001 + 67)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO069 (IDS_MLDMNEMO001 + 68)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO070 (IDS_MLDMNEMO001 + 69)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO071 (IDS_MLDMNEMO001 + 70)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO072 (IDS_MLDMNEMO001 + 71)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO073 (IDS_MLDMNEMO001 + 72)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO074 (IDS_MLDMNEMO001 + 73)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO075 (IDS_MLDMNEMO001 + 74)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO076 (IDS_MLDMNEMO001 + 75)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO077 (IDS_MLDMNEMO001 + 76)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO078 (IDS_MLDMNEMO001 + 77)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO079 (IDS_MLDMNEMO001 + 78)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO080 (IDS_MLDMNEMO001 + 79)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO081 (IDS_MLDMNEMO001 + 80)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO082 (IDS_MLDMNEMO001 + 81)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO083 (IDS_MLDMNEMO001 + 82)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO084 (IDS_MLDMNEMO001 + 83)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO085 (IDS_MLDMNEMO001 + 84)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO086 (IDS_MLDMNEMO001 + 85)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO087 (IDS_MLDMNEMO001 + 86)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO088 (IDS_MLDMNEMO001 + 87)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO089 (IDS_MLDMNEMO001 + 88)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO090 (IDS_MLDMNEMO001 + 89)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO091 (IDS_MLDMNEMO001 + 90)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO092 (IDS_MLDMNEMO001 + 91)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO093 (IDS_MLDMNEMO001 + 92)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO094 (IDS_MLDMNEMO001 + 93)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO095 (IDS_MLDMNEMO001 + 94)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO096 (IDS_MLDMNEMO001 + 95)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO097 (IDS_MLDMNEMO001 + 96)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO098 (IDS_MLDMNEMO001 + 97)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO099 (IDS_MLDMNEMO001 + 98)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO100 (IDS_MLDMNEMO001 + 99)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO101 (IDS_MLDMNEMO001 +100)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO102 (IDS_MLDMNEMO001 +101)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO103 (IDS_MLDMNEMO001 +102)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO104 (IDS_MLDMNEMO001 +103)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO105 (IDS_MLDMNEMO001 +104)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO106 (IDS_MLDMNEMO001 +105)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO107 (IDS_MLDMNEMO001 +106)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO108 (IDS_MLDMNEMO001 +107)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO109 (IDS_MLDMNEMO001 +108)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO110 (IDS_MLDMNEMO001 +109)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO111 (IDS_MLDMNEMO001 +110)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO112 (IDS_MLDMNEMO001 +111)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO113 (IDS_MLDMNEMO001 +112)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO114 (IDS_MLDMNEMO001 +113)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO115 (IDS_MLDMNEMO001 +114)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO116 (IDS_MLDMNEMO001 +115)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO117 (IDS_MLDMNEMO001 +116)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO118 (IDS_MLDMNEMO001 +117)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO119 (IDS_MLDMNEMO001 +118)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO120 (IDS_MLDMNEMO001 +119)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO121 (IDS_MLDMNEMO001 +120)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO122 (IDS_MLDMNEMO001 +121)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO123 (IDS_MLDMNEMO001 +122)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO124 (IDS_MLDMNEMO001 +123)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO125 (IDS_MLDMNEMO001 +124)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO126 (IDS_MLDMNEMO001 +125)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO127 (IDS_MLDMNEMO001 +126)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO128 (IDS_MLDMNEMO001 +127)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO129 (IDS_MLDMNEMO001 +128)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO130 (IDS_MLDMNEMO001 +129)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO131 (IDS_MLDMNEMO001 +130)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO132 (IDS_MLDMNEMO001 +131)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO133 (IDS_MLDMNEMO001 +132)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO134 (IDS_MLDMNEMO001 +133)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO135 (IDS_MLDMNEMO001 +134)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO136 (IDS_MLDMNEMO001 +135)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO137 (IDS_MLDMNEMO001 +136)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO138 (IDS_MLDMNEMO001 +137)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO139 (IDS_MLDMNEMO001 +138)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO140 (IDS_MLDMNEMO001 +139)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO141 (IDS_MLDMNEMO001 +140)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO142 (IDS_MLDMNEMO001 +141)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO143 (IDS_MLDMNEMO001 +142)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO144 (IDS_MLDMNEMO001 +143)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO145 (IDS_MLDMNEMO001 +144)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO146 (IDS_MLDMNEMO001 +145)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO147 (IDS_MLDMNEMO001 +146)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO148 (IDS_MLDMNEMO001 +147)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO149 (IDS_MLDMNEMO001 +148)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO150 (IDS_MLDMNEMO001 +149)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO151 (IDS_MLDMNEMO001 +150)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO152 (IDS_MLDMNEMO001 +151)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO153 (IDS_MLDMNEMO001 +152)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO154 (IDS_MLDMNEMO001 +153)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO155 (IDS_MLDMNEMO001 +154)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO156 (IDS_MLDMNEMO001 +155)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO157 (IDS_MLDMNEMO001 +156)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO158 (IDS_MLDMNEMO001 +157)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO159 (IDS_MLDMNEMO001 +158)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO160 (IDS_MLDMNEMO001 +159)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO161 (IDS_MLDMNEMO001 +160)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO162 (IDS_MLDMNEMO001 +161)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO163 (IDS_MLDMNEMO001 +162)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO164 (IDS_MLDMNEMO001 +163)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO165 (IDS_MLDMNEMO001 +164)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO166 (IDS_MLDMNEMO001 +165)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO167 (IDS_MLDMNEMO001 +166)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO168 (IDS_MLDMNEMO001 +167)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO169 (IDS_MLDMNEMO001 +168)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO170 (IDS_MLDMNEMO001 +169)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO171 (IDS_MLDMNEMO001 +170)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO172 (IDS_MLDMNEMO001 +171)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO173 (IDS_MLDMNEMO001 +172)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO174 (IDS_MLDMNEMO001 +173)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO175 (IDS_MLDMNEMO001 +174)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO176 (IDS_MLDMNEMO001 +175)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO177 (IDS_MLDMNEMO001 +176)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO178 (IDS_MLDMNEMO001 +177)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO179 (IDS_MLDMNEMO001 +178)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO180 (IDS_MLDMNEMO001 +179)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO181 (IDS_MLDMNEMO001 +180)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO182 (IDS_MLDMNEMO001 +181)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO183 (IDS_MLDMNEMO001 +182)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO184 (IDS_MLDMNEMO001 +183)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO185 (IDS_MLDMNEMO001 +184)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO186 (IDS_MLDMNEMO001 +185)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO187 (IDS_MLDMNEMO001 +186)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO188 (IDS_MLDMNEMO001 +187)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO189 (IDS_MLDMNEMO001 +188)     /* Default Mnemonic Resource ID */

/*** NCR2172 ***/
#define IDS_MLDMNEMO190 (IDS_MLDMNEMO001 +189)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO191 (IDS_MLDMNEMO001 +190)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO192 (IDS_MLDMNEMO001 +191)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO193 (IDS_MLDMNEMO001 +192)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO194 (IDS_MLDMNEMO001 +193)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO195 (IDS_MLDMNEMO001 +194)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO196 (IDS_MLDMNEMO001 +195)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO197 (IDS_MLDMNEMO001 +196)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO198 (IDS_MLDMNEMO001 +197)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO199 (IDS_MLDMNEMO001 +198)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO200 (IDS_MLDMNEMO001 +199)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO201 (IDS_MLDMNEMO001 +200)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO202 (IDS_MLDMNEMO001 +201)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO203 (IDS_MLDMNEMO001 +202)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO204 (IDS_MLDMNEMO001 +203)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO205 (IDS_MLDMNEMO001 +204)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO206 (IDS_MLDMNEMO001 +205)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO207 (IDS_MLDMNEMO001 +206)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO208 (IDS_MLDMNEMO001 +207)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO209 (IDS_MLDMNEMO001 +208)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO210 (IDS_MLDMNEMO001 +209)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO211 (IDS_MLDMNEMO001 +210)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO212 (IDS_MLDMNEMO001 +211)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO213 (IDS_MLDMNEMO001 +212)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO214 (IDS_MLDMNEMO001 +213)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO215 (IDS_MLDMNEMO001 +214)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO216 (IDS_MLDMNEMO001 +215)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO217 (IDS_MLDMNEMO001 +216)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO218 (IDS_MLDMNEMO001 +217)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO219 (IDS_MLDMNEMO001 +218)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO220 (IDS_MLDMNEMO001 +219)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO221 (IDS_MLDMNEMO001 +220)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO222 (IDS_MLDMNEMO001 +221)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO223 (IDS_MLDMNEMO001 +222)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO224 (IDS_MLDMNEMO001 +223)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO225 (IDS_MLDMNEMO001 +224)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO226 (IDS_MLDMNEMO001 +225)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO227 (IDS_MLDMNEMO001 +226)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO228 (IDS_MLDMNEMO001 +227)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO229 (IDS_MLDMNEMO001 +228)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO230 (IDS_MLDMNEMO001 +229)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO231 (IDS_MLDMNEMO001 +230)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO232 (IDS_MLDMNEMO001 +231)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO233 (IDS_MLDMNEMO001 +232)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO234 (IDS_MLDMNEMO001 +233)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO235 (IDS_MLDMNEMO001 +234)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO236 (IDS_MLDMNEMO001 +235)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO237 (IDS_MLDMNEMO001 +236)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO238 (IDS_MLDMNEMO001 +237)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO239 (IDS_MLDMNEMO001 +238)     /* Default Mnemonic Resource ID */
#define IDS_MLDMNEMO240 (IDS_MLDMNEMO001 +239)     /* Default Mnemonic Resource ID */

/* #define IDS_MLDERR      (IDS_MLDMNEMO001 +200)     Error Message */
#define IDS_MLDERR      (IDS_MLDMNEMO001 +240)     /* Error Message */

#ifdef __cplusplus

extern "C" {

#endif

/*
* ===========================================================================
*       External Work Area Declarations
* ===========================================================================
*/
extern  WCHAR    szCommRptSaveName[PEP_FILENAME_LEN]; /* report ini file name */
extern	WCHAR	 szCommTouchLayoutSaveName[PEP_FILENAME_LEN]; //ESMITH LAYOUT
extern	WCHAR	 szCommKeyboardLayoutSaveName[PEP_FILENAME_LEN];
extern	WCHAR	 szCommLogoSaveName[PEP_FILENAME_LEN]; //JHALL Receipt LOGO
extern	WCHAR	 szCommIconSaveName[PEP_FILENAME_LEN]; //JHALL Icon Directory

extern HANDLE		hResourceDll;   /* Instance Handle of Resource.DLL	defined in pep.c*/
extern HFONT		hResourceFont;	/* font handle for the standard font to use */

/*
* ===========================================================================
*       Structure Type Definision for Common Function
* ===========================================================================
*/
typedef struct  tagPEPSPIN {    /* Spin Button Parameter */
    long    lMax;                   /* Max. Value of EditText       */
    long    lMin;                   /* Min. Value of EditText       */
    short   nStep;                  /* step of increment/decrement  */
    short   nTurbo;                 /* turbo point                  */
    short   nTurStep;               /* step in turbo                */
    BOOL    fSpinStyle;             /* SpinButton Style             */
}PEPSPIN, *LPPEPSPIN;

typedef HGLOBAL *    LPHGLOBAL;
typedef HCURSOR *    LPHCURSOR;
typedef HBRUSH  *    LPHBRUSH;
typedef BOOL    *    LPBOOL;
typedef UINT    *    LPUINT;
typedef short   *    LPSHORT;
typedef WORD    *    LPUSHORT;

/*
* ===========================================================================
*       Function Prototype Declarations
* ===========================================================================
*/
VOID   PepCommInitialize(HANDLE, HANDLE);

#if defined(POSSIBLE_DEAD_CODE)
VOID   PepCommFinalize(VOID);
#endif
void   PepConv4Lto3C(LPBYTE, DWORD);
void   PepConv3Cto4L(LPBYTE, LPDWORD);
void   PepReplaceMnemonic(CONST WCHAR *, WCHAR *, short, BOOL);
wchar_t *PepFetchMdcMnemonic (USHORT usAddr, UCHAR uchMask, UCHAR uchVal, wchar_t *wcsBuff, USHORT usMaxLen); // function source in P001.C for defines.
BOOL   PepSpinProc(HWND, WPARAM, int, LPPEPSPIN);
BOOL   PepMenuControl(HWND, WPARAM, LPARAM);
short  PepFileBackup(HWND, LPCWSTR, BOOL);
void   PepFileDelete(void);
void   PepSetFont(HWND, int);
void   PepSetFileName(HWND, LPCWSTR);

BOOL   MldCreateFile(HWND);
BOOL   RsnCreateFile(HWND);

short  PepIndexFileBackup(HWND, LPCWSTR, BOOL);     /* ###ADD Saratoga */
void   PepIndexFileDelete(void);
short  PepDeptFileBackup(HWND, LPCWSTR, BOOL);     /* ###ADD Saratoga */
void   PepDeptFileDelete(void);
short  PepFileBackupIn(HWND, LPCWSTR, LPCWSTR, BOOL);/* ###ADD Saratoga */

#ifdef __cplusplus
};
#endif
#endif
/* ===== End of File (PEPCOMM.H) ===== */
