/*
* ---------------------------------------------------------------------------
* Title         :   Flexible Memory Assignment Header File (Prog. 2)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P097.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
*
* ===========================================================================
* ===========================================================================
*/
/*
* ===========================================================================
*       Define Declarations
* ===========================================================================
*/

/*
* ===========================================================================
*       Value definition
* ===========================================================================
*/


/*
* ===========================================================================
*       Dialog ID definition
* ===========================================================================
*/

/*    Dialog ID default value */

#define IDD_P97_ITEM         20301

#define IDD_P97_CB_TERMNO			(IDD_P97_ITEM + 1)
#define IDD_P97_EB_CODEPAGE			(IDD_P97_ITEM + 2)
#define IDD_P97_CB_MENUPAGENO		(IDD_P97_ITEM + 3)
#define IDD_P97_EB_REGISTERNO		(IDD_P97_ITEM + 4)
#define IDD_P97_EB_STORENUMBER		(IDD_P97_ITEM + 5)
#define IDD_P97_CB_KEYB_TYPE		(IDD_P97_ITEM + 6)
#define IDD_P97_EB_DEFLAYOUT		(IDD_P97_ITEM + 7)
#define IDD_P97_CB_LANGUAGE			(IDD_P97_ITEM + 8)
#define IDD_P97_CB_DEFLAYOUT		(IDD_P97_ITEM + 9)

/*CSMALL*/
#define IDC_EB_UnlockCode			(IDD_P97_ITEM + 10)
#define IDC_EB_TermSerialNo 		(IDD_P97_ITEM + 11)
#define IDC_EB_IP1					(IDD_P97_ITEM + 12)
#define IDC_EB_IP2					(IDD_P97_ITEM + 13)
#define IDC_EB_IP3					(IDD_P97_ITEM + 14)
#define IDC_EB_IP4					(IDD_P97_ITEM + 15)


#define IDD_P97_STATIC_FIRST			(IDD_P97_ITEM + 16)
#define IDD_P97_STATIC_MENUPAGENO		(IDD_P97_STATIC_FIRST)
#define IDD_P97_STATIC_REGNO			(IDD_P97_STATIC_MENUPAGENO+1)
#define IDD_P97_STATIC_STORENO			(IDD_P97_STATIC_REGNO+1)
#define IDD_P97_STATIC_KEYBOARDTYPE		(IDD_P97_STATIC_STORENO+1)
#define IDD_P97_STATIC_TERMNO			(IDD_P97_STATIC_KEYBOARDTYPE+1)
#define IDD_P97_STATIC_DEFAULTLAYOUT	(IDD_P97_STATIC_TERMNO+1)
#define IDD_P97_STATIC_PRINTCODEPAGE	(IDD_P97_STATIC_DEFAULTLAYOUT+1)
#define IDD_P97_STATIC_KEYBOARDFRAME	(IDD_P97_STATIC_PRINTCODEPAGE+1)
#define IDD_P97_STATIC_LANGUAGE			(IDD_P97_STATIC_KEYBOARDFRAME+1)
#define IDD_P97_ODT_CB					(IDD_P97_STATIC_LANGUAGE + 1)

/*CSMALL*/
#define IDC_STATIC_UnlockCode		(IDD_P97_ODT_CB + 1)
#define IDC_STATIC_IPaddress		(IDC_STATIC_UnlockCode + 1)
#define IDC_STATIC_TermSerialNo		(IDC_STATIC_IPaddress + 1)
#define IDC_STATIC_ORDDEC			(IDC_STATIC_TermSerialNo +1)


#define IDD_P97_STATIC_LAST				IDC_STATIC_ORDDEC

#define IDS_P97_TYPE0       (IDD_P97_STATIC_LAST + 1)

/*
* ===========================================================================
*       Resource ID definition
* ===========================================================================
*/


/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
#define P97_ODT_NUM				5   /* No of Order Declaration Types (including "Not Used") JHHJ*/

typedef struct _P97PARA {
    TERMINALINFO aCurt[16];     /* current parameter */
    TERMINALINFO aPrev[16];     /* previous parameter */
} P97PARA,  *LPP97PARA;


BOOL    P97InitDlg(HWND, LPDWORD, LPDWORD, LPP97PARA, LPHGLOBAL);
BOOL    P97FinDlg(HWND, LPP97PARA, HGLOBAL, WPARAM);
BOOL    P97InitDlgData(HWND hDlg, TERMINALINFO *pTermInfo, int nIndex);
BOOL    P97GetDlgData(HWND hDlg, TERMINALINFO *pTermInfo, int nIndex);
BOOL    ValidateIP(HWND, WPARAM);
VOID	DispErrorIP(HWND, WPARAM);

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/

#define P097_STORENO_MAX	9999         /* Max. Store Number ESMITH  */
#define P097_STORENO_MIN    0		     /* Min. Store Number ESMITH  */
#define P097_REGNO_MAX		999			 /* Max. Register Number ESMITH  */
#define P097_REGNO_MIN		0		     /* Min. Register Number ESMITH  */
#define P097_IP_MAX			255			/* Max. IP field value CSMALL */
#define P097_IP_MIN			0			/* Min. IP field value CSMALL */

#define P097_CAPTION				WIDE("Terminal Setup P(097)")
#define P097_STORENO_ERR			WIDE("Specify a value between 0 and 9999")
#define P097_REGNO_ERR				WIDE("Specify a value between 0 and 999")
#define P097_IP_ERR					WIDE("Specify a value between 0 and 255")
#define P097_ERR_CHECK				2
#define P097_MAX_STORELEN			4
#define P097_MAX_REGNOLEN			3

/* ===== End of P097.H ===== */