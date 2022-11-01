/*
* ---------------------------------------------------------------------------
* Title         :   Header File of Loader Board Communication
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRALOAD.H
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
#define _INC_TRANSFER_LOADER

#define TRANS_LOADER_MAX_BAUD       2       /* Baud Rate of Loader Board    */

#define TRANS_LOADER_OK             1       /* success                      */
#define TRANS_LOADER_ABORT          0       /* aborted                      */
#define TRANS_LOADER_FAIL           -1      /* failure                      */

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

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
/* ===== TRALOAD.C ===== */
void TransLoaderControlButton(HWND);

/* ===== TRALSEL.C ===== */
BOOL TransLoaderSelectFilesSet(HWND);
BOOL TransLoaderSelectFilesGet(HWND);

/* ===== End of TRALOAD.H ===== */
