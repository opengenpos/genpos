/*
* ---------------------------------------------------------------------------
* Title         :   Transfer Functions
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRANS.C
* Copyright (C) :   1995, NCR Corp. E&M-OISO, All rights reserved.
* --------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /c /AL /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:     This file defines the function which is called from other
*               layers and the function used by transfer.
*               The following function is contained.
* --------------------------------------------------------------------------
* Update Histories
*
* Date          Ver.       Name             Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Apr-06-95 : 03.00.00 : H.Ishida   : Add R3.0
* Jan-22-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-29-98 : 03.03.00 : A.Mitsui   : V3.3
* Jan-28-00 : 01.00.00 : hrkato     : Saratoga
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
#include <winerror.h>
#include <Shlobj.h>
#include <string.h>
#include <stdlib.h>

#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <plu.h>
#include <csop.h>
#include <cscas.h>
#include <csetk.h>
#include <csprgrpt.h>
#include <r20_pif.h>

#include "pepcomm.h"
#include "trans.h"
#include "transl.h"

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

static TRANSFER Trans;
WCHAR        szTransPort[TRANS_MAX_PORT][TRANS_MAX_PORT_LEN] =
            {WIDE("COM1"), WIDE("COM2"), WIDE("COM3"), WIDE("COM4")};
WCHAR        szTransBaud[TRANS_MAX_BAUD][TRANS_MAX_BAUD_LEN] =
            {WIDE("57600"), WIDE("38400"), WIDE("19200"), WIDE("9600"), WIDE("4800"), WIDE("2400"), WIDE("1200")};

WCHAR    szTransDept[]    = WIDE("PARAMDEP");
WCHAR    szTransPlu[]     = WIDE("PARAM$PL");  /* Saratoga */
/* char    szTransPlu[]     = "PARAMPLU"; */
WCHAR    szTransPluIndex[]= WIDE("PARAMIDX");  /* Saratoga */
WCHAR    szTransWaiter[]  = WIDE("PARAMWAI");
WCHAR    szTransCashier[] = WIDE("PARAMCAS");
WCHAR    szTransEtk[]     = WIDE("PARAMETK");
WCHAR    szTransCpn[]     = WIDE("PARAMCPN");
WCHAR    szTransCstr[]    = WIDE("PARAMCST");
WCHAR    szTransProgRpt[] = WIDE("PARAMRPT");
WCHAR    szTransPPI[]     = WIDE("PARAMPPI");
WCHAR    szTransMLD[]     = WIDE("PARAMMLD");  /* V3.3 */
WCHAR    szTransRSN[]     = WIDE("PARAMRSN");  /* V3.3 */
WCHAR    szTransOep[]     = WIDE("PARAMOEP");  /* Saratoga */
WCHAR	 szTransTouchLAY[]	  = WIDE("PARAMLA"); //ESMITH LAYOUT
WCHAR	 szTransKeyboardLAY[]	= WIDE("PARAMLB");
WCHAR	 szTransReceiptLogo[]	= WIDE("PARAMLO1");
WCHAR	 szTransLogoFile[]		= WIDE("PARAMLO%d");
WCHAR	 szTransLogoIndex[]		= WIDE("PARAMLID");
WCHAR	 szTransFPDB[]			= WIDE("DFPR_DB");

WCHAR        szTransIPFileName[] = WIDE("PEPHOSTS.INI");

WCHAR        szPepAppDataFolder[256];

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
** Synopsis:    void TransInitialize(void)
*
** Return:      nothing
*
** Description: This function initializes the transfer function.
*===========================================================================
*/
void TransInitialize(void)
{
	DWORD dwGetLastError = 0;

    /* Terminal */
    Trans.Term.byPort     = 1;
    Trans.Term.wBaudRate  = 9600;
    Trans.Term.byByteSize = 8;
    Trans.Term.byParity   = 0;
    Trans.Term.byStopBits = 1;

    Trans.Term.byTerm    = 0;
    Trans.Term.fbyStatus = 0;
    Trans.Term.iDir      = 0;

    /* Loader Board */
    Trans.Loader.byPort     = 1;
    Trans.Loader.wBaudRate  = 9600;
    Trans.Loader.byByteSize = 8;
    Trans.Loader.byParity   = 1;
    Trans.Loader.byStopBits = 1;

    Trans.fwTransTerm   = 0;
    Trans.fwTransLoader = TRANS_ACCESS_PARA;
    Trans.fwUpdate      = 0;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szPepAppDataFolder))) {
		wcscat(szPepAppDataFolder, L"\\PEP\\");
		if (!CreateDirectory(szPepAppDataFolder, NULL)) {
			dwGetLastError = GetLastError();
		}
	}

}

/*
*===========================================================================
** Synopsis:    void FAR * TransGetDataAddr(void)
*
** Return:      Address of data
*
** Description: This function returns the first address of the data
*               structure of transfer.
*===========================================================================
*/
void FAR * TransGetDataAddr(void)
{
    return ((void FAR *)&Trans);
}

/*
*===========================================================================
** Synopsis:    WORD TransGetDataLen(void)
*
** Return:      size of the data
*
** Description: This function returns the size of the data structure of
*               transfer.
*===========================================================================
*/
WORD TransGetDataLen(void)
{
    return ((WORD)sizeof(Trans));
}

/*
*===========================================================================
** Synopsis:    void    TransAccessFlexMem(WORD fwFile)
*     Input:    fwFile - Access File
*
** Return:      nothing
*
** Description: This function sets the thing to change the memory size in
*               program mode 2 as a flag of the corresponding file.
*===========================================================================
*/
void    TransAccessFlexMem(WPARAM fwFile)
{
    Trans.fwFlexMem |= fwFile;
}

/*
*===========================================================================
** Synopsis:    void TransResetFlexMem(WORD fwFile)
*     Input:    fwFile - Access File
*
** Return:      nothing
*
** Description: This function sets the thing to change the memory size in
*               program mode 2 as a flag of the corresponding file.
*===========================================================================
*/
void TransResetFlexMem(void)
{
    Trans.fwFlexMem = 0;
}

/*
*===========================================================================
** Synopsis:    void TransAccessData(WORD fwData)
*     Input:    fwFile - Access Parameter/File
*
** Return:      nothing
*
** Description: This function sets the thing to maintain the file in the
*               flag.
*===========================================================================
*/
void TransAccessData(WPARAM fwData)
{
    Trans.fwUpdate |= fwData;
}

/*
*===========================================================================
** Synopsis:    void TransResetData(WORD fwData)
*     Input:    fwFile - Access Parameter/File
*
** Return:      nothing
*
** Description: This function sets the thing to maintain the file in the
*               flag.
*===========================================================================
*/
void TransResetData(void)
{
    if ((Trans.fwTransTerm & TRANS_ACCESS_PROG) || (Trans.fwTransLoader & TRANS_ACCESS_PROG)) {
        Trans.fwUpdate &= ~TRANS_ACCESS_PROG;
    }
    if ((Trans.fwTransTerm & TRANS_ACCESS_ACT) || (Trans.fwTransLoader & TRANS_ACCESS_ACT)) {
        Trans.fwUpdate &= ~TRANS_ACCESS_ACT;
    }
    if ((Trans.fwTransTerm & TRANS_ACCESS_DEPT) || (Trans.fwTransLoader & TRANS_ACCESS_DEPT)) {
        Trans.fwUpdate &= ~TRANS_ACCESS_DEPT;
    }
    if ((Trans.fwTransTerm & TRANS_ACCESS_PLU) || (Trans.fwTransLoader & TRANS_ACCESS_PLU)) {
        Trans.fwUpdate &= ~TRANS_ACCESS_PLU;
    }
    if ((Trans.fwTransTerm & TRANS_ACCESS_WAITER) || (Trans.fwTransLoader & TRANS_ACCESS_WAITER)) {
        Trans.fwUpdate &= ~TRANS_ACCESS_WAITER;
    }
    if ((Trans.fwTransTerm & TRANS_ACCESS_CASHIER) || (Trans.fwTransLoader & TRANS_ACCESS_CASHIER)) {
        Trans.fwUpdate &= ~TRANS_ACCESS_CASHIER;
    }
    if ((Trans.fwTransTerm & TRANS_ACCESS_ETK) || (Trans.fwTransLoader & TRANS_ACCESS_ETK)) {
        Trans.fwUpdate &= ~TRANS_ACCESS_ETK;
    }
    if ((Trans.fwTransTerm & TRANS_ACCESS_CPN) || (Trans.fwTransLoader & TRANS_ACCESS_CPN)) {
        Trans.fwUpdate &= ~TRANS_ACCESS_CPN;
    }
    if ((Trans.fwTransTerm & TRANS_ACCESS_CSTR) || (Trans.fwTransLoader & TRANS_ACCESS_CSTR)) {
        Trans.fwUpdate &= ~TRANS_ACCESS_CSTR;
    }
    if ((Trans.fwTransTerm & TRANS_ACCESS_RPT) || (Trans.fwTransLoader & TRANS_ACCESS_RPT)) {
        Trans.fwUpdate &= ~TRANS_ACCESS_RPT;
    }
    if ((Trans.fwTransTerm & TRANS_ACCESS_PPI) || (Trans.fwTransLoader & TRANS_ACCESS_PPI)) {
        Trans.fwUpdate &= ~TRANS_ACCESS_PPI;
    }

    /* V3.3 */
    if ((Trans.fwTransTerm & TRANS_ACCESS_MLD) || (Trans.fwTransLoader & TRANS_ACCESS_MLD)) {
		Trans.fwUpdate &= ~TRANS_ACCESS_MLD;
    }

    if ((Trans.fwTransTerm & TRANS_ACCESS_LAYTOUCH) || (Trans.fwTransLoader & TRANS_ACCESS_LAYTOUCH)) {
		Trans.fwUpdate &= ~TRANS_ACCESS_LAYTOUCH;
    }

    if ((Trans.fwTransTerm & TRANS_ACCESS_LAYKEYBRD) || (Trans.fwTransLoader & TRANS_ACCESS_LAYKEYBRD)) {
		Trans.fwUpdate &= ~TRANS_ACCESS_LAYKEYBRD;
    }

	if ((Trans.fwTransTerm & TRANS_ACCESS_LOGO) || (Trans.fwTransLoader & TRANS_ACCESS_LOGO)){
		Trans.fwUpdate &= ~TRANS_ACCESS_LOGO;
	}

	if((Trans.fwTransTerm & TRANS_ACCESS_ICON) || (Trans.fwTransLoader & TRANS_ACCESS_ICON)){
		Trans.fwUpdate &= ~TRANS_ACCESS_ICON;
	}

}

/*
*===========================================================================
** Synopsis:    int TransMessage(HWND hWnd, UINT uAlert, UINT fuStyle,
*                                WORD wTitle, WORD wMessage, ...)
*     Input:    hWnd     - handle of parent window
*               uAlert   - alert level
*               fuStyle  - style of message box
*               wTitle   - title of message box
*               wMessage - message of message box
*               ...      - argument
*
** Return:      MessageBox() status
*
** Description: This function displays the message by the display form which
*               the user specified. At this time, the displayed message is
*               made acquisition from the resource file.
*===========================================================================
*/
int TransMessage(HWND hWnd, UINT uAlert, UINT fuStyle, UINT wTitle,
                 UINT wMessage, ...)
{
    WCHAR       szFmt[TRANS_MAX_MESSAGE_LEN];
    WCHAR       szMessage[TRANS_MAX_MESSAGE_LEN];
    HINSTANCE   hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

    SetCursor(LoadCursor(NULL, IDC_ARROW));     /* set normal cursor*/

    MessageBeep(uAlert);

    LoadString(hResourceDll, wMessage, szFmt, PEP_STRING_LEN_MAC(szFmt));
    wsPepf/*wvsprintf*/ (szMessage, szFmt, *(&wMessage + 1));
    LoadString(hResourceDll, wTitle, szFmt, PEP_STRING_LEN_MAC(szFmt));
    return MessageBoxPopUp(hWnd, szMessage, szFmt, fuStyle);
}

/*
*===========================================================================
** Synopsis:    short TransGetIndex(WORD wBaudRate)
*
** Return:      >= 0 - Index number.
*               <  0 - not found.
*
** Description: This function returns whether the specified baud rate is
*               defined in table.
*===========================================================================
*/
short TransGetIndex(WORD wBaudRate)
{
    WORD    wCount;
    WCHAR   szBuffer[17];

    _itow(wBaudRate, szBuffer, 10);

    for (wCount = 0; wCount < TRANS_MAX_BAUD; wCount++) {
        if (wcscmp(szTransBaud[wCount], szBuffer) == 0) {
            return (wCount);
        }
    }

    return (-1);
}

/*
*===========================================================================
** Synopsis:    void TransCreateFiles(void *pNewFlexMem, void *pPrevFlexMem)
*     Input:    pNewFlexMem  - new flex memory data
*               pPrevFlexMem - reveious flex memory data
*
** Return:      nothing
*
** Description: This function creates each files.
*===========================================================================
*/
void TransCreateFiles(void *pNewFlexMem, void *pPrevFlexMem)
{
    FLEXMEM *pNew  = pNewFlexMem;
    FLEXMEM *pPrev = pPrevFlexMem;

    /* ----- check department data changing ----- */

    if (((pNew + FLEX_DEPT_ADR - 1)->ulRecordNumber !=
         (pPrev + FLEX_DEPT_ADR - 1)->ulRecordNumber) ||
        ((pNew + FLEX_DEPT_ADR - 1)->uchPTDFlag !=
         (pPrev + FLEX_DEPT_ADR - 1)->uchPTDFlag)) {
        PifDeleteFile(szTransDept);
        OpDeptCreatFile(
                        (USHORT)((pNew + FLEX_DEPT_ADR - 1)->ulRecordNumber),
                        0);
    }

    /* ----- Check PLU data changing ----- */

    if (((pNew + FLEX_PLU_ADR - 1)->ulRecordNumber !=
         (pPrev + FLEX_PLU_ADR - 1)->ulRecordNumber) ||

        ((pNew + FLEX_PLU_ADR - 1)->uchPTDFlag !=
         (pPrev + FLEX_PLU_ADR - 1)->uchPTDFlag)) {
        PluFinalize();
        PifDeleteFile(szTransPlu);
        PifDeleteFile(szTransPluIndex);     /* Saratoga */
        PifDeleteFile(szTransOep);     /* Saratoga */
        PluInitialize();
        PluCreateFile(FILE_PLU, (pNew + FLEX_PLU_ADR - 1)->ulRecordNumber);
        PluCreateFile(FILE_PLU_INDEX, (pNew + FLEX_PLU_ADR - 1)->ulRecordNumber);
		{
			USHORT  usOepTableNo = 0;
			for (usOepTableNo = 0; usOepTableNo < 2; usOepTableNo++) {
				OpOepCreatFilev221(usOepTableNo, (USHORT)((pNew + FLEX_PLU_ADR - 1)->ulRecordNumber), 0);
			}
		}
    }

#if defined(POSSIBLE_DEAD_CODE)
    /* ----- Check server data changing ----- */
    if (((pNew + FLEX_WT_ADR - 1)->ulRecordNumber !=
         (pPrev + FLEX_WT_ADR - 1)->ulRecordNumber) ||

        ((pNew + FLEX_WT_ADR - 1)->uchPTDFlag !=
         (pPrev + FLEX_WT_ADR - 1)->uchPTDFlag)) {
        PifDeleteFile(szTransWaiter);
        WaiCreatFile((UCHAR)((pNew + FLEX_WT_ADR - 1)->ulRecordNumber));
    }
#endif

    /* ----- Check cashier data changing ----- */
    if (((pNew + FLEX_CAS_ADR - 1)->ulRecordNumber !=
         (pPrev + FLEX_CAS_ADR - 1)->ulRecordNumber)  ||

        ((pNew + FLEX_CAS_ADR - 1)->uchPTDFlag !=
         (pPrev + FLEX_CAS_ADR - 1)->uchPTDFlag)) {
        PifDeleteFile(szTransCashier);
        CasCreatFile((UCHAR)((pNew + FLEX_CAS_ADR - 1)->ulRecordNumber));
    }

    /* ----- Check ETK data changing ----- */
    if ((pNew + FLEX_ETK_ADR - 1)->ulRecordNumber !=
        (pPrev + FLEX_ETK_ADR - 1)->ulRecordNumber) {
        PifDeleteFile(szTransEtk);
        EtkCreatFile((UCHAR)((pNew + FLEX_ETK_ADR - 1)->ulRecordNumber));
    }

    /* ----- check Combination Coupon data changing ----- */
    if (((pNew + FLEX_CPN_ADR - 1)->ulRecordNumber !=
         (pPrev + FLEX_CPN_ADR - 1)->ulRecordNumber) ||
        ((pNew + FLEX_CPN_ADR - 1)->uchPTDFlag !=
         (pPrev + FLEX_CPN_ADR - 1)->uchPTDFlag)) {
        PifDeleteFile(szTransCpn);
        OpCpnCreatFile(
                       (USHORT)((pNew + FLEX_CPN_ADR - 1)->ulRecordNumber),
                       0);
    }

    /* ----- Check Control String data changing ----- */

    if ((pNew + FLEX_CSTRING_ADR - 1)->ulRecordNumber !=
        (pPrev + FLEX_CSTRING_ADR - 1)->ulRecordNumber) {
        PifDeleteFile(szTransCstr);
        OpCstrCreatFile(
                        (USHORT)((pNew + FLEX_CSTRING_ADR - 1)->ulRecordNumber),
                        0);
    }

    /* ----- Create Programmable Report data changing ----- */
    /* always create and delete */
    PifDeleteFile(szTransProgRpt);
    ProgRptCreate((UCHAR)(pNew + FLEX_PROGRPT_ADR - 1)->ulRecordNumber);


    /* ----- Check PPI data changing ----- */
    if ((pNew + FLEX_PPI_ADR - 1)->ulRecordNumber !=
        (pPrev + FLEX_PPI_ADR - 1)->ulRecordNumber) {
        PifDeleteFile(szTransPPI);
        OpPpiCreatFile(
                 (USHORT)((pNew + FLEX_PPI_ADR - 1)->ulRecordNumber),
                 0);
    }

    /* add V3.3 */
    /* ----- Create MLD data changing ----- */
    if ((pNew + FLEX_MLD_ADR - 1)->ulRecordNumber !=
        (pPrev + FLEX_MLD_ADR - 1)->ulRecordNumber) {
        PifDeleteFile(szTransMLD);
        OpMldCreatFile((UCHAR)(pNew + FLEX_MLD_ADR - 1)->ulRecordNumber, 0);
    }

}

TERMINAL getTransTerm(){
	return Trans.Term;
}

const void * getPtrTransTerm(){
	return &Trans.Term;
}

BYTE getTermByPort(){
	return Trans.Term.byPort;
}

void setTermByPort(BYTE byte){
	Trans.Term.byPort = byte;
}

WORD getTermWBaudRate(){
	return Trans.Term.wBaudRate;
}

void setTermWBaudRate(WORD word){
	Trans.Term.wBaudRate = word;
}

BYTE getTermByByteSize(){
	return Trans.Term.byByteSize;
}

BYTE getTermByParity(){
	return Trans.Term.byParity;
}

BYTE getTermByStopBits(){
	return Trans.Term.byStopBits;
}

BYTE getTermByTerm(){
	return Trans.Term.byTerm;
}

void setTermByTerm(BYTE byte){
	Trans.Term.byTerm = byte;
}

BYTE getTermFByStatus(){
	return Trans.Term.fbyStatus;
}

void andTermFByStatus(BYTE byte){
	Trans.Term.fbyStatus &= byte;
}

void orTermFByStatus(BYTE byte){
	Trans.Term.fbyStatus |= byte;
}

short getTermIDir(){
	return Trans.Term.iDir;
}

void incrTermIDir(short num){
	Trans.Term.iDir += num;
}

WORD getTransFwTransTerm(){
	return Trans.fwTransTerm;
}

const void * getPtrTransFwTransTerm(){
	return &Trans.fwTransTerm;
}

void orTransFwTransTerm(WORD word){
	Trans.fwTransTerm |= word;
}

void andTransFwTransTerm(WORD word){
	Trans.fwTransTerm &= word;
}

WORD getTransFwUpdate(){
	return Trans.fwUpdate;
}

/* ===== End of TRANS.C ===== */