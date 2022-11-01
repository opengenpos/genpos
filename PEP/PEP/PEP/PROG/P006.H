/*
* ---------------------------------------------------------------------------
* Title         :   Program Mode Security Code Header File (Prog. 6)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P006.H
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
#define IDD_P06_EDIT    20601       /* EditText for Security Code       */
#define IDD_P06_SPIN    20602       /* Spin Button for Security Code    */
#define IDD_P06_SECURITY_CODE	20603 //Multilingual 4-23-03 RPH
#define IDD_P06_RNG		20604
/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P06_DATA_MAX     9999       /* Maximum Value of Security Code   */
#define P06_DATA_MIN        0       /* Minimum Value of Security Code   */
#define P06_DATA_LEN        4       /* Length of the Text in EditText   */
#define P06_SPIN_STEP       1       /* Normal Step of Spin Button       */
#define P06_SPIN_TURBO     30       /* Turbo Point of Spin Button       */
#define P06_SPIN_TURSTEP   10       /* Turbo Step of Spin Button        */

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
VOID    P06DispError(HWND);

/* ===== End of File (P006.H) ===== */
