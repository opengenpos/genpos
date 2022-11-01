/*
* ---------------------------------------------------------------------------
* Title         :   New File Creation
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILENEW.C
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following function
*               FileNew():  creates new file
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Oct-14-98 : 02.01.00 : A.Mitsui   : V2.1 BugFixed
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
*       Include File Declarations
* ===========================================================================
*/
#include    <windows.h>
#include    <string.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>
#include    <pararam.h>

#include    "pep.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "trans.h"
#include    "file.h"
#include    "filel.h"
#include	"plu.h"
/* NCR2172 */

/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL FileNew()                              *|
|*              hInst    Instance handle of PEP.EXE         *|
|*              hwnd     handle of the main window          *|
|*              hdlgProg handle of dialogbox in PROG.EXE    *|
|*              hdlgAct  handle of dialogbox in ACTION.EXE  *|
|*                                                          *|
|* Description: This function clears memory and deletes     *|
|*              files.                                      *|
|*                                                          *|
\************************************************************/
BOOL FileNew(HANDLE hInst, HWND hwnd)
{
    void    *pBufStart;
    /*** NCR2172
    void    *pBufEnd;
    ***/
	HANDLE	layoutHandle;
    WCHAR   szWinPath[256];
	WCHAR   szMsg[FILE_MAXLEN_TEXT];
	WCHAR	szLayFile[PEP_FILENAME_LEN]; //ESMITH LAYOUT
	WCHAR   szWork[FILE_LEN_ID];
	int		nID;
	int     iCount = 0, iIDS = 0;

    
    if (PepQueryModFlag(PEP_MF_ALL, 0) == TRUE) {
                                        /* already changed file */
        MessageBeep(MB_ICONQUESTION);
        LoadString(hResourceDll, IDS_FILE_ERR_CONT, szMsg, PEP_STRING_LEN_MAC(szMsg));
        LoadFileCaption(hResourceDll, IDS_FILE_CAP_NEW);
        if (MessageBoxPopUpFileCaption(hwnd,            /* output message box   */
                       szMsg,
                       MB_ICONQUESTION | MB_YESNO) == IDNO) {
            return FALSE;               /* cancel   */
        }
    }

    /* clear memory     */
    /* configuration    */
    memset(&Fconfig, 0, sizeof(FCONFIG));
	Fconfig.ulStartAddr = (unsigned long)FILE_MADDR;    /* master terminal */
    Fconfig.ulStartAddr += (unsigned long)FILE_FAT_ADDR;    /* use RAM disk driver ,Bug fix*/

	Fconfig.chKBType = FILE_KB_TOUCH;  // set default keyboard to be touch keyboard

    Fconfig.usMemory = (FILE_SIZE_64KB * FILE_SIZE_48MBTOKB);  /* set default Flash memory 48MB ESMITH*/

    /* parameter        */

    /*** NCR2172
    pBufStart = ParaStart;
    pBufEnd   = ParaEnd;
    ***/
    pBufStart = &Para;

    /*** NCR2172
    memcpy(pBufStart, lpPepParam, FP_OFF(pBufEnd) - FP_OFF(pBufStart));
    ***/
    memcpy(pBufStart, lpPepParam, sizeof (Para));

	// insert the P20 mnemonics from the resource file
	memset (ParaTransMnemo, 0, sizeof(ParaTransMnemo));
	for (iCount = 0, iIDS = IDS_P20_TRANS_DEFAULT_001; iIDS <= IDS_P20_TRANS_DEFAULT_400; iCount++, iIDS++) {
		LoadString(hResourceDll, iIDS, szMsg, PEP_STRING_LEN_MAC(szMsg));
		wcsncpy(ParaTransMnemo[iCount], szMsg, PARA_TRANSMNEMO_LEN );
	}

	// insert the P21 mnemonics from the resource file
	memset (Para_LeadThru, 0, sizeof(Para_LeadThru));
	for (iCount = 0, iIDS = IDS_P21_LDT_DEFAULT_001; iIDS <= IDS_P21_LDT_DEFAULT_192; iCount++, iIDS++) {
		LoadString(hResourceDll, iIDS, szMsg, PEP_STRING_LEN_MAC(szMsg));
		wcsncpy(Para_LeadThru[iCount], szMsg, PARA_LEADTHRU_LEN );
	}

	// insert the P22 mnemonics from the resource file
	memset (ParaReportName, 0, sizeof(ParaReportName));
	for (iCount = 0, iIDS = IDS_P22_RPT_NAME_DEFAULT_001; iIDS <= IDS_P22_RPT_NAME_DEFAULT_024; iCount++, iIDS++) {
		LoadString(hResourceDll, iIDS, szMsg, PEP_STRING_LEN_MAC(szMsg));
		wcsncpy(ParaReportName[iCount], szMsg, PARA_REPORTNAME_LEN );
	}

	// insert the P23 mnemonics from the resource file
	memset (ParaSpeMnemo, 0, sizeof(ParaSpeMnemo));
	for (iCount = 0, iIDS = IDS_P23_SPCL_MNEM_DEFAULT_001; iIDS <= IDS_P23_SPCL_MNEM_DEFAULT_050; iCount++, iIDS++) {
		LoadString(hResourceDll, iIDS, szMsg, PEP_STRING_LEN_MAC(szMsg));
		wcsncpy(ParaSpeMnemo[iCount], szMsg, PARA_SPEMNEMO_LEN );
	}

	// insert the P46 mnemonics from the resource file
	memset (ParaAdjMnemo, 0, sizeof(ParaAdjMnemo));
	for (iCount = 0, iIDS = IDS_P46_ADJ_MNEM_DEFAULT_001; iIDS <= IDS_P46_ADJ_MNEM_DEFAULT_020; iCount++, iIDS++) {
		LoadString(hResourceDll, iIDS, szMsg, PEP_STRING_LEN_MAC(szMsg));
		wcsncpy(ParaAdjMnemo[iCount], szMsg, PARA_ADJMNEMO_LEN );
	}

	// insert the P47 mnemonics from the resource file
	memset (ParaPrtModi, 0, sizeof(ParaPrtModi));
	for (iCount = 0, iIDS = IDS_P47_PRINT_MNEM_DEFAULT_001; iIDS <= IDS_P47_PRINT_MNEM_DEFAULT_008; iCount++, iIDS++) {
		LoadString(hResourceDll, iIDS, szMsg, PEP_STRING_LEN_MAC(szMsg));
		wcsncpy(ParaPrtModi[iCount], szMsg, PARA_PRTMODI_LEN );
	}

	// insert the P48 mnemonics from the resource file
	memset (ParaMajorDEPT, 0, sizeof(ParaMajorDEPT));
	for (iCount = 0, iIDS = IDS_P48_RPT_CODE_MNEM_DEFAULT_001; iIDS <= IDS_P48_RPT_CODE_MNEM_DEFAULT_030; iCount++, iIDS++) {
		LoadString(hResourceDll, iIDS, szMsg, PEP_STRING_LEN_MAC(szMsg));
		wcsncpy(ParaMajorDEPT[iCount], szMsg, PARA_MAJORDEPT_LEN );
	}

	// insert the P57 mnemonics from the resource file
	memset (ParaChar24, 0, sizeof(ParaChar24));
	for (iCount = 0, iIDS = IDS_P57_24_CHR_MNEM_DEFAULT_001; iIDS <= IDS_P57_24_CHR_MNEM_DEFAULT_020; iCount++, iIDS++) {
		LoadString(hResourceDll, iIDS, szMsg, PEP_STRING_LEN_MAC(szMsg));
		wcsncpy(ParaChar24[iCount], szMsg, PARA_CHAR24_LEN );
	}

    /* transfer         */
    TransInitialize();

    /* delete files     */
    FileTempDelete(hInst);

    /* clear file name  */
    memset(szFileSaveName, 0, sizeof(szFileSaveName));
    memset(szCommRptSaveName, 0, sizeof(szCommRptSaveName));
	memset(szCommKeyboardLayoutSaveName, 0, sizeof(szCommKeyboardLayoutSaveName)); //ESMITH LAYOUT
	memset(szCommTouchLayoutSaveName, 0, sizeof(szCommTouchLayoutSaveName));
	memset(szCommLogoSaveName, 0, sizeof(szCommLogoSaveName));
	memset(szCommIconSaveName, 0 ,sizeof(szCommIconSaveName));

    LoadString(hResourceDll, IDS_FILE_ID_VER, szWork, PEP_STRING_LEN_MAC(szWork)); //New version information for NHPOS JHHJ
	wcsncpy(Para.szVer, szWork,FILE_LEN_ID);


    /* Create MLD file, IDS_FILE_NAME_MLD */
    MldCreateFile(hInst);
    /* Create Reason Code mnemonics file, IDS_FILE_NAME_RSN */
    RsnCreateFile(hInst);

	/* Create Layout Files ESMITH LAYOUT */
	for( nID = IDS_FILE_NAME_TOUCH_LAYOUTFILE; nID <= IDS_FILE_NAME_LOGOFILEA; nID++)
	{
#if 1
		GetPepTemporaryFolder (NULL, szWinPath, PEP_STRING_LEN_MAC(szWinPath));
#else
		GetPepModuleFileName(NULL, szWinPath, PEP_STRING_LEN_MAC(szWinPath));
		szWinPath[wcslen(szWinPath)-8] = '\0';  //remove pep.exe from path
		wcscat( szWinPath, WIDE("\\"));
#endif

		LoadString(hResourceDll, nID, szLayFile, PEP_STRING_LEN_MAC(szLayFile));

		wcscat(szWinPath, szLayFile);
		layoutHandle = CreateFilePep(szWinPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(layoutHandle);
	}

	/* ----- set file name to PEP title ----- */
	LoadString(hResourceDll, IDS_FILE_NAME_UNTITLED, szMsg, PEP_STRING_LEN_MAC(szMsg));
	PepSetFileName(hwnd, szMsg);

	PepResetModFlag(PEP_MF_ALL, 0);     /* reset change flag    */
	PepSetModFlag(hwnd, PEP_MF_OPEN, 0);      /* set open flag        */
	PluInitialize();
        
    return TRUE;
}

/* ===== End of FILENEW.C ===== */
