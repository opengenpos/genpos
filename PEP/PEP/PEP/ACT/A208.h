/*
* ---------------------------------------------------------------------------
* Title         :   Set Boundary Age Header File (AC 208) 
* Category      :   Maintenance, NCR 2172 PEP for Windows (Hotel US Model)
* Program Name  :   A007.H
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
* Nov-08-99 : 00.00.01 :            : Initial
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

#define A208_SPIN_OFFSET    10
#define IDD_A208_ADDRA      12081
#define IDD_A208_ADDRB      (IDD_A208_ADDRA +1)
#define IDD_A208_ADDRC      (IDD_A208_ADDRA +2)

#define IDD_A208_ASPIN      (IDD_A208_ADDRA + A208_SPIN_OFFSET)
#define IDD_A208_BSPIN      (IDD_A208_ADDRB + A208_SPIN_OFFSET)
#define IDD_A208_CSPIN      (IDD_A208_ADDRC + A208_SPIN_OFFSET)

#define IDD_A208_STRADDRA   20801
#define IDD_A208_STRADDRB   (IDD_A208_STRADDRA + 1)
#define IDD_A208_STRADDRC   (IDD_A208_STRADDRA + 2)
#define IDD_A208_STRDESC    (IDD_A208_STRADDRA + 3)

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/

#define IDS_A208_CAPTION   (IDS_A208 + 0)
#define IDS_A208_ADDRA     (IDS_A208 + 1)
#define IDS_A208_ADDRB     (IDS_A208 + 2)
#define IDS_A208_ADDRC     (IDS_A208 + 3)
#define IDS_A208_DESC      (IDS_A208 + 4)

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/

#define A208_ADDR_MAX        MAX_AGE_SIZE    /* Max. Boundary age        */
#define A208_DATA_MIN        0               /* Min. Value of Data Range */
#define A208_DATA_MAX        100             /* Max. Value of Data Range */
#define A208_DATA_LEN        3               /* Limit Length of Data     */

#define A208_SPIN_STEP       1               /* Normal Step of Spin      */
#define A208_SPIN_TURBO      30              /* Turbo Point of Spin      */
#define A208_SPIN_TURSTEP    10              /* Turbo Step of SpinButton */

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
BOOL    A208ChkDatePos(HWND hDlg, WPARAM wID);
VOID    A208DispErr(HWND hDlg, WPARAM wID);

/* ===== End of File (A208.H) ===== */
