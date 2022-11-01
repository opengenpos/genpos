/*
* ---------------------------------------------------------------------------
* Title         :   Wage Rate for Job Code Header File (AC 154)
* Category      :   Setup, NCR 2170 PEP for Windows (US Model)
* Program Name  :   A54.H
* Copyright (C) :   1996, NCR Corp. E&M-OISO, All rights reserved.
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
* Jan-30-96 : 03.00.05 : M.Suzuki   : Initial
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
#define IDD_A154_DESC01	15401
#define IDD_A154_DESC02	(IDD_A154_DESC01 + 1)
#define IDD_A154_DESC03	(IDD_A154_DESC01 + 2)
#define IDD_A154_DESC04	(IDD_A154_DESC01 + 3)
#define IDD_A154_DESC05	(IDD_A154_DESC01 + 4)
#define IDD_A154_DESC06	(IDD_A154_DESC01 + 5)
#define IDD_A154_DESC07	(IDD_A154_DESC01 + 6)
#define IDD_A154_DESC08	(IDD_A154_DESC01 + 7)
#define IDD_A154_DESC09	(IDD_A154_DESC01 + 8)
#define IDD_A154_DESC10	(IDD_A154_DESC01 + 9)

#define IDD_A154_EDIT01	(IDD_A154_DESC01 + 10)
#define IDD_A154_EDIT02	(IDD_A154_DESC01 + 11)
#define IDD_A154_EDIT03	(IDD_A154_DESC01 + 12)
#define IDD_A154_EDIT04	(IDD_A154_DESC01 + 13)
#define IDD_A154_EDIT05	(IDD_A154_DESC01 + 14)
#define IDD_A154_EDIT06	(IDD_A154_DESC01 + 15)
#define IDD_A154_EDIT07	(IDD_A154_DESC01 + 16)
#define IDD_A154_EDIT08	(IDD_A154_DESC01 + 17)
#define IDD_A154_EDIT09	(IDD_A154_DESC01 + 18)
#define IDD_A154_EDIT10	(IDD_A154_DESC01 + 19)

#define IDD_A154_SPIN01	(IDD_A154_DESC01 + 20)
#define IDD_A154_SPIN02	(IDD_A154_DESC01 + 21)
#define IDD_A154_SPIN03	(IDD_A154_DESC01 + 22)
#define IDD_A154_SPIN04	(IDD_A154_DESC01 + 23)
#define IDD_A154_SPIN05	(IDD_A154_DESC01 + 24)
#define IDD_A154_SPIN06	(IDD_A154_DESC01 + 25)
#define IDD_A154_SPIN07	(IDD_A154_DESC01 + 26)
#define IDD_A154_SPIN08	(IDD_A154_DESC01 + 27)
#define IDD_A154_SPIN09	(IDD_A154_DESC01 + 28)
#define IDD_A154_SPIN10	(IDD_A154_DESC01 + 29)

#define IDD_A154_SCBAR	(IDD_A154_DESC01 + 30)

#define IDD_A154_DESC	15432
#define IDD_A154_RATE	15433
#define IDD_A154_CAPTION1	15434

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_A154_DESC01  IDS_A154
#define IDS_A154_DESC02  (IDS_A154_DESC01 + 1)
#define IDS_A154_DESC03  (IDS_A154_DESC01 + 2)
#define IDS_A154_DESC04  (IDS_A154_DESC01 + 3)
#define IDS_A154_DESC05  (IDS_A154_DESC01 + 4)
#define IDS_A154_DESC06  (IDS_A154_DESC01 + 5)
#define IDS_A154_DESC07  (IDS_A154_DESC01 + 6)
#define IDS_A154_DESC08  (IDS_A154_DESC01 + 7)
#define IDS_A154_DESC09  (IDS_A154_DESC01 + 8)
#define IDS_A154_DESC10  (IDS_A154_DESC01 + 9)
#define IDS_A154_DESC11  (IDS_A154_DESC01 + 10)
#define IDS_A154_DESC12  (IDS_A154_DESC01 + 11)
#define IDS_A154_DESC13  (IDS_A154_DESC01 + 12)
#define IDS_A154_DESC14  (IDS_A154_DESC01 + 13)
#define IDS_A154_DESC15  (IDS_A154_DESC01 + 14)
#define IDS_A154_DESC16  (IDS_A154_DESC01 + 15)
#define IDS_A154_DESC17  (IDS_A154_DESC01 + 16)
#define IDS_A154_DESC18  (IDS_A154_DESC01 + 17)
#define IDS_A154_DESC19  (IDS_A154_DESC01 + 18)
#define IDS_A154_DESC20  (IDS_A154_DESC01 + 19)
#define IDS_A154_DESC21  (IDS_A154_DESC01 + 20)
#define IDS_A154_DESC22  (IDS_A154_DESC01 + 21)
#define IDS_A154_DESC23  (IDS_A154_DESC01 + 22)
#define IDS_A154_DESC24  (IDS_A154_DESC01 + 23)
#define IDS_A154_DESC25  (IDS_A154_DESC01 + 24)
#define IDS_A154_DESC26  (IDS_A154_DESC01 + 25)
#define IDS_A154_DESC27  (IDS_A154_DESC01 + 26)
#define IDS_A154_DESC28  (IDS_A154_DESC01 + 27)
#define IDS_A154_DESC29  (IDS_A154_DESC01 + 28)
#define IDS_A154_DESC30  (IDS_A154_DESC01 + 29)
#define IDS_A154_DESC31  (IDS_A154_DESC01 + 30)
#define IDS_A154_DESC32  (IDS_A154_DESC01 + 31)
#define IDS_A154_DESC33  (IDS_A154_DESC01 + 32)
#define IDS_A154_DESC34  (IDS_A154_DESC01 + 33)
#define IDS_A154_DESC35  (IDS_A154_DESC01 + 34)
#define IDS_A154_DESC36  (IDS_A154_DESC01 + 35)
#define IDS_A154_DESC37  (IDS_A154_DESC01 + 36)
#define IDS_A154_DESC38  (IDS_A154_DESC01 + 37)
#define IDS_A154_DESC39  (IDS_A154_DESC01 + 38)
#define IDS_A154_DESC40  (IDS_A154_DESC01 + 39)
#define IDS_A154_DESC41  (IDS_A154_DESC01 + 40)
#define IDS_A154_DESC42  (IDS_A154_DESC01 + 41)
#define IDS_A154_DESC43  (IDS_A154_DESC01 + 42)
#define IDS_A154_DESC44  (IDS_A154_DESC01 + 43)
#define IDS_A154_DESC45  (IDS_A154_DESC01 + 44)
#define IDS_A154_DESC46  (IDS_A154_DESC01 + 45)
#define IDS_A154_DESC47  (IDS_A154_DESC01 + 46)
#define IDS_A154_DESC48  (IDS_A154_DESC01 + 47)
#define IDS_A154_DESC49  (IDS_A154_DESC01 + 48)
#define IDS_A154_DESC50  (IDS_A154_DESC01 + 49)
#define IDS_A154_DESC51  (IDS_A154_DESC01 + 50)
#define IDS_A154_DESC52  (IDS_A154_DESC01 + 51)
#define IDS_A154_DESC53  (IDS_A154_DESC01 + 52)
#define IDS_A154_DESC54  (IDS_A154_DESC01 + 53)
#define IDS_A154_DESC55  (IDS_A154_DESC01 + 54)
#define IDS_A154_DESC56  (IDS_A154_DESC01 + 55)
#define IDS_A154_DESC57  (IDS_A154_DESC01 + 56)
#define IDS_A154_DESC58  (IDS_A154_DESC01 + 57)
#define IDS_A154_DESC59  (IDS_A154_DESC01 + 58)
#define IDS_A154_DESC60  (IDS_A154_DESC01 + 59)
#define IDS_A154_DESC61  (IDS_A154_DESC01 + 60)
#define IDS_A154_DESC62  (IDS_A154_DESC01 + 61)
#define IDS_A154_DESC63  (IDS_A154_DESC01 + 62)
#define IDS_A154_DESC64  (IDS_A154_DESC01 + 63)
#define IDS_A154_DESC65  (IDS_A154_DESC01 + 64)
#define IDS_A154_DESC66  (IDS_A154_DESC01 + 65)
#define IDS_A154_DESC67  (IDS_A154_DESC01 + 66)
#define IDS_A154_DESC68  (IDS_A154_DESC01 + 67)
#define IDS_A154_DESC69  (IDS_A154_DESC01 + 68)
#define IDS_A154_DESC70  (IDS_A154_DESC01 + 69)
#define IDS_A154_DESC71  (IDS_A154_DESC01 + 70)
#define IDS_A154_DESC72  (IDS_A154_DESC01 + 71)
#define IDS_A154_DESC73  (IDS_A154_DESC01 + 72)
#define IDS_A154_DESC74  (IDS_A154_DESC01 + 73)
#define IDS_A154_DESC75  (IDS_A154_DESC01 + 74)
#define IDS_A154_DESC76  (IDS_A154_DESC01 + 75)
#define IDS_A154_DESC77  (IDS_A154_DESC01 + 76)
#define IDS_A154_DESC78  (IDS_A154_DESC01 + 77)
#define IDS_A154_DESC79  (IDS_A154_DESC01 + 78)
#define IDS_A154_DESC80  (IDS_A154_DESC01 + 79)
#define IDS_A154_DESC81  (IDS_A154_DESC01 + 80)
#define IDS_A154_DESC82  (IDS_A154_DESC01 + 81)
#define IDS_A154_DESC83  (IDS_A154_DESC01 + 82)
#define IDS_A154_DESC84  (IDS_A154_DESC01 + 83)
#define IDS_A154_DESC85  (IDS_A154_DESC01 + 84)
#define IDS_A154_DESC86  (IDS_A154_DESC01 + 85)
#define IDS_A154_DESC87  (IDS_A154_DESC01 + 86)
#define IDS_A154_DESC88  (IDS_A154_DESC01 + 87)
#define IDS_A154_DESC89  (IDS_A154_DESC01 + 88)
#define IDS_A154_DESC90  (IDS_A154_DESC01 + 89)
#define IDS_A154_DESC91  (IDS_A154_DESC01 + 90)
#define IDS_A154_DESC92  (IDS_A154_DESC01 + 91)
#define IDS_A154_DESC93  (IDS_A154_DESC01 + 92)
#define IDS_A154_DESC94  (IDS_A154_DESC01 + 93)
#define IDS_A154_DESC95  (IDS_A154_DESC01 + 94)
#define IDS_A154_DESC96  (IDS_A154_DESC01 + 95)
#define IDS_A154_DESC97  (IDS_A154_DESC01 + 96)
#define IDS_A154_DESC98  (IDS_A154_DESC01 + 97)
#define IDS_A154_DESC99  (IDS_A154_DESC01 + 98)
#define IDS_A154_DESC100 (IDS_A154_DESC01 + 99)
                          
/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define A154_ADDR_MAX    MAX_LABORCOST_SIZE /* Maximum Number of Address    */

#define A154_RATE_MAX    65535              /* Maximum Value of Wage Rate Data   */
#define A154_RATE_MIN    0                  /* Minimum Value of Wage Rate Data   */

#define A154_SPIN_STEP   1
#define A154_SPIN_TURBO  0

#define A154_RATE_LEN    5                   /* Maximum Length of Wage Rate Data  */
#define A154_CAP_LEN     PEP_CAPTION_LEN     /* Length of MessgeBox Caption  */
#define A154_ERR_LEN     PEP_OVER_LEN        /* Length of Error Message      */
#define A154_DESC_LEN    128                 /* Length of Description        */

#define A154_ONE_LINE    1                   /* Scroll to One Line Up/ Down  */
#define A154_MAX_PAGE    10                  /* No. of Item in One Page      */
#define A154_ONE_PAGE    (A154_MAX_PAGE - A154_ONE_LINE)
                                            /* Scroll to One Page Up/ Down  */

#define A154_SCBAR_MIN   0                   /* Min. Value of Scroll Range   */
#define A154_SCBAR_MAX   (A154_ADDR_MAX - A154_MAX_PAGE)
                                            /* Max. Value of Scroll Range   */
/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/
#define A154GETINDEX(id, top)   (BYTE)(((id) - IDD_A154_EDIT01) + (top))

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
VOID    A154InitDlg(HWND, LPWORD);
BOOL    A154GetData(HWND, WPARAM, LPWORD);
VOID    A154ShowErr(HWND);
VOID    A154ResetData(HWND, WPARAM);
VOID    A154SpinProc(HWND, WPARAM, LPARAM, LPWORD);
VOID    A154ScrlProc(HWND, WPARAM, LPARAM, LPWORD);
BOOL    A154CalcIndex(WORD wScrlCode);
VOID    A154RedrawText(HWND, LPWORD);
VOID    A154RedrawThumb(LPARAM);

/* ===== End of File (A154.H) ===== */
