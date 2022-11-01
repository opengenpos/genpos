/*
* ---------------------------------------------------------------------------
* Title         :   Header File of Terminal Communication
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRATERM.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* --------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /c /AL /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:
*
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date          Ver.       Name             Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Jan-23-96 : 03.00.05 : M.Suzuki   : Chg TRANS_TERM_DIR_NAME_LEN 32->22 R3.1
*           :          :            : Chg TRANS_TERM_DIR_TAB 135->91 R3.1
*
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/


/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/
#define _INC_TRANSFER_TERMINAL


/*
*===========================================================================
*   Terminal Type Combo Box
*===========================================================================
*/
#define TRANS_TERM_TYPE_MAX        17       /* master, b-master, terminal...*/
#define TRANS_TERM_MASTER_NUM       2       /* master, b-master */
#define TRANS_TERM_TYPE_LEN     (13 + 1)    /* with null terminate character */
#define TRANS_TERM_CONNECT_MAX      3       /* serial, modem, lan, saratoga */
#define TRANS_TERM_CONNECT_LEN  (13 + 1)    /* with null terminate character */


/*
*===========================================================================
*   Terminal Communication Flags
*===========================================================================
*/
#define TRANS_TERM_FLAG_CONNECT    0x0001
#define TRANS_TERM_FLAG_PASS       0x0002
#define TRANS_TERM_FLAG_ALL_CAST   0x0004
#define TRANS_TERM_FLAG_SUP_CAST   0x0008
#define TRANS_TERM_FLAG_PLU_CAST   0x0010
#define TRANS_TERM_FLAG_LAN        0x0020   /* Saratoga */
#define TRANS_TERM_FLAG_SERIAL	   0x0040	/*JHHJ new flag for use in conjuction with the LAN now being the
											  the first option in the transfer dialog, this flag is mainly used
											   for choosing the correct mnemonic to be displayed.*/
#define TRANS_TERM_FLAG_LAY_CAST   0x0080   /* Broadcast Layout Files ESMITH LAYOUT */
#define TRANS_TERM_FLAG_LOGO_CAST  0x0100	/* Broadcast Logo Files JHH*/


/*
*===========================================================================
*   Directory List Box
*===========================================================================
*/
#define TRANS_TERM_DIR_NAME_LEN    22           /* with ".." */
#define TRANS_TERM_DIR_NO_LEN      19           /* with ".." */
#define TRANS_TERM_DIR_LEN      (TRANS_TERM_DIR_NAME_LEN + TRANS_TERM_DIR_NO_LEN)

#define TRANS_TERM_DIR_TAB         91


/*
*===========================================================================
*   Dial Type
*===========================================================================
*/
#define TRANS_TERM_DIR_TYPE_DIAL     0x01


/*
*===========================================================================
*   File Buffer of Global Memory
*===========================================================================
*/
#define TRANS_TERM_DIR_MEM_BUFF     0x7FFF


/*
*===========================================================================
*   Change Password
*===========================================================================
*/
#define TRANS_TERM_PASS_RETRY           3   /* times of retry */

#define TRANS_TERM_PASS_MODE_OLD        0   /* enter old password */
#define TRANS_TERM_PASS_MODE_NEW        1   /* enter new password */
#define TRANS_TERM_PASS_MODE_RE         2   /* enter re-enter new password */

#define TRANS_TERM_PASS_MASK         0xAA   /* security mask */


/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/


/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
/* ===== TRATERM.C ===== */
extern WCHAR         szTransFile[];
extern DIRECTORY    TransDirList;
extern int          iTransDirIndex;
extern WORD         wTransDirCmd;
extern LONG         lTransTester;               /* counter of loop tester */

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
/* ===== TRATERM.C ===== */
void TransTermChangeDir(HWND);
void TransTermInsertDir(HWND);
void TransTermStatus(HWND);
void TransTermControlButton(HWND);
void TransTermSetIconDirectory(HWND);

/* ===== TRADIR.C ===== */
BOOL TransTermDir(HWND hWnd);
BOOL WINAPI TransTermDirDlgProc(HWND, UINT, WPARAM, LPARAM);
void TransTermDirInit(void);
void TransTermDirRead(HWND);
void TransTermDirAdd(HWND);
void TransTermDirChange(HWND);
void TransTermDirDelete(HWND);

/* ===== TRAPASS.C ===== */
BOOL TransTermPass(HWND);
BOOL WINAPI TransTermPassDlgProc(HWND, UINT, WPARAM, LPARAM);
void TransTermPassConvert(WCHAR *);

/* ===== TRATSEL.C ===== */
BOOL TransTermSelectFilesSet(HWND);
BOOL TransTermSelectFilesGet(HWND);
BOOL    TransTermSelectFilesEnable(HWND);

/* ===== End of TRATERM.H ===== */
