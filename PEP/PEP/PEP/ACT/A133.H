/*
* ---------------------------------------------------------------------------
* Title         :   Service Time Parameter Maintenance Header File (AC 133) 
* Category      :   Maintenance, NCR 2170 PEP for Windows (US Model)
* Program Name  :   A133.H
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
* Jan-30-96 : 03.01.00 : M.Suzuki   : Initial
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
#define A133_SPIN_OFFSET     10
#define IDD_A133_EDIT01      13201
#define IDD_A133_EDIT02      (IDD_A133_EDIT01 + 1)
#define IDD_A133_SPIN01      (IDD_A133_EDIT01 + A133_SPIN_OFFSET)
#define IDD_A133_SPIN02      (IDD_A133_EDIT02 + A133_SPIN_OFFSET)

#define IDD_A133_CAPTION1	13205
#define IDD_A133_CAPTION2	13206
#define IDD_A133_CAPTION3	13207

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define A133_ADDR_MAX        MAX_SERVICETIME_SIZE     /* Max. Address No. of Data */
#define A133_DATA_MIN        0               /* Min. Value of Data Range */
#define A133_DATA_MAX        9999            /* Max. Value of Data Range */
#define A133_DATA_LEN        4               /* Limit Length of Data     */

#define A133_SPIN_STEP       1               /* Normal Step of Spin      */
#define A133_SPIN_TURBO      30              /* Turbo Point of Spin      */
#define A133_SPIN_TURSTEP    10              /* Turbo Step of SpinButton */

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

/* ===== End of File (A133.H) ===== */
