#if defined(POSSIBLE_DEAD_CODE)
// This file is unused and a candidate for deletion.
/*
* ---------------------------------------------------------------------------
* Title         :   Common Routines for Loader Board Communication
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRALCOMM.C
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
#include <stdlib.h>
#include <string.h>

#include <ecr.h>
#include <pif.h>

#include "file.h"
#include "trans.h"
#include "transl.h"
#include "traload.h"
#include "tralcomm.h"

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
*   Public Area
*===========================================================================
*/
TRALOADERIDX    aTransLoaderIndex[TRANS_LOADER_COMM_MAX_FILE + TRANS_LOADER_COMM_RESERVED];
WCHAR   *pszTransName[TRANS_LOADER_COMM_NO_FILE] = {WIDE("APPLPARA"),
                                                   WIDE("PARAMDEP"),
                                                   WIDE("PARAMPLU"),
                                                   WIDE("PARAMWAI"),
                                                   WIDE("PARAMCAS"),
                                                   WIDE("PARAMETK"),
                                                   WIDE("PARAMCPN"),
                                                   WIDE("PARAMCST"),
                                                   WIDE("PARAMRPT"),
                                                   WIDE("PARAMPPI"),
                                                   WIDE("PARAMMLD")};
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
** Synopsis:    SHORT TransLoaderCommOpen(HWND hDlg)
*               hDlg:   the handle of window
*
** Return:      >= 0    handle 
*               < 0     error ocuurs
*
** Description: This function open serial port connected with loader board
*===========================================================================
*/
SHORT TransLoaderCommOpen(HWND hDlg)
{
    PROTOCOL    Protocol;
    LRESULT     lPos;

    /* set protocol     */
    Protocol.fPip = PIF_COM_PROTOCOL_YAM;

    /* set baud rate    */
    lPos = SendDlgItemMessage(hDlg,
                              IDD_TRANS_LOADER_BAUD,
                              CB_GETCURSEL,
                              0,
                              0L);

    Protocol.usComBaud = (USHORT)atoi(szTransBaud[(WORD)lPos]);

    /* set format       */
    Protocol.uchComByteFormat = 0x03;

    /* get port number  */
    lPos = SendDlgItemMessage(hDlg, IDD_TRANS_LOADER_PORT, CB_GETCURSEL, 0, 0L);
    lPos += 1;
        
    return(PifOpenCom((USHORT)lPos, &Protocol));
}
    
/*
*===========================================================================
** Synopsis:    VOID TransLoaderCommClose(USHORT usPort, HANDLE hMem)
*               hPort:  the handle of serial port
*               hMem:   the handle of global memory
*
** Return:      none 
*
** Description: This function unlocks and frees global memory, and 
*               close serial port.
*===========================================================================
*/
VOID TransLoaderCommClose(USHORT usPort, HANDLE hMem)
{
    /* unlock and free global memory    */
    GlobalUnlock(hMem);
    GlobalFree(hMem);

    /* close serial port    */
    PifCloseCom(usPort);
}

/*
*===========================================================================
** Synopsis:    BOOL TransLoaderCommControl(USHORT usPort)
*
** Return:      TRUE:   success
*               FALSE:  failure 
*
** Description: This function sets timeout value
*===========================================================================
*/
BOOL TransLoaderCommControl(USHORT usPort)
{
    /* set timeout value to receive */
    if (PifControlCom(usPort,
                      PIF_YAM_RECEIVE_TIMER,
                      TRANS_LOADER_COMM_TIMEOUT) == PIF_OK) {

        /* set timeout value to send    */
        if (PifControlCom(usPort,
                          PIF_YAM_SEND_TIMER,
                          TRANS_LOADER_COMM_TIMEOUT) == PIF_OK) {
            return TRUE;
        }
    }

    /* close serial port    */
    PifCloseCom(usPort);

    return FALSE;
}

/*
*===========================================================================
** Synopsis:    SHORT TransLoaderCommCheckFile(CHAR FAR *pszFileName)
*               pszFileName     the pointer of file name
*
** Return:      0 >:    valid file
*               0:      application parameter
*               0 <:    invalid file (ex: total files) 
*
** Description: This function checkss file name.
*===========================================================================
*/
SHORT TransLoaderCommCheckFile(CHAR FAR *pszFileName)
{
    CHAR    i;

    for (i = 0; i < TRANS_LOADER_COMM_NO_FILE; i++) {

        /* check temporary file */
        if (strcmp(pszFileName, pszTransName[i]) == 0) {
            
            if (i == TRANS_LOADER_COMM_APPL) {       /* application parameter      */
                return((SHORT)i);
            } else if (i == TRANS_LOADER_COMM_DEPT) {   /* dept file               */
                return((SHORT)(Trans.fwTransLoader & TRANS_UPDATE_DEPT ? i : -1));
            } else if (i == TRANS_LOADER_COMM_PLU) {    /* PLU regular file        */
                return((SHORT)(Trans.fwTransLoader & TRANS_UPDATE_PLU ? i : -1));
            } else if (i == TRANS_LOADER_COMM_WAI) {    /* server file             */
                return((SHORT)(Trans.fwTransLoader & TRANS_UPDATE_WAITER ? i : -1));
            } else if (i == TRANS_LOADER_COMM_CAS) {    /* cashier file            */
                return((SHORT)(Trans.fwTransLoader & TRANS_UPDATE_CASHIER ? i : -1));
            } else if (i == TRANS_LOADER_COMM_CPN) {    /* Combination Coupon file */
                return((SHORT)(Trans.fwTransLoader & TRANS_UPDATE_CPN ? i : -1));
            } else if (i == TRANS_LOADER_COMM_CSTR) {   /* Control String file     */
                return((SHORT)(Trans.fwTransLoader & TRANS_UPDATE_CSTR ? i : -1));
            } else if (i == TRANS_LOADER_COMM_RPT) {   /* Control String file     */
                return((SHORT)(Trans.fwTransLoader & TRANS_UPDATE_RPT ? i : -1));
            } else if (i == TRANS_LOADER_COMM_PPI) {   /* Control String file     */
                return((SHORT)(Trans.fwTransLoader & TRANS_UPDATE_PPI ? i : -1));
            } else if (i == TRANS_LOADER_COMM_MLD) {   /* MLD file, V3.3           */
                return((SHORT)(Trans.fwTransLoader & TRANS_UPDATE_MLD ? i : -1));
            } else {                                    /* ETK file     */
                return((SHORT)(Trans.fwTransLoader & TRANS_UPDATE_ETK ? i : -1));
            }
        }
    }

    return(-1);
}

/*
*===========================================================================
** Synopsis:    VOID TransLoaderCommSetMessage(HWND hDlg, CHAR FAR *pszFileName
*                                          CHAR chFlag)
*               hDlg            the handle of window
*               pszFileName     the pointer of file name
*               chFlag          "send" or "receive"
*
** Return:      none
*
** Description: This function sets and outputs message.
*===========================================================================
*/
VOID TransLoaderCommSetMessage(HWND hDlg, CHAR FAR *pszFileName, CHAR chFlag)
{
    WORD    wId;

    if (lstrcmp(pszFileName, pszTransName[TRANS_LOADER_COMM_DEPT]) == 0) {

        if (chFlag == TRANS_LOADER_COMM_LOAD) {
            wId = IDS_TRANS_LOADER_LOADDEPT;
        } else {
            wId = IDS_TRANS_LOADER_SAVEDEPT;
        }
    } else if (lstrcmp(pszFileName, pszTransName[TRANS_LOADER_COMM_PLU]) == 0) {

        if (chFlag == TRANS_LOADER_COMM_LOAD) {
            wId = IDS_TRANS_LOADER_LOADPLU;
        } else {
            wId = IDS_TRANS_LOADER_SAVEPLU;
        }

    } else if (lstrcmp(pszFileName, pszTransName[TRANS_LOADER_COMM_WAI]) == 0) {

        if (chFlag == TRANS_LOADER_COMM_LOAD) {
            wId = IDS_TRANS_LOADER_LOADWAI;
        } else {
            wId = IDS_TRANS_LOADER_SAVEWAI;
        }

    } else if (lstrcmp(pszFileName, pszTransName[TRANS_LOADER_COMM_CAS]) == 0) {

        if (chFlag == TRANS_LOADER_COMM_LOAD) {
            wId = IDS_TRANS_LOADER_LOADCAS;
        } else {
            wId = IDS_TRANS_LOADER_SAVECAS;
        }

    } else if (lstrcmp(pszFileName, pszTransName[TRANS_LOADER_COMM_CPN]) == 0) {

        if (chFlag == TRANS_LOADER_COMM_LOAD) {
            wId = IDS_TRANS_LOADER_LOADCPN;
        } else {
            wId = IDS_TRANS_LOADER_SAVECPN;
        }

    } else if (lstrcmp(pszFileName, pszTransName[TRANS_LOADER_COMM_CSTR]) == 0) {

        if (chFlag == TRANS_LOADER_COMM_LOAD) {
            wId = IDS_TRANS_LOADER_LOADCSTR;
        } else {
            wId = IDS_TRANS_LOADER_SAVECSTR;
        }


    } else if (lstrcmp(pszFileName, pszTransName[TRANS_LOADER_COMM_RPT]) == 0) {

        if (chFlag == TRANS_LOADER_COMM_LOAD) {
            wId = IDS_TRANS_LOADER_LOADRPT;
        } else {
            wId = IDS_TRANS_LOADER_SAVERPT;
        }


    } else if (lstrcmp(pszFileName, pszTransName[TRANS_LOADER_COMM_PPI]) == 0) {

        if (chFlag == TRANS_LOADER_COMM_LOAD) {
            wId = IDS_TRANS_LOADER_LOADPPI;
        } else {
            wId = IDS_TRANS_LOADER_SAVEPPI;
        }


    } else if (lstrcmp(pszFileName, pszTransName[TRANS_LOADER_COMM_MLD]) == 0) {

        if (chFlag == TRANS_LOADER_COMM_LOAD) {
            wId = IDS_TRANS_LOADER_LOADMLD;
        } else {
            wId = IDS_TRANS_LOADER_SAVEMLD;
        }


    } else {

        if (chFlag == TRANS_LOADER_COMM_LOAD) {
            wId = IDS_TRANS_LOADER_LOADETK;
        } else {
            wId = IDS_TRANS_LOADER_SAVEETK;
        }

    }

    TransAbortDlgMessage(hDlg, wId);
}

/* ===== End of TRALCOM.C ===== */
#endif