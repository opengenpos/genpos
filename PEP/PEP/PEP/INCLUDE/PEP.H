/*
* ---------------------------------------------------------------------------
* Title         :   2170 PEP (Parameter Entry Program) Header File
* Category      :   NCR 2170 PEP for Windows    (Hotel US Model)
* Program Name  :   PEP.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /c /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Dec-22-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Mar-13-95 : 03.00.00 : H.Ishida   : Chg. from 0x1A00 to 0x1C00 by PEP_PARAM_LEN
* Nov-18-99 :          :            : Chg. from 0x4000 to 0x9DC4 by PEP_PARAM_LEN
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

#ifndef PEP_INCLUDED_DEFINED
#define PEP_INCLUDED_DEFINED

/*
* ===========================================================================
*       Define Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Dialog ID definition
* ===========================================================================
*/
/*----- Transfer ----- */
#ifdef A
#define IDD_SEND        3031        /* broadcast (send to 2170) A66         */
#define IDD_RECV        3032        /* broadcast (receive from 2170)        */
#define IDD_LINE        3033        /* select line configulation            */
#endif

/* ----- Help ----- */
#define IDD_HLP_IDX     2311        /* Help Index                           */
#define IDD_HLP_KEY     2312        /* Help Keyboards                       */
#define IDD_HLP_COMM    2313        /* Help Commands                        */
#define IDD_HLP_PROC    2314        /* Help Procedures                      */
#define IDD_HLP_USE     2315        /* Help Using                           */
#define IDD_HLP_ABOUT   2316        /* Help About                           */

#define IDD_HLP_CAPTION1	2317
#define IDD_HLP_CAPTION2	2318
#define IDD_HLP_CAPTION3	2319
#define IDD_HLP_CAPTION4	2320
#define IDD_HLP_CAPTION5	2321
#define IDD_HLP_CAPTION6	2322

/*
* ===========================================================================
*       Resource ID definition
* ===========================================================================
*/
/* ----- Icon ID ----- */

#define IDI_PEPICON1    20
#define IDI_PEPICON2    21
#define IDI_PEPICON3    22
#define IDI_PEPICON4    23
#define IDI_PEPICON5    24
#define IDI_PEPICON6    25
#define IDI_PEPICON7    26
#define IDI_PEPICON8    27
#define IDI_PEPICON9    28

/* ----- String ID ----- */

#define IDS_PEP                    10000

#define IDS_PEP_MODEL           (IDS_PEP + 2)
#define IDS_PEP_WINCLASS        (IDS_PEP + 3)
#define IDS_PEP_NONE			(IDS_PEP + 4)

#define IDS_PEP_ERR_DLL_PROG    (IDS_PEP + 16)
#define IDS_PEP_ERR_DLL_ACT     (IDS_PEP + 17)
#define IDS_PEP_ERR_DLL_COMM    (IDS_PEP + 18)
#define IDS_PEP_NAME_DLL_PROG   (IDS_PEP + 19)
#define IDS_PEP_NAME_DLL_ACT    (IDS_PEP + 20)
#define IDS_PEP_NAME_DLL_COMM   (IDS_PEP + 21)

#define IDS_PEP_ERR_DLL_STUB32  (IDS_PEP + 22)  /* V3.3 */
#define IDS_PEP_ERR_DLL_PIF32   (IDS_PEP + 23)  /* V3.3 */

/*JHHJ 1-8-04, Moved from P002.H and placed in PEP.H , renamed to IDS_PEP_....
so that it these mnemonics can be used in multiple places, mainly in the
error message for transfer "could not take "      ".  This will help by 
not having to use additional mnemonics for something that was already defined.*/
#define IDS_PEP_FILE_DEPT       (IDS_PEP + 24)
#define IDS_PEP_FILE_PLU        (IDS_PEP + 25)
#define IDS_PEP_FILE_SER        (IDS_PEP + 26)
#define IDS_PEP_FILE_CAS        (IDS_PEP + 27)
#define IDS_PEP_FILE_EJ         (IDS_PEP + 28)
#define IDS_PEP_FILE_ETK        (IDS_PEP + 29)
#define IDS_PEP_FILE_ITEM       (IDS_PEP + 30)
#define IDS_PEP_FILE_CONS       (IDS_PEP + 31)
#define IDS_PEP_FILE_GC         (IDS_PEP + 32)
#define IDS_PEP_FILE_CPN        (IDS_PEP + 33)
#define IDS_PEP_FILE_CSTR       (IDS_PEP + 34)
#define IDS_PEP_FILE_RPT        (IDS_PEP + 35)
#define IDS_PEP_FILE_PPI        (IDS_PEP + 36)
#define IDS_PEP_FILEINFO_FRMT	(IDS_PEP + 37)	//format string for file info in main window
#define IDS_PEP_TXSYS_VAT        (IDS_PEP + 38)
#define IDS_PEP_TXSYS_US        (IDS_PEP + 39)
#define IDS_PEP_TXSYS_CAN       (IDS_PEP + 40)

#define IDS_HELP                (IDS_PEP + 512)

/*
* ===========================================================================
*       Value definition for Resource
* ===========================================================================
*/
#define PEP_MENU_FILE           0

#define PEP_TITLE_LEN          48
#define PEP_TITLE_TEXT_LEN     23
#define PEP_TITLE_MODEL_LEN    13

#define PEP_STAT_CY             2
#define PEP_STAT_Y              1

#define PEP_TITLE_CALC_STD      2
#define PEP_TITLE_WIDTH        30
#define PEP_TITLE_HIGHT         3
#define PEP_TITLE_ADJ          10

#define PEP_BK_COLOR            RGB(0, 0, 0xc0)q//blue
//#define PEP_TEXT_COLOR          RGB(255, 255, 255)	//white
#define PEP_TEXT_COLOR          RGB(0, 0, 0)	//black

/*** NCR2172
#define PEP_PARAM_LEN      0x4000
***/
#if 1
#define PEP_PARAM_LEN      0xB436//0x9FCE
#endif

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#define PEP_PARAM_LEN      0x97B8
#endif

//for use in retriving MDC data for displaying file description info
#define PEP_GET_LOBIT(x)           (BYTE)(0x0f & x)
#define PEP_GET_HIBIT(x)           (BYTE)((0xf0 & x) >> 4)
#define PEP_CHK_ADDR				2
#define PEP_CHK_BIT				   0x01
#define PEP_CHK_4BITS			   0x0f

//strings used in file description info
#define IDS_PEP_GCDSCRB1		   3599
#define IDS_PEP_GCDSCRB2		   3600
#define IDS_PEP_GCDSCRB3		   3601
#define IDS_PEP_GCDSCRB4		   3602
#define IDS_PEP_BU_SYS			   30464
#define IDS_PEP_NOBU_SYS		   30465

/*
* ===========================================================================
*       Macro definition 
* ===========================================================================
*/
/*
* ===========================================================================
*       Structure type definition 
* ===========================================================================
*/

/*
* ===========================================================================
*       User Message Define Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       External Work Area Declarations
* ===========================================================================
*/
extern  HANDLE      hPepInst;       /* Instance Handle of PEP Window        */
extern  HANDLE      hPepProgInst;   /* Instance Handle of PEPPROG.DLL       */
#if defined(POSSIBLE_DEAD_CODE)
extern  HANDLE      hPepActInst;    /* Instance Handle of PEPACT.DLL        */
#endif
extern  HWND        hwndPepMain;    /* Window Handle of PEP Window          */
extern  HWND        hwndPepDlgChk2; /* Window Handle of Modeless DialogBox  */
extern  VOID        *lpPepParam;    /* address of default parameter set area*/

/*
* ===========================================================================
*       Function Prototype Declarations
* ===========================================================================
*/
LONG    WINAPI  PepWndProc(HWND, UINT, WPARAM, LPARAM);
LONG    WINAPI  StatWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

BOOL    WINAPI  DoHelp(HWND, DLGPROC, WORD);

int		PepChangeLanguage (HWND hWnd);
#if defined(POSSIBLE_DEAD_CODE)
static  HANDLE  GetDllInst(HANDLE, UINT);
#endif
static  HWND    InitPepWnd(HANDLE, HANDLE, int);
static  void    DrawTitle(HDC, HWND);
static  void    PepSubExe(HWND, WPARAM, LPARAM, UINT, UINT);
static  BOOL    CheckExistDlg(HWND);
static  void    ExecuteProg(HWND, WORD wEditID);
static  void    ExecuteAct(HWND, WORD wEditID);

VOID	GetLangFont(int iLiSel);
VOID	LangInitDlgStrings(HWND hDlg);
VOID    ProgInitialize(HANDLE hPep);
#endif
/* ===== End of File (PEP.H) ===== */

