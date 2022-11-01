/*
* ---------------------------------------------------------------------------
* Title         :   Tender Key Setup Dialog  P98
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P002.C
* Copyright (C) :   1995, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* ===========================================================================
*7/13/04				 C.Hattrich : Update to work with MDC settings
* ===========================================================================
*/
/*
* ===========================================================================
*       Include File Declarations
* ===========================================================================
*/
#include    <Windows.h>
#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>

#include    <ecr.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <paraequ.h>
#include	<para.h>
#include    "plu.h"
#include    <para.h>
#include    <csop.h>
#include    <cscas.h>
#include    <csetk.h>
#include    <csprgrpt.h>
#include    <r20_pif.h>
#include    "pep.h"

#include    "prog.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "pepcalc.h"
#include    "file.h"
#include    "trans.h"
#include    "p098.h"
#include    "mypifdefs.h"
#include	"../RES/resource.h"


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

/*the following struct is used to hold the MDC addresses for all 11 tender
options. these addresses are needed to get and set MDC data for the 11 tenders - cdh*/
struct _tagTenderMDCAddr {
	int nAddr1;
	int nAddr2;
	int nAddr3;
} nTenderMDCAddr [] = {
	{ MDC_TEND11_ADR, MDC_TEND12_ADR, MDC_CPTEND1_ADR},			//tender 1
	{ MDC_TEND21_ADR, MDC_TEND22_ADR, MDC_CPTEND2_ADR},			//tender 2
	{ MDC_TEND31_ADR, MDC_TEND32_ADR, MDC_CPTEND3_ADR},			//tender 3
	{ MDC_TEND41_ADR, MDC_TEND42_ADR, MDC_CPTEND4_ADR},			//tender 4
	{ MDC_TEND51_ADR, MDC_TEND52_ADR, MDC_CPTEND5_ADR},			//tender 5
	{ MDC_TEND61_ADR, MDC_TEND62_ADR, MDC_CPTEND6_ADR},			//tender 6
	{ MDC_TEND71_ADR, MDC_TEND72_ADR, MDC_CPTEND7_ADR},			//tender 7
	{ MDC_TEND81_ADR, MDC_TEND82_ADR, MDC_CPTEND8_ADR},			//tender 8
	{ MDC_TEND91_ADR, MDC_TEND92_ADR, MDC_CPTEND9_ADR},			//tender 9
	{ MDC_TEND101_ADR, MDC_TEND102_ADR, MDC_CPTEND10_ADR},		//tender 10
	{ MDC_TEND111_ADR, MDC_TEND112_ADR, MDC_CPTEND11_ADR},		//tender 11
	{ MDC_TEND1201_ADR, MDC_TEND1202_ADR, MDC_CPTEND12_ADR},			//tender 12
	{ MDC_TEND1301_ADR, MDC_TEND1302_ADR, MDC_CPTEND13_ADR},			//tender 13
	{ MDC_TEND1401_ADR, MDC_TEND1402_ADR, MDC_CPTEND14_ADR},			//tender 14
	{ MDC_TEND1501_ADR, MDC_TEND1502_ADR, MDC_CPTEND15_ADR},			//tender 15
	{ MDC_TEND1601_ADR, MDC_TEND1602_ADR, MDC_CPTEND16_ADR},			//tender 16
	{ MDC_TEND1701_ADR, MDC_TEND1702_ADR, MDC_CPTEND17_ADR},			//tender 17
	{ MDC_TEND1801_ADR, MDC_TEND1802_ADR, MDC_CPTEND18_ADR},			//tender 18
	{ MDC_TEND1901_ADR, MDC_TEND1902_ADR, MDC_CPTEND19_ADR},			//tender 19
	{ MDC_TEND2001_ADR, MDC_TEND2002_ADR, MDC_CPTEND20_ADR},		//tender 20
	{ 0, 0}
};
/*
* ===========================================================================
*       Public Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Static Work Area Declarations
* ===========================================================================
*/

static LPCFCONFIG   lpFile;


/*added 7-13-04 to implement the use of getting and setting MDC settings
for Tender keys - cdh */
static  BYTE    abMDCWorkData[MAX_MDC_SIZE];    /* Buffer of MDC Data       */
static  BYTE    abMDCSaveData[MAX_MDC_SIZE];    /* Buffer of MDC Data       */
/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

static TENDERKEYINFO *pTermInfo;
static int nTermInfoIndex;
static WCHAR  tenderMnemonics[STD_TENDER_MAX][STD_TRANSMNEM_LEN + 1] = {0};

WPARAM    P98GetRadioButton(HWND hDlg, WPARAM wBtnIDFirst, WPARAM wBtnIDLast)
{
    WPARAM    bI;

	for (bI = wBtnIDFirst; bI <= wBtnIDLast; bI++) {
		if (IsDlgButtonChecked(hDlg, bI) == TRUE) {
			return bI;
		}
	}
	return 0;
}


VOID    P98SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam);

/*added 7-13-04 to implement the use of getting and setting MDC settings
for Tender keys. Uses same values from P01 - cdh */
VOID    P098SetMDCData(int nAddress, BYTE bChkBit, BYTE bShift, BYTE bOnOff);
VOID	P098GetMDCData(int nAddress, BOOL *bOnOff);

/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P098DlgProc()
*
*   Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
**
*   Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a dialgbox procedure for the Tender Key Setup dialog.
* ===========================================================================
*/
BOOL    WINAPI  P098DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static DWORD    dwFreeBlock,
                    dwTotalMem,
                    adwMem[MAX_FLXMEM_SIZE];
    static P98PARA  Para;
    static HGLOBAL  hMem;
	int	   nVal;
	int    ipCheck; /* used to run for-loops to validate IP inputs - PDINU */
	int    keyCheck;	/* to check keyoption */
	short  nKeyOption;
	long    nPort;

	WCHAR  szTerm[128];
    //BOOL            fChk;

    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

		/* ----- Initialize dialogbox ----- */
        if (P98InitDlg(hDlg, &dwFreeBlock, adwMem, &Para, &hMem) == TRUE) {
            /* configration error */
            /* ----- Destroy dialogbox ----- */
            EndDialog(hDlg, FALSE);
        }
		SendMessage (hDlg, WM_SETFONT, 0, 0);

        /* ----- Enable/Disable to push OK button ----- */
        //EnableWindow(GetDlgItem(hDlg, IDOK), fChk);

        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int i;

			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);

			for (i = IDD_P98_ITEM; i <= IDD_P98_STATIC_LAST; i++) {
				SendDlgItemMessage(hDlg, i, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
		}
		return FALSE;

    case PM_GET_FCONF:
        /* ----- Receive Address of File Configulation from PEP:Main ----- */
        lpFile = (LPCFCONFIG)lParam;
        return TRUE;

    case WM_VSCROLL:
        P98SpinProc(hDlg, wParam, lParam);
        return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
		/* Check IP fields as address fields are entered - PDINU 05/17/06 */
		case    IDD_P98_IPADD11:
        case    IDD_P98_IPADD12:
        case    IDD_P98_IPADD13:
        case    IDD_P98_IPADD14:
			if (HIWORD(wParam) == EN_UPDATE ) {
				for(ipCheck = IDD_P98_IPADD11; ipCheck <= IDD_P98_IPADD14; ipCheck++)
				{
					if ( P98ValidateIP(hDlg, ipCheck) ) 
					{
						P98DispErrorIP(hDlg, ipCheck);
						return TRUE;
					}
				}				
			}
			break;
		case	IDC_P98_PORTNO:
			if( HIWORD(wParam) == EN_UPDATE ) 
			{
				nPort = (long)GetDlgItemInt(hDlg, IDC_P98_PORTNO, NULL, TRUE);
				if((nPort < P98_IP_PORT_MIN) || (nPort > P98_IP_PORT_MAX)) 
				{
					P98DispErrorPort(hDlg, IDC_P98_PORTNO);
				}
			}
			break;

		/* Check Key Option field as number is entered - LDelhomme 6/23/6  */
		case	IDD_P98_KEYOPTION:		
			if (HIWORD(wParam) == EN_UPDATE ) 
			{
				keyCheck = IDD_P98_KEYOPTION;
				nKeyOption = (short)GetDlgItemInt(hDlg, keyCheck, NULL, TRUE);
				if((nKeyOption < P98_TO_DATA_MIN)|| (nKeyOption >P98_TO_DATA_MAX)) 
				{
						P98DispErrorKey(hDlg, keyCheck);
				}
			}
			break;

		case IDD_P98_BTN_MERCHANTID_RESET:                  // reset the contents of the IDD_P98_EB_MERCHANTID Merchant Id edit box.
			wcscpy (szTerm, L"000000000000000000000000");   // same as SCF_DEFAULTDATA_MERCHANTID in SCF of GenPOS
			DlgItemSendTextMessage(hDlg, IDD_P98_EB_MERCHANTID, WM_SETTEXT, (WPARAM)-1, (LPARAM)(szTerm));
			break;

		case  IDC_P98_COMBO_TRANTYPE:
			if (HIWORD(wParam) == CBN_SELCHANGE) {      /* data change */
				int nIndex = SendDlgItemMessage(hDlg, IDC_P98_COMBO_TRANTYPE, CB_GETCURSEL, 0, 0L);
				if (nIndex != CB_ERR) {
					DlgItemSendTextMessage(hDlg, IDC_P98_COMBO_TRANCODE, CB_RESETCONTENT, (WPARAM)-1, (LPARAM)(szTerm));
					for (nVal = 0; tranCodeTable[nIndex].nDlgTranCode[nVal].nDlgId != 0; nVal++) {
						LoadString(hResourceDll, tranCodeTable[nIndex].nDlgTranCode[nVal].nDlgIds, szTerm, PEP_STRING_LEN_MAC(szTerm));
						DlgItemSendTextMessage(hDlg, IDC_P98_COMBO_TRANCODE, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szTerm));
						if (pTermInfo->usTranCode == tranCodeTable[nIndex].nDlgTranCode[nVal].nDlgId)
							DlgItemSendTextMessage(hDlg, IDC_P98_COMBO_TRANCODE, CB_SETCURSEL, (WPARAM)nVal, (LPARAM)(szTerm));
						else
							SendDlgItemMessage(hDlg,IDC_P98_COMBO_TRANCODE,CB_SETCURSEL,0,0L);
					}
				}
			}
			break;

		case  IDD_P98_CB_KEYNO:
            if (HIWORD(wParam) == CBN_SELCHANGE) {      /* data change */
				int nIdex = SendDlgItemMessage(hDlg, IDD_P98_CB_KEYNO, CB_GETCURSEL, 0, 0L);

				for(ipCheck = IDD_P98_IPADD11; ipCheck <= IDD_P98_IPADD14; ipCheck++)
				{
					if ( P98ValidateIP(hDlg, ipCheck) ) 
					{
						P98DispErrorIP(hDlg, ipCheck);
						return TRUE;
					}
				}

				if (nIdex != CB_ERR) {
					P98GetDlgData(hDlg, pTermInfo, nTermInfoIndex);
					pTermInfo = &(Para.aCurt [nIdex]);
					nTermInfoIndex = nIdex + 1;
					P98InitDlgData (hDlg, pTermInfo, nTermInfoIndex);
				}
			}
			break;

		case IDC_P98_RADIO_TENDMDC01:
			if ( IsDlgButtonChecked(hDlg, IDC_P98_RADIO_TENDMDC01) ) {
				//Addr (63,65,67,69,71,73,75,77,191,193,195) Bit D off
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr1, P098_CHK_BIT, 0, P098_BIT_OFF);
			}
			break;

		case IDC_P98_RADIO_TENDMDC02:
			if ( IsDlgButtonChecked(hDlg, IDC_P98_RADIO_TENDMDC02) ){
				//Addr (63,65,67,69,71,73,75,77,191,193,195) Bit C off
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr1, P098_CHK_BIT, P098_1BITSHIFT, P098_BIT_OFF);

				//Addr (63,65,67,69,71,73,75,77,191,193,195) Bit D on
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr1, P098_CHK_BIT, 0, P098_BIT_ON);
			}
			break;

		case IDC_P98_RADIO_TENDMDC03:
			if ( IsDlgButtonChecked(hDlg, IDC_P98_RADIO_TENDMDC03) ){
				//Addr (63,65,67,69,71,73,75,77,191,193,195) Bit C on
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr1, P098_CHK_BIT, P098_1BITSHIFT, P098_BIT_ON);

				//Addr (63,65,67,69,71,73,75,77,191,193,195) Bit D on
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr1, P098_CHK_BIT, 0, P098_BIT_ON);
			}

			break;

		case IDC_P98_CHECK_TENDMDC01:
			if( IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC01) ) {
				//Addr (63,65,67,69,71,73,75,77,191,193,195) Bit B off
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr1, P098_CHK_BIT, P098_2BITSHIFT, P098_BIT_OFF);
			}
			else {
				//Addr (63,65,67,69,71,73,75,77,191,193,195) Bit B on
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr1, P098_CHK_BIT, P098_2BITSHIFT, P098_BIT_ON);
			}
			break;

		case IDC_P98_CHECK_TENDMDC02:
			if( IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC02) ) {
				//Addr (63,65,67,69,71,73,75,77,191,193,195) Bit A off
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr1, P098_CHK_BIT, P098_3BITSHIFT, P098_BIT_OFF);
			}
			else {
				//Addr (63,65,67,69,71,73,75,77,191,193,195) Bit A on
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr1, P098_CHK_BIT, P098_3BITSHIFT, P098_BIT_ON);
			}
			break;

		case IDC_P98_CHECK_TENDMDC03:
			if( IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC03) ) {
				//Addr (64,66,68,70,72,74,76,78,192,194,196) Bit D off
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr2, P098_CHK_BIT, 0, P098_BIT_OFF);
			}
			else {
				//Addr (64,66,68,70,72,74,76,78,192,194,196) Bit D on
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr2, P098_CHK_BIT, 0, P098_BIT_ON);
			}
			break;

		case IDC_P98_CHECK_TENDMDC04:
			if( IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC04) ) {
				//Addr (64,66,68,70,72,74,76,78,192,194,196) Bit C off
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr2, P098_CHK_BIT, P098_1BITSHIFT, P098_BIT_OFF);
			}
			else {
				//Addr (64,66,68,70,72,74,76,78,192,194,196) Bit C on
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr2, P098_CHK_BIT, P098_1BITSHIFT, P098_BIT_ON);
			}
			break;

		case IDC_P98_CHECK_TENDMDC05:
			if( IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC05) ) {
				//Addr (64,66,68,70,72,74,76,78,192,194,196) Bit B on
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr2, P098_CHK_BIT, P098_2BITSHIFT, P098_BIT_ON);
			}
			else {
				//Addr (64,66,68,70,72,74,76,78,192,194,196) Bit B off
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr2, P098_CHK_BIT, P098_2BITSHIFT, P098_BIT_OFF);
			}
			break;

		case IDC_P98_CHECK_TENDMDC06:
			if( IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC06) ) {
				//Addr (64,66,68,70,72,74,76,78,192,194,196) Bit A on
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr2, P098_CHK_BIT, P098_3BITSHIFT, P098_BIT_ON);
			}
			else {
				//Addr (64,66,68,70,72,74,76,78,192,194,196) Bit A off
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr2, P098_CHK_BIT, P098_3BITSHIFT, P098_BIT_OFF);
			}
			break;

		case IDC_P98_CHECK_TENDMDC07:
			if( IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC07) ) {
				//Addr (180,181,182,183,184,185,186,187,188,189,190) Bit D on
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr3, P098_CHK_BIT, 0, P098_BIT_ON);
			}
			else {
				//Addr (180,181,182,183,184,185,186,187,188,189,190) Bit D off
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr3, P098_CHK_BIT, 0, P098_BIT_OFF);
			}
			break;

		case IDC_P98_CHECK_TENDMDC08:
			if( IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC08) ) {
				//Addr (180,181,182,183,184,185,186,187,188,189,190) Bit C on
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr3, P098_CHK_BIT, P098_1BITSHIFT, P098_BIT_ON);
			}
			else {
				//Addr (180,181,182,183,184,185,186,187,188,189,190) Bit C off
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr3, P098_CHK_BIT, P098_1BITSHIFT, P098_BIT_OFF);
			}
			break;

		case IDC_P98_CHECK_TENDMDC09:
			if( IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC09) ) {
				//Addr (180,181,182,183,184,185,186,187,188,189,190) Bit B on
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr3, P098_CHK_BIT, P098_2BITSHIFT, P098_BIT_ON);
			}
			else {
				//Addr (180,181,182,183,184,185,186,187,188,189,190) Bit B off
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr3, P098_CHK_BIT, P098_2BITSHIFT, P098_BIT_OFF);
			}
			break;

		case IDC_P98_CHECK_TENDMDC010:
			if( IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC010) ) {
				//Addr (180,181,182,183,184,185,186,187,188,189,190) BitA on
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr3, P098_CHK_BIT, P098_3BITSHIFT, P098_BIT_ON);
			}
			else {
				//Addr (180,181,182,183,184,185,186,187,188,189,190) Bit A off
				P098SetMDCData(nTenderMDCAddr[nTermInfoIndex - 1].nAddr3, P098_CHK_BIT, P098_3BITSHIFT, P098_BIT_OFF);
			}
			break;

        case IDOK:
			for(ipCheck = IDD_P98_IPADD11; ipCheck <= IDD_P98_IPADD14; ipCheck++)
			{
				if ( P98ValidateIP(hDlg, ipCheck) ) 
				{
					P98DispErrorIP(hDlg, ipCheck);
					return TRUE;
				}
			}

			P98GetDlgData(hDlg, pTermInfo, nTermInfoIndex);
			// fall through since we have to do dialog finish and exit

			/* Check Key Option field before OK'ing   -LDelhomme 6/23/6*/
			keyCheck = IDD_P98_KEYOPTION;
 			nKeyOption = (short)GetDlgItemInt(hDlg, keyCheck, NULL, TRUE);

			if((nKeyOption < P98_TO_DATA_MIN)|| (nKeyOption >P98_TO_DATA_MAX))
			{
					P98DispErrorKey(hDlg, keyCheck);
					return TRUE;
			}

			/* Check IP Port field before exiting window with OK - LDelhomme 6/23/6 */
			nPort = (long)GetDlgItemInt(hDlg, IDC_P98_PORTNO, NULL, TRUE);

			if((nPort < P98_IP_PORT_MIN) || (nPort > P98_IP_PORT_MAX))
			{
				P98DispErrorPort(hDlg, IDC_P98_PORTNO);
				return TRUE;
			}

        case IDCANCEL:
            /* ----- Get data and save them to file ----- */
            if (P98FinDlg(hDlg, &Para, hMem, wParam) == TRUE) {
                /* ----- Destroy dialogbox ----- */
                EndDialog(hDlg, TRUE);
                return TRUE;
            }
            return FALSE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P98InitDlg()
*
*   Input   :   HWND        hDlg    - handle of a dialogbox procedure
*               LPDWORD     lpdwFree - address of free block
*               LPDWORD     lpwMem  - address of memory useage
*               LPP02PARA   lpPara  - address of data buffer
*               LPHGLOBAL   lphMem  - handle of global memory lock for description
**
*   Return  :   TRUE                - error occured
*               FALSE               - user process is executed
*
**  Description:
*       This procedure initialize dialogbox.
* ===========================================================================
*/
BOOL    P98InitDlg(HWND hDlg, LPDWORD lpdwFree, LPDWORD lpdwMem,
                            LPP98PARA lpPara, LPHGLOBAL lphMem)
{
    WORD        wI;
	WCHAR		szTemp [64];
    USHORT      usReturnLen, usRet;

    /* ----- Set max entry data length ----- */
    SendDlgItemMessage(hDlg, IDD_P98_EB_MERCHANTID, EM_LIMITTEXT, 24, 0L);
    SendDlgItemMessage(hDlg, IDC_P98_EB_MNEMONIC, EM_LIMITTEXT, 20, 0L);
    SendDlgItemMessage(hDlg, IDC_P98_PORTNO, EM_LIMITTEXT, 5, 0L);
    SendDlgItemMessage(hDlg, IDD_P98_KEYOPTION, EM_LIMITTEXT, 2, 0L);
    SendDlgItemMessage(hDlg, IDD_P98_TEND_AC, EM_LIMITTEXT, 1, 0L);
	SendDlgItemMessage(hDlg, IDD_P98_IPADD11, EM_LIMITTEXT, 3, 0L);
	SendDlgItemMessage(hDlg, IDD_P98_IPADD12, EM_LIMITTEXT, 3, 0L);
	SendDlgItemMessage(hDlg, IDD_P98_IPADD13, EM_LIMITTEXT, 3, 0L);
	SendDlgItemMessage(hDlg, IDD_P98_IPADD14, EM_LIMITTEXT, 3, 0L);


    /* ----- Store initial data for checking chage ----- */
    memcpy(lpPara->aPrev, lpPara->aCurt, sizeof(FLEXMEM) * MAX_FLXMEM_SIZE);

    /* ----- Set data to the edit-box ----- */
	for (wI = 1; wI <= STD_TENDER_MAX; wI++) {
		_ltow(wI, szTemp, 10);
		DlgItemSendTextMessage(hDlg, IDD_P98_CB_KEYNO, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szTemp));
	}

	SendDlgItemMessage(hDlg, IDD_P98_CB_KEYNO, CB_SETCURSEL, 0, 0L);

    ParaAllRead(CLASS_TENDERKEY_INFO, (UCHAR *)lpPara->aPrev, sizeof(lpPara->aPrev), 0, (USHORT *)&usReturnLen);

	ParaAllRead(CLASS_PARAMDC, (UCHAR *)abMDCSaveData, sizeof(abMDCSaveData), 0, &usRet);

	for (wI = 0; wI < STD_TENDER_MAX; wI++) {
		if (wI < 8) {
			RflGetTranMnem (&tenderMnemonics[wI][0], wI + TRN_TEND1_ADR);
		} else if (wI < 11) {
			RflGetTranMnem (&tenderMnemonics[wI][0], wI - 8 + TRN_TEND9_ADR);
		} else {
			RflGetTranMnem (&tenderMnemonics[wI][0], wI - 11 + TRN_TENDER12_ADR);
		}
	}

	memcpy (lpPara->aCurt, lpPara->aPrev, sizeof(lpPara->aPrev));
	memcpy (abMDCWorkData, abMDCSaveData, sizeof(abMDCSaveData));

	pTermInfo = &(lpPara->aCurt [0]); nTermInfoIndex = 1;

	P98InitDlgData (hDlg, pTermInfo, nTermInfoIndex);

    return FALSE;
}


// The following tables are used to translate between a data value and a string to
// display to the user representing that data value.  Since PEP is multi-lingual,
// the actual text in the strings is pulled from the current resource file and is
// not hardcoded.  For some controls, such as combo boxes, the order of these tables
// is used to fill in the combo box text and to translate the current selection back
// to a data value.  So when these tables are used, the combo box, list box, or whatever
// should have sorting turned off so that the order the box is filled in will be the
// order of the table entries.
struct _tagDlgItemStrings {
	int nDlgId;
	int nDlgIds;
} sDlgItemStrings [] = {
	{ IDC_P98_RADIO_TENDMDC01, IDS_P098_TENDMDC_001},
	{ IDC_P98_RADIO_TENDMDC02, IDS_P098_TENDMDC_002},
	{ IDC_P98_RADIO_TENDMDC03, IDS_P098_TENDMDC_003},
	{ IDC_P98_CHECK_TENDMDC01, IDS_P098_TENDMDC_011},
	{ IDC_P98_CHECK_TENDMDC02, IDS_P098_TENDMDC_021},
	{ IDC_P98_CHECK_TENDMDC03, IDS_P098_TENDMDC_031},
	{ IDC_P98_CHECK_TENDMDC04, IDS_P098_TENDMDC_041},
	{ IDC_P98_CHECK_TENDMDC05, IDS_P098_TENDMDC_052},
	{ IDC_P98_CHECK_TENDMDC06, IDS_P098_TENDMDC_062},
	{ IDC_P98_CHECK_TENDMDC07, IDS_P098_TENDMDC_072},
	{ IDC_P98_CHECK_TENDMDC08, IDS_P098_TENDMDC_082},
	{ IDC_P98_CHECK_TENDMDC09, IDS_P098_TENDMDC_092},
	{ IDC_P98_CHECK_TENDMDC010, IDS_P098_TENDMDC_102},
	{ 0, 0}
};

struct _tagDlgItemStrings sTranTypeCombo [] = {
	{ TENDERKEY_TRANTYPE_CREDIT, IDS_P098_TRANTYPE_CREDIT},
	{ TENDERKEY_TRANTYPE_DEBIT, IDS_P098_TRANTYPE_DEBIT},
	{ TENDERKEY_TRANTYPE_EBT, IDS_P098_TRANTYPE_EBT},
	{ TENDERKEY_TRANTYPE_CHARGE, IDS_P098_TRANTYPE_CASH},
	{ TENDERKEY_TRANTYPE_CHECK, IDS_P098_TRANTYPE_CHECK},
	{ TENDERKEY_TRANTYPE_PREPAID, IDS_P098_TRANTYPE_PREPAID},
	{ TENDERKEY_TRANTYPE_EPD, IDS_P098_TRANTYPE_EPD},
	{ TENDERKEY_TRANTYPE_EMEAL, IDS_P098_TRANTYPE_EMEAL},
	{ 0, 0}
};

BOOL    P98InitDlgData(HWND hDlg, TENDERKEYINFO *pTermInfo, int nIndex)
{
	int  nTranType;
	int nVal=0;
	WORD wBtnID;
	WCHAR szTerm[128];
	BOOL bSetCheck1[4];
	BOOL bSetCheck2[4];
	BOOL bSetCheck3[4];
	BOOL bSetType = FALSE;

	nTranType = 0;

	// locate the table entry to be used for the specified tender key transaction type
	// we do this in case there are gaps in the tender key type numeric range
	for (nVal = 0; sTranTypeCombo[nVal].nDlgId != 0; nVal++) {
		if (pTermInfo->usTranType == sTranTypeCombo[nVal].nDlgId) {
			nTranType = nVal;
			break;
		}
	}

	for (nVal = 0; sDlgItemStrings[nVal].nDlgId != 0; nVal++) {
		LoadString(hResourceDll, sDlgItemStrings[nVal].nDlgIds, szTerm, PEP_STRING_LEN_MAC(szTerm));
		DlgItemRedrawText(hDlg, sDlgItemStrings[nVal].nDlgId, szTerm);
		SendDlgItemMessage(hDlg, sDlgItemStrings[nVal].nDlgId, WM_SETFONT, (WPARAM)hResourceFont, 0);
	}

	DlgItemSendTextMessage(hDlg, IDC_P98_COMBO_TRANTYPE, CB_RESETCONTENT, (WPARAM)-1, (LPARAM)(szTerm));
	for (nVal = 0; sTranTypeCombo[nVal].nDlgId != 0; nVal++) {
		LoadString(hResourceDll, sTranTypeCombo[nVal].nDlgIds, szTerm, PEP_STRING_LEN_MAC(szTerm));
		DlgItemSendTextMessage(hDlg, IDC_P98_COMBO_TRANTYPE, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szTerm));
		if (pTermInfo->usTranType == sTranTypeCombo[nVal].nDlgId)
			DlgItemSendTextMessage(hDlg, IDC_P98_COMBO_TRANTYPE, CB_SETCURSEL, (WPARAM)nVal, (LPARAM)(szTerm));
	}

	DlgItemSendTextMessage(hDlg, IDC_P98_COMBO_TRANCODE, CB_RESETCONTENT, (WPARAM)-1, (LPARAM)(szTerm));
	for (nVal = 0; tranCodeTable[nTranType].nDlgTranCode[nVal].nDlgId != 0; nVal++) {
		LoadString(hResourceDll, tranCodeTable[nTranType].nDlgTranCode[nVal].nDlgIds, szTerm, PEP_STRING_LEN_MAC(szTerm));
		DlgItemSendTextMessage(hDlg, IDC_P98_COMBO_TRANCODE, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szTerm));
		if (pTermInfo->usTranCode == tranCodeTable[nTranType].nDlgTranCode[nVal].nDlgId) {
			DlgItemSendTextMessage(hDlg, IDC_P98_COMBO_TRANCODE, CB_SETCURSEL, (WPARAM)nVal, (LPARAM)(szTerm));
			bSetType = TRUE;
		}
	}

	if ( bSetType == FALSE ) {
		SendDlgItemMessage(hDlg,IDC_P98_COMBO_TRANCODE,CB_SETCURSEL,0,0L);
	}

	wcsncpy (szTerm, pTermInfo->tcMerchantID, 24);
	szTerm[24] = 0;
	DlgItemSendTextMessage(hDlg, IDD_P98_EB_MERCHANTID, WM_SETTEXT, (WPARAM)-1, (LPARAM)(szTerm));

	wcsncpy (szTerm, &tenderMnemonics[nIndex - 1][0], STD_TRANSMNEM_LEN);
	szTerm[STD_TRANSMNEM_LEN] = 0;   // end of string insurance
    DlgItemSendTextMessage(hDlg, IDC_P98_EB_MNEMONIC, WM_SETTEXT, (WPARAM)-1, (LPARAM)(szTerm));

    SetDlgItemInt(hDlg, IDD_P98_KEYOPTION, pTermInfo->uchTend, FALSE);
	szTerm[0] =  pTermInfo->auchEptTend;
	szTerm[1] = 0;
    DlgItemSendTextMessage(hDlg, IDD_P98_TEND_AC, WM_SETTEXT, (WPARAM)-1, (LPARAM)(szTerm));

    SetDlgItemInt(hDlg, IDD_P98_IPADD11, pTermInfo->uchIPAddress[0], FALSE);
    SetDlgItemInt(hDlg, IDD_P98_IPADD12, pTermInfo->uchIPAddress[1], FALSE);
    SetDlgItemInt(hDlg, IDD_P98_IPADD13, pTermInfo->uchIPAddress[2], FALSE);
    SetDlgItemInt(hDlg, IDD_P98_IPADD14, pTermInfo->uchIPAddress[3], FALSE);

    SetDlgItemInt(hDlg, IDC_P98_PORTNO, pTermInfo->usIPPortNo, FALSE);

	/*Call funtion for each of the three address for tender MDC settings
	to determine if the bits are off or on - cdh*/
	P098GetMDCData(nTenderMDCAddr[nIndex - 1].nAddr1, &bSetCheck1[0]);
	P098GetMDCData(nTenderMDCAddr[nIndex - 1].nAddr2, &bSetCheck2[0]);
	P098GetMDCData(nTenderMDCAddr[nIndex - 1].nAddr3, &bSetCheck3[0]);


	//if "According to C Selection" is selected for Bit D, check Bit C data
	if( bSetCheck1[0] ) {
		//if "prohibit amount entry" is selected for Bit C,
		if ( bSetCheck1[1] ) {
			wBtnID = IDC_P98_RADIO_TENDMDC03;
		}
		//if "require amount entry" is selected for Bit C
		else {
			wBtnID = IDC_P98_RADIO_TENDMDC02;
		}
	}
	//"Allow AMount Entry" was selected for Bit D
	else {
		wBtnID = IDC_P98_RADIO_TENDMDC01;
	}

	//check appropriate radio Btn
	CheckRadioButton(hDlg, IDC_P98_RADIO_TENDMDC01, IDC_P98_RADIO_TENDMDC03, wBtnID);

	//set check boxes according to MDC info for Bits A and B (addresses 63,65,67,69,71,73,75,77,191,193,and 195)
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC01,  BM_SETCHECK, !bSetCheck1[2], 0L);		//Allow over Amt Entry
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC02,  BM_SETCHECK, !bSetCheck1[3], 0L);		//Allow partial Amt. Entry

	//set check boxes according to MDC info for addresses 64,66,68,70,72,74,76,78,192,194,196 -
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC03,  BM_SETCHECK, !bSetCheck2[0], 0L);		//Prohibit Validiation Print
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC04,  BM_SETCHECK, !bSetCheck2[1], 0L);		//Open Drawer
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC05,  BM_SETCHECK, bSetCheck2[2], 0L);		//Compulsory Acct # Entry
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC06,  BM_SETCHECK, bSetCheck2[3], 0L);		//Compulsory Exp. Date Entry

	//set check boxes according to MDC info for addresses 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC07,  BM_SETCHECK, bSetCheck3[0], 0L);		//Perform Room Chg
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC08,  BM_SETCHECK, bSetCheck3[1], 0L);		//Perform Acct Charge
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC09,  BM_SETCHECK, bSetCheck3[2], 0L);		//Perform Credit Card Auth.
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC010,  BM_SETCHECK, bSetCheck3[3], 0L);	//Perform EPT


	/*SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC01, BM_SETCHECK, (pTermInfo->ulFlags1 & TENDERKEY_CHECK_TENDMDC01), 0L);
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC02, BM_SETCHECK, (pTermInfo->ulFlags1 & TENDERKEY_CHECK_TENDMDC02), 0L);
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC03, BM_SETCHECK, (pTermInfo->ulFlags1 & TENDERKEY_CHECK_TENDMDC03), 0L);
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC04, BM_SETCHECK, (pTermInfo->ulFlags1 & TENDERKEY_CHECK_TENDMDC04), 0L);
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC05, BM_SETCHECK, (pTermInfo->ulFlags1 & TENDERKEY_CHECK_TENDMDC05), 0L);
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC06, BM_SETCHECK, (pTermInfo->ulFlags1 & TENDERKEY_CHECK_TENDMDC06), 0L);
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC07, BM_SETCHECK, (pTermInfo->ulFlags1 & TENDERKEY_CHECK_TENDMDC07), 0L);
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC08, BM_SETCHECK, (pTermInfo->ulFlags1 & TENDERKEY_CHECK_TENDMDC08), 0L);
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC09, BM_SETCHECK, (pTermInfo->ulFlags1 & TENDERKEY_CHECK_TENDMDC09), 0L);
	SendDlgItemMessage(hDlg, IDC_P98_CHECK_TENDMDC010, BM_SETCHECK, (pTermInfo->ulFlags1 & TENDERKEY_CHECK_TENDMDC010), 0L);*/

	return TRUE;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P98SpinProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   Specifies a Scroll Bar Code
*                   LONG    lParam  -   Current Position/ Handle of ScrollBar
*
**  Return      :   No return value.
*
**  Description :
*       This function controls SpinButton.
* ===========================================================================
*/
VOID    P98SpinProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    PEPSPIN PepSpin;        /* SpinButton Style Structure   */
    int     idEdit;         /* EditText ID for SpinButton   */

    /* ----- Get SpinButton ID ----- */

    idEdit = GetDlgCtrlID((HWND)lParam);

    PepSpin.nStep      = P98_SPIN_STEP;
    PepSpin.nTurbo     = P98_SPIN_TURBO;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

	if (idEdit == IDD_P98_KEYOPTIONSPN) {
		PepSpin.lMin       = (long)P98_TO_DATA_MIN;
		PepSpin.lMax       = (long)P98_TO_DATA_MAX;
	}
	else if (idEdit == IDD_P98_TEND_AC_SPN) {
		PepSpin.lMin       = (long)P98_AC_DATA_MIN;
		PepSpin.lMax       = (long)P98_AC_DATA_MAX;
	}
	else {
		PepSpin.lMin       = (long)P98_IP_DATA_MIN;
		PepSpin.lMax       = (long)P98_IP_DATA_MAX;
	}

    /* ----- Set EditText ID with SpinButton ID ----- */
    idEdit -= P98_SPIN_OFFSET;

    PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);
}


BOOL    P98GetDlgData(HWND hDlg, TENDERKEYINFO *pTermInfo, int nIndex)
{
	int nCurSel, nCurSelType;
	int i;
	WORD wBtnID;
	WCHAR  szTemp[128];

	// Get the merchant id from the edit box, make sure it is at most 24 characters
	// then copy it into the tcMerchantID field of TENDERKEYINFO.
	for (i = 0; i < 24; i++)
		pTermInfo->tcMerchantID[i] = L' ';
	DlgItemGetText(hDlg, IDD_P98_EB_MERCHANTID, szTemp, 24+1);
	szTemp[24] = 0;
	wcscpy (pTermInfo->tcMerchantID, szTemp);

	nCurSel = SendDlgItemMessage(hDlg, IDC_P98_COMBO_TRANTYPE, CB_GETCURSEL, 0, 0L);
	if (nCurSel == CB_ERR)
		nCurSel = 0;
	pTermInfo->usTranType = sTranTypeCombo[nCurSel].nDlgId;

	nCurSelType = SendDlgItemMessage(hDlg, IDC_P98_COMBO_TRANCODE, CB_GETCURSEL, 0, 0L);
	if (nCurSel == CB_ERR)
		nCurSel = 0;
	pTermInfo->usTranCode = tranCodeTable[nCurSel].nDlgTranCode[nCurSelType].nDlgId;//sTranCodeCombo[nCurSel].nDlgId;

    pTermInfo->uchTend = GetDlgItemInt(hDlg, IDD_P98_KEYOPTION, &nCurSel, FALSE);
	DlgItemGetText(hDlg, IDD_P98_TEND_AC, szTemp, 1+1);
    pTermInfo->auchEptTend = szTemp[0];

    pTermInfo->uchIPAddress[0] = GetDlgItemInt(hDlg, IDD_P98_IPADD11, &nCurSel, FALSE);
    pTermInfo->uchIPAddress[1] = GetDlgItemInt(hDlg, IDD_P98_IPADD12, &nCurSel, FALSE);
    pTermInfo->uchIPAddress[2] = GetDlgItemInt(hDlg, IDD_P98_IPADD13, &nCurSel, FALSE);
    pTermInfo->uchIPAddress[3] = GetDlgItemInt(hDlg, IDD_P98_IPADD14, &nCurSel, FALSE);

    pTermInfo->usIPPortNo = GetDlgItemInt(hDlg, IDC_P98_PORTNO, &nCurSel, FALSE);

	wBtnID = P98GetRadioButton(hDlg, IDC_P98_RADIO_TENDMDC01, IDC_P98_RADIO_TENDMDC03);
	if (wBtnID) {
		wBtnID -= IDC_P98_RADIO_TENDMDC01;

		TENDERKEY_CHECK_RADIO_SET_AMNT_ENTRY(pTermInfo, wBtnID);
	}

	pTermInfo->ulFlags1 = 0;
	pTermInfo->ulFlags1 |= (IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC01) ? TENDERKEY_CHECK_TENDMDC01 : 0);
	pTermInfo->ulFlags1 |= (IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC02) ? TENDERKEY_CHECK_TENDMDC02 : 0);
	pTermInfo->ulFlags1 |= (IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC03) ? TENDERKEY_CHECK_TENDMDC03 : 0);
	pTermInfo->ulFlags1 |= (IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC04) ? TENDERKEY_CHECK_TENDMDC04 : 0);
	pTermInfo->ulFlags1 |= (IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC05) ? TENDERKEY_CHECK_TENDMDC05 : 0);
	pTermInfo->ulFlags1 |= (IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC06) ? TENDERKEY_CHECK_TENDMDC06 : 0);
	pTermInfo->ulFlags1 |= (IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC07) ? TENDERKEY_CHECK_TENDMDC07 : 0);
	pTermInfo->ulFlags1 |= (IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC08) ? TENDERKEY_CHECK_TENDMDC08 : 0);
	pTermInfo->ulFlags1 |= (IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC09) ? TENDERKEY_CHECK_TENDMDC09 : 0);
	pTermInfo->ulFlags1 |= (IsDlgButtonChecked(hDlg, IDC_P98_CHECK_TENDMDC010) ? TENDERKEY_CHECK_TENDMDC010 : 0);

	DlgItemGetText(hDlg, IDC_P98_EB_MNEMONIC, szTemp, STD_TRANSMNEM_LEN + 1);
	wcsncpy (&tenderMnemonics[nIndex - 1][0], szTemp, STD_TRANSMNEM_LEN);

	return TRUE;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P98FinDlg()
*
*   Input   :   HWND        hDlg   - handle of a dialogbox procedure
*               LPP02PARA   lpPara - address of data buffer
*               HGLOBAL     hMem   - handle of global memory lock for description
*               WORD        wID    - parameter of dialogbox ID
**
*   Return  :   TRUE               - user process is executed.
*               FALSE              - cancel process is expected.
*
**  Description:
*       This procedure finalize dialogbox.
* ===========================================================================
*/
BOOL    P98FinDlg(HWND hDlg, LPP98PARA lpPara, HGLOBAL hMem, WPARAM wID)
{
    USHORT      usReturnLen, usRet;
    HCURSOR     hCursor;

    if (LOWORD(wID) == IDOK) {       /* OK button pushed */
		WORD  wI;

        /* ----- Change cursor (arrow -> hour-glass) ----- */
        hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
        ShowCursor(TRUE);

        /* ----- Get data from dialogbox ----- */
        /* ----- Check whether size changed or not and create file ----- */
        /* ----- Write data to file ----- */

		//write tender key info
        ParaAllWrite(CLASS_TENDERKEY_INFO, (UCHAR *)lpPara->aCurt, sizeof(lpPara->aCurt), 0, &usReturnLen);

		//write MDC info for tender keys
		ParaAllWrite(CLASS_PARAMDC, (UCHAR *)abMDCWorkData, sizeof(abMDCWorkData), 0, &usRet);

		for (wI = 0; wI < STD_TENDER_MAX; wI++) {
			if (wI < 8) {
				RflPutTranMnem (&tenderMnemonics[wI][0], wI + TRN_TEND1_ADR);
			} else if (wI < 11) {
				RflPutTranMnem (&tenderMnemonics[wI][0], wI - 8 + TRN_TEND9_ADR);
			} else {
				RflPutTranMnem (&tenderMnemonics[wI][0], wI - 11 + TRN_TENDER12_ADR);
			}
		}

        /* ----- Set Transaction Flag ----- */
//        SendMessage(hwndProgMain, PM_SET_TRAFLAG, (WPARAM)Check.fwFile, 0L);

        /* ----- Set file status flag ----- */
		/* Add Status Flag to show that P97 has been changed ESMITH*/
          PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);

//        PepSetModFlag(hwndProgMain, Check.nModFileID, 0);

        /* ----- Restore cursor (hour-glass -> arrow) ----- */
        ShowCursor(FALSE);
        SetCursor(hCursor);
    }

    /* ----- Free memory area ----- */
    GlobalUnlock(hMem);
    GlobalFree(hMem);

    return TRUE;
}

/*
* ===========================================================================
**  Synopsis:   VOID    P098SetMDCData()
*
*   Input   :   int         nAddress   - MDC address to change
*               BYTE        bChkBit	   - Bit Type for Mask Data
*               BYTE        bShift     - Bit Shift Count
*               BYTE        bOnOff     - is bit being turned On/Off
**
*   Return  :
*
*
**  Description:
*       This procedure sets MDC settings in the abMDCWorkData buffer as user makes changes
* ===========================================================================
*/
VOID P098SetMDCData(int nAddress, BYTE bChkBit, BYTE bShift, BYTE bOnOff)
{
   BYTE    bWork;
   USHORT  usBuffAddr;

    // ----- Get Current Selected Address of Data Buffer ----- //
    usBuffAddr = ((nAddress - 1) / P098_CHK_ADDR);

    // ----- Get Bit Status of Selected Address ----- //
    if (nAddress % P098_CHK_ADDR) {  // Address is Odd (Get from Lo-Bit) //
        bWork = P098_GET_LOBIT(abMDCWorkData[usBuffAddr]);
    } else {                        // Address is Even(Get from Hi-Bit) //
        bWork = P098_GET_HIBIT(abMDCWorkData[usBuffAddr]);
    }

    // ----- Reset Data Buffer ----- //
    bWork &= (BYTE)~(bChkBit << bShift);

    // ----- Set Inputed Data to Buffer ----- //
    bWork |= (BYTE)(bOnOff << bShift);

    if (nAddress % P098_CHK_ADDR) {  // Address is Odd (Get from Lo-Bit) //
        abMDCWorkData[usBuffAddr] = P098_SET_LOBIT(abMDCWorkData[usBuffAddr], bWork);
    } else {                        // Address is Even(Get from Hi-Bit) //
        abMDCWorkData[usBuffAddr] = P098_SET_HIBIT(abMDCWorkData[usBuffAddr], bWork);
    }
}

/*
* ===========================================================================
**  Synopsis:   VOID    P098SetMDCData()
*
*   Input   :   int         nAddress   - MDC address to get data for
*               BOOL        bOnOff	   - buffer to hold bit info
**
*   Return  :
*
*
**  Description:
*       This procedure gets MDC bit settings for a particular address
* ===========================================================================
*/
VOID P098GetMDCData(int nAddress, BOOL *bOnOff)
{
    WPARAM  wTopID = 0;
    BYTE    bMDCWork, bWork;
	USHORT  usBuffAddr;
    WORD    wI;

    // ----- Get Target Address of Buffer ----- //
    usBuffAddr = ((nAddress - 1) / P098_CHK_ADDR);

    if (nAddress % P098_CHK_ADDR) {  // Address is Odd (Get from Lo-Bit) //
        bMDCWork = P098_GET_LOBIT(abMDCWorkData[usBuffAddr]);
    } else {                        // Address is Even(Get from Hi-Bit) //
        bMDCWork = P098_GET_HIBIT(abMDCWorkData[usBuffAddr]);
    }

	//get data for each of the 4 bits
    for (wI = 0; wI < P098_BIT_NO; wI++) {
        // ----- Check Current MDC Bit whether ON/OFF ----- //
        bWork = (BYTE)((bMDCWork >> wI) & P098_CHK_BIT);
		bOnOff[wI] = bWork;
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P98ValidateIP(HWND hDlg, WPARAM wID)
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Inputed Data
*
**  Return      :   BOOL    TRUE    -   Inputed Data is Out of Range
*                           FALSE   -   Inputed Data is within Range
*
**  Description :
*       This function determines whether the inputed IP address fields are
*		valid. It returns TRUE, if the inputed data is out of range. Otherwise
*		it returns FALSE.
* ===========================================================================
*/
BOOL    P98ValidateIP(HWND hDlg, WPARAM wID)
{
    BOOL    fCheck = FALSE;
    short   nRndPos;

    /* ----- Get the Inputed Data from the Specified EditText ----- */
    nRndPos = (short)GetDlgItemInt(hDlg, wID, NULL, TRUE);

    /* ----- Determine Whether the Inputed Data is Out of Range or Not ----- */
    switch (LOWORD(wID)) {
        case    IDD_P98_IPADD11:
        case    IDD_P98_IPADD12:
        case    IDD_P98_IPADD13:
        case    IDD_P98_IPADD14:
            if ((nRndPos < P98_IP_DATA_MIN) || (nRndPos > P98_IP_DATA_MAX))
            {
                 fCheck = TRUE;
            }

        default:
            break;
    }
    return (fCheck);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P98DispErrorIP()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Error Occurred
*
**  Return      :   No return value.
*
**  Description :
*       This function displays the error message of data range over with 
*		MessageBox.
* ===========================================================================
*/
VOID    P98DispErrorIP(HWND hDlg, WPARAM wID)
{
    WCHAR    szWork[PEP_OVER_LEN], szMessage[PEP_OVER_LEN];

    /* ----- Load String from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_OVER_LEN);

    /* ----- Make Error Message ----- */
    wsPepf(szMessage, szWork, P98_IP_DATA_MIN, P98_IP_DATA_MAX);

    /* ----- Display Error Message with MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMessage, WIDE("IP Address"), MB_OK | MB_ICONEXCLAMATION);

    /* ----- Set Focus to Error Occurred EditText ----- */
    SendDlgItemMessage(hDlg, wID, EM_SETSEL, 0, MAKELONG(0, -1));
    SetFocus(GetDlgItem(hDlg, wID));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P98DispErrorKey() - LDelhomme
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Error Occurred
*
**  Return      :   No return value.
*
**  Description :
*       This function displays the error message of data range over with 
*		MessageBox.
* ===========================================================================
*/
VOID    P98DispErrorKey(HWND hDlg, WPARAM wID)
{
    WCHAR    szWork[PEP_OVER_LEN], szMessage[PEP_OVER_LEN];

    /* ----- Load String from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_OVER_LEN);

    /* ----- Make Error Message ----- */
    wsPepf(szMessage, szWork, P98_TO_DATA_MIN, P98_TO_DATA_MAX);

    /* ----- Display Error Message with MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMessage, WIDE("Key Option"), MB_OK | MB_ICONEXCLAMATION);

    /* ----- Set Focus to Error Occurred EditText ----- */
    SendDlgItemMessage(hDlg, wID, EM_SETSEL, 0, MAKELONG(0, -1));
    SetFocus(GetDlgItem(hDlg, wID));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P98DispErrorPort() - LDelhomme
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Error Occurred
*
**  Return      :   No return value.
*
**  Description :
*       This function displays the error message of data range over with 
*		MessageBox.
* ===========================================================================
*/
VOID    P98DispErrorPort(HWND hDlg, WPARAM wID)
{
    WCHAR    szWork[PEP_OVER_LEN], szMessage[PEP_OVER_LEN];

    /* ----- Load String from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_OVER_LEN);

    /* ----- Make Error Message ----- */
    wsPepf(szMessage, szWork, P98_IP_PORT_MIN, P98_IP_PORT_MAX);

    /* ----- Display Error Message with MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMessage, WIDE("IP Port"), MB_OK | MB_ICONEXCLAMATION);

    /* ----- Set Focus to Error Occurred EditText ----- */
    SendDlgItemMessage(hDlg, wID, EM_SETSEL, 0, MAKELONG(0, -1));
    SetFocus(GetDlgItem(hDlg, wID));
}


/* ===== End of P098.C ===== */  