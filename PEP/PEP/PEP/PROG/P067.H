/*
* ---------------------------------------------------------------------------
* Title         :	Auto Combinational Coupons (Prog. 67)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P067.H
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
*							RLZ
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
#define P67_AUTO_OFFSET    10

#define IDD_P67_01      26701
#define IDD_P67_02      26702
#define IDD_P67_03      26703
#define IDD_P67_04      26704
#define IDD_P67_05      26705
#define IDD_P67_06      26706
#define IDD_P67_07      26707
#define IDD_P67_08      26708

#define IDD_P67_AUTO1   (IDD_P67_01 + P67_AUTO_OFFSET)  
#define IDD_P67_AUTO2   (IDD_P67_02 + P67_AUTO_OFFSET)  
#define IDD_P67_AUTO3   (IDD_P67_03 + P67_AUTO_OFFSET)  
#define IDD_P67_AUTO4   (IDD_P67_04 + P67_AUTO_OFFSET)  
#define IDD_P67_AUTO5   (IDD_P67_05 + P67_AUTO_OFFSET)  
#define IDD_P67_AUTO6   (IDD_P67_06 + P67_AUTO_OFFSET)  
#define IDD_P67_AUTO7   (IDD_P67_07 + P67_AUTO_OFFSET) 
#define IDD_P67_AUTO8   (IDD_P67_08 + P67_AUTO_OFFSET)
 
#define IDD_P67_AUTO_PRINT		26720
#define IDD_P67_ADDRESS1	26722
#define IDD_P67_ADDRESS2	26723
#define IDD_P67_ADDRESS3	26724
#define IDD_P67_ADDRESS4	26725
#define IDD_P67_ADDRESS5	26726
#define IDD_P67_ADDRESS6	26727
#define IDD_P67_ADDRESS7	26728
#define IDD_P67_ADDRESS8	26729
#define IDD_P67_ADDRESS1_RNG	26730
#define IDD_P67_ADDRESS2_RNG	26731
#define IDD_P67_ADDRESS3_RNG	26732
#define IDD_P67_ADDRESS4_RNG	26733
#define IDD_P67_ADDRESS5_RNG	26734
#define IDD_P67_ADDRESS6_RNG	26735
#define IDD_P67_ADDRESS7_RNG	26736
#define IDD_P67_ADDRESS8_RNG	26737

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/

#define P67_ADDR_MAX    8  
#define P67_ADDR_NOTUSE (IDD_P67_08 - IDD_P67_02)
              
#define P67_DATA_MAX		99                           
#define P67_DATA1_MAX   999999              
#define P67_DATA2_MAX		99              
#define P67_DATA4_MAX	   150              
#define P67_DATA_MIN		 0              
#define P67_DATA4_MIN		50              
#define P67_DATA3_MAX		 9              

#define P67_DATA_LEN     2              
#define P67_DATA1_LEN    12             

#define P67_SPIN_STEP    1              /* Value of SpinButton Step     */
#define P67_SPIN_TURBO   0              /* SpinButton Turbo Flag        */

             
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
VOID    P67InitDlg(HWND, LPBYTE);
VOID    P67DispErr(HWND, WPARAM, UINT, UINT);
VOID    P67SpinProc(HWND, WPARAM, LPARAM);
BOOL    P67SaveData(HWND);

/* ===== End of File (P067.H) ===== */