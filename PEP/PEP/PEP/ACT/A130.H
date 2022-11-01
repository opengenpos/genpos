/*
* ---------------------------------------------------------------------------
* Title         :   Set Pig Rule Table Header File (AC 130)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A130.H
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
#define A130_SPIN_OFFSET        20
#define IDD_A130_COUNTER        13001       /* Counter Limit for GST    */
#define IDD_A130_PRICE1         13002       /* Max. Unit Price As #1    */
#define IDD_A130_PRICE2         13003       /* Max. Unit Price As #2    */
#define IDD_A130_PRICE3         13004       /* Max. Unit Price As #3    */
#define IDD_A130_PRICE4         13005       /* Max. Unit Price As #4    */
#define IDD_A130_PRICE5         13006       /* Max. Unit Price As #5    */

#define IDD_A130_CAPTION1		13011
#define IDD_A130_CAPTION2		13012
#define IDD_A130_CAPTION3		13013
#define IDD_A130_CAPTION4		13014
#define IDD_A130_CAPTION5		13015
#define IDD_A130_CAPTION6		13016
#define IDD_A130_CAPTION7		13017

#define IDD_A130_COUNT_SPIN     (IDD_A130_COUNTER + A130_SPIN_OFFSET)
#define IDD_A130_PRICE1_SPIN    (IDD_A130_PRICE1  + A130_SPIN_OFFSET)
#define IDD_A130_PRICE2_SPIN    (IDD_A130_PRICE2  + A130_SPIN_OFFSET)
#define IDD_A130_PRICE3_SPIN    (IDD_A130_PRICE3  + A130_SPIN_OFFSET)
#define IDD_A130_PRICE4_SPIN    (IDD_A130_PRICE4  + A130_SPIN_OFFSET)
#define IDD_A130_PRICE5_SPIN    (IDD_A130_PRICE5  + A130_SPIN_OFFSET)

#define IDD_A130_ADDR1_RNG		13031
#define IDD_A130_ADDR2_RNG		13032
#define IDD_A130_ADDR3_RNG		13033
#define IDD_A130_ADDR4_RNG		13034
#define IDD_A130_ADDR5_RNG		13035
#define IDD_A130_ADDR6_RNG		13036


/*
* ===========================================================================
*       Number
* ===========================================================================
*/
#define A130_COUNT_ADDR     0
#define A130_DATA_MIN       0L
#define A130_COUNT_MAX      9L
#define A130_PRICE_MAX      9999999L
#define A130_COUNT_LEN      1
#define A130_PRICE_LEN      7
#define A130_SPIN_STEP      1
#define A130_SPIN_TURBO     50
#define A130_SPIN_TURSTEP   50

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
VOID    A130InitDlg(HWND);
VOID    A130SaveData(HWND);

/* ===== End of File (A130.H) ===== */

