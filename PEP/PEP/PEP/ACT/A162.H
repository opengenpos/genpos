/*
* ---------------------------------------------------------------------------
* Title         :   Set Discount/Surcharge/Bonus % Rate Header File (AC 162)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A0162.H
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
#define A162_SPIN_OFFSET 10

#define IDD_A162_DESC01 16201
#define IDD_A162_DESC02 (IDD_A162_DESC01 + 1)
#define IDD_A162_DESC03 (IDD_A162_DESC01 + 2)
#define IDD_A162_DESC04 (IDD_A162_DESC01 + 3)
#define IDD_A162_DESC05 (IDD_A162_DESC01 + 4)
#define IDD_A162_DESC06 (IDD_A162_DESC01 + 5)
#define IDD_A162_DESC07 (IDD_A162_DESC01 + 6)
#define IDD_A162_DESC08 (IDD_A162_DESC01 + 7)
#define IDD_A162_DESC09 (IDD_A162_DESC01 + 8)
#define IDD_A162_DESC10 (IDD_A162_DESC01 + 9)

#define IDD_A162_EDIT01 (IDD_A162_DESC01 + 10)
#define IDD_A162_EDIT02 (IDD_A162_DESC01 + 11)
#define IDD_A162_EDIT03 (IDD_A162_DESC01 + 12)
#define IDD_A162_EDIT04 (IDD_A162_DESC01 + 13)
#define IDD_A162_EDIT05 (IDD_A162_DESC01 + 14)
#define IDD_A162_EDIT06 (IDD_A162_DESC01 + 15)
#define IDD_A162_EDIT07 (IDD_A162_DESC01 + 16)
#define IDD_A162_EDIT08 (IDD_A162_DESC01 + 17)
#define IDD_A162_EDIT09 (IDD_A162_DESC01 + 18)
#define IDD_A162_EDIT10 (IDD_A162_DESC01 + 19)

#define IDD_A162_SPIN01 (IDD_A162_EDIT01 + A162_SPIN_OFFSET)
#define IDD_A162_SPIN02 (IDD_A162_EDIT02 + A162_SPIN_OFFSET)
#define IDD_A162_SPIN03 (IDD_A162_EDIT03 + A162_SPIN_OFFSET)
#define IDD_A162_SPIN04 (IDD_A162_EDIT04 + A162_SPIN_OFFSET)
#define IDD_A162_SPIN05 (IDD_A162_EDIT05 + A162_SPIN_OFFSET)
#define IDD_A162_SPIN06 (IDD_A162_EDIT06 + A162_SPIN_OFFSET)
#define IDD_A162_SPIN07 (IDD_A162_EDIT07 + A162_SPIN_OFFSET)
#define IDD_A162_SPIN08 (IDD_A162_EDIT08 + A162_SPIN_OFFSET)
#define IDD_A162_SPIN09 (IDD_A162_EDIT09 + A162_SPIN_OFFSET)
#define IDD_A162_SPIN10 (IDD_A162_EDIT10 + A162_SPIN_OFFSET)

#define IDD_A162_SCBAR  (IDD_A162_SPIN10 + 1)

#define IDD_A162_RENG01 (IDD_A162_SCBAR  + 1)
#define IDD_A162_RENG02 (IDD_A162_RENG01 + 1)
#define IDD_A162_RENG03 (IDD_A162_RENG01 + 2)
#define IDD_A162_RENG04 (IDD_A162_RENG01 + 3)
#define IDD_A162_RENG05 (IDD_A162_RENG01 + 4)
#define IDD_A162_RENG06 (IDD_A162_RENG01 + 5)
#define IDD_A162_RENG07 (IDD_A162_RENG01 + 6)
#define IDD_A162_RENG08 (IDD_A162_RENG01 + 7)
#define IDD_A162_RENG09 (IDD_A162_RENG01 + 8)
#define IDD_A162_RENG10 (IDD_A162_RENG01 + 9)

#define IDD_A162_CAPTION1	16242
#define IDD_A162_CAPTION2	16243
#define IDD_A162_CAPSET01   16244
#define IDD_A162_BTNSET01   16245
#define IDD_A162_CAPSET02   16246
#define IDD_A162_BTNSET02   16247

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_A162_DESC01  IDS_A162
#define IDS_A162_DESC02  (IDS_A162_DESC01 + 1)
#define IDS_A162_DESC03  (IDS_A162_DESC01 + 2)
#define IDS_A162_DESC04  (IDS_A162_DESC01 + 3)
#define IDS_A162_DESC05  (IDS_A162_DESC01 + 4)
#define IDS_A162_DESC06  (IDS_A162_DESC01 + 5)
#define IDS_A162_DESC07  (IDS_A162_DESC01 + 6)
#define IDS_A162_DESC08  (IDS_A162_DESC01 + 7)
#define IDS_A162_DESC09  (IDS_A162_DESC01 + 8)
#define IDS_A162_DESC10  (IDS_A162_DESC01 + 9)
#define IDS_A162_DESC11  (IDS_A162_DESC01 + 10)
#define IDS_A162_DESC12  (IDS_A162_DESC01 + 11)
#define IDS_A162_DESC13  (IDS_A162_DESC01 + 12)
#define IDS_A162_DESC14  (IDS_A162_DESC01 + 13)
#define IDS_A162_DESC15  (IDS_A162_DESC01 + 14)
#define IDS_A162_DESC16  (IDS_A162_DESC01 + 15)
#define IDS_A162_DESC17  (IDS_A162_DESC01 + 16)
#define IDS_A162_DESC18  (IDS_A162_DESC01 + 17)
#define IDS_A162_DESC19  (IDS_A162_DESC01 + 18)
#define IDS_A162_DESC20  (IDS_A162_DESC01 + 19)
#define IDS_A162_DESC21  (IDS_A162_DESC01 + 20)
#define IDS_A162_DESC22  (IDS_A162_DESC01 + 21)
#define IDS_A162_DESC23  (IDS_A162_DESC01 + 22)
#define IDS_A162_DESC24  (IDS_A162_DESC01 + 23)
#define IDS_A162_DESC25  (IDS_A162_DESC01 + 24)
#define IDS_A162_DESC26  (IDS_A162_DESC01 + 25)
#define IDS_A162_DESC27  (IDS_A162_DESC01 + 26)
#define IDS_A162_DESC28  (IDS_A162_DESC01 + 27)
#define IDS_A162_DESC29  (IDS_A162_DESC01 + 28)
#define IDS_A162_DESC30  (IDS_A162_DESC01 + 29)
#define IDS_A162_DESC31  (IDS_A162_DESC01 + 30)
#define IDS_A162_DESC32  (IDS_A162_DESC01 + 31)
#define IDS_A162_DESC33  (IDS_A162_DESC01 + 32)
#define IDS_A162_DESC34  (IDS_A162_DESC01 + 33)
#define IDS_A162_DESC35  (IDS_A162_DESC01 + 34)
#define IDS_A162_DESC36  (IDS_A162_DESC01 + 35)

#define IDS_A162_RENG01  (IDS_A162_DESC01 + 36)
#define IDS_A162_RENG02  (IDS_A162_DESC01 + 37)
#define IDS_A162_RENG03  (IDS_A162_DESC01 + 38)
#define IDS_A162_RENG04  (IDS_A162_DESC01 + 39)
#define IDS_A162_RENG05  (IDS_A162_DESC01 + 40)
#define IDS_A162_RENG06  (IDS_A162_DESC01 + 41)
#define IDS_A162_RENG07  (IDS_A162_DESC01 + 42)
#define IDS_A162_RENG08  (IDS_A162_DESC01 + 43)
#define IDS_A162_RENG09  (IDS_A162_DESC01 + 44)
#define IDS_A162_RENG10  (IDS_A162_DESC01 + 45)
#define IDS_A162_RENG11  (IDS_A162_DESC01 + 46)
#define IDS_A162_RENG12  (IDS_A162_DESC01 + 47)
#define IDS_A162_RENG13  (IDS_A162_DESC01 + 48)
#define IDS_A162_RENG14  (IDS_A162_DESC01 + 49)
#define IDS_A162_RENG15  (IDS_A162_DESC01 + 50)
#define IDS_A162_RENG16  (IDS_A162_DESC01 + 51)
#define IDS_A162_RENG17  (IDS_A162_DESC01 + 52)
#define IDS_A162_RENG18  (IDS_A162_DESC01 + 53)
#define IDS_A162_RENG19  (IDS_A162_DESC01 + 54)
#define IDS_A162_RENG20  (IDS_A162_DESC01 + 55)
#define IDS_A162_RENG21  (IDS_A162_DESC01 + 56)
#define IDS_A162_RENG22  (IDS_A162_DESC01 + 57)
#define IDS_A162_RENG23  (IDS_A162_DESC01 + 58)
#define IDS_A162_RENG24  (IDS_A162_DESC01 + 59)
#define IDS_A162_RENG25  (IDS_A162_DESC01 + 60)
#define IDS_A162_RENG26  (IDS_A162_DESC01 + 61)
#define IDS_A162_RENG27  (IDS_A162_DESC01 + 62)
#define IDS_A162_RENG28  (IDS_A162_DESC01 + 63)
#define IDS_A162_RENG29  (IDS_A162_DESC01 + 64)
#define IDS_A162_RENG30  (IDS_A162_DESC01 + 65)
#define IDS_A162_RENG31  (IDS_A162_DESC01 + 66)
#define IDS_A162_RENG32  (IDS_A162_DESC01 + 67)
#define IDS_A162_RENG33  (IDS_A162_DESC01 + 68)
#define IDS_A162_RENG34  (IDS_A162_DESC01 + 69)
#define IDS_A162_RENG35  (IDS_A162_DESC01 + 70)
#define IDS_A162_RENG36  (IDS_A162_DESC01 + 71)

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define A162_ADDR_MAX    MAX_FLEXDRIVE_SIZE  /* Maximum Number of Address    */

#define A162_DATA_MAX    100                 /* Max. Value of Input Data     */
#define A162_DATA_MIN    0                   /* Min. Value of Input Data     */

#define A162_SPIN_STEP   1
#define A162_SPIN_TURBO  0

#define A162_DATA_LEN    2                   /* Maxi. Length of Input Data   */
#define A162_CAP_LEN     PEP_CAPTION_LEN     /* Length of MessgeBox Caption  */
#define A162_ERR_LEN     PEP_OVER_LEN        /* Length of Error Message      */
#define A162_DESC_LEN    128                 /* Length of Description        */
#define A162_DESC_RENG_LEN    10             /* Length of Renge Description  */

#define A162_ONE_LINE    1                   /* Scroll to One Line Up/ Down  */
#define A162_MAX_PAGE    10                  /* No. of Item in One Page      */
#define A162_ONE_PAGE    (A162_MAX_PAGE - A162_ONE_LINE)
                                            /* Scroll to One Page Up/ Down  */

#define A162_SCBAR_MIN   0                   /* Min. Value of Scroll Range   */
#define A162_SCBAR_MAX   (A162_ADDR_MAX - A162_MAX_PAGE)
                                            /* Max. Value of Scroll Range   */
/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/
#define A162GETINDEX(id, top)   (BYTE)(((id) - IDD_A162_EDIT01) + (top))

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
VOID    A162InitDlg(HWND, LPBYTE);
BOOL    A162ChkData(HWND, WORD wEditId, LPBYTE);
VOID    A162ShowErr(HWND, WPARAM);
VOID    A162ResetData(HWND, WPARAM);
VOID    A162SpinProc(HWND, WPARAM, LPARAM, LPBYTE);
VOID    A162ScrlProc(HWND, WPARAM, LPARAM, LPBYTE);
BOOL    A162CalcIndex(WORD wScrlCode);
VOID    A162RedrawText(HWND, LPBYTE);
VOID    A162RedrawThumb(LPARAM);

/* ===== End of File (A0162.H) ===== */
