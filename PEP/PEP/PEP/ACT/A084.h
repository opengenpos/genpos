/*
* ---------------------------------------------------------------------------
* Title         :   Set Rounding Table Header File (AC 84)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A084.H
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
#define A84_SPIN_OFFSET 10

#define IDD_A84_SCBAR	    18400
#define IDD_A84_DESC01	    (IDD_A84_SCBAR  +  1)
#define IDD_A84_DESC02	    (IDD_A84_SCBAR  +  2)
#define IDD_A84_DESC03	    (IDD_A84_SCBAR  +  3)
#define IDD_A84_DESC04	    (IDD_A84_SCBAR  +  4)
#define IDD_A84_DESC05	    (IDD_A84_SCBAR  +  5)
#define IDD_A84_DESC06	    (IDD_A84_SCBAR  +  6)
#define IDD_A84_DESC07	    (IDD_A84_SCBAR  +  7)
#define IDD_A84_DESC08	    (IDD_A84_SCBAR  +  8)
#define IDD_A84_DESC09	    (IDD_A84_SCBAR  +  9)
#define IDD_A84_DESC10	    (IDD_A84_SCBAR  + 10)

#define IDD_A84_DMT01       (IDD_A84_SCBAR  + 11)
#define IDD_A84_DMT02       (IDD_A84_SCBAR  + 12)
#define IDD_A84_DMT03       (IDD_A84_SCBAR  + 13)
#define IDD_A84_DMT04       (IDD_A84_SCBAR  + 14)
#define IDD_A84_DMT05       (IDD_A84_SCBAR  + 15)
#define IDD_A84_DMT06       (IDD_A84_SCBAR  + 16)
#define IDD_A84_DMT07       (IDD_A84_SCBAR  + 17)
#define IDD_A84_DMT08       (IDD_A84_SCBAR  + 18)
#define IDD_A84_DMT09       (IDD_A84_SCBAR  + 19)
#define IDD_A84_DMT10       (IDD_A84_SCBAR  + 20)
#define IDD_A84_DMT_SP01    (IDD_A84_DMT01  + A84_SPIN_OFFSET)
#define IDD_A84_DMT_SP02    (IDD_A84_DMT02  + A84_SPIN_OFFSET)
#define IDD_A84_DMT_SP03    (IDD_A84_DMT03  + A84_SPIN_OFFSET)
#define IDD_A84_DMT_SP04    (IDD_A84_DMT04  + A84_SPIN_OFFSET)
#define IDD_A84_DMT_SP05    (IDD_A84_DMT05  + A84_SPIN_OFFSET)
#define IDD_A84_DMT_SP06    (IDD_A84_DMT06  + A84_SPIN_OFFSET)
#define IDD_A84_DMT_SP07    (IDD_A84_DMT07  + A84_SPIN_OFFSET)
#define IDD_A84_DMT_SP08    (IDD_A84_DMT08  + A84_SPIN_OFFSET)
#define IDD_A84_DMT_SP09    (IDD_A84_DMT09  + A84_SPIN_OFFSET)
#define IDD_A84_DMT_SP10    (IDD_A84_DMT10  + A84_SPIN_OFFSET)

#define IDD_A84_MOD01       (IDD_A84_SCBAR  + 31)
#define IDD_A84_MOD02       (IDD_A84_SCBAR  + 32)
#define IDD_A84_MOD03       (IDD_A84_SCBAR  + 33)
#define IDD_A84_MOD04       (IDD_A84_SCBAR  + 34)
#define IDD_A84_MOD05       (IDD_A84_SCBAR  + 35)
#define IDD_A84_MOD06       (IDD_A84_SCBAR  + 36)
#define IDD_A84_MOD07       (IDD_A84_SCBAR  + 37)
#define IDD_A84_MOD08       (IDD_A84_SCBAR  + 38)
#define IDD_A84_MOD09       (IDD_A84_SCBAR  + 39)
#define IDD_A84_MOD10       (IDD_A84_SCBAR  + 40)
#define IDD_A84_MOD_SP01    (IDD_A84_MOD01  + A84_SPIN_OFFSET)
#define IDD_A84_MOD_SP02    (IDD_A84_MOD02  + A84_SPIN_OFFSET)
#define IDD_A84_MOD_SP03    (IDD_A84_MOD03  + A84_SPIN_OFFSET)
#define IDD_A84_MOD_SP04    (IDD_A84_MOD04  + A84_SPIN_OFFSET)
#define IDD_A84_MOD_SP05    (IDD_A84_MOD05  + A84_SPIN_OFFSET)
#define IDD_A84_MOD_SP06    (IDD_A84_MOD06  + A84_SPIN_OFFSET)
#define IDD_A84_MOD_SP07    (IDD_A84_MOD07  + A84_SPIN_OFFSET)
#define IDD_A84_MOD_SP08    (IDD_A84_MOD08  + A84_SPIN_OFFSET)
#define IDD_A84_MOD_SP09    (IDD_A84_MOD09  + A84_SPIN_OFFSET)
#define IDD_A84_MOD_SP10    (IDD_A84_MOD10  + A84_SPIN_OFFSET)

#define IDD_A84_RND01       (IDD_A84_SCBAR  + 51)
#define IDD_A84_RND02       (IDD_A84_SCBAR  + 52)
#define IDD_A84_RND03       (IDD_A84_SCBAR  + 53)
#define IDD_A84_RND04       (IDD_A84_SCBAR  + 54)
#define IDD_A84_RND05       (IDD_A84_SCBAR  + 55)
#define IDD_A84_RND06       (IDD_A84_SCBAR  + 56)
#define IDD_A84_RND07       (IDD_A84_SCBAR  + 57)
#define IDD_A84_RND08       (IDD_A84_SCBAR  + 58)
#define IDD_A84_RND09       (IDD_A84_SCBAR  + 59)
#define IDD_A84_RND10       (IDD_A84_SCBAR  + 60)
#define IDD_A84_RND_SP01    (IDD_A84_RND01  + A84_SPIN_OFFSET)
#define IDD_A84_RND_SP02    (IDD_A84_RND02  + A84_SPIN_OFFSET)
#define IDD_A84_RND_SP03    (IDD_A84_RND03  + A84_SPIN_OFFSET)
#define IDD_A84_RND_SP04    (IDD_A84_RND04  + A84_SPIN_OFFSET)
#define IDD_A84_RND_SP05    (IDD_A84_RND05  + A84_SPIN_OFFSET)
#define IDD_A84_RND_SP06    (IDD_A84_RND06  + A84_SPIN_OFFSET)
#define IDD_A84_RND_SP07    (IDD_A84_RND07  + A84_SPIN_OFFSET)
#define IDD_A84_RND_SP08    (IDD_A84_RND08  + A84_SPIN_OFFSET)
#define IDD_A84_RND_SP09    (IDD_A84_RND09  + A84_SPIN_OFFSET)
#define IDD_A84_RND_SP10    (IDD_A84_RND10  + A84_SPIN_OFFSET)

#define IDD_A84_DESC        (IDD_A84_SCBAR  + 91)
#define IDD_A84_RANGE       (IDD_A84_SCBAR  + 92)

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_A84_DESC01  IDS_A84
#define IDS_A84_DESC02  (IDS_A84_DESC01 + 1)
#define IDS_A84_DESC03  (IDS_A84_DESC01 + 2)
#define IDS_A84_DESC04  (IDS_A84_DESC01 + 3)
#define IDS_A84_DESC05  (IDS_A84_DESC01 + 4)
#define IDS_A84_DESC06  (IDS_A84_DESC01 + 5)
#define IDS_A84_DESC07  (IDS_A84_DESC01 + 6)
#define IDS_A84_DESC08  (IDS_A84_DESC01 + 7)
#define IDS_A84_DESC09  (IDS_A84_DESC01 + 8)
#define IDS_A84_DESC10  (IDS_A84_DESC01 + 9)
#define IDS_A84_DESC11  (IDS_A84_DESC01 + 10)
#define IDS_A84_DESC12  (IDS_A84_DESC01 + 11)
#define IDS_A84_DESC13  (IDS_A84_DESC01 + 12)

/*** NCR2172 ***/

#define IDS_A84_DESC14  (IDS_A84_DESC01 + 13)
#define IDS_A84_DESC15  (IDS_A84_DESC01 + 14)
#define IDS_A84_DESC16  (IDS_A84_DESC01 + 15)
#define IDS_A84_DESC17  (IDS_A84_DESC01 + 16)
#define IDS_A84_DESC18  (IDS_A84_DESC01 + 17)
#define IDS_A84_DESC19  (IDS_A84_DESC01 + 18)
#define IDS_A84_DESC20  (IDS_A84_DESC01 + 19)
#define IDS_A84_DESC21  (IDS_A84_DESC01 + 20)
#define IDS_A84_DESC22  (IDS_A84_DESC01 + 21)
#define IDS_A84_DESC23  (IDS_A84_DESC01 + 22)
#define IDS_A84_DESC24  (IDS_A84_DESC01 + 23)
#define IDS_A84_DESC25  (IDS_A84_DESC01 + 24)
#define IDS_A84_DESC26  (IDS_A84_DESC01 + 25)
#define IDS_A84_DESC27  (IDS_A84_DESC01 + 26)
#define IDS_A84_DESC28  (IDS_A84_DESC01 + 27)
#define IDS_A84_DESC29  (IDS_A84_DESC01 + 28)
#define IDS_A84_DESC30  (IDS_A84_DESC01 + 29)

/*** NCR2172
#define IDS_A84_DESC    (IDS_A84_DESC01 + 20)
#define IDS_A84_RANGE   (IDS_A84_DESC01 + 21)
 ***/
#define IDS_A84_DESC    (IDS_A84_DESC01 + 30)
#define IDS_A84_RANGE   (IDS_A84_DESC01 + 31)

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define A84_ADDR_MAX    MAX_RNDTBL_SIZE
#define A84_ADDR_SIZE   MAX_RND_DATA
#define A84_DMT_ADDR    0
#define A84_MOD_ADDR    1
#define A84_RND_ADDR    2

#define A84_DMT_MAX     99
#define A84_MOD_MAX     99
#define A84_RND_MAX     3
#define A84_DMT_MIN     0
#define A84_MOD_MIN     0
#define A84_RND_MIN     -1

#define A84_DATA_LEN    2

#define A84_SPIN_STEP   1
#define A84_SPIN_TURBO  0

#define A84_ONE_LINE    1                   /* Scroll to One Line Up/ Down */
#define A84_MAX_PAGE    10                  /* No. of Item in One Page     */
#define A84_ONE_PAGE    (A84_MAX_PAGE - A84_ONE_LINE)
                                            /* Scroll to One Page Up/ Down */

#define A84_SCBAR_MIN   0                   /* Min. Value of Scroll Range  */
#define A84_SCBAR_MAX   (A84_ADDR_MAX - A84_MAX_PAGE)
                                            /* Max. Value of Scroll Range  */
/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/
#define A84GETDMTADR(id, top)   (BYTE)((id - IDD_A84_DMT01) + top)
#define A84GETMODADR(id, top)   (BYTE)((id - IDD_A84_MOD01) + top)
#define A84_GETRNDPOSI(x)       (short)((x >= 0x80) ? ((short)x | 0xffff) : x)

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
static  void    InitDlg(HWND, LPBYTE);
static  void    RedrawText(HWND, LPBYTE);
static  void    SetData(HWND, WPARAM, LPBYTE);
static  BOOL    GetRndData(HWND, WPARAM, LPBYTE);
static  void    ShowError(HWND);
static  void    ResetRndData(HWND, WPARAM);
static  void    SpinProc(HWND, WPARAM, LPARAM, LPBYTE);
static  void    ScrollProc(HWND, WPARAM, LPARAM, LPBYTE);
static  BOOL    CalcIndex(WORD wScrlCode);

/* ===== End of File (A084.H) ===== */
