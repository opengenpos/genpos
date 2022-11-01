/*
* ---------------------------------------------------------------------------
* Title         :   2170 PEP Flag Control Procedures
* Category      :   NCR 2170 PEP for Windows    (Hotel US Model)
* Program Name  :   PEPFLAG.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All right reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Apr-06-95 : 03.00.00 : H.Ishida   : Add R3.0
* Feb-02-96 : 03.00.05 : M.Suzuki   : Add R3.1
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

#include    "pepcomm.h"
#include    "pepflag.h"
#include    "trans.h"

/*
* ===========================================================================
*       Compiler Message Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Public Work Area Declarations
* ===========================================================================
*/
ULONG   nPepChkFlag;        /* checking flag for file control           */ //ESMITH LAYOUT
/*                11111111:11111111
*                 |||||||| |||||||+->     modify setup parameter file
*                 |||||||| ||||||+-->     modify maintenance parameter file
*                 |||||||| |||||+--->     modify dept. file
*                 |||||||| ||||+---->     modify plu file
*                 |||||||| |||+----->     modify server file
*                 |||||||| ||+------>     modify cashier file
*                 |||||||| |+------->     modify ETK file
*                 |||||||| +-------->     modify Combination Coupon file
*                 |||||||+---------->     modify transfer file
*                 ||||||+----------->     configulation file flag
*                 |||||+------------>     open file flag
*                 ||||+------------->     modify Control String file
*                 |||+-------------->     modify Prog. Report file
*                 ||+--------------->     modify PPI file
*                 |+---------------->     modify MLD file
*                 +----------------->     not used
*/

/*
* ===========================================================================
*       Static Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/


/*
* ===========================================================================
**  Synopsis:   void PEPENTRY PepSetModFlag()
*
**  Input   :   HWND    hWnd        - handle of window
*               UINT    nModFileID  - modification file identifier
*               UINT    nReserved   - reserved area (must set 0)
**
*   Return  :   None
*
**  Description:
*       This procedure sets a PEP File Status flag to modification.
*           nModFileID
*               PEP_MF_PROG     - Parameter (setup) file ID
*               PEP_MF_ACT      - Parameter (maintenance) file ID
*               PEP_MF_DEPT     - Department file ID
*               PEP_MF_PLU      - PLU regular file ID
*               PEP_MF_SERV     - Server file ID
*               PEP_MF_CASH     - Cashier file ID
*               PEP_MF_ETK      - ETK file ID ===> Dropped 7/23/93
*               PEP_MF_CPN      - Combination Coupon file ID
*               PEP_MF_CSTR     - Control String file ID
*               PEP_MF_RPT      - Programable Report file ID
*               PEP_MF_PPI      - PPI file ID
*               PEP_MF_MLD      - MLD file ID
*               PEP_MF_CONF     - Configulation file
*               PEP_MF_OPEN     - Open file
*               PEP_MF_ALL      - All files w/o PEP_MF_OPEN
* ===========================================================================
*/
//void PEPENTRY PepSetModFlag(HWND hWnd, UINT nModFileID, UINT nReserved)
void PEPENTRY PepSetModFlag(HWND hWnd, ULONG nModFileID, UINT nReserved)
{
    DWORD    fwData = 0;

    if (nModFileID == PEP_MF_ALL) {
        nPepChkFlag |= (PEP_MF_PROG | PEP_MF_ACT | PEP_MF_DEPT | PEP_MF_PLU
                        | PEP_MF_SERV | PEP_MF_CASH | PEP_MF_ETK
                        | PEP_MF_CONF | PEP_MF_TRANS | PEP_MF_CPN | PEP_MF_CSTR
                        | PEP_MF_RPT  | PEP_MF_PPI | PEP_MF_MLD | PEP_MF_LAYTOUCH 
						| PEP_MF_LAYKEYBRD | PEP_MF_LOGO | PEP_MF_ICON);
    } else {
        nPepChkFlag |= nModFileID;      /* set modification status */
    }
    nReserved = nReserved;              /* erase a warning message */

    if (nModFileID & PEP_MF_PROG) {
        fwData |= TRANS_ACCESS_PROG;
    }
    if (nModFileID & PEP_MF_ACT) {
        fwData |= TRANS_ACCESS_ACT;
    }
    if (nModFileID & PEP_MF_DEPT) {
        fwData |= TRANS_ACCESS_DEPT;
    }
    if (nModFileID & PEP_MF_PLU) {
        fwData |= TRANS_ACCESS_PLU;
    }
    if (nModFileID & PEP_MF_SERV) {
        fwData |= TRANS_ACCESS_WAITER;
    }
    if (nModFileID & PEP_MF_CASH) {
        fwData |= TRANS_ACCESS_CASHIER;
    }
    if (nModFileID & PEP_MF_ETK) {
        fwData |= TRANS_ACCESS_ETK;
    }
    if (nModFileID & PEP_MF_CPN) {
        fwData |= TRANS_ACCESS_CPN;
    }
    if (nModFileID & PEP_MF_CSTR) {
        fwData |= TRANS_ACCESS_CSTR;
    }
    if (nModFileID & PEP_MF_RPT) {
        fwData |= TRANS_ACCESS_RPT;
    }
    if (nModFileID & PEP_MF_PPI) {
        fwData |= TRANS_ACCESS_PPI;
    }
    if (nModFileID & PEP_MF_MLD) {
        fwData |= TRANS_ACCESS_MLD;
    }
	if (nModFileID & PEP_MF_LAYTOUCH) { //ESMITH LAYOUT
		fwData |= TRANS_ACCESS_LAYTOUCH;
	}
	if (nModFileID & PEP_MF_LAYKEYBRD) {
		fwData |= TRANS_ACCESS_LAYKEYBRD;
	}
	if (nModFileID & PEP_MF_LOGO) {
		fwData |= TRANS_ACCESS_LOGO;
	}
	if (nModFileID & PEP_MF_ICON) {
		fwData |= TRANS_ACCESS_ICON;
	}

    SendMessage(hWnd, PM_TRA_ACCESS, fwData, 0L);
}

/*
* ===========================================================================
**  Synopsis:   void PEPENTRY PepResetModFlag()
*
*   Input   :   UINT    nModFileID  - modification file identifier
*               UINT    nReserved   - reserved area (must set 0)
**
*   Return  :   None
*
**  Description:
*       This procedure resets a PEP File Status flag to modification.
*           nModFileID : See "PepSetModFlag" function
* ===========================================================================
*/
void PEPENTRY PepResetModFlag(UINT nModFileID, UINT nReserved)
{
    if (nModFileID == PEP_MF_ALL) {
        nPepChkFlag &= ~(PEP_MF_PROG | PEP_MF_ACT | PEP_MF_DEPT | PEP_MF_PLU
                        | PEP_MF_SERV | PEP_MF_CASH | PEP_MF_ETK
                        | PEP_MF_CONF | PEP_MF_TRANS| PEP_MF_CPN | PEP_MF_CSTR
                        | PEP_MF_RPT  | PEP_MF_PPI | PEP_MF_MLD);
    } else {
        nPepChkFlag &= ~nModFileID;     /* reset modification status */
    }
    nReserved = nReserved;              /* erase a warning message */
}

/*
* ===========================================================================
**  Synopsis:   UINT PEPENTRY PepQueryModFlag()
*
*   Input   :   UINT    nChkFileID  - checking file identifier
*               UINT    nReserved   - reserved area (must set 0)
**
*   Return  :   TRUE                - a specified file is changed
*               FLASE               - a specified file is not changed yet
*
**  Description:
*       This procedure queries a File Modification Flag Status.
*           nModFileID : See "PepSetModFlag" function
* ===========================================================================
*/
UINT PEPENTRY PepQueryModFlag(UINT nChkFileID, UINT nReserved)
{
    UINT    nRet = FALSE;

    if (nPepChkFlag & nChkFileID) {     /* check a specified file */
        nRet = TRUE;                    /* if modification, return TRUE */
    }
    nReserved = nReserved;              /* erase a warning message */
    return (nRet);
}

/* ===== End of PEPFLAG.C ===== */
