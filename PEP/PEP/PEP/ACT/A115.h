/*
* ---------------------------------------------------------------------------
* Title         :   Set Department No. on Keyed Department Key ( AC 115 )
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P010.H
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
* Nov-17-99 : 00.00.01 :            : Initial (NCR2172)
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
#define IDD_A115_DESC01  11501
#define IDD_A115_DESC02  (IDD_A115_DESC01 + 1)
#define IDD_A115_DESC03  (IDD_A115_DESC01 + 2)
#define IDD_A115_DESC04  (IDD_A115_DESC01 + 3)
#define IDD_A115_DESC05  (IDD_A115_DESC01 + 4)
#define IDD_A115_DESC06  (IDD_A115_DESC01 + 5)
#define IDD_A115_DESC07  (IDD_A115_DESC01 + 6)
#define IDD_A115_DESC08  (IDD_A115_DESC01 + 7)
#define IDD_A115_DESC09  (IDD_A115_DESC01 + 8)
#define IDD_A115_DESC10  (IDD_A115_DESC01 + 9)

#define IDD_A115_EDIT01  (IDD_A115_DESC01 + 10)
#define IDD_A115_EDIT02  (IDD_A115_DESC01 + 11)
#define IDD_A115_EDIT03  (IDD_A115_DESC01 + 12)
#define IDD_A115_EDIT04  (IDD_A115_DESC01 + 13)
#define IDD_A115_EDIT05  (IDD_A115_DESC01 + 14)
#define IDD_A115_EDIT06  (IDD_A115_DESC01 + 15)
#define IDD_A115_EDIT07  (IDD_A115_DESC01 + 16)
#define IDD_A115_EDIT08  (IDD_A115_DESC01 + 17)
#define IDD_A115_EDIT09  (IDD_A115_DESC01 + 18)
#define IDD_A115_EDIT10  (IDD_A115_DESC01 + 19)

#define IDD_A115_SPIN01  (IDD_A115_DESC01 + 20)
#define IDD_A115_SPIN02  (IDD_A115_DESC01 + 21)
#define IDD_A115_SPIN03  (IDD_A115_DESC01 + 22)
#define IDD_A115_SPIN04  (IDD_A115_DESC01 + 23)
#define IDD_A115_SPIN05  (IDD_A115_DESC01 + 24)
#define IDD_A115_SPIN06  (IDD_A115_DESC01 + 25)
#define IDD_A115_SPIN07  (IDD_A115_DESC01 + 26)
#define IDD_A115_SPIN08  (IDD_A115_DESC01 + 27)
#define IDD_A115_SPIN09  (IDD_A115_DESC01 + 28)
#define IDD_A115_SPIN10  (IDD_A115_DESC01 + 29)

#define IDD_A115_SCBAR   (IDD_A115_DESC01 + 30)

#define IDD_A115_ADDR	11532
#define IDD_A115_DESC	11533
#define IDD_A115_DEPT	11534
#define IDD_A115_CAPTION1	11535

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_A115_DESC01  IDS_A115
#define IDS_A115_DESC02  (IDS_A115_DESC01 + 1)
#define IDS_A115_DESC03  (IDS_A115_DESC01 + 2)
#define IDS_A115_DESC04  (IDS_A115_DESC01 + 3)
#define IDS_A115_DESC05  (IDS_A115_DESC01 + 4)
#define IDS_A115_DESC06  (IDS_A115_DESC01 + 5)
#define IDS_A115_DESC07  (IDS_A115_DESC01 + 6)
#define IDS_A115_DESC08  (IDS_A115_DESC01 + 7)
#define IDS_A115_DESC09  (IDS_A115_DESC01 + 8)
#define IDS_A115_DESC10  (IDS_A115_DESC01 + 9)
#define IDS_A115_DESC11  (IDS_A115_DESC01 + 10)
#define IDS_A115_DESC12  (IDS_A115_DESC01 + 11)
#define IDS_A115_DESC13  (IDS_A115_DESC01 + 12)
#define IDS_A115_DESC14  (IDS_A115_DESC01 + 13)
#define IDS_A115_DESC15  (IDS_A115_DESC01 + 14)
#define IDS_A115_DESC16  (IDS_A115_DESC01 + 15)
#define IDS_A115_DESC17  (IDS_A115_DESC01 + 16)
#define IDS_A115_DESC18  (IDS_A115_DESC01 + 17)
#define IDS_A115_DESC19  (IDS_A115_DESC01 + 18)
#define IDS_A115_DESC20  (IDS_A115_DESC01 + 19)
#define IDS_A115_DESC21  (IDS_A115_DESC01 + 20)
#define IDS_A115_DESC22  (IDS_A115_DESC01 + 21)
#define IDS_A115_DESC23  (IDS_A115_DESC01 + 22)
#define IDS_A115_DESC24  (IDS_A115_DESC01 + 23)
#define IDS_A115_DESC25  (IDS_A115_DESC01 + 24)
#define IDS_A115_DESC26  (IDS_A115_DESC01 + 25)
#define IDS_A115_DESC27  (IDS_A115_DESC01 + 26)
#define IDS_A115_DESC28  (IDS_A115_DESC01 + 27)
#define IDS_A115_DESC29  (IDS_A115_DESC01 + 28)
#define IDS_A115_DESC30  (IDS_A115_DESC01 + 29)
#define IDS_A115_DESC31  (IDS_A115_DESC01 + 30)
#define IDS_A115_DESC32  (IDS_A115_DESC01 + 31)
#define IDS_A115_DESC33  (IDS_A115_DESC01 + 32)
#define IDS_A115_DESC34  (IDS_A115_DESC01 + 33)
#define IDS_A115_DESC35  (IDS_A115_DESC01 + 34)
#define IDS_A115_DESC36  (IDS_A115_DESC01 + 35)
#define IDS_A115_DESC37  (IDS_A115_DESC01 + 36)
#define IDS_A115_DESC38  (IDS_A115_DESC01 + 37)
#define IDS_A115_DESC39  (IDS_A115_DESC01 + 38)
#define IDS_A115_DESC40  (IDS_A115_DESC01 + 39)
#define IDS_A115_DESC41  (IDS_A115_DESC01 + 40)
#define IDS_A115_DESC42  (IDS_A115_DESC01 + 41)
#define IDS_A115_DESC43  (IDS_A115_DESC01 + 42)
#define IDS_A115_DESC44  (IDS_A115_DESC01 + 43)
#define IDS_A115_DESC45  (IDS_A115_DESC01 + 44)
#define IDS_A115_DESC46  (IDS_A115_DESC01 + 45)
#define IDS_A115_DESC47  (IDS_A115_DESC01 + 46)
#define IDS_A115_DESC48  (IDS_A115_DESC01 + 47)
#define IDS_A115_DESC49  (IDS_A115_DESC01 + 48)
#define IDS_A115_DESC50  (IDS_A115_DESC01 + 49)

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define A115_ADDR_MAX    50                  /* Maximum Number of Address    */

#define A115_DEPT_MAX    9999                /* Maximum Value of Dept No. Data   */
#define A115_DEPT_MIN    0                   /* Minimum Value of Dept No. Data   */

#define A115_SPIN_STEP   1
#define A115_SPIN_TURBO  0

#define A115_DEPT_LEN    4                   /* Maximum Length of HALO Data  */
#define A115_CAP_LEN     PEP_CAPTION_LEN     /* Length of MessgeBox Caption  */
#define A115_ERR_LEN     PEP_OVER_LEN        /* Length of Error Message      */
#define A115_DESC_LEN    128                 /* Length of Description        */

#define A115_ONE_LINE    1                   /* Scroll to One Line Up/ Down  */
#define A115_MAX_PAGE    10                  /* No. of Item in One Page      */
#define A115_ONE_PAGE    (A115_MAX_PAGE - A115_ONE_LINE)
                                            /* Scroll to One Page Up/ Down  */

#define A115_SCBAR_MIN   0                   /* Min. Value of Scroll Range   */
#define A115_SCBAR_MAX   (A115_ADDR_MAX - A115_MAX_PAGE)
                                            /* Max. Value of Scroll Range   */
/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/
#define A115GETINDEX(id, top)   (BYTE)(((id) - IDD_A115_EDIT01) + (top))

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
VOID    A115InitDlg(HWND, USHORT*);
BOOL    A115GetData(HWND, WPARAM, USHORT*);
VOID    A115ShowErr(HWND);
VOID    A115ResetData(HWND, WPARAM);
VOID    A115SpinProc(HWND, WPARAM, LPARAM, USHORT*);
VOID    A115ScrlProc(HWND, WPARAM, LPARAM, USHORT*);
BOOL    A115CalcIndex(WORD wScrlCode);
VOID    A115RedrawText(HWND, USHORT*);
VOID    A115RedrawThumb(LPARAM);

/* ===== End of File (A115.H) ===== */
