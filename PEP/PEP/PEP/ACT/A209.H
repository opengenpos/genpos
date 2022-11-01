/*
* ---------------------------------------------------------------------------
* Title         :   Set Color Palette Header File (AC 209) 
* Category      :   Enhancement for Release 2.1
* Program Name  :   A209.H
* ---------------------------------------------------------------------------
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Jul-20-06 : 00.00.01 :            : Initial
*
* ===========================================================================
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

#define A209_SPIN_OFFSET    10
#define IDD_A209_PALETTE_ADR      12091
#define IDD_A209_FOREGROUND      (IDD_A209_PALETTE_ADR +1)
#define IDD_A209_BACKGROUND      (IDD_A209_PALETTE_ADR +2)

#define IDD_A209_ASPIN      (IDD_A209_PALETTE_ADR + A209_SPIN_OFFSET)
#define IDD_A209_BSPIN      (IDD_A209_FOREGROUND + A209_SPIN_OFFSET)
#define IDD_A209_CSPIN      (IDD_A209_BACKGROUND + A209_SPIN_OFFSET)

#define IDD_A209_STRPALETTE_ADR		20901
#define IDD_A209_STRFOREGROUND		(IDD_A209_STRPALETTE_ADR + 1)
#define IDD_A209_STRBACKGROUND		(IDD_A209_STRPALETTE_ADR + 2)
#define IDD_A209_STRDESC			(IDD_A209_STRPALETTE_ADR + 3)
#define IDD_A209_STRNOTE			(IDD_A209_STRPALETTE_ADR + 4)

#define IDD_A209_COMBO_ADR              1023
#define IDD_A209_FOREGROUND_SET         1024
#define IDD_A209_BACKGROUND_SET         1025
#define IDD_A209_SAMPLE                 1026

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/

#define IDS_A209_CAPTION		(IDS_A209 + 0)
#define IDS_A209_PALETTE_ADR	(IDS_A209 + 1)
#define IDS_A209_FOREGROUND		(IDS_A209 + 2)
#define IDS_A209_BACKGROUND		(IDS_A209 + 3)
#define IDS_A209_DESC			(IDS_A209 + 4)
#define IDS_A209_NOTE			(IDS_A209 + 5)

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/

#define A209_ADDR_MAX        MAX_COLORPALETTE_SIZE    /* Max. Boundary age        */
#define A209_DATA_MIN        0               /* Min. Value of Data Range */
#define A209_DATA_MAX        255             /* Max. Value of Data Range */
#define A209_DATA_LEN        3               /* Limit Length of Data     */

#define A209_SPIN_STEP       1               /* Normal Step of Spin      */
#define A209_SPIN_TURBO      30              /* Turbo Point of Spin      */
#define A209_SPIN_TURSTEP    10              /* Turbo Step of SpinButton */

#define A209_PALETTE_MAX		255		/* Max number of Color Palette Codes */
#define A209_PALETTE_MIN		1
#define A209_PALETTE_LEN		3
#define	A209_PALETTE_OK			0		// Value used when color choice made
#define	A209_PALETTE_CANCEL		1		// value used when color choice canceled

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
VOID    A209DispErr(HWND hDlg, WPARAM wID);
VOID    A209DrawSample(HWND hDlg, WORD wIndex);
BOOL    WINAPI  A209ColorProc(HWND, UINT, WPARAM, LPARAM);

/* ===== End of File (A209.H) ===== */
