/*
* ---------------------------------------------------------------------------
* Title         :   Hourly Activity Block Header File (Prog. 17)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P017.H
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
#define IDD_P17_BLK         21701       /* EditText for Activity Block      */ 
#define IDD_P17_HH          21702       /* EditText for Time Data (Hour)    */
#define IDD_P17_MM          21703       /* EditText for Time Data (Minute)  */
#define IDD_P17_SET         21704       /* PushButton (Activity Blocks)     */
#define IDD_P17_ENTER       21705       /* PushButton (Time Segment Data)   */
#define IDD_P17_COMB        21706       /* ComboBox (Time Segment 1 - 24)   */
#define IDD_P17_BLKSCR      21707       /* Vertical Scroll Bar (No. of Blk) */
#define IDD_P17_HHSCR       21708       /* Vertical Scroll Bar (Time Data)  */
#define IDD_P17_MMSCR       21709       /* Vertical Scroll Bar (Time Data)  */

#define IDD_P17_NUM_BLOCK	21710
#define IDD_P17_TIME_BLOCK	21711
#define IDD_P17_MIL_TIME	21712
#define IDD_BLOCK_RNG		21713
/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_P017_CAUTION    IDS_P17         /* Time Segment Data Clear */
#define IDS_P017_NUMBER     (IDS_P17 + 1)   /* ComboBox Description    */

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P17_ADDR_MAX        MAX_HOUR_NO /* Max. Number of Address       */
#define P17_HM_MAX          MAX_HM_DATA /* Max. Hour & Minute Address   */
#define P17_SEGMENT_MAX     48          /* Max. Number of Time Segment  V3.3 */
#define P17_BLOCK_ADDR      0           /* Address of No. of Blocks     */
#define P17_TIME1_ADDR      1           /* Address of Time Segment No.1 */
#define P17_HOUR_ADDR       0           /* Hour Data Address            */
#define P17_MIN_ADDR        1           /* Minute Data Address          */

#define P17_DATA_MIN        0           /* Min. Number of Inputed Data  */
#define P17_HH_MAX          23          /* Max of Hour Data             */
#define P17_MM_MAX          59          /* Max of Minute Data           */
#define P17_MINDATA         60          /* Minute Value for Calculate   */
#define P17_MIN_MAX     (P17_HH_MAX * P17_MINDATA + P17_MM_MAX)
                                        /* Max. of Line Position        */
#define P17_DATA_MAX        24          /* Max of Hourly Time           */

#define P17_SPIN_STEP       1
#define P17_SPIN_TURBO      0

#define P17_TRIPOS1         0           /* Triangle Position Saved Adr. */
#define P17_TRIPOS2         1           /* Triangle Position Saved Adr. */
#define P17_TRIPOS3         2           /* Triangle Position Saved Adr. */
#define P17_TRI_POINT       3           /* Triangle Point               */

#define P17_ERR_LEN     PEP_OVER_LEN    /* Error Message Length         */
#define P17_CAP_LEN     PEP_CAPTION_LEN /* MessageBx Caption Length     */
#define P17_MESS_LEN        120         /* Caution Message Length       */
#define P17_DATA_LEN        2           /* Length of Input Data         */
#define P17_MINLEN          3           /* Length of Minute Data (Text) */
#define P17_DESC_LEN        9           /* Description(ComboBox) Length */

#define P17_ORIGIN          0           /* View Port Origin Position    */
#define P17_WINDOW_RIGHT    1800        /* Client Area Width  (GDI)     */
#define P17_WINDOW_BOTTOM   1200        /* Client Area Height (GDI)     */

#define P17_WINDOW_MIN      0           /* Client Area Origin Position  */
#define P17_WINDOWX_MAX     480         /* Client Area Width  (Mouse)   */
#define P17_WINDOWY_MAX     320         /* Client Area Height (Mouse)   */

#define P17_BLACK_LEFT      120         /* Time Table Position (Left)   */
#define P17_BLACK_TOP       675         /* Time Table Position (Top)    */
#define P17_BLACK_RIGHT     1620        /* Time Table Position (Right)  */
#define P17_BLACK_BOTTOM    840         /* Time Table Position (Bottom) */

#define P17_TRI1_X          150         /* Triangle Default Position(X) */
#define P17_TRI2_X          130         /* Triangle Default Position(X) */
#define P17_TRI3_X          170         /* Triangle Default Position(X) */
#define P17_TRI_Y           875         /* Triangle Default Position(Y) */

#define P17_TRIMAX          (1440 + P17_TRI1_X)
                                        /* Max. Position of Triangle    */
#define P17_TRIX_LEN        (P17_TRI1_X - P17_TRI2_X)
                                        /* Length of Triangle Cursor    */

#define P17_BLACK           0           /* Line Color Style (Black)     */
#define P17_WHITE           1           /* Line Color Style (White)     */
#define P17_YELLOW          2           /* Line Color Style (Yellow)    */
#define P17_PEN_WIDTH       1

#define P17_MOUSEX_MIN      40          /* Min. Position of Mouse (x)   */
#define P17_MOUSEX_MAX      428         /* Max. Position of Mouse (x)   */
#define P17_MOUSEY_MIN      180         /* Min. Position of Mouse (y)   */
#define P17_MOUSEY_MAX      236         /* Max. Position of Mouse (y)   */
#define P17_MOUSEDOWN_Y     224         /* Min. Position of Mouse Down  */

#define P17_REMAIN_3        3           /* Mouse Position Check Digit   */
#define P17_REMAIN_2        2           /* Mouse Position Check Digit   */
#define P17_REMAIN_1        1           /* Mouse Position Check Digit   */
#define P17_MOUSE_ADJ_5     2           /* Adjust Mouse Position        */
#define P17_MOUSE_ADJ_10    3           /* Adjust Mouse Position        */
#define P17_MOUSE_ADJ_15    4           /* Adjust Mouse Position        */

/*
* ===========================================================================
*       Macro Definition
* ===========================================================================
*/
#define P17_CALCMINUTE(hh, mm)  (short)(hh * P17_MINDATA + mm)
#define P17_CALCPOS(min)        (short)((min == P17_MIN_MAX) ? (1440 + P17_TRI1_X): (min + P17_TRI1_X))
#define P17_GETTIME(x)          (short)((x - P17_MOUSEX_MIN) * 15 / 4)
#define P17_ADJUSTTIME(x)       (short)((x - P17_MOUSEX_MIN) * 15 % 4)
#define P17_GETPOS(x)           (short)((x - P17_MOUSEX_MIN) * 15 / 4 + P17_TRI1_X)

#define P17_CLIENT_LEFT(x)      (int)(x *  1 / 15 - P17_TRIX_LEN)
#define P17_CLIENT_RIGHT(x)     (int)(x *  9 / 10 + P17_TRIX_LEN)
#define P17_CLIENT_TOP(y)       (int)(y *  5 /  9)
#define P17_CLIENT_BOTTOM(y)    (int)(y * 11 / 15 + 10)
#define P17_TRI_TOP(y)          (int)(y *  7 / 10)

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct  tagP17LINE {    /* Line Position & Line Color Data  */
        short   xPosition;              /* Draw Position (Horizon)  */
        char    chColor;                /* Color of Line            */
} P17LINE, FAR *LPP17LINE;

typedef struct  tagP17CURSOR {  /* Cursor Position & Cursor Color Data */
        POINT   ptTri[P17_TRI_POINT];   /*  Draw Position   */
        char    chColor;                /* Color of Cursor  */
} P17CURSOR, FAR *LPP17CURSOR;

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
BOOL    P17InitDlg(HWND);                   /* Initialize DialogBox     */
VOID    P17InitCombo(HWND);                 /* Initialize ComboBox      */
WORD    P17SelCombo(HWND, WORD wIndex);            /* Slection ComboBox        */
VOID    P17SetLinePos(WORD wIndex, short);         /* Set Line & Cursor Pos.   */
VOID    P17ChkData(HWND, WORD wEditId);             /* Input in EditText        */
VOID    P17ResetBlk(HWND);                  /* Reset No. of Blocks      */
VOID    P17SetTime(HWND);                   /* Set Time Segment Data    */
VOID    P17DrawTime(HWND, BYTE, WORD wIndex);      /* Draw Time Table          */
BOOL    P17ChkPos(PPOINTS);                 /* Check Mouse Position     */
VOID    P17DispMove(HWND, PPOINTS, WORD wIndex);   /* Display Cursor           */
VOID    P17DispPre(HWND);                   /* Display Previous Cursor  */
BOOL    P17ChkDownPos(HWND, PPOINTS, LPWORD);/* Check L-Button Down Pos. */
VOID    P17ChkCtrlID(HWND, WORD wEditId, BYTE);   /* Check Control ID         */
short   P17CalcMinute(int);                 /* Calculate Minute         */
VOID    P17ScrlProc(HWND, WPARAM, LPARAM);

/* ===== End of File (P017.H) ===== */
