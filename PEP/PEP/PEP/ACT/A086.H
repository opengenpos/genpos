/*
* ---------------------------------------------------------------------------
* Title         :   Set Discount/Surcharge/Bonus % Rate Header File (AC 86)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A086.H
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
* Sep-14-98 : 03.03.00 : A.Mitsui   : V3.3
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
#define A86_SPIN_OFFSET 10

#define IDD_A86_DESC01  18601
#define IDD_A86_DESC02  (IDD_A86_DESC01 + 1)
#define IDD_A86_DESC03  (IDD_A86_DESC01 + 2)
#define IDD_A86_DESC04  (IDD_A86_DESC01 + 3)
#define IDD_A86_DESC05  (IDD_A86_DESC01 + 4)
#define IDD_A86_DESC06  (IDD_A86_DESC01 + 5)
#define IDD_A86_DESC07  (IDD_A86_DESC01 + 6)
#define IDD_A86_DESC08  (IDD_A86_DESC01 + 7)
#define IDD_A86_DESC09  (IDD_A86_DESC01 + 8)
#define IDD_A86_DESC10  (IDD_A86_DESC01 + 9)

#define IDD_A86_EDIT01  (IDD_A86_DESC01 + 10)
#define IDD_A86_EDIT02  (IDD_A86_DESC01 + 11)
#define IDD_A86_EDIT03  (IDD_A86_DESC01 + 12)
#define IDD_A86_EDIT04  (IDD_A86_DESC01 + 13)
#define IDD_A86_EDIT05  (IDD_A86_DESC01 + 14)
#define IDD_A86_EDIT06  (IDD_A86_DESC01 + 15)
#define IDD_A86_EDIT07  (IDD_A86_DESC01 + 16)
#define IDD_A86_EDIT08  (IDD_A86_DESC01 + 17)
#define IDD_A86_EDIT09  (IDD_A86_DESC01 + 18)
#define IDD_A86_EDIT10  (IDD_A86_DESC01 + 19)

#define IDD_A86_SPIN01  (IDD_A86_EDIT01 + A86_SPIN_OFFSET)
#define IDD_A86_SPIN02  (IDD_A86_EDIT02 + A86_SPIN_OFFSET)
#define IDD_A86_SPIN03  (IDD_A86_EDIT03 + A86_SPIN_OFFSET)
#define IDD_A86_SPIN04  (IDD_A86_EDIT04 + A86_SPIN_OFFSET)
#define IDD_A86_SPIN05  (IDD_A86_EDIT05 + A86_SPIN_OFFSET)
#define IDD_A86_SPIN06  (IDD_A86_EDIT06 + A86_SPIN_OFFSET)
#define IDD_A86_SPIN07  (IDD_A86_EDIT07 + A86_SPIN_OFFSET)
#define IDD_A86_SPIN08  (IDD_A86_EDIT08 + A86_SPIN_OFFSET)
#define IDD_A86_SPIN09  (IDD_A86_EDIT09 + A86_SPIN_OFFSET)
#define IDD_A86_SPIN10  (IDD_A86_EDIT10 + A86_SPIN_OFFSET)

#define IDD_A86_SCBAR   (IDD_A86_SPIN10 + 1)

#define IDD_A86_STRDESC (IDD_A86_SCBAR + 1)
#define IDD_A86_STRLEN  (IDD_A86_SCBAR + 2)

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_A86_DESC01  IDS_A86
#define IDS_A86_DESC02  (IDS_A86_DESC01 + 1)
#define IDS_A86_DESC03  (IDS_A86_DESC01 + 2)
#define IDS_A86_DESC04  (IDS_A86_DESC01 + 3)
#define IDS_A86_DESC05  (IDS_A86_DESC01 + 4)
#define IDS_A86_DESC06  (IDS_A86_DESC01 + 5)
#define IDS_A86_DESC07  (IDS_A86_DESC01 + 6)
#define IDS_A86_DESC08  (IDS_A86_DESC01 + 7)
#define IDS_A86_DESC09  (IDS_A86_DESC01 + 8)
#define IDS_A86_DESC10  (IDS_A86_DESC01 + 9)
#define IDS_A86_DESC11  (IDS_A86_DESC01 + 10)
#define IDS_A86_DESC12  (IDS_A86_DESC01 + 11)
#define IDS_A86_DESC13  (IDS_A86_DESC01 + 12)
#define IDS_A86_DESC14  (IDS_A86_DESC01 + 13)
#define IDS_A86_DESC15  (IDS_A86_DESC01 + 14)
#define IDS_A86_DESC16  (IDS_A86_DESC01 + 15)
#define IDS_A86_DESC17  (IDS_A86_DESC01 + 16)
#define IDS_A86_DESC18  (IDS_A86_DESC01 + 17)
#define IDS_A86_DESC19  (IDS_A86_DESC01 + 18)
#define IDS_A86_DESC20  (IDS_A86_DESC01 + 19)

#define IDS_A86_DESC21  (IDS_A86_DESC01 + 20)
#define IDS_A86_DESC22  (IDS_A86_DESC01 + 21)
#define IDS_A86_DESC23  (IDS_A86_DESC01 + 22)
#define IDS_A86_DESC24  (IDS_A86_DESC01 + 23)
#define IDS_A86_DESC25  (IDS_A86_DESC01 + 24)
#define IDS_A86_DESC26  (IDS_A86_DESC01 + 25)
#define IDS_A86_DESC27  (IDS_A86_DESC01 + 26)
#define IDS_A86_DESC28  (IDS_A86_DESC01 + 27)
#define IDS_A86_DESC29  (IDS_A86_DESC01 + 28)
#define IDS_A86_DESC30  (IDS_A86_DESC01 + 29)
#define IDS_A86_DESC31  (IDS_A86_DESC01 + 30)
#define IDS_A86_DESC32  (IDS_A86_DESC01 + 31)
#define IDS_A86_DESC33  (IDS_A86_DESC01 + 32)
#define IDS_A86_DESC34  (IDS_A86_DESC01 + 33)
#define IDS_A86_DESC35  (IDS_A86_DESC01 + 34)
#define IDS_A86_DESC36  (IDS_A86_DESC01 + 35)
#define IDS_A86_DESC37  (IDS_A86_DESC01 + 36)
#define IDS_A86_DESC38  (IDS_A86_DESC01 + 37)
#define IDS_A86_DESC39  (IDS_A86_DESC01 + 38)
#define IDS_A86_DESC40  (IDS_A86_DESC01 + 39)

#define IDS_A86_DESC41  (IDS_A86_DESC01 + 40)
#define IDS_A86_DESC42  (IDS_A86_DESC01 + 41)
#define IDS_A86_DESC43  (IDS_A86_DESC01 + 42)
#define IDS_A86_DESC44  (IDS_A86_DESC01 + 43)
#define IDS_A86_DESC45  (IDS_A86_DESC01 + 44)
#define IDS_A86_DESC46  (IDS_A86_DESC01 + 45)
#define IDS_A86_DESC47  (IDS_A86_DESC01 + 46)
#define IDS_A86_DESC48  (IDS_A86_DESC01 + 47)
#define IDS_A86_DESC49  (IDS_A86_DESC01 + 48)
#define IDS_A86_DESC50  (IDS_A86_DESC01 + 49)
#define IDS_A86_DESC51  (IDS_A86_DESC01 + 50)
#define IDS_A86_DESC52  (IDS_A86_DESC01 + 51)
#define IDS_A86_DESC53  (IDS_A86_DESC01 + 52)
#define IDS_A86_DESC54  (IDS_A86_DESC01 + 53)
#define IDS_A86_DESC55  (IDS_A86_DESC01 + 54)
#define IDS_A86_DESC56  (IDS_A86_DESC01 + 55)
#define IDS_A86_DESC57  (IDS_A86_DESC01 + 56)
#define IDS_A86_DESC58  (IDS_A86_DESC01 + 57)
#define IDS_A86_DESC59  (IDS_A86_DESC01 + 58)
#define IDS_A86_DESC60  (IDS_A86_DESC01 + 59)

#define IDS_A86_DESC61  (IDS_A86_DESC01 + 60)
#define IDS_A86_DESC62  (IDS_A86_DESC01 + 61)
#define IDS_A86_DESC63  (IDS_A86_DESC01 + 62)
#define IDS_A86_DESC64  (IDS_A86_DESC01 + 63)
#define IDS_A86_DESC65  (IDS_A86_DESC01 + 64)
#define IDS_A86_DESC66  (IDS_A86_DESC01 + 65)
#define IDS_A86_DESC67  (IDS_A86_DESC01 + 66)
#define IDS_A86_DESC68  (IDS_A86_DESC01 + 67)
#define IDS_A86_DESC69  (IDS_A86_DESC01 + 68)
#define IDS_A86_DESC70  (IDS_A86_DESC01 + 69)
#define IDS_A86_DESC71  (IDS_A86_DESC01 + 70)
#define IDS_A86_DESC72  (IDS_A86_DESC01 + 71)
#define IDS_A86_DESC73  (IDS_A86_DESC01 + 72)
#define IDS_A86_DESC74  (IDS_A86_DESC01 + 73)
#define IDS_A86_DESC75  (IDS_A86_DESC01 + 74)
#define IDS_A86_DESC76  (IDS_A86_DESC01 + 75)
#define IDS_A86_DESC77  (IDS_A86_DESC01 + 76)
#define IDS_A86_DESC78  (IDS_A86_DESC01 + 77)
#define IDS_A86_DESC79  (IDS_A86_DESC01 + 78)
#define IDS_A86_DESC80  (IDS_A86_DESC01 + 79)

#define IDS_A86_DESC81  (IDS_A86_DESC01 + 80)
#define IDS_A86_DESC82  (IDS_A86_DESC01 + 81)
#define IDS_A86_DESC83  (IDS_A86_DESC01 + 82)
#define IDS_A86_DESC84  (IDS_A86_DESC01 + 83)
#define IDS_A86_DESC85  (IDS_A86_DESC01 + 84)
#define IDS_A86_DESC86  (IDS_A86_DESC01 + 85)
#define IDS_A86_DESC87  (IDS_A86_DESC01 + 86)
#define IDS_A86_DESC88  (IDS_A86_DESC01 + 87)
#define IDS_A86_DESC89  (IDS_A86_DESC01 + 88)
#define IDS_A86_DESC90  (IDS_A86_DESC01 + 89)
#define IDS_A86_DESC91  (IDS_A86_DESC01 + 90)
#define IDS_A86_DESC92  (IDS_A86_DESC01 + 91)
#define IDS_A86_DESC93  (IDS_A86_DESC01 + 92)
#define IDS_A86_DESC94  (IDS_A86_DESC01 + 93)
#define IDS_A86_DESC95  (IDS_A86_DESC01 + 94)
#define IDS_A86_DESC96  (IDS_A86_DESC01 + 95)
#define IDS_A86_DESC97  (IDS_A86_DESC01 + 96)
#define IDS_A86_DESC98  (IDS_A86_DESC01 + 97)
#define IDS_A86_DESC99  (IDS_A86_DESC01 + 98)
#define IDS_A86_DESC100  (IDS_A86_DESC01 + 99)

#define IDS_A86_DESC101  (IDS_A86_DESC01 + 100)
#define IDS_A86_DESC102  (IDS_A86_DESC01 + 101)
#define IDS_A86_DESC103  (IDS_A86_DESC01 + 102)
#define IDS_A86_DESC104  (IDS_A86_DESC01 + 103)
#define IDS_A86_DESC105  (IDS_A86_DESC01 + 104)
#define IDS_A86_DESC106  (IDS_A86_DESC01 + 105)
#define IDS_A86_DESC107  (IDS_A86_DESC01 + 106)
#define IDS_A86_DESC108  (IDS_A86_DESC01 + 107)
#define IDS_A86_DESC109  (IDS_A86_DESC01 + 108)
#define IDS_A86_DESC110  (IDS_A86_DESC01 + 109)
#define IDS_A86_DESC111  (IDS_A86_DESC01 + 110)
#define IDS_A86_DESC112  (IDS_A86_DESC01 + 111)
#define IDS_A86_DESC113  (IDS_A86_DESC01 + 112)
#define IDS_A86_DESC114  (IDS_A86_DESC01 + 113)
#define IDS_A86_DESC115  (IDS_A86_DESC01 + 114)
#define IDS_A86_DESC116  (IDS_A86_DESC01 + 115)
#define IDS_A86_DESC117  (IDS_A86_DESC01 + 116)
#define IDS_A86_DESC118  (IDS_A86_DESC01 + 117)
#define IDS_A86_DESC119  (IDS_A86_DESC01 + 118)
#define IDS_A86_DESC120  (IDS_A86_DESC01 + 119)

#define IDS_A86_CAPTION (IDS_A86_DESC01 + 120)
#define IDS_A86_STRDESC (IDS_A86_DESC01 + 121)
#define IDS_A86_STRLEN  (IDS_A86_DESC01 + 122)

#define IDS_A86_MDC_01  (IDS_A86_DESC01 + 123)
#define IDS_A86_MDC_02  (IDS_A86_DESC01 + 124)
#define IDS_A86_MDC_03  (IDS_A86_DESC01 + 125)
#define IDS_A86_MDC_04  (IDS_A86_DESC01 + 126)
#define IDS_A86_MDC_05  (IDS_A86_DESC01 + 127)
#define IDS_A86_MDC_06  (IDS_A86_DESC01 + 128)
#define IDS_A86_MDC_07  (IDS_A86_DESC01 + 129)
#define IDS_A86_MDC_08  (IDS_A86_DESC01 + 130)
#define IDS_A86_MDC_09  (IDS_A86_DESC01 + 131)
#define IDS_A86_MDC_10  (IDS_A86_DESC01 + 132)

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define A86_ADDR_MAX    MAX_DISCRATE_SIZE   /* Maximum Number of Address    */

#define A86_DATA_MAX    100                 /* Max. Value of Input Data     */
#define A86_DATA_MIN    0                   /* Min. Value of Input Data     */

#define A86_SPIN_STEP   1
#define A86_SPIN_TURBO  0

#define A86_DATA_LEN    3                   /* Maxi. Length of Input Data   */
#define A86_CAP_LEN     PEP_CAPTION_LEN     /* Length of MessgeBox Caption  */
#define A86_ERR_LEN     PEP_OVER_LEN        /* Length of Error Message      */
#define A86_DESC_LEN    128                 /* Length of Description        */

#define A86_ONE_LINE    1                   /* Scroll to One Line Up/ Down  */
#define A86_MAX_PAGE    10                  /* No. of Item in One Page      */
#define A86_ONE_PAGE    (A86_MAX_PAGE - A86_ONE_LINE)
                                            /* Scroll to One Page Up/ Down  */

#define A86_SCBAR_MIN   0                   /* Min. Value of Scroll Range   */
#define A86_SCBAR_MAX   (A86_ADDR_MAX - A86_MAX_PAGE)
                                            /* Max. Value of Scroll Range   */
/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/
#define A86GETINDEX(id, top)   (BYTE)(((id) - IDD_A86_EDIT01) + (top))
#define A86GETINDEXBTN(id, top)   (BYTE)(((id) - IDS_A86_MDC_01) + (top))

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
VOID    A086InitDlg(HWND, LPBYTE);
BOOL    A086ChkData(HWND, WORD wEditId, LPBYTE);
VOID    A086ShowErr(HWND);
VOID    A086ResetData(HWND, WPARAM);
VOID    A086SpinProc(HWND, WPARAM, LPARAM, LPBYTE);
VOID    A086ScrlProc(HWND, WPARAM, LPARAM, LPBYTE);
BOOL    A086CalcIndex(WORD wScrlCode);
VOID    A086RedrawText(HWND, LPBYTE);
VOID    A086RedrawThumb(LPARAM);

/* ===== End of File (A086.H) ===== */
