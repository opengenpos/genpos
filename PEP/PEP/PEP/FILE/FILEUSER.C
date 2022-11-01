/*
* ---------------------------------------------------------------------------
* Title         :   File Function for User
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILEUSER.C
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following fuction
*               FileConfigGet():    get configuration
*               FileTempDelete():   deletes temporary files
* ---------------------------------------------------------------------------
* Update Histories
*
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
*       Include File Declarations
* ===========================================================================
*/
#include    <windows.h>

#include    <ecr.h>
#include    <r20_pif.h>
#include    <plu.h>

#include    "pepcomm.h"
#include    "file.h"
#include    "filel.h"

/************************************************************\
|*                                                          *|
|*  Synopsis:   FCONFIG const FAR *FileConfigGet(VOID)      *|
|*                                                          *|
|*  Return:     pointer of configuration                    *|
|*                                                          *|
|* Description: This function gets the pointer of           *|
|*              configuration                               *|
|*                                                          *|
\************************************************************/
FCONFIG const FAR *FileConfigGet(VOID)
{
    return (&Fconfig);
}

/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL FileTempDelete(HANDLE hInst)           *|
|*              hInst   PEP windows instance                *|
|*                                                          *|
|*  Return:     TRUE:   success                             *|
|*                                                          *|
|* Description: This function deletess temporary files.     *|
|*                                                          *|
\************************************************************/
BOOL FileTempDelete(HANDLE hInst)
{
    CHAR    i;
    WCHAR    szFileName[FILE_MAXLEN_TEXT];
    UINT    nID;

    /* ----- Finalize mass memory module, Saratoga ----- */

    PluFinalize();

    for (i = 0, nID = IDS_FILE_NAME_DEPT; i < FILE_NUM_ALLFILE_V34; i++, nID++) {
        LoadString(hResourceDll, nID, szFileName, PEP_STRING_LEN_MAC(szFileName));
        PifDeleteFile(szFileName);
    }

	nID = IDS_FILE_NAME_RSN;
    LoadString(hResourceDll, nID, szFileName, PEP_STRING_LEN_MAC(szFileName));
    PifDeleteFile(szFileName);

	for(i = 0, nID = IDS_FILE_NAME_PLUINDEX; i < 2; i++, nID++) {
        LoadString(hResourceDll, nID, szFileName, PEP_STRING_LEN_MAC(szFileName));
        PifDeleteFile(szFileName);
    }

	//delete the fingerprint database file as well
	LoadString(hResourceDll, IDS_FILE_NAME_FPDB, szFileName, PEP_STRING_LEN_MAC(szFileName));
	PifDeleteFile(szFileName);

	PifDeleteFile(WIDE("PARAMIDX2"));
	PifDeleteFile(WIDE("PARAMTEMP0")); /* ESMITH Removal of TempFile */

	/* Delete Layout Files ESMITH LAYOUT */
	for( nID = IDS_FILE_NAME_TOUCH_LAYOUTFILE; nID <= IDS_FILE_NAME_LOGOFILEA; nID++)
	{
        LoadString(hResourceDll, nID, szFileName, PEP_STRING_LEN_MAC(szFileName));
        PifDeleteFile(szFileName);
	}

    /* ----- Initialize mass memory module, Saratoga ----- */

    PluInitialize();  // RJC added
    return TRUE;
}

/* ===== End of FILEUSER.C ===== */
