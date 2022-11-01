/*
* ---------------------------------------------------------------------------
* Title         :   Cashier A/ B Key Assignment Header File (AC 7) 
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
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
#define A07_SPIN_OFFSET     10
#define IDD_A07_AKEY        10701
#define IDD_A07_BKEY        (IDD_A07_AKEY + 1)
#define IDD_A07_ASPIN       (IDD_A07_AKEY + A07_SPIN_OFFSET)
#define IDD_A07_BSPIN       (IDD_A07_BKEY + A07_SPIN_OFFSET)

#define IDD_A07_STRAKEY     15700
#define IDD_A07_STRBKEY     (IDD_A07_STRAKEY + 1)
#define IDD_A07_STRDESC     (IDD_A07_STRAKEY + 2)
#define IDD_A07_OPER_CAP	(IDD_A07_STRAKEY + 3)
#define IDD_A07_JOB_CAP		(IDD_A07_STRAKEY + 4)

#define IDD_A07_CASH_A_JOBRNG1	(IDD_A07_STRAKEY + 5)
#define IDD_A07_CASH_A_JOBRNG2	(IDD_A07_STRAKEY + 6)
#define IDD_A07_CASH_B_JOBRNG1	(IDD_A07_STRAKEY + 7)
#define IDD_A07_CASH_B_JOBRNG2	(IDD_A07_STRAKEY + 8)

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/

#define IDS_A07_CAPTION     (IDS_A07 + 0)
#define IDS_A07_AKEY        (IDS_A07 + 1)
#define IDS_A07_BKEY        (IDS_A07 + 2)
#define IDS_A07_DESC        (IDS_A07 + 3)

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define A07_ADDR_MAX        MAX_AB_SIZE     /* Max. Address No. of Data */
#define A07_DATA_MIN        0               /* Min. Value of Data Range */
#define A07_DATA_MAX        99999999        /* Max. Value of Data Range */
#define A07_DATA_LEN        8               /* Limit Length of Data     */
#define A07_DATA_JOB_LEN	3				// Limit length of Job data

#define A07_SPIN_STEP       1               /* Normal Step of Spin      */
#define A07_SPIN_TURBO      30              /* Turbo Point of Spin      */
#define A07_SPIN_TURSTEP    10              /* Turbo Step of SpinButton */

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

/* ===== End of File (A007.H) ===== */
