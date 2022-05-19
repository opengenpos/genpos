/*************************************************************************
 *
 * UIESCAN.h
 *
 * $Workfile:: UIESCAN.h                                                 $
 *
 * $Revision::                                                           $
 *
 * $Archive::                                                            $
 *
 * $Date::                                                               $
 *
 *************************************************************************
 *
 * Copyright (c) NCR Corporation 2000.  All rights reserved.
 *
 *************************************************************************
 *
 * $History:: UIESCAN.h                                                  $
 *
 * Oct-27-15  GenPOS 2.2.1 R.Chambers  Get rid of old defines and other cruft,
*                                     move as much as possible into specific device
*                                     interface source files.
************************************************************************/

#if !defined(_UIE_SCAN_)
#define _UIE_SCAN_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
*===========================================================================
*   Public Define Declarations
*===========================================================================
*/

#include <uie.h>

/*
*===========================================================================
*   scanner interface   FSC_SCANNER, PERIPHERAL_DEVICE_SCANNER and SCF_TYPE_SCANNER
*
*   See also the definitions in the UIE subsystem of BusinessLogic which is
*   the user interface and which has similar definitions that need to match
*   these definitions. See file BUSINESSLOGIC\Sa\UIE\uieio.h
*
*===========================================================================
*/

#define UIE_ERR_TONE            100         /* length of error tone */

#define UIE_TOTAL_SCANNER_LEN   (1 + UIE_LEN_SCANNER + 1)   /* with STX and ETX */
#define UIE_NUMBER_SCANNER      1
#define UIE_MAX_SCANNER_LEN     (UIE_TOTAL_SCANNER_LEN * UIE_NUMBER_SCANNER)

#define UIE_SCANNER1_BAUD       9600        /* Baud Rate                  */
#define UIE_SCANNER1_BYTE       0x0A        /* Odd, 1 Stop, 7 Data        */

#define UIE_SCANNER_STX         0x02        /* STX Code                   */
#define UIE_SCANNER_ETX         0x03        /* ETX Code                   */

#define UIE_SCANNER_OPEN_ERROR  2000        /* sleep timer after open error     */
#if defined (ORIGINAL_7448)
#define UIE_SCANNER_READ        220         /* read timer of scanner data       */
// 11/22/2001 for Win32 Program
#elif defined (_WIN32)
#define UIE_SCANNER_READ		25			/* read timer of scanner data       */
#else
#define UIE_SCANNER_READ        10          /* read timer of scanner data       */
#endif
#define UIE_SCANNER_MONITOR     200         /* monitor timer of scanner disable */
#define UIE_SCANNER_RESPONSE    50          /* response timer                   */

#define UIE_SCANNER_UNKNOWN     (-1)        /* unknown scanner type       */
#define UIE_SCANNER_78SO        0           /* NCR 78xx scanner only      */
#define UIE_SCANNER_78SS        1           /* NCR 78xx scanner/scale     */
#define UIE_SCANNER_78SSBCC     2           /* NCR 78xx scanner/scale BCC */
#define UIE_SCANNER_7852        3           /* NCR 7852                   */
#define UIE_SCANNER_7814        4           /* NCR 7814                   */

#define UIE_SCANNER_REQ_ENA     0x01        /* request scanner enable     */
#define UIE_SCANNER_REQ_DIS     0x02        /* request scanner disable    */
#define UIE_SCANNER_REQ_NOF_ENA 0x04        /* request scanner NOF enable */

#define UIE_SCANNER_SCALE_EXEC  (1 + 1000 / UIE_SCANNER_READ)
#define UIE_SCANNER_SCALE_NONE  (-1)        /* no scale request           */
#define UIE_SCANNER_SCALE_REQ   (-2)        /* wait for request data      */
#define UIE_SCANNER_SCALE_RES   (-3)        /* wait for response data     */

typedef struct _UIECOM {
    UCHAR   uchDevice;                      /* device number              */
    USHORT  usPort;                         /* port number                */
    SHORT   hsHandle;                       /* handle of port             */
    SHORT   sType;                          /* type of scanner            */
    UCHAR   fchRequest;                     /* request status             */
    SHORT   fPip;                           /* type of protocol           */
    USHORT  usBaud;                         /* baud rate                  */
    UCHAR   uchByte;                        /* byte format                */
    UCHAR   uchText;                        /* text format                */
    CHAR    chPhase;                        /* phase of scale function    */
    USHORT  usSem;                          /* exclusive semaphore        */
    USHORT  usSync;                         /* synchronous semaphore      */
    UIE_SCALE   *pData;                     /* address of scale data      */
    SHORT   sResult;                        /* result code of scale       */
} UIECOM;

typedef struct _UIESCANFC {
    SHORT   sLength;                        /* length of function code    */
    CHAR   *puchCode;                       /* function code              */
} UIESCANFC;


/////////////////////////////////////////////////////////////////////////////

class CUieScan
{
public:
	CUieScan();
	virtual ~CUieScan();

public:
	BOOL    UieScannerInit(USHORT usPort, USHORT usBaud, UCHAR uchFormat, HANDLE hScanner, BOOL bScale);
	BOOL    UieScannerTerm(VOID);
	BOOL    UieNotonFile(USHORT usControl);
	BOOL    UieScannerEnable(VOID);
	BOOL    UieScannerDisable(VOID);
	BOOL    UieAddScanner(DWORD dwResult, UCHAR* puchData, USHORT usLength);
	BOOL	UieRemoveScanner(DWORD* pdwResult, UCHAR* puchData, USHORT* pusLength);
	VOID    UieRemoveAllScanner();
	SHORT   UieReadScale(UIE_SCALE *pData);

private:
	static UINT	DoDeviceThread(LPVOID pParam);
	UINT		UieScannerThread(VOID *pvData);

protected:
	SHORT   UieCheckScannerScale(UIECOM *pCom, UCHAR *puchData, SHORT sDataLen);
	SHORT   UieCheck7814(UCHAR *puchData, SHORT sDataLen,
                     UCHAR **puchSca, USHORT *pusScaLen);
	SHORT   UieCheckNCR(UCHAR *puchData, SHORT sDataLen,
                    UCHAR **puchSca, USHORT *pusScaLen);
	VOID    UieScannerRequestStatus(UIECOM *pCom);
	VOID    UieScannerCommand(CONST UIECOM *pCom, CONST UIESCANFC  *pCommand);
	SHORT   UieScannerType(SHORT hsHandle);
	SHORT   UieScannerScale(UIECOM *pCom, UCHAR *puchData, SHORT sLength);
	SHORT   UieScannerScaleRequest(UIECOM *pCom);
	UCHAR   UieScannerScaleBCC(UCHAR *puchData, SHORT sLength);
	BOOL	SetPoweredRS232(USHORT usPort, BOOL fState);

protected:
	UIECOM				m_infoStatus;
//	UIECOM*				m_pSScale;
	USHORT				m_fchUieDevice;
	BOOL				m_bScale;
	BOOL				m_bResult;
	BOOL				m_bDoMyJob;
	HANDLE				m_hScanner;
	HANDLE				m_hDone;
	CWinThread*			m_pThread;
	CPtrList			m_lstScanner;
	CRITICAL_SECTION	m_csScanner;
	BOOL				m_bPoweredRS232;
};


/////////////////////////////////////////////////////////////////////////////

#endif // !defined(_UIE_SCAN_)
