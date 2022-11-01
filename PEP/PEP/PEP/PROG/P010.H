/*
* ---------------------------------------------------------------------------
* Title         :   High Amount Lock-out Limit Header File (Prog. 10)
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
* Dec-22-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Sep-06-98 : 03.03.00 : A.Mitsui   : V3.3
* Nov-19-99 :          : K.Yanagida : NCR2172
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
#define IDD_P10_DESC01  21001
#define IDD_P10_DESC02  (IDD_P10_DESC01 + 1)
#define IDD_P10_DESC03  (IDD_P10_DESC01 + 2)
#define IDD_P10_DESC04  (IDD_P10_DESC01 + 3)
#define IDD_P10_DESC05  (IDD_P10_DESC01 + 4)
#define IDD_P10_DESC06  (IDD_P10_DESC01 + 5)
#define IDD_P10_DESC07  (IDD_P10_DESC01 + 6)
#define IDD_P10_DESC08  (IDD_P10_DESC01 + 7)
#define IDD_P10_DESC09  (IDD_P10_DESC01 + 8)
#define IDD_P10_DESC10  (IDD_P10_DESC01 + 9)

#define IDD_P10_EDIT01  (IDD_P10_DESC01 + 10)
#define IDD_P10_EDIT02  (IDD_P10_DESC01 + 11)
#define IDD_P10_EDIT03  (IDD_P10_DESC01 + 12)
#define IDD_P10_EDIT04  (IDD_P10_DESC01 + 13)
#define IDD_P10_EDIT05  (IDD_P10_DESC01 + 14)
#define IDD_P10_EDIT06  (IDD_P10_DESC01 + 15)
#define IDD_P10_EDIT07  (IDD_P10_DESC01 + 16)
#define IDD_P10_EDIT08  (IDD_P10_DESC01 + 17)
#define IDD_P10_EDIT09  (IDD_P10_DESC01 + 18)
#define IDD_P10_EDIT10  (IDD_P10_DESC01 + 19)

#define IDD_P10_SPIN01  (IDD_P10_DESC01 + 20)
#define IDD_P10_SPIN02  (IDD_P10_DESC01 + 21)
#define IDD_P10_SPIN03  (IDD_P10_DESC01 + 22)
#define IDD_P10_SPIN04  (IDD_P10_DESC01 + 23)
#define IDD_P10_SPIN05  (IDD_P10_DESC01 + 24)
#define IDD_P10_SPIN06  (IDD_P10_DESC01 + 25)
#define IDD_P10_SPIN07  (IDD_P10_DESC01 + 26)
#define IDD_P10_SPIN08  (IDD_P10_DESC01 + 27)
#define IDD_P10_SPIN09  (IDD_P10_DESC01 + 28)
#define IDD_P10_SPIN10  (IDD_P10_DESC01 + 29)

#define IDD_P10_CHECK01  (IDD_P10_DESC01 + 30)
#define IDD_P10_CHECK02  (IDD_P10_DESC01 + 31)
#define IDD_P10_CHECK03  (IDD_P10_DESC01 + 32)
#define IDD_P10_CHECK04  (IDD_P10_DESC01 + 33)
#define IDD_P10_CHECK05  (IDD_P10_DESC01 + 34)
#define IDD_P10_CHECK06  (IDD_P10_DESC01 + 35)
#define IDD_P10_CHECK07  (IDD_P10_DESC01 + 36)
#define IDD_P10_CHECK08  (IDD_P10_DESC01 + 37)
#define IDD_P10_CHECK09  (IDD_P10_DESC01 + 38)
#define IDD_P10_CHECK10  (IDD_P10_DESC01 + 39)

#define IDD_P10_SCBAR   (IDD_P10_DESC01 + 40)

#define IDD_P10_ADDR	    (IDD_P10_SCBAR + 1)
#define IDD_P10_DESC	    (IDD_P10_SCBAR + 2)
#define IDD_P10_HALO	    (IDD_P10_SCBAR + 3)
#define IDD_P10_RANGE_HALO	(IDD_P10_SCBAR + 4)

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_P10_DESC01  IDS_P10
#define IDS_P10_DESC02  (IDS_P10_DESC01 + 1)
#define IDS_P10_DESC03  (IDS_P10_DESC01 + 2)
#define IDS_P10_DESC04  (IDS_P10_DESC01 + 3)
#define IDS_P10_DESC05  (IDS_P10_DESC01 + 4)
#define IDS_P10_DESC06  (IDS_P10_DESC01 + 5)
#define IDS_P10_DESC07  (IDS_P10_DESC01 + 6)
#define IDS_P10_DESC08  (IDS_P10_DESC01 + 7)
#define IDS_P10_DESC09  (IDS_P10_DESC01 + 8)
#define IDS_P10_DESC10  (IDS_P10_DESC01 + 9)
#define IDS_P10_DESC11  (IDS_P10_DESC01 + 10)
#define IDS_P10_DESC12  (IDS_P10_DESC01 + 11)
#define IDS_P10_DESC13  (IDS_P10_DESC01 + 12)
#define IDS_P10_DESC14  (IDS_P10_DESC01 + 13)
#define IDS_P10_DESC15  (IDS_P10_DESC01 + 14)
#define IDS_P10_DESC16  (IDS_P10_DESC01 + 15)
#define IDS_P10_DESC17  (IDS_P10_DESC01 + 16)
#define IDS_P10_DESC18  (IDS_P10_DESC01 + 17)
#define IDS_P10_DESC19  (IDS_P10_DESC01 + 18)
#define IDS_P10_DESC20  (IDS_P10_DESC01 + 19)
#define IDS_P10_DESC21  (IDS_P10_DESC01 + 20)
#define IDS_P10_DESC22  (IDS_P10_DESC01 + 21)
#define IDS_P10_DESC23  (IDS_P10_DESC01 + 22)
#define IDS_P10_DESC24  (IDS_P10_DESC01 + 23)
#define IDS_P10_DESC25  (IDS_P10_DESC01 + 24)
#define IDS_P10_DESC26  (IDS_P10_DESC01 + 25)
#define IDS_P10_DESC27  (IDS_P10_DESC01 + 26)

#define IDS_P10_DESC28  (IDS_P10_DESC01 + 27) /* V3.3 */
#define IDS_P10_DESC29  (IDS_P10_DESC01 + 28) /* V3.3 */
#define IDS_P10_DESC30  (IDS_P10_DESC01 + 29) /* NCR2172 */
#define IDS_P10_DESC31  (IDS_P10_DESC01 + 30) /* NCR2172 */
#define IDS_P10_DESC32  (IDS_P10_DESC01 + 31) /* NCR2172 */
#define IDS_P10_DESC33  (IDS_P10_DESC01 + 32) /* NCR2172 */
#define IDS_P10_DESC34  (IDS_P10_DESC01 + 33) /* NCR2172 */
#define IDS_P10_DESC35  (IDS_P10_DESC01 + 34) /* NCR2172 */
#define IDS_P10_DESC36  (IDS_P10_DESC01 + 35) /* ###ADD Saratoga */

#define IDS_P10_DESC37  (IDS_P10_DESC01 + 36)
#define IDS_P10_DESC38  (IDS_P10_DESC01 + 37)
#define IDS_P10_DESC39  (IDS_P10_DESC01 + 38)
#define IDS_P10_DESC40  (IDS_P10_DESC01 + 39)
#define IDS_P10_DESC41  (IDS_P10_DESC01 + 40)
#define IDS_P10_DESC42  (IDS_P10_DESC01 + 41)
#define IDS_P10_DESC43  (IDS_P10_DESC01 + 42)
#define IDS_P10_DESC44  (IDS_P10_DESC01 + 43)
#define IDS_P10_DESC45  (IDS_P10_DESC01 + 44)
#define IDS_P10_DESC46  (IDS_P10_DESC01 + 45)
#define IDS_P10_DESC47  (IDS_P10_DESC01 + 46)

#define IDS_P10_DESC48  (IDS_P10_DESC01 + 47) /* V3.3 */
#define IDS_P10_DESC49  (IDS_P10_DESC01 + 48) /* V3.3 */
#define IDS_P10_DESC50  (IDS_P10_DESC01 + 49) /* NCR2172 */
/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P10_ADDR_MAX    MAX_HALOKEY_NO      /* Maximum Number of Address    */

#define P10_HALO_MAX    79                  /* Maximum Value of HALO Data   */
#define P10_HALO_MIN    0                   /* Minimum Value of HALO Data   */

#define P10_SPIN_STEP   1
#define P10_SPIN_TURBO  0

#define P10_HALO_LEN    2                   /* Maximum Length of HALO Data  */
#define P10_CAP_LEN     PEP_CAPTION_LEN     /* Length of MessgeBox Caption  */
#define P10_ERR_LEN     PEP_OVER_LEN        /* Length of Error Message      */
#define P10_DESC_LEN    128                 /* Length of Description        */

#define P10_ONE_LINE    1                   /* Scroll to One Line Up/ Down  */
#define P10_MAX_PAGE    10                  /* No. of Item in One Page      */
#define P10_ONE_PAGE    (P10_MAX_PAGE - P10_ONE_LINE)
                                            /* Scroll to One Page Up/ Down  */

#define P10_SCBAR_MIN   0                   /* Min. Value of Scroll Range   */
#define P10_SCBAR_MAX   (P10_ADDR_MAX - P10_MAX_PAGE)
                                            /* Max. Value of Scroll Range   */
/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/
#define P10GETINDEX(id, top)   (BYTE)(((id) - IDD_P10_EDIT01) + (top))

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
VOID    P10InitDlg(HWND, LPBYTE);
BOOL    P10GetData(HWND, WPARAM, LPBYTE);
VOID    P10ShowErr(HWND);
VOID    P10ResetData(HWND, WPARAM);
VOID    P10SpinProc(HWND, WPARAM, LPARAM, LPBYTE);
VOID    P10ScrlProc(HWND, WPARAM, LPARAM, LPBYTE);
BOOL    P10CalcIndex(WORD wScrlCode);
VOID    P10RedrawText(HWND, LPBYTE);
VOID    P10RedrawThumb(LPARAM);

/* ===== End of File (P010.H) ===== */
