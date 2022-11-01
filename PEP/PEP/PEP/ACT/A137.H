/*
* ---------------------------------------------------------------------------
* Title         :   Beverage Dispenser Parameter Header File (A/C 137)
* Category      :   Setup, NCR 2170 PEP for Windows (US R3.1 Model)
* Program Name  :   A137.H
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
* Mar-08-96 : 03.00.05 : M.Suzuki   : Iniyial (Hp US R3.1 Model)
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
#define A137_EDIT_OFFSET 10
#define A137_SPIN_OFFSET 10

#define IDD_A137_SCBAR   13700

#define IDD_A137_DESC01  (IDD_A137_SCBAR + 1)
#define IDD_A137_DESC02  (IDD_A137_SCBAR + 2)
#define IDD_A137_DESC03  (IDD_A137_SCBAR + 3)
#define IDD_A137_DESC04  (IDD_A137_SCBAR + 4)
#define IDD_A137_DESC05  (IDD_A137_SCBAR + 5)
#define IDD_A137_DESC06  (IDD_A137_SCBAR + 6)
#define IDD_A137_DESC07  (IDD_A137_SCBAR + 7)
#define IDD_A137_DESC08  (IDD_A137_SCBAR + 8)
#define IDD_A137_DESC09  (IDD_A137_SCBAR + 9)
#define IDD_A137_DESC10  (IDD_A137_SCBAR + 10)

#define IDD_A137_PLU01 (IDD_A137_DESC01 + A137_EDIT_OFFSET)
#define IDD_A137_PLU02 (IDD_A137_DESC02 + A137_EDIT_OFFSET)
#define IDD_A137_PLU03 (IDD_A137_DESC03 + A137_EDIT_OFFSET)
#define IDD_A137_PLU04 (IDD_A137_DESC04 + A137_EDIT_OFFSET)
#define IDD_A137_PLU05 (IDD_A137_DESC05 + A137_EDIT_OFFSET)
#define IDD_A137_PLU06 (IDD_A137_DESC06 + A137_EDIT_OFFSET)
#define IDD_A137_PLU07 (IDD_A137_DESC07 + A137_EDIT_OFFSET)
#define IDD_A137_PLU08 (IDD_A137_DESC08 + A137_EDIT_OFFSET)
#define IDD_A137_PLU09 (IDD_A137_DESC09 + A137_EDIT_OFFSET)
#define IDD_A137_PLU10 (IDD_A137_DESC10 + A137_EDIT_OFFSET)

#define IDD_A137_PLUSP01 (IDD_A137_PLU01 + A137_SPIN_OFFSET)
#define IDD_A137_PLUSP02 (IDD_A137_PLU02 + A137_SPIN_OFFSET)
#define IDD_A137_PLUSP03 (IDD_A137_PLU03 + A137_SPIN_OFFSET)
#define IDD_A137_PLUSP04 (IDD_A137_PLU04 + A137_SPIN_OFFSET)
#define IDD_A137_PLUSP05 (IDD_A137_PLU05 + A137_SPIN_OFFSET)
#define IDD_A137_PLUSP06 (IDD_A137_PLU06 + A137_SPIN_OFFSET)
#define IDD_A137_PLUSP07 (IDD_A137_PLU07 + A137_SPIN_OFFSET)
#define IDD_A137_PLUSP08 (IDD_A137_PLU08 + A137_SPIN_OFFSET)
#define IDD_A137_PLUSP09 (IDD_A137_PLU09 + A137_SPIN_OFFSET)
#define IDD_A137_PLUSP10 (IDD_A137_PLU10 + A137_SPIN_OFFSET)

#define IDD_A137_ADJ01 (IDD_A137_PLUSP01 + A137_EDIT_OFFSET)
#define IDD_A137_ADJ02 (IDD_A137_PLUSP02 + A137_EDIT_OFFSET)
#define IDD_A137_ADJ03 (IDD_A137_PLUSP03 + A137_EDIT_OFFSET)
#define IDD_A137_ADJ04 (IDD_A137_PLUSP04 + A137_EDIT_OFFSET)
#define IDD_A137_ADJ05 (IDD_A137_PLUSP05 + A137_EDIT_OFFSET)
#define IDD_A137_ADJ06 (IDD_A137_PLUSP06 + A137_EDIT_OFFSET)
#define IDD_A137_ADJ07 (IDD_A137_PLUSP07 + A137_EDIT_OFFSET)
#define IDD_A137_ADJ08 (IDD_A137_PLUSP08 + A137_EDIT_OFFSET)
#define IDD_A137_ADJ09 (IDD_A137_PLUSP09 + A137_EDIT_OFFSET)
#define IDD_A137_ADJ10 (IDD_A137_PLUSP10 + A137_EDIT_OFFSET)

#define IDD_A137_ADJSP01 (IDD_A137_ADJ01 + A137_SPIN_OFFSET)
#define IDD_A137_ADJSP02 (IDD_A137_ADJ02 + A137_SPIN_OFFSET)
#define IDD_A137_ADJSP03 (IDD_A137_ADJ03 + A137_SPIN_OFFSET)
#define IDD_A137_ADJSP04 (IDD_A137_ADJ04 + A137_SPIN_OFFSET)
#define IDD_A137_ADJSP05 (IDD_A137_ADJ05 + A137_SPIN_OFFSET)
#define IDD_A137_ADJSP06 (IDD_A137_ADJ06 + A137_SPIN_OFFSET)
#define IDD_A137_ADJSP07 (IDD_A137_ADJ07 + A137_SPIN_OFFSET)
#define IDD_A137_ADJSP08 (IDD_A137_ADJ08 + A137_SPIN_OFFSET)
#define IDD_A137_ADJSP09 (IDD_A137_ADJ09 + A137_SPIN_OFFSET)
#define IDD_A137_ADJSP10 (IDD_A137_ADJ10 + A137_SPIN_OFFSET)

#define IDD_A137_OFFSETCODE  (IDD_A137_ADJSP01 + A137_EDIT_OFFSET)
#define IDD_A137_NEXTORDER   (IDD_A137_ADJSP02 + A137_EDIT_OFFSET)
#define IDD_A137_ENDPOUR     (IDD_A137_ADJSP03 + A137_EDIT_OFFSET)

#define IDD_A137_OFFSETSP    (IDD_A137_OFFSETCODE + A137_SPIN_OFFSET)
#define IDD_A137_NEXTORDERSP (IDD_A137_NEXTORDER + A137_SPIN_OFFSET)
#define IDD_A137_ENDPOURSP   (IDD_A137_ENDPOUR + A137_SPIN_OFFSET)

#define IDD_A137_OFFSETDES    (IDD_A137_OFFSETSP + A137_EDIT_OFFSET)
#define IDD_A137_NEXTORDERDES (IDD_A137_NEXTORDERSP + A137_EDIT_OFFSET)
#define IDD_A137_ENDPOURDES   (IDD_A137_ENDPOURSP + A137_EDIT_OFFSET)

#define IDD_A137_GUN          (IDD_A137_OFFSETDES + A137_EDIT_OFFSET)
#define IDD_A137_TYPE         (IDD_A137_NEXTORDERDES + A137_EDIT_OFFSET)

#define IDD_A137_CAPTION1	13790
#define IDD_A137_CAPTION2	13791
#define IDD_A137_CAPTION3	13792
#define IDD_A137_STATIC_RNG    13793

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_A137_DESC         IDS_A137
#define IDS_A137_OFFSETCODE   IDS_A137 + 1
#define IDS_A137_NEXTORDER    IDS_A137 + 2
#define IDS_A137_ENDPOUR      IDS_A137 + 3
#define IDS_A137_TYPE_NORMAL  IDS_A137 + 4
#define IDS_A137_TYPE_SHORT   IDS_A137 + 5
#define IDS_A137_TYPE_LONG    IDS_A137 + 6
#define IDS_A137_TYPE_MIX     IDS_A137 + 7
#define IDS_A137_PLU_OVER	  IDS_A137 + 8
#define IDS_A137_ADJ_OVER	  IDS_A137 + 9

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define A137_ADDR_MAX    MAX_DISPPARA_SIZE  /* Maximum Number of Address    */
#define A137_DRINK_MAX   16                 /* Max. Value of 1 type         */

#define A137_PLU_DATA_MAX     9999           /* Max. Value of PLU Data       */
#define A137_ADJ_DATA_MAX       20           /* Max. Value of Adjective Data */
#define A137_OFFSET_DATA_MAX    MAX_DISPPARA_OFFSET /* Max. Value of Offset Data */
#define A137_NEXTORDER_DATA_MAX MAX_DISPPARA_TOTAL /* Max. Value of Next Order Data */
#define A137_ENDPOUR_DATA_MAX   MAX_DISPPARA_TOTAL /* Max. Value of End Pour Data  */
#define A137_DATA_MIN    0                   /* Min. Value of Input Data     */
#define A137_PLU_DATA_LEN        4           /* Max. Length of PLU Data      */
#define A137_ADJ_DATA_LEN        2           /* Max. Length of Adjective Data*/
#define A137_OFFSET_DATA_LEN     5           /* Max. Length of Offset Data   */
#define A137_NEXTORDER_DATA_LEN  2           /* Max. Length of Next Order Data */
#define A137_ENDPOUR_DATA_LEN    2           /* Max. Length of End Pour Data */

#define A137_OFFSET_BASE       256           /* Offset of Offset Data   */
#define A137_NEXTORDER_BASE    257           /* Offset of Next Order Data */
#define A137_ENDPOUR_BASE      258           /* Offset of End Pour Data */

#define A137_DESC_LEN    128                 /* Length of Description        */

#define A137_ONE_LINE    1                   /* Scroll to One Line Up/ Down  */
#define A137_MAX_PAGE    10                  /* No. of Item in One Page      */
#define A137_ONE_PAGE    (A137_MAX_PAGE - A137_ONE_LINE)
                                             /* Scroll to One Page Up/ Down  */
#define A137_SCBAR_MIN   0                   /* Min. Value of Scroll Range   */
#define A137_SCBAR_MAX   (A137_DRINK_MAX - A137_MAX_PAGE)
                                             /* Max. Value of Scroll Range   */

#define A137_CHK_ADDR    2                   /* Even/Odd Address Checker     */

#define A137_SPIN_STEP   1
#define A137_SPIN_TURBO  0

#define A137_GUN_MAX     64                 /* One GUN Max Address  */
#define A137_TYPE_MAX    16                 /* One Type Max Address */

#define A137_GUN_NO_MAX   4                 /* GUN Max No           */
#define A137_TYPE_NO_MAX  4                 /* Type Max No          */

#define A137_DEF_GUNNO    1                 /* default GUN No           */

/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/
#define A137GETPLUINDEX(id, top) (BYTE)((id - IDD_A137_PLU01) + top)
#define A137GETADJINDEX(id, top) (BYTE)((id - IDD_A137_ADJ01) + top)

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct _A137data {
    WORD    wPlu;         /* Data Value of A137      */
    BYTE    bAdj;         /* Data Value of A137      */
} A137DATA, FAR *LPA137DATA;

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
VOID    A137InitDlg(HWND, LPA137DATA);
BOOL    A137GetData(HWND, WPARAM, LPA137DATA);
VOID    A137DispErr(HWND, WPARAM);
VOID    A137ResetData(HWND, WPARAM);
VOID    A137SpinProc(HWND, WPARAM, LPARAM, LPA137DATA);
VOID    A137ScrlProc(HWND, WPARAM, LPARAM, LPA137DATA);
BOOL    A137CalcIndex(WORD wScrlCode);
VOID    A137RedrawText(HWND, LPA137DATA);
VOID    A137RedrawThumb(LPARAM);
VOID    A137SetGunData(HWND, LPA137DATA);
VOID    A137SetTypeData(HWND, LPA137DATA);

/* ===== End of File (A137.H) ===== */
