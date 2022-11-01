/*
* ---------------------------------------------------------------------------
* Title         :   Assignment Terminal No. Header File (AC 160)
* Category      :   Maintenance, NCR 2170 PEP for Windows (HP US Enhance Model)
* Program Name  :   A160.H
* Copyright (C) :   1995, NCR Corp. E&M-OISO, All rights reserved.
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
* Feb-17-95 : 03.00.00 : H.Ishida   : Initial (Migrate from HP US Model)
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
#define A160_EDIT_OFFSET 20
#define A160_SPIN_OFFSET 10

#define IDD_A160_SCBAR  16000

#define IDD_A160_DESC01         (IDD_A160_SCBAR + 1)
#define IDD_A160_DESC02         (IDD_A160_SCBAR + 2)
#define IDD_A160_DESC03         (IDD_A160_SCBAR + 3)
#define IDD_A160_DESC04         (IDD_A160_SCBAR + 4)
#define IDD_A160_DESC05         (IDD_A160_SCBAR + 5)
#define IDD_A160_DESC06         (IDD_A160_SCBAR + 6)
#define IDD_A160_DESC07         (IDD_A160_SCBAR + 7)
#define IDD_A160_DESC08         (IDD_A160_SCBAR + 8)
                            
#define IDD_A160_CAPTION1	16009
#define IDD_A160_CAPTION2	16010
                            
#define IDD_A160_D101           (IDD_A160_DESC01 + A160_EDIT_OFFSET)
#define IDD_A160_D102           (IDD_A160_D101 + 1)
#define IDD_A160_D103           (IDD_A160_D101 + 2)
#define IDD_A160_D104           (IDD_A160_D101 + 3)
#define IDD_A160_D105           (IDD_A160_D101 + 4)
#define IDD_A160_D106           (IDD_A160_D101 + 5)
#define IDD_A160_D107           (IDD_A160_D101 + 6)
#define IDD_A160_D108           (IDD_A160_D101 + 7)
#define IDD_A160_D109           (IDD_A160_D101 + 8)
#define IDD_A160_D110           (IDD_A160_D101 + 9)

#define IDD_A160_D101_SPIN      (IDD_A160_D101 + A160_SPIN_OFFSET)
#define IDD_A160_D102_SPIN      (IDD_A160_D102 + A160_SPIN_OFFSET)
#define IDD_A160_D103_SPIN      (IDD_A160_D103 + A160_SPIN_OFFSET)
#define IDD_A160_D104_SPIN      (IDD_A160_D104 + A160_SPIN_OFFSET)
#define IDD_A160_D105_SPIN      (IDD_A160_D105 + A160_SPIN_OFFSET)
#define IDD_A160_D106_SPIN      (IDD_A160_D106 + A160_SPIN_OFFSET)
#define IDD_A160_D107_SPIN      (IDD_A160_D107 + A160_SPIN_OFFSET)
#define IDD_A160_D108_SPIN      (IDD_A160_D108 + A160_SPIN_OFFSET)
#define IDD_A160_D109_SPIN      (IDD_A160_D109 + A160_SPIN_OFFSET)
#define IDD_A160_D110_SPIN      (IDD_A160_D110 + A160_SPIN_OFFSET)

#define IDD_A160_D201           (IDD_A160_D101 + A160_EDIT_OFFSET)
#define IDD_A160_D202           (IDD_A160_D201 + 1)
#define IDD_A160_D203           (IDD_A160_D201 + 2)
#define IDD_A160_D204           (IDD_A160_D201 + 3)
#define IDD_A160_D205           (IDD_A160_D201 + 4)
#define IDD_A160_D206           (IDD_A160_D201 + 5)
#define IDD_A160_D207           (IDD_A160_D201 + 6)
#define IDD_A160_D208           (IDD_A160_D201 + 7)
#define IDD_A160_D209           (IDD_A160_D201 + 8)
#define IDD_A160_D210           (IDD_A160_D201 + 9)

#define IDD_A160_D201_SPIN      (IDD_A160_D201 + A160_SPIN_OFFSET)
#define IDD_A160_D202_SPIN      (IDD_A160_D202 + A160_SPIN_OFFSET)
#define IDD_A160_D203_SPIN      (IDD_A160_D203 + A160_SPIN_OFFSET)
#define IDD_A160_D204_SPIN      (IDD_A160_D204 + A160_SPIN_OFFSET)
#define IDD_A160_D205_SPIN      (IDD_A160_D205 + A160_SPIN_OFFSET)
#define IDD_A160_D206_SPIN      (IDD_A160_D206 + A160_SPIN_OFFSET)
#define IDD_A160_D207_SPIN      (IDD_A160_D207 + A160_SPIN_OFFSET)
#define IDD_A160_D208_SPIN      (IDD_A160_D208 + A160_SPIN_OFFSET)
#define IDD_A160_D209_SPIN      (IDD_A160_D209 + A160_SPIN_OFFSET)
#define IDD_A160_D210_SPIN      (IDD_A160_D210 + A160_SPIN_OFFSET)

#define IDD_A160_D301           (IDD_A160_D201 + A160_EDIT_OFFSET)
#define IDD_A160_D302           (IDD_A160_D301 + 1)
#define IDD_A160_D303           (IDD_A160_D301 + 2)
#define IDD_A160_D304           (IDD_A160_D301 + 3)
#define IDD_A160_D305           (IDD_A160_D301 + 4)
#define IDD_A160_D306           (IDD_A160_D301 + 5)
#define IDD_A160_D307           (IDD_A160_D301 + 6)
#define IDD_A160_D308           (IDD_A160_D301 + 7)
#define IDD_A160_D309           (IDD_A160_D301 + 8)
#define IDD_A160_D310           (IDD_A160_D301 + 9)

#define IDD_A160_D301_SPIN      (IDD_A160_D301 + A160_SPIN_OFFSET)
#define IDD_A160_D302_SPIN      (IDD_A160_D302 + A160_SPIN_OFFSET)
#define IDD_A160_D303_SPIN      (IDD_A160_D303 + A160_SPIN_OFFSET)
#define IDD_A160_D304_SPIN      (IDD_A160_D304 + A160_SPIN_OFFSET)
#define IDD_A160_D305_SPIN      (IDD_A160_D305 + A160_SPIN_OFFSET)
#define IDD_A160_D306_SPIN      (IDD_A160_D306 + A160_SPIN_OFFSET)
#define IDD_A160_D307_SPIN      (IDD_A160_D307 + A160_SPIN_OFFSET)
#define IDD_A160_D308_SPIN      (IDD_A160_D308 + A160_SPIN_OFFSET)
#define IDD_A160_D309_SPIN      (IDD_A160_D309 + A160_SPIN_OFFSET)
#define IDD_A160_D310_SPIN      (IDD_A160_D310 + A160_SPIN_OFFSET)

#define IDD_A160_D401           (IDD_A160_D301 + A160_EDIT_OFFSET)
#define IDD_A160_D402           (IDD_A160_D401 + 1)
#define IDD_A160_D403           (IDD_A160_D401 + 2)
#define IDD_A160_D404           (IDD_A160_D401 + 3)
#define IDD_A160_D405           (IDD_A160_D401 + 4)
#define IDD_A160_D406           (IDD_A160_D401 + 5)
#define IDD_A160_D407           (IDD_A160_D401 + 6)
#define IDD_A160_D408           (IDD_A160_D401 + 7)
#define IDD_A160_D409           (IDD_A160_D401 + 8)
#define IDD_A160_D410           (IDD_A160_D401 + 9)

#define IDD_A160_D401_SPIN      (IDD_A160_D401 + A160_SPIN_OFFSET)
#define IDD_A160_D402_SPIN      (IDD_A160_D402 + A160_SPIN_OFFSET)
#define IDD_A160_D403_SPIN      (IDD_A160_D403 + A160_SPIN_OFFSET)
#define IDD_A160_D404_SPIN      (IDD_A160_D404 + A160_SPIN_OFFSET)
#define IDD_A160_D405_SPIN      (IDD_A160_D405 + A160_SPIN_OFFSET)
#define IDD_A160_D406_SPIN      (IDD_A160_D406 + A160_SPIN_OFFSET)
#define IDD_A160_D407_SPIN      (IDD_A160_D407 + A160_SPIN_OFFSET)
#define IDD_A160_D408_SPIN      (IDD_A160_D408 + A160_SPIN_OFFSET)
#define IDD_A160_D409_SPIN      (IDD_A160_D409 + A160_SPIN_OFFSET)
#define IDD_A160_D410_SPIN      (IDD_A160_D410 + A160_SPIN_OFFSET)

#define IDD_A160_D501          (IDD_A160_D401 + A160_EDIT_OFFSET)
#define IDD_A160_D502          (IDD_A160_D501 + 1)
#define IDD_A160_D503          (IDD_A160_D501 + 2)
#define IDD_A160_D504          (IDD_A160_D501 + 3)
#define IDD_A160_D505          (IDD_A160_D501 + 4)
#define IDD_A160_D506          (IDD_A160_D501 + 5)
#define IDD_A160_D507          (IDD_A160_D501 + 6)
#define IDD_A160_D508          (IDD_A160_D501 + 7)
#define IDD_A160_D509          (IDD_A160_D501 + 8)
#define IDD_A160_D510          (IDD_A160_D501 + 9)

#define IDD_A160_D501_SPIN     (IDD_A160_D501 + A160_SPIN_OFFSET)
#define IDD_A160_D502_SPIN     (IDD_A160_D502 + A160_SPIN_OFFSET)
#define IDD_A160_D503_SPIN     (IDD_A160_D503 + A160_SPIN_OFFSET)
#define IDD_A160_D504_SPIN     (IDD_A160_D504 + A160_SPIN_OFFSET)
#define IDD_A160_D505_SPIN     (IDD_A160_D505 + A160_SPIN_OFFSET)
#define IDD_A160_D506_SPIN     (IDD_A160_D506 + A160_SPIN_OFFSET)
#define IDD_A160_D507_SPIN     (IDD_A160_D507 + A160_SPIN_OFFSET)
#define IDD_A160_D508_SPIN     (IDD_A160_D508 + A160_SPIN_OFFSET)
#define IDD_A160_D509_SPIN     (IDD_A160_D509 + A160_SPIN_OFFSET)
#define IDD_A160_D510_SPIN     (IDD_A160_D510 + A160_SPIN_OFFSET)

#define IDD_A160_D601          (IDD_A160_D501 + A160_EDIT_OFFSET)
#define IDD_A160_D602          (IDD_A160_D601 + 1)
#define IDD_A160_D603          (IDD_A160_D601 + 2)
#define IDD_A160_D604          (IDD_A160_D601 + 3)
#define IDD_A160_D605          (IDD_A160_D601 + 4)
#define IDD_A160_D606          (IDD_A160_D601 + 5)
#define IDD_A160_D607          (IDD_A160_D601 + 6)
#define IDD_A160_D608          (IDD_A160_D601 + 7)
#define IDD_A160_D609          (IDD_A160_D601 + 8)
#define IDD_A160_D610          (IDD_A160_D601 + 9)

#define IDD_A160_D601_SPIN     (IDD_A160_D601 + A160_SPIN_OFFSET)
#define IDD_A160_D602_SPIN     (IDD_A160_D602 + A160_SPIN_OFFSET)
#define IDD_A160_D603_SPIN     (IDD_A160_D603 + A160_SPIN_OFFSET)
#define IDD_A160_D604_SPIN     (IDD_A160_D604 + A160_SPIN_OFFSET)
#define IDD_A160_D605_SPIN     (IDD_A160_D605 + A160_SPIN_OFFSET)
#define IDD_A160_D606_SPIN     (IDD_A160_D606 + A160_SPIN_OFFSET)
#define IDD_A160_D607_SPIN     (IDD_A160_D607 + A160_SPIN_OFFSET)
#define IDD_A160_D608_SPIN     (IDD_A160_D608 + A160_SPIN_OFFSET)
#define IDD_A160_D609_SPIN     (IDD_A160_D609 + A160_SPIN_OFFSET)
#define IDD_A160_D610_SPIN     (IDD_A160_D610 + A160_SPIN_OFFSET)

#define IDD_A160_D701          (IDD_A160_D601 + A160_EDIT_OFFSET)
#define IDD_A160_D702          (IDD_A160_D701 + 1)
#define IDD_A160_D702          (IDD_A160_D701 + 1)
#define IDD_A160_D703          (IDD_A160_D701 + 2)
#define IDD_A160_D704          (IDD_A160_D701 + 3)
#define IDD_A160_D705          (IDD_A160_D701 + 4)
#define IDD_A160_D706          (IDD_A160_D701 + 5)
#define IDD_A160_D707          (IDD_A160_D701 + 6)
#define IDD_A160_D708          (IDD_A160_D701 + 7)
#define IDD_A160_D709          (IDD_A160_D701 + 8)
#define IDD_A160_D710          (IDD_A160_D701 + 9)

#define IDD_A160_D701_SPIN     (IDD_A160_D701 + A160_SPIN_OFFSET)
#define IDD_A160_D702_SPIN     (IDD_A160_D702 + A160_SPIN_OFFSET)
#define IDD_A160_D703_SPIN     (IDD_A160_D703 + A160_SPIN_OFFSET)
#define IDD_A160_D704_SPIN     (IDD_A160_D704 + A160_SPIN_OFFSET)
#define IDD_A160_D705_SPIN     (IDD_A160_D705 + A160_SPIN_OFFSET)
#define IDD_A160_D706_SPIN     (IDD_A160_D706 + A160_SPIN_OFFSET)
#define IDD_A160_D707_SPIN     (IDD_A160_D707 + A160_SPIN_OFFSET)
#define IDD_A160_D708_SPIN     (IDD_A160_D708 + A160_SPIN_OFFSET)
#define IDD_A160_D709_SPIN     (IDD_A160_D709 + A160_SPIN_OFFSET)
#define IDD_A160_D710_SPIN     (IDD_A160_D710 + A160_SPIN_OFFSET)

#define IDD_A160_D801          (IDD_A160_D701 + A160_EDIT_OFFSET)
#define IDD_A160_D802          (IDD_A160_D801 + 1)
#define IDD_A160_D803          (IDD_A160_D801 + 2)
#define IDD_A160_D804          (IDD_A160_D801 + 3)
#define IDD_A160_D805          (IDD_A160_D801 + 4)
#define IDD_A160_D806          (IDD_A160_D801 + 5)
#define IDD_A160_D807          (IDD_A160_D801 + 6)
#define IDD_A160_D808          (IDD_A160_D801 + 7)
#define IDD_A160_D809          (IDD_A160_D801 + 8)
#define IDD_A160_D810          (IDD_A160_D801 + 9)

#define IDD_A160_D801_SPIN     (IDD_A160_D801 + A160_SPIN_OFFSET)
#define IDD_A160_D802_SPIN     (IDD_A160_D802 + A160_SPIN_OFFSET)
#define IDD_A160_D803_SPIN     (IDD_A160_D803 + A160_SPIN_OFFSET)
#define IDD_A160_D804_SPIN     (IDD_A160_D804 + A160_SPIN_OFFSET)
#define IDD_A160_D805_SPIN     (IDD_A160_D805 + A160_SPIN_OFFSET)
#define IDD_A160_D806_SPIN     (IDD_A160_D806 + A160_SPIN_OFFSET)
#define IDD_A160_D807_SPIN     (IDD_A160_D807 + A160_SPIN_OFFSET)
#define IDD_A160_D808_SPIN     (IDD_A160_D808 + A160_SPIN_OFFSET)
#define IDD_A160_D809_SPIN     (IDD_A160_D809 + A160_SPIN_OFFSET)
#define IDD_A160_D810_SPIN     (IDD_A160_D810 + A160_SPIN_OFFSET)

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_A160_DESC    IDS_A160

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define A160_ADDR_MAX    99                  /* Maximum Number of Address , V3.3    */
#define A160_ADDR_NO     99

#define A160_DATA_MAX    99                  /* Max. Value of Input Data     */
#define A160_DATA_MIN    0                   /* Min. Value of Input Data     */
#define A160_DATA_LEN    2                   /* Max. Length of Input Data    */

#define A160_DESC_LEN    128                 /* Length of Description        */

#define A160_ONE_LINE    1                   /* Scroll to One Line Up/ Down  */
#define A160_MAX_PAGE    8                   /* No. of Item in One Page      */
#define A160_MAX_GROUP   10                  /* Number of Group              */
#define A160_ONE_PAGE    (A160_MAX_PAGE - A160_ONE_LINE)
                                            /* Scroll to One Page Up/ Down  */
#define A160_SCBAR_MIN   0                   /* Min. Value of Scroll Range   */
#define A160_SCBAR_MAX   (A160_ADDR_NO - A160_MAX_PAGE)
                                            /* Max. Value of Scroll Range   */

#define A160_CHK_ADDR    2                   /* Even/Odd Address Checker     */

#define A160_SPIN_STEP   1
#define A160_SPIN_TURBO  0

/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/


/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct _A160data {
    UCHAR  UNINT ParaOep[MAX_TABLE_NO][MAX_TABLE_ADDRESS];         /* Data Value of A160 */
} A160DATA, FAR *LPA160DATA;

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

BOOL    A160AllocMem(HWND hDlg,LPHGLOBAL lphOep);
VOID    A160FreeMem(LPHGLOBAL lphOep);
VOID    A160InitDlg(HWND hDlg);
BOOL    A160GetData(HWND hDlg, WPARAM wParam);
VOID    A160DispErr(HWND hDlg);
VOID    A160ResetData(HWND hDlg, WPARAM wParam);
VOID    A160SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
VOID    A160ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
BOOL    A160CalcIndex(WORD wScrlCode);
VOID    A160RedrawText(HWND hDlg);
VOID    A160RedrawThumb(LPARAM lCtrl);

HWND	A160ModeLessDialog (HWND hParentWnd, int nCmdShow);
HWND	A160ModeLessDialogWnd (void);
