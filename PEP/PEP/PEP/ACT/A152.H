/*
* ---------------------------------------------------------------------------
* Title         :   Employee Number Assignment Header File (AC 152)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A152.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
* ---------------------------------------------------------------------------
* Abstract :
*
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Jan-29-96 : 03.00.05 : M.Suzuki   : Add R3.1
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
#define IDD_A152_MAX        12001   /* maximum employee records         */
#define IDD_A152_CUR        12002   /* current employee records         */
#define IDD_A152_LIST       12003   /* listbox                          */
#define IDD_A152_ADDCHG     12004   /* add, change button               */
#define IDD_A152_DEL        12005   /* delete button                    */
#define IDD_A152_NO         12006   /* employee No.                     */
#define IDD_A152_CODE1      12007   /* job code 1                       */
#define IDD_A152_CODE2      12008   /* job code 2                       */
#define IDD_A152_CODE3      12009   /* job code 3                       */
#define IDD_A152_NAME       12010   /* name                             */
#define IDD_A152_CONF       12011   /* delete confirmation checkbox     */
#define A152_SPIN_OFFSET       10
#define IDD_A152_NOSPIN     (IDD_A152_NO    + A152_SPIN_OFFSET)
#define IDD_A152_SPIN1      (IDD_A152_CODE1 + A152_SPIN_OFFSET)
#define IDD_A152_SPIN2      (IDD_A152_CODE2 + A152_SPIN_OFFSET)
#define IDD_A152_SPIN3      (IDD_A152_CODE3 + A152_SPIN_OFFSET)

#define IDD_A152_GROUPBOX1	12020
#define IDD_A152_CAPTION1	12021
#define IDD_A152_CAPTION2	12022
#define IDD_A152_CAPTION3	12023
#define IDD_A152_CAPTION4	12024
#define IDD_A152_CAPTION5	12025
#define IDD_A152_CAPTION6	12026
#define IDD_A152_NO_RNG		12027
#define IDD_A152_CODE1_RNG	12028
#define IDD_A152_CODE2_RNG	12029
#define IDD_A152_CODE3_RNG	12030

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_A152_MAXETK         (IDS_A152 + 0) /* max. employee records */
#define IDS_A152_CURETK         (IDS_A152 + 1) /* current records */
#define IDS_A152_DELCONF        (IDS_A152 + 2) /* delete confirmation */
#define IDS_A152_FLEX_EMPTY     (IDS_A152 + 3) /* empty flex memory */
#define IDS_A152_OVERRECORD     (IDS_A152 + 4) /* over maximum */
#define IDS_A152_LISTSTRING     (IDS_A152 + 5) /* list string */
#define IDS_A152_NUMBERSTRING   (IDS_A152 + 6) /* number string */
/*
* ===========================================================================
*       Number
* ===========================================================================
*/
#define A152_NO_LEN          9  /* length of employee No.               */
#define A152_CODE_LEN        2  /* length of job code                   */
#define A152_NAME_LEN  ETK_MAX_NAME_SIZE  /* length of employee name    */
#define A152_MNE_LEN        32  /* length of employee description       */
#define A152_BUFF_LEN       96  /* employee max No. name buffer length  */
#define A152_CONF_CHK     0x01  /* delete confirmation check flag       */
#define A152_SPIN_STEP       1  /* step count of Spin button            */
#define A152_SPIN_TURBO     50  /* trubo point of spin button           */
#define A152_SPIN_TURSTEP   50  /* turbo count of spin button           */
#define A152_DATA_MIN       1L  /* minimum value of ETK No./Code        */
#define A152_ETK_NOMAX 999999999L  /* maximum value of ETK No.          */
#define A152_CODE_MIN       0L  /* minimum value of Code No.            */
#define A152_CODE_MAX      99L  /* maximum value of Code No.            */

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
USHORT  A152InitDlg(HWND, LPUSHORT);
VOID    A152SetEditRec(HWND, USHORT);
BOOL    A152GetEditRec(HWND, LPUSHORT, WPARAM, USHORT);
BOOL    A152ChkMax(HWND, DWORD, USHORT, WPARAM, USHORT);
BOOL    A152RefreshList(HWND, DWORD, LPUSHORT, WPARAM, LPWSTR);
int     A152DispDelConf(HWND, DWORD);
VOID    A152ChkExist(HWND);
VOID    A152FinDlg(HWND, WPARAM);
VOID    A152DispError(HWND, WPARAM);

/* ===== End of File (A152.H) ===== */
