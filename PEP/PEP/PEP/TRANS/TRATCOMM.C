/*
* ---------------------------------------------------------------------------
* Title         :   Common Routines for Terminal Communication
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRATCOMM.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* --------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /c /AL /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:     As for this file, the function, which transmits the
*               parameter and the file to 2170, is defined.
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date          Ver.       Name             Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Mar-14-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-25-98 : 03.03.00 : A.Mitsui   : Add V3.3
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
#include <stdlib.h>
#include <stdio.h>

#include <ecr.h>
#include <cscas.h>
#include <pcstbcas.h>
#include <isp.h>
#include <pcstbisp.h>
#include <pcif.h>
#include <r20_pif.h>
#include <nb.h>

#include "trans.h"
#include "transl.h"
#include "traterm.h"
#include "tratcomm.h"
#include "AW_Interfaces.h"
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
** Synopsis:    short TransTermCommPortOpen(HWND hWnd)
*     Input:    hDlg - handle of parent window
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description: This function opens serial port so that the forwarding may
*               begin.
*===========================================================================
*/
short TransTermCommPortOpen(HWND hWnd)
{
    COMCONTROL  ComOpen;

    /* Set Serial Port */
	ComOpen.uchPort     = getTermByPort();
    ComOpen.usBaudRate  = getTermWBaudRate();
    ComOpen.uchByteSize = getTermByByteSize();
    ComOpen.uchParity   = getTermByParity();
    ComOpen.uchStopBits = getTermByStopBits();
	

    switch (PcifOpen(&ComOpen)) {
    case PCIF_SUCCESS:
        return TRUE;

    case PCIF_ALREADY:
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_SERIAL_ALREADY,
                     szTransPort[getTermByPort() - 1]);
        break;

    default:
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_SERIAL_OPEN);
        break;
    }
    return FALSE;
}

/*
*===========================================================================
** Synopsis:    short TransTermCommPortClose(HWND hWnd)
*     Input:    hDlg - handle of parent window
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description: To end forwarding, this function closes serial port.
*===========================================================================
*/
short TransTermCommPortClose(HWND hWnd)
{
    switch (PcifClose()) {
    case PCIF_SUCCESS:
        return TRUE;

    default:
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_SERIAL_CLOSE);
        break;
    }
    return FALSE;
}


/*
*===========================================================================
** Synopsis:    short TransTermCommModemConnect(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description:  To call, this function transmits the command to the modem.
*===========================================================================
*/
short TransTermCommModemConnect(HWND hWnd)
{
    UCHAR    szBuffer[128];
    SHORT   sStatus;
	int     iLoop;

    TransAbortDlgMessage(hWnd,
                         IDS_TRANS_NOW_CONNECT);

    /* ----- set user status(default) ----- */
    strcpy(szBuffer, "AT");                 /* AT command */
    strcat(szBuffer, "&B1&K1&M4&H1&R2");    /* serial port flow control */
    strcat(szBuffer, "\r\n");               /* end of AT command */
    PcifConnect(szBuffer);         /* send AT commands */

    if (wcslen(TransDirList.szInit)) {      /* user initial strings */
        PifSleep(500);
 		/*
			Convert the WCHAR string to UCHAR since the modem
			only uses UCHAR strings.  Then send the initialization
			string to the modem.
		 */
		for (iLoop = 0; iLoop < sizeof(szBuffer) && TransDirList.szInit[iLoop]; iLoop++) {
			szBuffer[iLoop] = (UCHAR)TransDirList.szInit[iLoop];
		}
		szBuffer[iLoop] = '\0';

        strcat(szBuffer, "\r\n");
        PcifConnect(szBuffer);         /* send AT commands */
    }

    /* ----- call dial ----- */
    PifSleep(500);
    if (TransDirList.fbyStatus & TRANS_TERM_DIR_TYPE_DIAL) {  /* pulse */
        strcpy(szBuffer, "ATDP");
    } else {                                    /* tone */
        strcpy(szBuffer, "ATDT");
    }
 	/*
		Convert the WCHAR string to UCHAR since the modem
		only uses UCHAR strings.  Then send the initialization
		string to the modem.
	 */
	for (iLoop = 0; iLoop < sizeof(szBuffer) && TransDirList.szNumber[iLoop]; iLoop++) {
		szBuffer[iLoop+4] = (UCHAR)TransDirList.szNumber[iLoop];
	}
	szBuffer[iLoop+4] = '\0';
    strcat(szBuffer, "\r\n");                   /* end of AT commands */
    if ((sStatus = PcifConnect(szBuffer)) >= 0) {
        switch (sStatus) {
        case PCIF_RESULT_OK:
        case PCIF_RESULT_RING:
        case PCIF_RESULT_NO_CARRIER:
        case PCIF_RESULT_ERROR:
        case PCIF_RESULT_NO_DIAL_TONE:
        case PCIF_RESULT_BUSY:
        case PCIF_RESULT_ANSWER:
            break;

        case PCIF_RESULT_HAND_SET:
            TransMessage(hWnd,
                         MB_ICONEXCLAMATION,
                         MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                         IDS_TRANS_NAME,
                         IDS_TRANS_ERR_HAND_SET);
            return FALSE;

        case PCIF_RESULT_CONNECT:
        default:
            TransAbortDlgMessage(hWnd,
                                 IDS_TRANS_SUC_CONNECT);
            return TRUE;
        }
    }

    TransMessage(hWnd,
                 MB_ICONEXCLAMATION,
                 MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                 IDS_TRANS_NAME,
                 IDS_TRANS_ERR_CONNECT);
    return FALSE;
}

/*
*===========================================================================
** Synopsis:    short TransTermCommModemDisconnect(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description: To cut the telephone, this function transmits the command
*               to the modem.
*===========================================================================
*/
short TransTermCommModemDisconnect(HWND hWnd)
{
    TransAbortDlgMessage(hWnd,
                         IDS_TRANS_NOW_DISCONNECT);

    if (PcifDisconnect() == PCIF_RESULT_NO_CARRIER) {
        TransAbortDlgMessage(hWnd,
                             IDS_TRANS_SUC_DISCONNECT);
        return TRUE;
    } else {
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_DISCONNECT);
        return FALSE;
    }
}

/*
*===========================================================================
** Synopsis:    short TransTermCommLogon(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description: This function is logged on to the terminal so that
*               forwarding the data may begin.
*===========================================================================
*/
short TransTermCommLogon(HWND hWnd)
{
    char    achIP[TRANS_TERM_COMM_IP_LEN] =
                {(char)0xC0, (char)0x00, (char)0x00, (char)0x01};

    TransAbortDlgMessage(hWnd,
                         IDS_TRANS_NOW_LOGON);
    if (getTermByTerm() < TRANS_TERM_MASTER_NUM) {  /* terminal is master */
        achIP[TRANS_TERM_COMM_IP_LEN - 1] = (char)(getTermByTerm() + 1);
    } else {                                       /* terminal is sattelite */
        achIP[TRANS_TERM_COMM_IP_LEN - 1] = getTermByTerm();
    }

    switch (IspLogOn((UCHAR *)achIP,
                     TransDirList.szPassword)) {
    case ISP_SUCCESS:
        TransAbortDlgMessage(hWnd,
                             IDS_TRANS_SUC_LOGON);
        return TRUE;

    case ISP_ERR_NOTMATCH_PASSWORD:
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_LOGON_PASS);
        return FALSE;

    case ISP_ERR_NOT_MASTER:
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_LOGON_MASTER);
        return FALSE;

    default:
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_LOGON);
        return FALSE;
    }
}

/*
*===========================================================================
** Synopsis:    short TransTermCommLanLogon(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description: This function is logged on to the terminal so that
*               forwarding the data may begin.
*===========================================================================
*/
short TransTermCommLanLogon(HWND hWnd)
{
    char    achIP[TRANS_TERM_COMM_IP_LEN] =
                {(char)0xC0, (char)0x00, (char)0x00, (char)0x01};
	UCHAR   auchIP[TRANS_TERM_COMM_IP_LEN];
	int		awchIP[TRANS_TERM_COMM_IP_LEN];	//swscanf outputs in 4bytes at a time (WCHAR was too small)
    WCHAR   szIPAddress[TRANS_CONVERT_WORK_SIZE];
    USHORT  usDhcp;
			// each name assumed to be max of 16 characters plus 2 padding. 16 Terminals plus 1 padding
	WCHAR   szPCNameKeys[18 * 17];
	WCHAR   *pKeys = 0;
	SHORT   sRetStatus;
	USHORT  unI1;
	WCHAR   szHostFileName[256] = { 0 };

	wcscat(szHostFileName, szPepAppDataFolder);
	wcscat(szHostFileName, szTransIPFileName);

    TransAbortDlgMessage(hWnd,
                         IDS_TRANS_NOW_LOGON);
    if (getTermByTerm() < TRANS_TERM_MASTER_NUM) {  /* terminal is master */
        achIP[TRANS_TERM_COMM_IP_LEN - 1] = (char)(getTermByTerm() + 1);
    } else {                                       /* terminal is sattelite */
        achIP[TRANS_TERM_COMM_IP_LEN - 1] = getTermByTerm();
    }

    if (GetPrivateProfileStringPep(WIDE("Hosts"),
                                    WIDE("DHCP"),
                                    WIDE(""),
                                    szIPAddress,
                                    sizeof(szIPAddress) / sizeof(szIPAddress[0]),
								szHostFileName)) {
        swscanf(szIPAddress, WIDE("%hd"), &usDhcp);

    } else {

        usDhcp = 1;
    }


	memset (szPCNameKeys, 0, sizeof(szPCNameKeys));

	if (GetPrivateProfileStringPep(WIDE("PCName"),
							NULL,
							WIDE(""),
							szPCNameKeys,
							sizeof(szPCNameKeys) / sizeof(szPCNameKeys[0]),
						szHostFileName))
	{
	}
	else
	{
		pKeys = szPCNameKeys;
		for (unI1 = 0; unI1 < TRANS_MAX_ROW; unI1++)
		{
			wsPepf(pKeys, WIDE("NCRPOS-%d"), unI1+1);
			pKeys += wcslen (pKeys) + 1;
		}
	}
	pKeys = szPCNameKeys;
	for (unI1 = 0; unI1 < achIP[TRANS_TERM_COMM_IP_LEN - 1]; unI1++)
	{
		if (unI1 +1 >= achIP[TRANS_TERM_COMM_IP_LEN - 1])
			break;
		pKeys += wcslen(pKeys) + 1;
	}
	if (usDhcp == 0) {

		/* get IP address from PEPHOSTS.INI */

		if (GetPrivateProfileStringPep(WIDE("PCName"),
									pKeys,
									WIDE(""),
									szIPAddress,
									sizeof(szIPAddress) / sizeof(szIPAddress[0]),
								szHostFileName)) {

			swscanf(szIPAddress, WIDE("%d:%d:%d:%d"), &awchIP[0],
															&awchIP[1],
															&awchIP[2],
															&awchIP[3]);

			auchIP[0] = (UCHAR)awchIP[0];
			auchIP[1] = (UCHAR)awchIP[1];
			auchIP[2] = (UCHAR)awchIP[2];
			auchIP[3] = (UCHAR)awchIP[3];
		}

		sRetStatus = IspLogOn((UCHAR *)auchIP, TransDirList.szPassword);

	}else
	{
		sRetStatus = IspHostLogOn(pKeys, TransDirList.szPassword);
	}


    switch (sRetStatus) {
    case ISP_SUCCESS:
        TransAbortDlgMessage(hWnd,
                             IDS_TRANS_SUC_LOGON);
        return TRUE;

    case ISP_ERR_NOTMATCH_PASSWORD:
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_LOGON_PASS);
        return FALSE;

    case ISP_ERR_NOT_MASTER:
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_LOGON_MASTER);
        return FALSE;

	case ISP_ERR_TIME_OUT:
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_LOGON_TIMEOUT);
		return FALSE;

	case ISP_ERR_NETWORK_NOT_OPEN:
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_LAN_NOEXIST);
		return FALSE;

	case ISP_ERR_HOST_NOT_FOUND:
    default:
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_LOGON);
        return FALSE;
    }
}

/*
*===========================================================================
** Synopsis:    short TransTermCommLogoff(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description: To end forwarding, This function is logged off to the
*               terminal.
*===========================================================================
*/
short TransTermCommLogoff(HWND hWnd)
{
    TransAbortDlgMessage(hWnd,
                         IDS_TRANS_NOW_LOGOFF);

    switch (IspLogOff()) {
    case ISP_SUCCESS:
        TransAbortDlgMessage(hWnd,
                             IDS_TRANS_SUC_LOGOFF);
        return TRUE;

    default:
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_LOGOFF);
        return FALSE;
    }
}

/*
*===========================================================================
** Synopsis:    short TransTermCommLanLogoff(HWND hWnd)
*     Input:    hWnd - handle of parent window
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description: To end forwarding, This function is logged off to the
*               terminal.
*===========================================================================
*/
short TransTermCommLanLogoff(HWND hWnd)
{
    TransAbortDlgMessage(hWnd,
                         IDS_TRANS_NOW_LOGOFF);

    switch (IspLogOff()) {
    case ISP_SUCCESS:
        TransAbortDlgMessage(hWnd,
                             IDS_TRANS_SUC_LOGOFF);
        return TRUE;

    default:
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_LOGOFF);
        return FALSE;
    }
}

/*
*===========================================================================
** Synopsis:    short TransTermCommLock(HWND hWnd)
*
** Return:      TRUE - successful
*               0 <    unsuccessful
*
** Description:
*===========================================================================
*/
short TransTermCommLock(HWND hWnd)
{
    SHORT   sWaiStatus = 0;
    SHORT   sCasStatus = 0;

    TransAbortDlgMessage(hWnd, IDS_TRANS_NOW_LOCK_KEYBOARD);

    sCasStatus = SerCasAllLock();
    if ((sCasStatus != CAS_PERFORM) &&
        (sCasStatus != CAS_FILE_NOT_FOUND)) {
        return TRANS_TERM_COMM_ABORT_LOCK;
    }

/*  V3.3 Removed
    sWaiStatus = SerWaiAllLock();
    if ((sWaiStatus != WAI_PERFORM) &&
        (sWaiStatus != WAI_FILE_NOT_FOUND)) {
        if (sCasStatus == CAS_PERFORM) {
            SerCasAllUnLock();
        }
        return TRANS_TERM_COMM_ABORT_LOCK;
    }
*/

    if (IspLockKeyBoard() != ISP_SUCCESS) {
        if (sCasStatus == CAS_PERFORM) {
            SerCasAllUnLock();
        }
        if (sCasStatus == CAS_PERFORM) {
/*            SerWaiAllUnLock();    */
        }
        return TRANS_TERM_COMM_ABORT_LOCK;
    }
    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransTermCommUnLock(HWND hWnd)
*
** Return:      TRUE - successful
*
** Description:
*===========================================================================
*/
short TransTermCommUnLock(HWND hWnd)
{
    TransAbortDlgMessage(hWnd, IDS_TRANS_NOW_UNLOCK_KEYBOARD);

    IspUnLockKeyBoard();
    SerCasAllUnLock();
/*    SerWaiAllUnLock();    */

    return TRUE;
}


/*
*===========================================================================
** Synopsis:    short TransTermCommBroadcast(HWND hWnd)
*
** Return:      TRUE - successful
*
** Description:
*===========================================================================
*/
short TransTermCommBroadcast(HWND hWnd)
{
    UCHAR   uchNbFlag = 0;

    if (getTermFByStatus() & TRANS_TERM_FLAG_ALL_CAST) {
        uchNbFlag |= NB_REQALLPARA;
    }
    if (getTermFByStatus() & TRANS_TERM_FLAG_SUP_CAST) {
        uchNbFlag |= NB_REQSUPER;
    }
    if (getTermFByStatus() & TRANS_TERM_FLAG_PLU_CAST) {
        uchNbFlag |= NB_REQPLU;
    }
	if (getTermFByStatus() & TRANS_TERM_FLAG_LAY_CAST) {
		uchNbFlag |= NB_REQLAYOUT;
	}//ESMITH LAYOUT
    if (uchNbFlag != 0) {
        TransAbortDlgMessage(hWnd, IDS_TRANS_NOW_BROADCAST);

        IspReqBroadcast(uchNbFlag);
    }

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransTermCommLanOpen(HWND hWnd)
*     Input:    hDlg - handle of parent window
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description: This function opens Lan port so that the forwarding may
*               begin. Saratoga
*===========================================================================
*/
short TransTermCommLanOpen(HWND hWnd)
{
	unsigned int myMsgNo;

    switch (PcifOpenEx(PCIF_FUNC_OPEN_LAN_PORT, 2173)) {
		case PCIF_SUCCESS:
			return TRUE;

		case PCIF_ALREADY:
			TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_LAN_ALREADY,
                     szTransPort[getTermByPort() - 1]);
			break;

		case PCIF_NET_MEMORY_ERR:
			myMsgNo = IDS_TRANS_ERR_LAN_MEMORY;
				break;
		case PCIF_NET_SETUP_PARMS:
			myMsgNo = IDS_TRANS_ERR_LAN_PARMS;
				break;
		case PCIF_NET_NO_NETORK:
			myMsgNo = IDS_TRANS_ERR_LAN_NOEXIST;
				break;
		case PCIF_NET_SOCK_ERR:
			myMsgNo = IDS_TRANS_ERR_LAN_SOCK_ERR;
				break;
		case PCIF_NET_BIND_ERR:
			myMsgNo = IDS_TRANS_ERR_LAN_BIND_ERR;
			break;
		default:
			myMsgNo = IDS_TRANS_ERR_LAN_OPEN;
        break;
    }

	// fall through and display the appropriate message
    TransMessage(hWnd,
                 MB_ICONEXCLAMATION,
                 MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                 IDS_TRANS_NAME,
                 myMsgNo);

    return FALSE;
}

/*
*===========================================================================
** Synopsis:    short TransTermCommLanClose(HWND hWnd)
*     Input:    hDlg - handle of parent window
*
** Return:      TRUE  - successful
*               FALSE - unsuccessful
*
** Description: To end forwarding, this function closes Lan port. Saratogaa
*===========================================================================
*/
short TransTermCommLanClose(HWND hWnd)
{
    switch (PcifCloseEx(PCIF_FUNC_CLOSE_LAN, NULL)) {
    case PCIF_SUCCESS:
        return TRUE;

    default:
        TransMessage(hWnd,
                     MB_ICONEXCLAMATION,
                     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                     IDS_TRANS_NAME,
                     IDS_TRANS_ERR_LAN_CLOSE);
        break;
    }
    return FALSE;
}


/* ===== End of TRATCOMM.C ===== */
