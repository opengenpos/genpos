/*
* ---------------------------------------------------------------------------
* Title         :   Preset Amount for Preset Cash Key Header File (Prog. 15)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P015.H
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
*       Dialog ID 
* ===========================================================================
*/
#define P15_SPIN_OFFSET    10

#define IDD_P15_01      21501
#define IDD_P15_02      21502
#define IDD_P15_03      21503
#define IDD_P15_04      21504
#define IDD_P15_05      21505
#define IDD_P15_06      21506
#define IDD_P15_07      21507
#define IDD_P15_08      21508

#define IDD_P15_SPIN1   (IDD_P15_01 + P15_SPIN_OFFSET)  
#define IDD_P15_SPIN2   (IDD_P15_02 + P15_SPIN_OFFSET)  
#define IDD_P15_SPIN3   (IDD_P15_03 + P15_SPIN_OFFSET)  
#define IDD_P15_SPIN4   (IDD_P15_04 + P15_SPIN_OFFSET)
#define IDD_P15_SPIN5   (IDD_P15_05 + P15_SPIN_OFFSET)  
#define IDD_P15_SPIN6   (IDD_P15_06 + P15_SPIN_OFFSET)  
#define IDD_P15_SPIN7   (IDD_P15_07 + P15_SPIN_OFFSET)  
#define IDD_P15_SPIN8   (IDD_P15_08 + P15_SPIN_OFFSET)

#define IDD_P15_DESC	21520
#define IDD_P15_CASH1	21521
#define IDD_P15_CASH2	21522
#define IDD_P15_CASH3	21523
#define IDD_P15_CASH4	21524
#define IDD_P15_CASH5	21525
#define IDD_P15_CASH6	21526
#define IDD_P15_CASH7	21527
#define IDD_P15_CASH8	21528
#define IDD_P15_DATA_RANGE	21530
#define IDD_P15_CAPTION  	21531

#define IDS_P15_OVER    (IDS_P15)

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P15_ADDR_MAX    MAX_PRESETAMOUNT_SIZE * 2   /* Max. Number of Address   */
#define P15_ADDR_LEN    (MAX_PRESETAMOUNT_SIZE * sizeof(ULONG))

#define P15_DATA_MAX_4      9999999L            /* 4 presets - Max. Value of Input Data */
#define P15_DATA_MAX_8      65000L              /* 8 presets - Max. Value of Input Data */
#define P15_DATA_MIN        0L                  /* Min. Value of Input Data */
#define P15_DATA_LEN        7                   /* Length of Input Data     */

#define P15_SPIN_STEP       1                   /* Spin Button Normal Step  */
#define P15_SPIN_TURBO      50                  /* Spin Button Turbo Point  */
#define P15_SPIN_TURSTEP    50                  /* Spin Button Turbo Step   */

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
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
VOID    P15InitDlg(HWND);
BOOL    P15SaveData(HWND);
BOOL    P15ChkRng(HWND, WORD, LPDWORD);
VOID    P15DispErr(HWND, WORD);

/* ===== End of File (P015.H) ===== */
