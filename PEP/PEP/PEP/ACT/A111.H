/*
* ---------------------------------------------------------------------------
* Title         :   Set Tare Table Header File (AC 111)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A111.H
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
#define A111_SPIN_OFFSET       30           /* SpinButton Offset    */
#define IDD_A111_ERR        11100           /* Error DialogBox      */

    /* ----- EditText ID ----- */
#define IDD_A111_EDIT01     11101
#define IDD_A111_EDIT02     11102
#define IDD_A111_EDIT03     11103
#define IDD_A111_EDIT04     11104
#define IDD_A111_EDIT05     11105
#define IDD_A111_EDIT06     11106
#define IDD_A111_EDIT07     11107
#define IDD_A111_EDIT08     11108
#define IDD_A111_EDIT09     11109
#define IDD_A111_EDIT10     11110
#define IDD_A111_EDIT11     11111
#define IDD_A111_EDIT12     11112
#define IDD_A111_EDIT13     11113
#define IDD_A111_EDIT14     11114
#define IDD_A111_EDIT15     11115
#define IDD_A111_EDIT16     11116
#define IDD_A111_EDIT17     11117
#define IDD_A111_EDIT18     11118
#define IDD_A111_EDIT19     11119
#define IDD_A111_EDIT20     11120
#define IDD_A111_EDIT21     11121
#define IDD_A111_EDIT22     11122
#define IDD_A111_EDIT23     11123
#define IDD_A111_EDIT24     11124
#define IDD_A111_EDIT25     11125
#define IDD_A111_EDIT26     11126
#define IDD_A111_EDIT27     11127
#define IDD_A111_EDIT28     11128
#define IDD_A111_EDIT29     11129
#define IDD_A111_EDIT30     11130

    /* ----- SpinButton ID ----- */
#define IDD_A111_SPIN01 (IDD_A111_EDIT01 + A111_SPIN_OFFSET)
#define IDD_A111_SPIN02 (IDD_A111_EDIT02 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN03 (IDD_A111_EDIT03 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN04 (IDD_A111_EDIT04 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN05 (IDD_A111_EDIT05 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN06 (IDD_A111_EDIT06 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN07 (IDD_A111_EDIT07 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN08 (IDD_A111_EDIT08 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN09 (IDD_A111_EDIT09 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN10 (IDD_A111_EDIT10 + A111_SPIN_OFFSET)  
#define IDD_A111_SPIN11 (IDD_A111_EDIT11 + A111_SPIN_OFFSET)
#define IDD_A111_SPIN12 (IDD_A111_EDIT12 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN13 (IDD_A111_EDIT13 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN14 (IDD_A111_EDIT14 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN15 (IDD_A111_EDIT15 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN16 (IDD_A111_EDIT16 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN17 (IDD_A111_EDIT17 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN18 (IDD_A111_EDIT18 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN19 (IDD_A111_EDIT19 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN20 (IDD_A111_EDIT20 + A111_SPIN_OFFSET) 
#define IDD_A111_SPIN21 (IDD_A111_EDIT21 + A111_SPIN_OFFSET)
#define IDD_A111_SPIN22 (IDD_A111_EDIT22 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN23 (IDD_A111_EDIT23 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN24 (IDD_A111_EDIT24 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN25 (IDD_A111_EDIT25 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN26 (IDD_A111_EDIT26 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN27 (IDD_A111_EDIT27 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN28 (IDD_A111_EDIT28 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN29 (IDD_A111_EDIT29 + A111_SPIN_OFFSET)    
#define IDD_A111_SPIN30 (IDD_A111_EDIT30 + A111_SPIN_OFFSET)  
 
    /* ----- RadioButton ID ----- */
#define IDD_A111RB1     (IDD_A111_SPIN30 + 1)   /* MDC Addr.29-B (0 thru 9) */
#define IDD_A111RB2     (IDD_A111_SPIN30 + 2)   /* MDC Addr.29-B (0 or 5)   */
#define IDD_A111RB3     (IDD_A111_SPIN30 + 3)   /* MDC Addr.29-D (0 - 999)  */
#define IDD_A111RB4     (IDD_A111_SPIN30 + 4)   /* MDC Addr.29-D (0 - 9999) */

    /* ----- Other Controls ----- */
#define IDD_A111EXTEND      (IDD_A111RB4 + 1)   /* Change MDC PushButton    */
#define IDD_A111CLOSE       (IDD_A111RB4 + 2)   /* Close MDC PushButton     */
#define IDD_A111CONDITION   (IDD_A111RB4 + 3)   /* Current MDC StaticText   */
#define IDD_A111EB1         (IDD_A111RB4 + 4)   /* RadioButton Error Dialog */
#define IDD_A111EB2         (IDD_A111RB4 + 5)   /* RadioButton Error Dialog */
#define IDD_A111GROUP_B     (IDD_A111RB4 + 6)   /* MDC Bit-B GroupBox       */
#define IDD_A111GROUP_D     (IDD_A111RB4 + 7)   /* MDC Bit-D GroupBox       */
#define IDD_A111TEXT_B      (IDD_A111RB4 + 8)   /* MDC Bit-B Description    */
#define IDD_A111TEXT_D      (IDD_A111RB4 + 9)   /* MDC Bit-D Description    */

#define IDD_A111_TARE1		(IDD_A111TEXT_D + 1)
#define IDD_A111_TARE2		(IDD_A111TEXT_D + 2)
#define IDD_A111_TARE3		(IDD_A111TEXT_D + 3)
#define IDD_A111_TARE4		(IDD_A111TEXT_D + 4)
#define IDD_A111_TARE5		(IDD_A111TEXT_D + 5)
#define IDD_A111_TARE6		(IDD_A111TEXT_D + 6)
#define IDD_A111_TARE7		(IDD_A111TEXT_D + 7)
#define IDD_A111_TARE8		(IDD_A111TEXT_D + 8)
#define IDD_A111_TARE9		(IDD_A111TEXT_D + 9)
#define IDD_A111_TARE10		(IDD_A111TEXT_D + 10)
#define IDD_A111_TARE11		(IDD_A111TEXT_D + 11)
#define IDD_A111_TARE12		(IDD_A111TEXT_D + 12)
#define IDD_A111_TARE13		(IDD_A111TEXT_D + 13)
#define IDD_A111_TARE14		(IDD_A111TEXT_D + 14)
#define IDD_A111_TARE15		(IDD_A111TEXT_D + 15)
#define IDD_A111_TARE16		(IDD_A111TEXT_D + 16)
#define IDD_A111_TARE17		(IDD_A111TEXT_D + 17)
#define IDD_A111_TARE18		(IDD_A111TEXT_D + 18)
#define IDD_A111_TARE19		(IDD_A111TEXT_D + 19)
#define IDD_A111_TARE20		(IDD_A111TEXT_D + 20)
#define IDD_A111_TARE21		(IDD_A111TEXT_D + 21)
#define IDD_A111_TARE22		(IDD_A111TEXT_D + 22)
#define IDD_A111_TARE23		(IDD_A111TEXT_D + 23)
#define IDD_A111_TARE24		(IDD_A111TEXT_D + 24)
#define IDD_A111_TARE25		(IDD_A111TEXT_D + 25)
#define IDD_A111_TARE26		(IDD_A111TEXT_D + 26)
#define IDD_A111_TARE27		(IDD_A111TEXT_D + 27)
#define IDD_A111_TARE28		(IDD_A111TEXT_D + 28)
#define IDD_A111_TARE29		(IDD_A111TEXT_D + 29)
#define IDD_A111_TARE30		(IDD_A111TEXT_D + 30)
#define	IDD_A111_CAPTION1	(IDD_A111TEXT_D + 31)
#define	IDD_A111_CAPTION2	(IDD_A111TEXT_D + 32)
#define	IDD_A111_CAPTION3	(IDD_A111TEXT_D + 33)
#define	IDD_A111_RADIOBUTTONS	(IDD_A111TEXT_D + 34)
#define	IDD_A111_GROUPBOX	(IDD_A111TEXT_D + 35)

/*
* ===========================================================================
*       String ID 
* ===========================================================================
*/
#define IDS_A111TEXT01      IDS_A111
#define IDS_A111TEXT02      (IDS_A111 + 1)
#define IDS_A111TEXT03      (IDS_A111 + 2)
#define IDS_A111_AGAIN      (IDS_A111 + 3)
#define IDS_A111_CLEAR      (IDS_A111 + 4)
#define IDS_A111_0OR5       (IDS_A111 + 5)
#define IDS_A111_ADDR       (IDS_A111 + 6)

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define A111_ADDR_MAX    MAX_TARE_SIZE  /* Number of Input Data         */
#define A111_DON_MAX         9999       /* Max. Value (Bit D-ON, B-ON)  */
#define A111_BOFF_MAX        9995       /* Max. Value (Bit D-ON, B-OFF) */
#define A111_DOFF_MAX         999       /* Max. Value (Bit D-OFF)       */
#define A111_DON_LEN            4       /* Limit Length (Bit D-ON)      */
#define A111_DOFF_LEN           3       /* Limit Length (Bit D-OFF)     */
#define A111_SPIN_BON_STEP      1       /* normal spin step (Bit B-ON)  */
#define A111_SPIN_BOFF_STEP     5       /* normal spin step (Bit B-OFF) */
#define A111_SPIN_TURBO        30       /* spin turbo point             */
#define A111_SPIN_BON_TURSTEP  10       /* turbo spin step (Bit B-ON)   */
#define A111_SPIN_BOFF_TURSTEP 50       /* turbo spin step (Bit B-OFF)  */

#define A111_ERR_INI           -1       /* initial error data           */
#define A111_INITIAL            0       /* initial tare data            */

#define A111_MDC_B           0x04       /* MDC address 29 Bit B         */
#define A111_MDC_D           0x01       /* MDC address 29 Bit D         */
#define A111_MDC_ADDR    MDC_SCALE_ADR  /* MDC address 29               */

#define A111_BUFFER          TRUE       /* flag of buffer error check   */
#define A111_EDITTEXT       FALSE       /* flag of edittext error check */

#define A111_ERR                3       /* number of error message      */
#define A111_OK                 0       /* MessageBox type              */
#define A111_OKCANCEL           1       /* MessageBox type              */
#define A111_EDITERR            2       /* error message type           */

#define A111_BUFF_LEN         120       /* Error Message Length         */
#define A111_NUM_LEN            3       /* Err Addr No. String Len      */

//6/29/04 - increased size to accomodate for french and spanish mnemonics
#define A111_TEXT_LEN          /*50*/75       /* Condition Text Length        */
#define A111_OFF_B              2       /* Value for Condition Text     */
#define A111_OFF_D              1       /* Value for Condition Text     */

#define A111_MULTIPLY           5       /* MDC Hide DlgBox Size         */
#define A111_DIVIDE             9       /* MDC Hide DlgBox Size         */

#define A111_LSTCHKER           5       /* least digit checker          */

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
VOID    A111InitDlg(HWND); 
WCHAR   A111CheckData(HWND, BOOL);
VOID    A111SetError(HWND, UINT, WORD wID);
VOID    A111SetCondition(HWND);
VOID    A111CheckEdit(HWND, WORD wID);
BOOL    A111SpinProc(HWND, WPARAM, LPARAM);
HWND	A111ModeLessDialog (HWND hParentWnd, int nCmdShow);
HWND	A111ModeLessDialogWnd (void);

/* ===== End of File (A111.H) ===== */
