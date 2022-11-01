/*
* ---------------------------------------------------------------------------
* Title         :   Printer Feed Control Header File (Prog. 18)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P018.H
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
#define P18_SPIN_OFFSET    10

#define IDD_P18_01      21801
#define IDD_P18_02      21802
#define IDD_P18_03      21803
#define IDD_P18_04      21804
#define IDD_P18_05      21805
#define IDD_P18_06      21806

#define IDD_P18_SPIN1   (IDD_P18_01 + P18_SPIN_OFFSET)  
#define IDD_P18_SPIN2   (IDD_P18_02 + P18_SPIN_OFFSET)  
#define IDD_P18_SPIN3   (IDD_P18_03 + P18_SPIN_OFFSET)  
#define IDD_P18_SPIN4   (IDD_P18_04 + P18_SPIN_OFFSET)  
#define IDD_P18_SPIN5   (IDD_P18_05 + P18_SPIN_OFFSET)  
#define IDD_P18_SPIN6   (IDD_P18_06 + P18_SPIN_OFFSET)  

#define IDD_P18_SLIP_PRINT	21820
#define IDD_P18_THERMAL_PRINT	21821
#define IDD_P18_ADDRESS1	21822
#define IDD_P18_ADDRESS2	21823
#define IDD_P18_ADDRESS3	21824
#define IDD_P18_ADDRESS4	21825
#define IDD_P18_ADDRESS5	21826
#define IDD_P18_ADDRESS6	21827
#define IDD_P18_ADDRESS1_RNG	21828
#define IDD_P18_ADDRESS2_RNG	21829
#define IDD_P18_ADDRESS3_RNG	21830
#define IDD_P18_ADDRESS4_RNG	21831
#define IDD_P18_ADDRESS5_RNG	21832
#define IDD_P18_ADDRESS6_RNG	21833

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P18_ADDR_MAX    MAX_SLIP_SIZE   /* Max. Number of Address       */
#define P18_ADDR_NOTUSE (IDD_P18_03 - IDD_P18_01)
                                        /* Addr. No. of Reserved        */
#define P18_DATA_MAX    99              /* Max. Value of Input Data     */
#define P18_DATA2_MAX   31              /* Max. Value of Data (Addr. 2) */
#define P18_DATA4_MAX  150              /* Max. Value of Data (Addr. 4) */
#define P18_DATA_MIN     0              /* Min. Value of Input Data     */
#define P18_DATA4_MIN   50              /* Min. Value of Data (Addr. 4) */
#define P18_DATA3_MAX    9              /* Max. Value of Data (Addr. 3) SR 47*/

#define P18_DATA_LEN     2              /* Max. Length of Input Data    */
#define P18_DATA4_LEN    3              /* Max. Length of Data (Addr.4) */

#define P18_SPIN_STEP    1              /* Value of SpinButton Step     */
#define P18_SPIN_TURBO   0              /* SpinButton Turbo Flag        */

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
VOID    P18InitDlg(HWND, LPBYTE);
VOID    P18DispErr(HWND, WPARAM, UINT, UINT);
VOID    P18SpinProc(HWND, WPARAM, LPARAM);
BOOL    P18SaveData(HWND);

/* ===== End of File (P018.H) ===== */
