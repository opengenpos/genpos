/*
* ---------------------------------------------------------------------------
* Title         :   Server Maintenance Header File (AC 9, 50)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A050.H
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
#define A50_SPIN_OFFSET        50
#define IDD_A50_MAX         15001   /* maximum server records           */
#define IDD_A50_CUR         15002   /* current server records           */
#define IDD_A50_LIST        15003   /* listbox                          */
#define IDD_A50_ADDCHG      15004   /* add, change button               */
#define IDD_A50_DEL         15005   /* delete button                    */
#define IDD_A50_NO          15006   /* server No.                       */
#define IDD_A50_NAME        15007   /* server name                      */
#define IDD_A50_TRAIN       15008   /* trainee status checkbox          */
#define IDD_A50_GCSTART     15009   /* GuestCheck start No.             */
#define IDD_A50_GCEND       15010   /* GuestCheck end No.               */
#define IDD_A50_CONF        15011   /* delete confirmation button       */
#define IDD_A50_NO_SPIN     (IDD_A50_NO      + A50_SPIN_OFFSET)
#define IDD_A50_START_SPIN  (IDD_A50_GCSTART + A50_SPIN_OFFSET)
#define IDD_A50_END_SPIN    (IDD_A50_GCEND   + A50_SPIN_OFFSET)

#define IDD_A50_MAINT		15012
#define IDD_A50_CAPTION1	15013
#define IDD_A50_CAPTION2	15014
#define IDD_A50_CAPTION3	15015
#define IDD_A50_GSTCK		15016
#define IDD_A50_STARTNO		15017
#define IDD_A50_ENDNO		15018
#define IDD_A50_CAPTION4	15019

/*
* ===========================================================================
*       String ID Define
* ===========================================================================
*/
#define IDS_A50_MAXSER          (IDS_A50 + 0)   /* max. servder records */
#define IDS_A50_CURSER          (IDS_A50 + 1)   /* current records */
#define IDS_A50_DELCONF         (IDS_A50 + 2)   /* delete confirmation */
#define IDS_A50_FLEX_EMPTY      (IDS_A50 + 3)   /* empty flex memory */
#define IDS_A50_OVERRECORD      (IDS_A50 + 4)   /* over maximum */
#define IDS_A50_OVERGCEND       (IDS_A50 + 5)   /* over GC end */
#define IDS_A50_GCEND_0         (IDS_A50 + 6)   /* GC end is 0 */

/*
* ===========================================================================
*       Number
* ===========================================================================
*/
#define A50_NO_LEN          3   /* input maximum size of server No. (999)   */
#define A50_GC_LEN          4   /* input maximum size of GuestCheck         */
#define A50_MNE_LEN        32   /* server name & description buffer length  */
#define A50_MAX_LEN        96   /* server max No. name buffer length        */
#define A50_NAME_LEN (WAI_NUMBER_OF_WAITER_NAME)
                                /* server name length                       */
#define A50_MEM_LEN       128   /* flex memory empty caution message length */
#define A50_TRAIN_CHK (WAI_TRAINING_WAITER)
                                /* training server check flag               */
#define A50_CONF_CHK     0x01   /* delete confirmation flag                 */

#define A50_SER_NO_MIN      1
#define A50_SER_NO_MAX    999  
#define A50_GC_MIN          0
#define A50_GC_MAX       9999

#define A50_SPIN_STEP       1
#define A50_SPIN_TURBO     50
#define A50_SPIN_TURSTEP   50

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef WAIIF FAR* LPWAIIF;

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
USHORT  A050InitDlg(HWND, LPUSHORT);
USHORT  A050ReadMax(VOID);
VOID    A050InitList(HWND, LPWAIIF);
VOID    A050SetMaxRec(HWND, USHORT);
VOID    A050SetCurRec(HWND, USHORT);
VOID    A050SetData(HWND, LPWAIIF, USHORT);
BOOL    A050GetData(HWND, WPARAM, LPWAIIF, LPUSHORT, USHORT);
BOOL    A050ChkMax(HWND, WPARAM, USHORT, USHORT, USHORT);
DWORD   A050FindNo(HWND, UINT);
BOOL    A050GetGCNo(HWND, LPWAIIF);
BOOL    A050RefreshList(HWND, LPWAIIF, LPUSHORT, WPARAM);
int     A050DispDelConf(HWND, USHORT);
VOID    A050ChkExist(HWND);
BOOL    A050ChkGCData(HWND);
VOID    A050FinDlg(HWND, WPARAM);

/* ===== End of A050.H ===== */
