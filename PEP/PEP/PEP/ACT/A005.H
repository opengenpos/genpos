/*
* ---------------------------------------------------------------------------
* Title         :   Set Menu Page Control Table Header File (AC 5)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A005.H
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
* Mar-21-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-11-98 : 03.03.00 : A.Mitsui   : V3.3
* Dec-09-99 :          : K.Yanagida : NCR2172
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
#define A005_SPIN_OFFSET   10

#define IDD_A005_OFFDEF 10500       /* Offset No. of AutoRadioButton        */

#define IDD_A005_DEF01  10501       /* AutoRadioButton (Default Page = 1)   */
#define IDD_A005_DEF02  10502       /* AutoRadioButton (Default Page = 2)   */
#define IDD_A005_DEF03  10503       /* AutoRadioButton (Default Page = 3)   */
#define IDD_A005_DEF04  10504       /* AutoRadioButton (Default Page = 4)   */
#define IDD_A005_DEF05  10505       /* AutoRadioButton (Default Page = 5)   */
#define IDD_A005_DEF06  10506       /* AutoRadioButton (Default Page = 6)   */
#define IDD_A005_DEF07  10507       /* AutoRadioButton (Default Page = 7)   */
#define IDD_A005_DEF08  10508       /* AutoRadioButton (Default Page = 8)   */
#define IDD_A005_DEF09  10509       /* AutoRadioButton (Default Page = 9)   */

#define IDD_A005_OFFNOT 10510       /* Offset No. of AutoCheckBox           */

#define IDD_A005_NOT01  10511       /* AutoCheckBox    (Not Use Page # 1)   */
#define IDD_A005_NOT02  10512       /* AutoCheckBox    (Not Use Page # 2)   */
#define IDD_A005_NOT03  10513       /* AutoCheckBox    (Not Use Page # 3)   */
#define IDD_A005_NOT04  10514       /* AutoCheckBox    (Not Use Page # 4)   */
#define IDD_A005_NOT05  10515       /* AutoCheckBox    (Not Use Page # 5)   */
#define IDD_A005_NOT06  10516       /* AutoCheckBox    (Not Use Page # 6)   */
#define IDD_A005_NOT07  10517       /* AutoCheckBox    (Not Use Page # 7)   */
#define IDD_A005_NOT08  10518       /* AutoCheckBox    (Not Use Page # 8)   */
#define IDD_A005_NOT09  10519       /* AutoCheckBox    (Not Use Page # 9)   */


/*** NCR2172 ***/
#define IDD_A005_ADJGRP 10520       /* Offset No. of AutoCheckBox           */

#define IDD_A005_ADJED01  10521     /* Edit (Adjective MenuPage # 1)         */
#define IDD_A005_ADJED02  10522     /* Edit (Adjective MenuPage # 2)         */
#define IDD_A005_ADJED03  10523     /* Edit (Adjective MenuPage # 3)         */
#define IDD_A005_ADJED04  10524     /* Edit (Adjective MenuPage # 4)         */
#define IDD_A005_ADJED05  10525     /* Edit (Adjective MenuPage # 5)         */
#define IDD_A005_ADJED06  10526     /* Edit (Adjective MenuPage # 6)         */
#define IDD_A005_ADJED07  10527     /* Edit (Adjective MenuPage # 7)         */
#define IDD_A005_ADJED08  10528     /* Edit (Adjective MenuPage # 8)         */
#define IDD_A005_ADJED09  10529     /* Edit (Adjective MenuPage # 9)         */

#define IDD_A005_ADJSP01  (IDD_A005_ADJED01 + A005_SPIN_OFFSET)    /* Scrollbar (Adjective MenuPage # 1)    */
#define IDD_A005_ADJSP02  (IDD_A005_ADJED02 + A005_SPIN_OFFSET)     /* Scrollbar (Adjective MenuPage # 2)    */
#define IDD_A005_ADJSP03  (IDD_A005_ADJED03 + A005_SPIN_OFFSET)     /* Scrollbar (Adjective MenuPage # 3)    */
#define IDD_A005_ADJSP04  (IDD_A005_ADJED04 + A005_SPIN_OFFSET)     /* Scrollbar (Adjective MenuPage # 4)    */
#define IDD_A005_ADJSP05  (IDD_A005_ADJED05 + A005_SPIN_OFFSET)     /* Scrollbar (Adjective MenuPage # 5)    */
#define IDD_A005_ADJSP06  (IDD_A005_ADJED06 + A005_SPIN_OFFSET)     /* Scrollbar (Adjective MenuPage # 6)    */
#define IDD_A005_ADJSP07  (IDD_A005_ADJED07 + A005_SPIN_OFFSET)     /* Scrollbar (Adjective MenuPage # 7)    */
#define IDD_A005_ADJSP08  (IDD_A005_ADJED08 + A005_SPIN_OFFSET)     /* Scrollbar (Adjective MenuPage # 8)    */
#define IDD_A005_ADJSP09  (IDD_A005_ADJED09 + A005_SPIN_OFFSET)     /* Scrollbar (Adjective MenuPage # 9)    */

#define IDD_A005_DESC01   10540
#define IDD_A005_DESC02   10541
#define IDD_A005_DESC03   10542
#define IDD_A005_DESC04   10543
#define IDD_A005_DESC05   10544
#define IDD_A005_DESC06   10545
#define IDD_A005_DESC07   10546
#define IDD_A005_DESC08   10547
#define IDD_A005_DESC09   10548
#define IDD_A005_DESC10   10549

#define IDD_A005_DESC11   10550
/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/

#define IDS_A005_CAPTION    (IDS_A05 + 0)

#define IDS_A005_OFFDEF     (IDS_A05 + 1)

#define IDS_A005_DEF01      (IDS_A05 + 2)
#define IDS_A005_DEF02      (IDS_A05 + 3)
#define IDS_A005_DEF03      (IDS_A05 + 4)
#define IDS_A005_DEF04      (IDS_A05 + 5)
#define IDS_A005_DEF05      (IDS_A05 + 6)
#define IDS_A005_DEF06      (IDS_A05 + 7)
#define IDS_A005_DEF07      (IDS_A05 + 8)
#define IDS_A005_DEF08      (IDS_A05 + 9)
#define IDS_A005_DEF09      (IDS_A05 +10)

#define IDS_A005_OFFNOT     (IDS_A05 +11)

#define IDS_A005_NOT01      (IDS_A05 +12)
#define IDS_A005_NOT02      (IDS_A05 +13)
#define IDS_A005_NOT03      (IDS_A05 +14)
#define IDS_A005_NOT04      (IDS_A05 +15)
#define IDS_A005_NOT05      (IDS_A05 +16)
#define IDS_A005_NOT06      (IDS_A05 +17)
#define IDS_A005_NOT07      (IDS_A05 +18)
#define IDS_A005_NOT08      (IDS_A05 +19)
#define IDS_A005_NOT09      (IDS_A05 +20)

/*** NCR2172 ***/
#define IDS_A005_ADJGRP     (IDS_A05 +21)

#define IDS_A005_DESC01     (IDS_A05 + 22)
#define IDS_A005_DESC02     (IDS_A05 + 23)
#define IDS_A005_DESC03     (IDS_A05 + 24)
#define IDS_A005_DESC04     (IDS_A05 + 25)
#define IDS_A005_DESC05     (IDS_A05 + 26)
#define IDS_A005_DESC06     (IDS_A05 + 27)
#define IDS_A005_DESC07     (IDS_A05 + 28)
#define IDS_A005_DESC08     (IDS_A05 + 29)
#define IDS_A005_DESC09     (IDS_A05 + 30)

#define IDS_A005_DESC10     (IDS_A05 + 31)


/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define A005_ADDR_MAX   MAX_PAGE_SIZE   /* Max. Number of Address       */
#define A005_ADDR_DEF   0               /* Address of Default Page No.  */
#define A005_ADDR_NOT   1               /* Offset Addr. of Not Use Page */

#define A005_PAGE_MAX   9               /* Max. Number of Menu Page     */
#define A005_PAGE_MIN   1               /* Min. Number of Menu Page     */

#define A005_ALLOW      0               /* Allow Menu Page              */
#define A005_PROHIBIT   1               /* Prohibit Menu Page           */

/*** NCR2172 ***/

#define A05_DATA_MIN     0               /* Min. Value of Position       */
#define A05_DATA_MAX     5               /* Max. Value of Position       */
#define A05_MAXLEN       1               /* Max. Length of Input Data    */
#define A05_SPIN_STEP    1               /* Normal Step Count of Spin Button     */
#define A05_SPIN_TURBO   0               /* Turbo Point of Spin Button (0 = OFF) */
#define A208_SPIN_TURSTEP    10          /* Turbo Step of SpinButton */

#define A05_OFFSET       10              /* Adjective offset addr.*/
/*
#define A05_SPIN_OFFSET  
*/
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
VOID    A005InitDlg(HWND);
VOID    A005SaveData(HWND);

/*** NCR2172 ***/

VOID    A005SpinProc(HWND, WPARAM, LPARAM);
BOOL    A005ChkPos(HWND, WORD wItem);
VOID    A005DispErr(HWND, WORD wItem);

/* ===== End of File (A005.H) ===== */
