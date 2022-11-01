/*
* ---------------------------------------------------------------------------
* Title         :   Automatic Alternative Remote Printer Header File (Prog. 49)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P049.H
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
* Jan-24-96 : 03.00.05 : M.Suzuki   : Add R3.1
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
#define P49_SPIN_OFFSET    10

#define IDD_P49_01      24901
#define IDD_P49_02      24902
#define IDD_P49_03      24903
#define IDD_P49_04      24904
#define IDD_P49_05      24905
#define IDD_P49_06      24906
#define IDD_P49_07      24907
#define IDD_P49_08      24908

#define IDD_P49_SPIN1   (IDD_P49_01 + P49_SPIN_OFFSET)  
#define IDD_P49_SPIN2   (IDD_P49_02 + P49_SPIN_OFFSET)  
#define IDD_P49_SPIN3   (IDD_P49_03 + P49_SPIN_OFFSET)  
#define IDD_P49_SPIN4   (IDD_P49_04 + P49_SPIN_OFFSET)  
#define IDD_P49_SPIN5   (IDD_P49_05 + P49_SPIN_OFFSET)  
#define IDD_P49_SPIN6   (IDD_P49_06 + P49_SPIN_OFFSET)  
#define IDD_P49_SPIN7   (IDD_P49_07 + P49_SPIN_OFFSET)  
#define IDD_P49_SPIN8   (IDD_P49_08 + P49_SPIN_OFFSET)  

#define IDD_P49_PRINT1	24930
#define IDD_P49_PRINT2	24931
#define IDD_P49_PRINT3	24932
#define IDD_P49_PRINT4	24933
#define IDD_P49_PRINT5	24934
#define IDD_P49_PRINT6	24935
#define IDD_P49_PRINT7	24936
#define IDD_P49_PRINT8	24937
#define IDD_P49_TITLE	24938
#define IDD_P49_CAPTION1	24939
#define IDD_P49_CAPTION2	24940

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P49_ADDR_MAX    MAX_DEST_SIZE   /* Max. Number of Address       */

#define P49_DATA_MAX    8               /* Max. Value of Input Data     */
#define P49_DATA_MIN    0               /* Min. Value of Input Data     */

#define P49_DATA_LEN    1               /* Max. Length of Input Data    */

#define P49_SPIN_STEP   1              /* Value of SpinButton Step     */
#define P49_SPIN_TURBO  0              /* SpinButton Turbo Flag        */

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
VOID    P49InitDlg(HWND);
VOID    P49DispErr(HWND, WPARAM);
VOID    P49SpinProc(HWND, WPARAM, LPARAM);
BOOL    P49SaveData(HWND);

/* ===== End of File (P049.H) ===== */
