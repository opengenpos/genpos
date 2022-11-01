/*
* ---------------------------------------------------------------------------
* Title         :   Set Promotional PLU Header File (AC 116)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A116.H
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
#define A116_SPIN_OFFSET    10
#define IDD_A116_MENULIST   11601
#define IDD_A116_PLULIST    (IDD_A116_MENULIST + 1)
#define IDD_A116_GROUP      (IDD_A116_MENULIST + 2)
#define IDD_A116_CONF       (IDD_A116_MENULIST + 3)
#define IDD_A116_CONTENT    (IDD_A116_MENULIST + 4)
#define IDD_A116_ADD        (IDD_A116_MENULIST + 5)
#define IDD_A116_DEL        (IDD_A116_MENULIST + 6)
#define IDD_A116_PLU        (IDD_A116_MENULIST + 7)
#define IDD_A116_ADJ        (IDD_A116_MENULIST + 8)

#define IDD_A116_VER        (IDD_A116_ADJ + 1)     /* Saratoga */

#define IDD_A116_CAPTION	11611
#define IDD_A116_GROUPBOX1	11612
#define IDD_A116_ADJCODE	11613
#define IDD_A116_CAPTION2	11614
#define IDD_A116_PLUNO		11615
#define IDD_A116_CAPTION3	11616

#define IDD_A116_PLU_SPIN   (IDD_A116_PLU + A116_SPIN_OFFSET)
#define IDD_A116_ADJ_SPIN   (IDD_A116_ADJ + A116_SPIN_OFFSET)
#define IDD_A116_NEXT       (IDD_A116_ADJ_SPIN + 1)
#define IDD_A116_PREV       (IDD_A116_ADJ_SPIN + 2)
#define IDD_A116_PLU_RNG	(IDD_A116_ADJ_SPIN + 3)
#define IDD_A116_ADJCODE_RNG (IDD_A116_ADJ_SPIN + 4)


/*
* ===========================================================================
*       Strings ID 
* ===========================================================================
*/
#define IDS_A116_CAUTION    IDS_A116
#define IDS_A116_NOFILE     (IDS_A116 + 1)
#define IDS_A116_NOTUSE     (IDS_A116 + 2)
#define IDS_A116_CONTENT    (IDS_A116 + 3)
#define IDS_A116_PREV       (IDS_A116 + 4)
#define IDS_A116_MENUPLU    (IDS_A116 + 5)
#define IDS_A116_PARTPLU    (IDS_A116 + 6)
#define IDS_A116_OPEN       (IDS_A116 + 7)

#define IDS_A116_ERR_PLUNO  (IDS_A116 + 30) /* Saratoga */
#define IDS_A116_LABEL      (IDS_A116 + 31) /* ###ADD Saratoga */
#define IDS_A116_ERR_PLUREC (IDS_A116 + 32) /* ###ADD Saratoga 09/14/00 */
/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
/*#define A116_MAXPLU     3000 *//* ###DEL Sarataoga *//* Max. Number of PLU File      */
#define A116_MAXPLU     FLEX_PLU_MAX        /* Max. Number of PLU File, ###ADD Saratoga */
//#define A116_PLU_MAX    MAX_PLU_NO          /* Max. Number for PLU, from old NCR 2170 and 4 digit PLU numbers  */
#define A116_PLU_MIN    1                   /* Min. Number of PLU           */

/*** NCR2172 ***/

#define A116_ADJ_MAX    MAX_ADJECTIVE_NO    /* Max. Number of Adjective     */
#define A116_ADJ_MIN    0                   /* Min. Number of Adjective     */
#define A116_ADJ_LEN    1                   /* Max. length of Adjective     */

#define A116_MENU_MAX   MAX_SETMENU_NO      /* Max. No. of Promotional PLU  */
#define A116_MENU_MIN   1                   /* Min. No. of Promotional PLU  */
#define A116_ADDR_MAX   6                   /* Promo. + Linked Address No.  */
#define A116_ADDR_MIN   0                   /* Min. No. of Program Address  */
#define A116_CHILD_MAX  5                   /* Max. Number of Linked PLU    */
#define A116_CHILD_MIN  1                   /* Min. Number of Linked PLU    */
#define A116_PART_ADDR  0                   /* Addr. No of Promo. Menu PLU  */
#define A116_PART_TYPE  0x01                /* ListBox Type is Parent       */

#define A116_OPEN       0                   /* PLU Type (Open Price)        */
#define A116_NON_ADJ    0                   /* PLU Type (Non Adjective)     */
#define A116_ONE_ADJ    1                   /* PLU Type (One Adjective)     */

#define A116_OPEN_01    0x40                /* PLU Code Check (Open Price)  */
#define A116_NON_10     0x80                /* PLU Code Check (Non Adj.)    */
#define A116_ONE_11     0xC0                /* PLU Code Check (One Adj.)    */

#define A116_DESC_LEN   108                  /* Max. Length of Description   */

#define A116_MNE_LEN    20                  /* Max. Length of PLU Mnemonic  */
#define A116_PRICE_LEN  7                   /* Max. Length of Price         */
#define A116_BUFF_LEN   128                 /* Delete Caution Message       */

#define A116_TEXT       10                  /* Linked PLU Button - Static   */

#define A116_DELCONF_ON 0x00                /* Delete Confirmation Status   */
#define A116_CONF_CHK   0xff                /* Delete Confirmation Status   */

#define A116_NO_FILE    -1
#define A116_EXIST      1

#define A116_SPIN_STEP      1
#define A116_SPIN_TURBO     50
#define A116_SPIN_TURSTEP   50

/*NCR2172*/
#define A116_ZEROPLU        L"0000000000000"

#define A116_ADJ_LOOP       5 /* Saratoga */

#define A116_OK             0L
#define A116_ERR           -1L
#define A116_LOOKUP_ERR    -2L


/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct _MenuPLU {
    MENUPLU aMenuPLU[A116_ADDR_MAX];    /* Promotional PLU Data Area        */
} A116PROMO, FAR *LPA116PROMO;

typedef MENUPLU FAR*    LPMENUPLU;

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

ULONG   A116InitDlg(HWND);

VOID    A116DelPromoPlu(HWND, char);
VOID    A116MakeDesc(LPMENUPLU, LPWSTR, WORD);
ULONG   A116FindPluNo(LPMENUPLU, ULONG);
VOID    A116ChgList(HWND, char, ULONG);
VOID    A116ChgContent(HWND, LPBYTE, ULONG);
VOID    A116ChgPromoPlu(HWND, char, ULONG);

VOID    A116ExecAllocErr(HWND, HGLOBAL, HGLOBAL);
VOID    A116SpinProc(HWND, WPARAM, LPARAM);
DWORD    A116GetPluNo(BOOL, WCHAR*);
VOID    MakePluListDesc(UCHAR*, UCHAR*, char*, USHORT, char*);

HWND	A116ModeLessDialog (HWND hParentWnd, int nCmdShow);
HWND	A116ModeLessDialogWnd (void);

/* ===== End of File (A116.H) ===== */
