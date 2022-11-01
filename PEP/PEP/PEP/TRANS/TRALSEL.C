#if defined(POSSIBLE_DEAD_CODE)
// This file is unused and a candidate for deletion.
/*
* ---------------------------------------------------------------------------
* Title         :   Select Files for Loader Board Communication
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRALSEL.C
* Copyright (C) :   1995, NCR Corp. E&M-OISO, All rights reserved.
* --------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /c /AL /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:     This file describes the dialog box where the transfer
*               file is selected.
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date          Ver.       Name             Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Apr-06-95 : 03.00.00 : H.Ishida   : Add R3.0
* Feb-07-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Oct-15-98 : 03.03.00 : O.Nakada   : Add V3.3
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
*   Include File Declarations
*===========================================================================
*/
#include <windows.h>
#include <string.h>

#include "ecr.h"
#include "pep.h"
#include "pepflag.h"
#include "trans.h"
#include "transl.h"
#include "traload.h"

/*                                                                                
*===========================================================================
*   Compile Message Declarations
*===========================================================================
*/
/*                                                                                
*===========================================================================
*   Work Area Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Public Work Area
*===========================================================================
*/
/*
*===========================================================================
*   Static Work Area 
*===========================================================================
*/
/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:   BOOL TransLoaderSelectFilesSet(HWND hDlg)

*     Input:    hDlg    - Handle of Dialog Procedure
*
** Return:      None
*
** Description: This function set file selection for loader board communication.
*===========================================================================
*/
BOOL    TransLoaderSelectFilesSet(HWND hDlg)
{
    short   nCount;
    WORD    fwTrans;
    WORD    fwUpdate;
    BOOL    fCtrl = FALSE;

    /* Store transfer flag to buffer */
    fwTrans  = Trans.fwTransLoader;
    fwUpdate = Trans.fwUpdate;

    /* Set Transfer Check Box */
    if ((fwTrans & TRANS_ACCESS_MASK) || (fwTrans & TRANS_ACCESS_MASK2)) {
        SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_TRA_PARA, BM_SETCHECK, TRUE, 0L);
        fCtrl |= TRUE;
    } else {
        SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_TRA_PARA, BM_SETCHECK, FALSE, 0L);
    }
    fwTrans >>= 2;                  /* 1 Bit Right Shift */
    for (nCount = IDD_TRANS_LOADER_TRA_DEPT;
         nCount <= IDD_TRANS_LOADER_TRA_MLD; nCount++) {
        if (fwTrans & TRANS_ACCESS_MASK) {
            SendDlgItemMessage(hDlg, nCount, BM_SETCHECK, TRUE, 0L);
            fCtrl |= TRUE;
        } else {
            SendDlgItemMessage(hDlg, nCount, BM_SETCHECK, FALSE, 0L);
        }
        fwTrans >>= 1;                  /* 1 Bit Right Shift */
    }

    /* Set Updated Edit Control */
    if ((fwUpdate & TRANS_ACCESS_MASK) || (fwUpdate & TRANS_ACCESS_MASK2)) {
        SetDlgItemText(hDlg, IDD_TRANS_LOADER_CHG_PARA, "X");
    } else {
        SetDlgItemText(hDlg, IDD_TRANS_LOADER_CHG_PARA, NULL);
    }
    fwUpdate >>= 2;                 /* 2 Bit Right Shift */
    for (nCount = IDD_TRANS_LOADER_CHG_DEPT;
         nCount <= IDD_TRANS_LOADER_CHG_MLD; nCount++) {
        if (fwUpdate & TRANS_ACCESS_MASK) {
            SetDlgItemText(hDlg, nCount, "X");
        } else {
            SetDlgItemText(hDlg, nCount, NULL);
        }
        fwUpdate >>= 1;                 /* 1 Bit Right Shift */
    }

    return (fCtrl);
}

/*
*===========================================================================
** Synopsis:    BOOL TransLoaderSelectFilesGet(HWND hDlg)

*     Input:    hDlg    - Handle of Dialog Procedure
*
** Return:      None
*
** Description: This function get file selection for loader board communication.
*===========================================================================
*/
BOOL    TransLoaderSelectFilesGet(HWND hDlg)
{
    short   nCount;
    WORD    fwTrans;
    WORD    fwOld;
    BOOL    fCtrl = FALSE;

    /* Store transfer flag to buffer */
    fwOld = Trans.fwTransLoader;

    /* Get Transfer Check Box */
    if (SendDlgItemMessage(hDlg,
                           IDD_TRANS_LOADER_TRA_PARA,
                           BM_GETCHECK,
                           0,
                           0L)) {
        Trans.fwTransLoader |= TRANS_ACCESS_PARA;
        fCtrl |= TRUE;
    } else {
        Trans.fwTransLoader &= ~TRANS_ACCESS_PARA;
    }
    fwTrans = TRANS_ACCESS_DEPT;
    for (nCount = IDD_TRANS_LOADER_TRA_DEPT;
         nCount <= IDD_TRANS_LOADER_TRA_MLD; nCount++) {

        /* Get Transfer Check Box */
        if (SendDlgItemMessage(hDlg,
                               nCount,
                               BM_GETCHECK,
                               0,
                               0L)) {
            Trans.fwTransLoader |= fwTrans;
            fCtrl |= TRUE;
        } else {
            Trans.fwTransLoader &= ~fwTrans;
        }
        fwTrans <<= 1;              /* 1 Bit Left Shift */
    }

    if (memcmp(&Trans.fwTransLoader, &fwOld, sizeof(fwOld))) {
        PepSetModFlag(hwndPepMain, PEP_MF_TRANS, 0);
    }

    return (fCtrl);
}

/* ===== End of TRALSEL.C ===== */
#endif