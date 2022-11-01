/*
* ---------------------------------------------------------------------------
* Title         :   Select Files for Terminal Communication
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRATSEL.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
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
#include "traterm.h"

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
** Synopsis:    BOOL TransTermSelectFilesSet(HWND hDlg)
*
*     Input:    hDlg    - Handle of Dialog Procedure
*
**   Return:    None
*
** Description: This function set file selection for terminal communication.
*===========================================================================
*/
BOOL    TransTermSelectFilesSet(HWND hDlg)
{
    short   nCount;
    WORD    fwTrans;
    WORD    fwUpdate;
    BOOL    fCtrl = FALSE;

	/*RPH 12/09/02 SR#77 flag to enable/disable Mark Changed Button*/
	BOOL	markChangedFlg = FALSE;

    /* Store transfer flag to buffer */
    fwTrans  = getTransFwTransTerm();
    fwUpdate = getTransFwUpdate();

    /* Set Transfer Check Box */
    for (nCount = IDD_TRANS_TERM_TRA_PROG;
         nCount <= IDD_TRANS_TERM_TRA_ICON; nCount++) { //ESMITH LAYOUT
        if (fwTrans & TRANS_ACCESS_MASK) {
			if (getTermByTerm() > TRANS_TERM_MASTER_NUM && (nCount == IDD_TRANS_TERM_TRA_PLU || nCount == IDD_TRANS_TERM_TRA_WAI || nCount == IDD_TRANS_TERM_TRA_CAS || nCount == IDD_TRANS_TERM_TRA_ETK 
							|| nCount == IDD_TRANS_TERM_TRA_RPT))   /* terminal is satellite and the checkbox that cannot be selected*/
			 {   SendDlgItemMessage(hDlg, nCount, BM_SETCHECK, FALSE, 0L); }
			 else{
				 SendDlgItemMessage(hDlg, nCount, BM_SETCHECK, TRUE, 0L);
				 fCtrl |= TRUE;
			 }
        } else {
            SendDlgItemMessage(hDlg, nCount, BM_SETCHECK, FALSE, 0L);
        }
        fwTrans >>= 1;                  /* 1 Bit Right Shift */
    }

    /* Set Updated Edit Control */
    for (nCount = IDD_TRANS_TERM_CHG_PROG;
		nCount <= IDD_TRANS_TERM_CHG_ICON; nCount++)
	{ //ESMITH LAYOUT
        if (fwUpdate & TRANS_ACCESS_MASK) {
            SetDlgItemText(hDlg, nCount, L"X");

			/*RPH 12/09/02 SR#77 Flag to Enable Mark Changed Button*/
			markChangedFlg = TRUE;
        } else {
            SetDlgItemText(hDlg, nCount, NULL);
        }
        fwUpdate >>= 1;                 /* 1 Bit Right Shift */
    }

	/*RPH 12/09/02 SR#77 Enable/Disable Mark Changed Button*/
	EnableWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_SET_CHANGED), markChangedFlg);

    return (fCtrl);
}

/*
*===========================================================================
** Synopsis:    BOOL TransTermSelectFilesGet(HWND hDlg)
*
*     Input:    hDlg    - Handle of Dialog Procedure
*
**   Return:    None
*
** Description: This function get file selection for terminal communication.
*===========================================================================
*/
BOOL    TransTermSelectFilesGet(HWND hDlg)
{
    short   nCount;
    WORD    fwTrans;
    WORD    fwOld;
    BOOL    fCtrl = FALSE;

    /* Store transfer flag to buffer */
    fwOld = getTransFwTransTerm();

    fwTrans = TRANS_ACCESS_PROG;
    for (nCount = IDD_TRANS_TERM_TRA_PROG;
         nCount <= IDD_TRANS_TERM_TRA_ICON; nCount++) {  //ESMITH LAYOUT
		//only add checkboxes to list to be sent if they are allowed to be sent. IE the files that cannot be sent 
		//to a satellite terminal will not be added to the list to be send
		if (getTermByTerm() < TRANS_TERM_MASTER_NUM || (nCount != IDD_TRANS_TERM_TRA_PLU || nCount != IDD_TRANS_TERM_TRA_WAI || nCount != IDD_TRANS_TERM_TRA_CAS || nCount != IDD_TRANS_TERM_TRA_ETK 
							|| nCount != IDD_TRANS_TERM_TRA_RPT)){
		 /* Get Transfer Check Box */
			if (SendDlgItemMessage(hDlg,
				                   nCount,
			                       BM_GETCHECK,
					               0,
						           0L)) {
			orTransFwTransTerm(fwTrans);
			fCtrl |= TRUE;
			} else {
				andTransFwTransTerm(~fwTrans);
			}
			fwTrans <<= 1;              /* 1 Bit Left Shift */
		}
    }

    if (memcmp(getPtrTransFwTransTerm(), &fwOld, sizeof(fwOld))) {
        PepSetModFlag(hwndPepMain, PEP_MF_TRANS, 0);
    }

    return (fCtrl);
}

/*
*===========================================================================
** Synopsis:    BOOL TransTermSelectFilesEnable(HWND hDlg)
*
*     Input:    hDlg    - Handle of Dialog Procedure
*
**   Return:    None
*
** Description: This function enable/disable file selection.
*===========================================================================
*/
BOOL    TransTermSelectFilesEnable(HWND hDlg)
{
    BOOL    fCtrl;

    if (getTermByTerm() < TRANS_TERM_MASTER_NUM) {  /* terminal is master */
        fCtrl = TRUE;
    } else {                                       /* terminal is sattelite */
        fCtrl = FALSE;
		//uncheck the boxes that will be hidden
		SendDlgItemMessage(hDlg, IDD_TRANS_TERM_TRA_PLU, BM_SETCHECK, FALSE, 0L);
		SendDlgItemMessage(hDlg, IDD_TRANS_TERM_TRA_WAI, BM_SETCHECK, FALSE, 0L);
		SendDlgItemMessage(hDlg, IDD_TRANS_TERM_TRA_CAS, BM_SETCHECK, FALSE, 0L);
		SendDlgItemMessage(hDlg, IDD_TRANS_TERM_TRA_ETK, BM_SETCHECK, FALSE, 0L);
		SendDlgItemMessage(hDlg, IDD_TRANS_TERM_TRA_RPT, BM_SETCHECK, FALSE, 0L);
    }

    /* ----- control select files checkbox ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_TRA_PLU), fCtrl);
    EnableWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_CHG_PLU), fCtrl);
	EnableWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_TRA_WAI), fCtrl);
    EnableWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_CHG_WAI), fCtrl);
    EnableWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_TRA_CAS), fCtrl);
    EnableWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_CHG_CAS), fCtrl);
    EnableWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_TRA_ETK), fCtrl);
    EnableWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_CHG_ETK), fCtrl);
    EnableWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_TRA_RPT), fCtrl);
    EnableWindow(GetDlgItem(hDlg, IDD_TRANS_TERM_CHG_RPT), fCtrl);



	
    return (fCtrl);
}

/* ===== End of TRATSEL.C ===== */
