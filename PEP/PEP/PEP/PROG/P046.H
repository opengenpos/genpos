/*
* ---------------------------------------------------------------------------
* Title         :   Adjective Mnemonics Header File (Prog. 46)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P046.H
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
#define IDD_P46_SCBAR   24601

#define IDD_P46_DESC01  (IDD_P46_SCBAR +  1)
#define IDD_P46_DESC02  (IDD_P46_SCBAR +  2)
#define IDD_P46_DESC03  (IDD_P46_SCBAR +  3)
#define IDD_P46_DESC04  (IDD_P46_SCBAR +  4)
#define IDD_P46_DESC05  (IDD_P46_SCBAR +  5)
#define IDD_P46_DESC06  (IDD_P46_SCBAR +  6)
#define IDD_P46_DESC07  (IDD_P46_SCBAR +  7)
#define IDD_P46_DESC08  (IDD_P46_SCBAR +  8)
#define IDD_P46_DESC09  (IDD_P46_SCBAR +  9)
#define IDD_P46_DESC10  (IDD_P46_SCBAR + 10)

#define IDD_P46_EDIT01  (IDD_P46_SCBAR + 11)
#define IDD_P46_EDIT02  (IDD_P46_SCBAR + 12)
#define IDD_P46_EDIT03  (IDD_P46_SCBAR + 13)
#define IDD_P46_EDIT04  (IDD_P46_SCBAR + 14)
#define IDD_P46_EDIT05  (IDD_P46_SCBAR + 15)
#define IDD_P46_EDIT06  (IDD_P46_SCBAR + 16)
#define IDD_P46_EDIT07  (IDD_P46_SCBAR + 17)
#define IDD_P46_EDIT08  (IDD_P46_SCBAR + 18)
#define IDD_P46_EDIT09  (IDD_P46_SCBAR + 19)
#define IDD_P46_EDIT10  (IDD_P46_SCBAR + 20)

#define IDD_P46_ADDR	24630
#define IDD_P46_DESC	24631
#define IDD_P46_MNEM	24632
#define IDD_P46_CAPTION	24633

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_P046DESC001 IDS_P46                 /* Description Resouce ID   */
#define IDS_P046DESC002 (IDS_P046DESC001 +  1)  /* Description Resouce ID   */
#define IDS_P046DESC003 (IDS_P046DESC001 +  2)  /* Description Resouce ID   */
#define IDS_P046DESC004 (IDS_P046DESC001 +  3)  /* Description Resouce ID   */
#define IDS_P046DESC005 (IDS_P046DESC001 +  4)  /* Description Resouce ID   */
#define IDS_P046DESC006 (IDS_P046DESC001 +  5)  /* Description Resouce ID   */
#define IDS_P046DESC007 (IDS_P046DESC001 +  6)  /* Description Resouce ID   */
#define IDS_P046DESC008 (IDS_P046DESC001 +  7)  /* Description Resouce ID   */
#define IDS_P046DESC009 (IDS_P046DESC001 +  8)  /* Description Resouce ID   */
#define IDS_P046DESC010 (IDS_P046DESC001 +  9)  /* Description Resouce ID   */
#define IDS_P046DESC011 (IDS_P046DESC001 + 10)  /* Description Resouce ID   */
#define IDS_P046DESC012 (IDS_P046DESC001 + 11)  /* Description Resouce ID   */
#define IDS_P046DESC013 (IDS_P046DESC001 + 12)  /* Description Resouce ID   */
#define IDS_P046DESC014 (IDS_P046DESC001 + 13)  /* Description Resouce ID   */
#define IDS_P046DESC015 (IDS_P046DESC001 + 14)  /* Description Resouce ID   */
#define IDS_P046DESC016 (IDS_P046DESC001 + 15)  /* Description Resouce ID   */
#define IDS_P046DESC017 (IDS_P046DESC001 + 16)  /* Description Resouce ID   */
#define IDS_P046DESC018 (IDS_P046DESC001 + 17)  /* Description Resouce ID   */
#define IDS_P046DESC019 (IDS_P046DESC001 + 18)  /* Description Resouce ID   */
#define IDS_P046DESC020 (IDS_P046DESC001 + 19)  /* Description Resouce ID   */

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P46_ADDR_MAX    MAX_ADJM_NO         /* Number of Address with       */
                                            /*              Reserved Area   */
#define P46_ADDR_NO     ADJ_ADR_MAX         /* Number of Address without    */
                                            /*              Reserved Area   */
#define P46_MNE_LEN     4
#define P46_DESC_LEN    64                  /* Max Length of Description    */

#define P46_ERR_LEN     PEP_ALLOC_LEN       /* Max Length of Error Message  */
#define P46_CAP_LEN     PEP_CAPTION_LEN     /* Length of MessageBox Caption */

#define P46_ONE_LINE    1                   /* Scroll to One Line Up/ Down  */
#define P46_ONE_PAGE    9                   /* Scroll to One Page Up/ Down  */
#define P46_PAGE_MAX    10                  /* No. of Items in One Page     */

#define P46_SCBAR_MIN   0                   /* Min. Value of Scroll Range   */
#define P46_SCBAR_MAX   (P46_ADDR_NO - P46_PAGE_MAX)

/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/
#define P46GETINDEX(id, top)   (((id) - IDD_P46_EDIT01) + (top))

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct  _Mnemonic {
            WCHAR    chMnemo[P46_MNE_LEN];       /* Mnemonic Data */
} P46MNEMO, FAR *LPP46MNEMO;

typedef struct  _Work {
            WCHAR    szWork[P46_MNE_LEN + 1];   /* Mnemonic Work Area */
} P46WORK, FAR *LPP46WORK;

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
VOID    P46InitDlg(HWND);
VOID    P46GetMnemo(HWND, WORD wItem);
VOID    P46DispAllocErr(HWND, HGLOBAL, HGLOBAL);
BOOL    P46ScrlProc(HWND, WPARAM, LPARAM);
BOOL    P46CalcIndex(WORD wScrlCode);
VOID    P46RedrawText(HWND);

/* ===== End of File (P46.H) ===== */
