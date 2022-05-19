/*************************************************************************
 *
 * UIESCAN.cpp
 *
 * $Workfile:: UIESCAN.cpp                                               $
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
 * $History:: UIESCAN.cpp                                                $
 *
 ************************************************************************/

#include "stdafx.h"
#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "uiescan.h"
#include "Device.h"
#include "DeviceIO.h"

#include "WINIOCTL.H"							// IOCTL header
#include "NCRSCOMM.h"

//      An Overview of Scanners Supported by GenPOS  FSC_SCANNER, PERIPHERAL_DEVICE_SCANNER and SCF_TYPE_SCANNER
//
// Two kinds of scanners are supported. The first is the older NCR scanner or scanner/scale
// which is a legacy from the NCR 2170 and NCR 7448 with NHPOS. The second is the newer
// handheld keyboard wedge type of scanner such as the Symbol hand held scanner.
//
// For older NCR scanner or scanner/scale see CUieScan::UieScannerThread() which contains the
// necessary logic to read in from a COM port scanner data and to then parse it in order to
// read the bar code digits.
//
// For newer USB keyboard wedge type scanners such as the Symbol, that logic is in 
// function CDeviceEngine::ConvertKey(). See also UIE_VIRTUALKEY_EVENT_SCANNER
//
// For the older NCR scanner, the scanner must be provisioned to transform the raw digits of
// the bar code label into an acceptable format for the CUieScan class. This means that:
//   - Enable SANYO ID Characters must be turned on (analyses bar code as to type and adds letter prefix)
//   - provide a single prefix character with the STX (0x02) or Start of Transmission character
//   - provide a single suffix character with the ETX (0x03) or End of Transmission character
//
// For the newer USB keyboard wedge scanner, the scanner must be provisioned to provide a prefix
// character and a suffix character so that the keyboard wedge functionality can isolate the bar
// code digits from the incoming stream of data.
//   - provide a single prefix character of capital letter M (0x4D)
//   - provide a single suffic character of carriage return (0x0D)

/*
*===========================================================================
*   General Declarations
*===========================================================================
*/

// Uncomenet if logs are desired.
//#define DEBUG_COMLOG

static SHORT PifWriteComLog(USHORT usPort, CONST VOID *pBuffer, USHORT usBytes)
{
	FILE *pFile = fopen("scannerlog.txt", "a+");
	SHORT sRet = PifWriteCom(usPort, pBuffer, usBytes);

	if (pFile) {
		char *p = "PifWriteComLog: ";
		fwrite(p, 1, strlen(p), pFile);
		if (sRet < 0) {
			char sBuffer[64] = { 0 };
			sprintf(sBuffer, "* error PifWriteCom() %d\r\n", sRet);
			fwrite(sBuffer, sizeof(unsigned char), strlen(sBuffer), pFile);
		}
		else {
			fwrite(pBuffer, sizeof(unsigned char), usBytes, pFile);
			fwrite("\r\n", 1, 2, pFile);
		}
		fclose(pFile);
	}
	return sRet;

}
static SHORT PifReadComLog(USHORT usPort, VOID *pBuffer, USHORT usBytes)
{
	FILE *pFile = fopen("scannerlog.txt", "a+");
	SHORT sRet = PifReadCom(usPort, pBuffer, usBytes);

	if (pFile) {
		char *p = "PifReadComLog: ";
		fwrite(p, 1, strlen(p), pFile);
		if (sRet < 0) {
			char sBuffer[64] = { 0 };
			sprintf(sBuffer, "* error PifReadCom() %d\r\n", sRet);
			fwrite(sBuffer, sizeof(unsigned char),strlen(sBuffer), pFile);
		}
		else {
			fwrite(pBuffer, sizeof(unsigned char), sRet, pFile);
			fwrite("\r\n", 1, 2, pFile);
		}
		fclose(pFile);
	}

	return sRet;
}

#if defined(DEBUG) && defined(DEBUG_COMLOG)
#pragma message("\n  **** DEBUG_COMLOG:  PifWriteCom() and PifReadCom() logging enabled.\n")
#define PifWriteCom PifWriteComLog
#define PifReadCom PifReadComLog
#endif

// function parameters

typedef struct _DataScanner
{
	DWORD		dwResult;
	USHORT		usLength;
	UCHAR*		pchLabel;
} DataScanner, *PDataScanner;


/*
*===========================================================================
*   Public Functions
*===========================================================================
*/

CUieScan::CUieScan()
{
	m_infoStatus.uchDevice  = 1;
	m_infoStatus.usPort     = 0;
	m_infoStatus.hsHandle   = -1;
	m_infoStatus.sType      = UIE_SCANNER_UNKNOWN;
	m_infoStatus.fchRequest = 0;
	m_infoStatus.fPip       = PIF_COM_PROTOCOL_NON;
	m_infoStatus.usBaud     = UIE_SCANNER1_BAUD;
	m_infoStatus.uchByte    = UIE_SCANNER1_BYTE;
	m_infoStatus.uchText    = PIF_COM_FORMAT_TEXT;
	m_infoStatus.chPhase    = UIE_SCANNER_SCALE_NONE;
	m_infoStatus.usSem      = 0xFFFF;
	m_infoStatus.usSync     = 0xFFFF;
	m_infoStatus.pData      = NULL;
	m_infoStatus.sResult    = 0;

	m_fchUieDevice = 0;
	m_bScale       = FALSE;
	m_bDoMyJob     = TRUE;
}

CUieScan::~CUieScan()
{
}

/*
*===========================================================================
** Synopsis:    BOOL UieScannerInit();
*
** Return:      nothing
*===========================================================================
*/
BOOL CUieScan::UieScannerInit(USHORT usPort, USHORT usBaud, UCHAR uchFormat,
	HANDLE hScanner, BOOL bScale)
{
	m_infoStatus.usPort  = usPort;
	m_infoStatus.usBaud  = usBaud;
	m_infoStatus.uchByte = uchFormat;

	m_hScanner = hScanner;
	m_bScale   = bScale;

	//
	::InitializeCriticalSection(&m_csScanner);

	// create event signals (auto reset & non-signaled initially)
	m_hDone = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	// create a device thread
	m_pThread = AfxBeginThread(DoDeviceThread, this);

	// set his priority higher a little bit
	m_pThread->SetThreadPriority(THREAD_PRIORITY_HIGHEST);

	// do not delete automatically, I'll take care !
	m_pThread->m_bAutoDelete = FALSE;

	// wait for a done signal
	::WaitForSingleObject(m_hDone, INFINITE);
	::ResetEvent(m_hDone);

	return m_bResult;
}


/*
*===========================================================================
** Synopsis:    BOOL UieScannerTerm(VOID);
*
** Return:      nothing
*===========================================================================
*/
BOOL CUieScan::UieScannerTerm(VOID)
{
	DWORD		dwMilliseconds;

	// quit our job !
	m_bDoMyJob = FALSE;

	// wait the thread to the end
#if	!defined(_WIN32_WCE_EMULATION)
	dwMilliseconds = INFINITE;
#else
	// The thread terminate event does not notify correctly
	// on Windows CE 2.0 emulation environment.
	// Change the time-out interval value.
	dwMilliseconds = 100;
#endif
	::WaitForSingleObject(m_hDone, INFINITE);
	::WaitForSingleObject(m_pThread->m_hThread, dwMilliseconds);

	delete m_pThread;						// Add V1.2

	::CloseHandle(m_hDone);

	::DeleteCriticalSection(&m_csScanner);

	return TRUE;
}


/*
*===========================================================================
** Synopsis:    BOOL DLLENTRY UieNotonFile(USHORT usControl)
*               usControl - new scanner status
*                   UIE_ENABLE  - not-on-file disable
*                   UIE_DISABLE - normal mode
*
** Return:      previous scanner status
*               UIE_ENABLE  - scanner not-on-file disable
*               UIE_DISABLE - normal mode
*
** Description: 
*
*               Not-On-File is a message sent to the scanner to trigger
*               a Not On File tone or beep to be played by the scanner.
*               This tone is used to inform the operator that the bar code
*               scanned is not on file or does not exist in the PLU database.
*               The scanner doesn't know if a PLU number exists or not and
*               merely provides the scanned bar code. The POS software has
*               to do the lookup and handle whether the bar code is on file
*               or not.
*===========================================================================
*/
BOOL CUieScan::UieNotonFile(USHORT usControl)
{
    BOOL  bPrevious;

    /* ----- get scanner previous status ----- */
    if (m_fchUieDevice & UIE_ENA_SCANNER) {                         /* enable scanner */
        bPrevious = TRUE;                                           /* normal mode */
    } else {                                                        /* disable scanner */
        bPrevious = FALSE;                                          /* scanner not-on-file disable */
    }

    if (usControl == UIE_ENABLE) {                                  /* not-on-file, scanner disable */
        m_fchUieDevice &= ~UIE_ENA_SCANNER;

        /* ----- reset request status of soft reset and disable ----- */
        m_infoStatus.fchRequest &= ~(UIE_SCANNER_REQ_ENA | UIE_SCANNER_REQ_DIS);

        /* ----- set request status of not-on-file disable ----- */
        m_infoStatus.fchRequest |= UIE_SCANNER_REQ_NOF_ENA;
    } else {                                                        /* soft reset */
        m_fchUieDevice |= UIE_ENA_SCANNER;

        UieScannerEnable();
    }

    return (bPrevious);
}


/*
*===========================================================================
** Synopsis:    BOOL UieScannerEnable(VOID)
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
BOOL CUieScan::UieScannerEnable(VOID)
{
    BOOL    bPrevious;

    /* ----- get scanner previous status ----- */
    if (m_fchUieDevice & UIE_ENA_SCANNER) {                         /* enable scanner */
        bPrevious = TRUE;                                           /* scanner not-on-file disable */
    } else {                                                        /* disable scanner */
        bPrevious = FALSE;                                          /* normal mode */
    }

    m_fchUieDevice |= UIE_ENA_SCANNER;

    /* ----- reset request status of not-on-file disable or disable ----- */
    m_infoStatus.fchRequest &= ~(UIE_SCANNER_REQ_NOF_ENA | UIE_SCANNER_REQ_DIS);

    /* ----- set request status of soft reset ----- */
    m_infoStatus.fchRequest |=  UIE_SCANNER_REQ_ENA;

    return (bPrevious);
}


/*
*===========================================================================
** Synopsis:    BOOL UieScannerDisable(VOID)
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
BOOL CUieScan::UieScannerDisable(VOID)
{
    BOOL    bPrevious;

    /* ----- get scanner previous status ----- */
    if (m_fchUieDevice & UIE_ENA_SCANNER) {                         /* enable scanner */
        bPrevious = TRUE;                                           /* scanner not-on-file disable */
    } else {                                                        /* disable scanner */
        bPrevious = FALSE;                                          /* normal mode */
    }

    m_fchUieDevice &= ~UIE_ENA_SCANNER;

    /* ----- reset request status of not-on-file disable or soft reset ----- */
    m_infoStatus.fchRequest &= ~UIE_SCANNER_REQ_ENA;

    /* ----- set request status of disable ----- */
    m_infoStatus.fchRequest |= UIE_SCANNER_REQ_DIS;

    return (bPrevious);
}


/*
*===========================================================================
** Synopsis:    
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
BOOL CUieScan::UieAddScanner(DWORD dwResult, UCHAR* puchData, USHORT usLength)
{
	PDataScanner	pLabel;

	// allocate a memory and copy the data
	pLabel           = new DataScanner;
	pLabel->dwResult = dwResult;
	pLabel->usLength = usLength;
	pLabel->pchLabel = new UCHAR [usLength];
	memcpy(pLabel->pchLabel, puchData, usLength);

	// add the label data to queue
	::EnterCriticalSection(&m_csScanner);
	m_lstScanner.AddTail(pLabel);
	::SetEvent(m_hScanner);
	::LeaveCriticalSection(&m_csScanner);

	return TRUE;
}


/*
*===========================================================================
** Synopsis:    
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
BOOL CUieScan::UieRemoveScanner(DWORD* pdwResult, UCHAR* puchData, USHORT* pusLength)
{
	PDataScanner	pLabel = NULL;			// assume no data

	// get the label data from queue
	::EnterCriticalSection(&m_csScanner);
	if (! m_lstScanner.IsEmpty())
	{
		pLabel = (PDataScanner)m_lstScanner.RemoveHead();
	}
	::LeaveCriticalSection(&m_csScanner);

	// no data ?
	if (! pLabel)
	{
		return FALSE;						// exit ...
	}

	// copy the label data
	memcpy(puchData, pLabel->pchLabel, pLabel->usLength);
	*pusLength = pLabel->usLength;
	*pdwResult = pLabel->dwResult;

	// free the memory
	delete [] pLabel->pchLabel;
	delete pLabel;

	return TRUE;
}


/*
*===========================================================================
** Synopsis:    
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
VOID CUieScan::UieRemoveAllScanner()
{
	::EnterCriticalSection(&m_csScanner);

	while (! m_lstScanner.IsEmpty())
	{
		PDataScanner	pLabel = (PDataScanner)m_lstScanner.RemoveHead();

		delete [] pLabel->pchLabel;
		delete pLabel;
	}

	::LeaveCriticalSection(&m_csScanner);
}


/*
*===========================================================================
** Synopsis:    SHORT UieReadScale(UIE_SCALE *pData);
*    Output:    pData - address of scale data
*
** Return:      0 >= successful
*               0 <  failer
*===========================================================================
*/
SHORT CUieScan::UieReadScale(UIE_SCALE *pData)
{
#if	defined(_2336_K016)
    return (UIE_SCALE_NOT_PROVIDE);
#endif

    SHORT   sResult = UIE_SCALE_ERROR;
    UIECOM  *pCom;

    if (! m_bScale) {                       /* scale not provide */
        return (UIE_SCALE_NOT_PROVIDE);
    }
    if (pData == NULL) {                    /* invalid argument  */
        return (sResult);
    }

    pCom = &m_infoStatus;                   /* set scale interface */

    PifRequestSem(pCom->usSem);
    if (pCom->chPhase != UIE_SCANNER_SCALE_NONE) {  /* already execute */
        PifReleaseSem(pCom->usSem);
        return (UIE_SCALE_ERROR);
    }
    pCom->chPhase = UIE_SCANNER_SCALE_REQ;  /* set request phase       */
    memset(pData, '\x00', sizeof(UIE_SCALE));
    pCom->pData   = pData;                  /* set data address        */
    pCom->sResult = UIE_SCALE_TIMEOUT;      /* assume timeout status   */
    PifReleaseSem(pCom->usSem);

    PifRequestSem(pCom->usSync);            /* wait for scale response */

    PifRequestSem(pCom->usSem);
    if (pCom->chPhase == UIE_SCANNER_SCALE_RES) {
        sResult = pCom->sResult;                /* set result code     */
    }
    pCom->chPhase = UIE_SCANNER_SCALE_NONE;     /* clear scale request */
    pCom->pData   = NULL;                       /* clear data address  */
    PifReleaseSem(pCom->usSem);

    return (sResult);
}


/*
*===========================================================================
*   Internal Functions
*===========================================================================
*/

UINT CUieScan::DoDeviceThread(LPVOID lpvData)
{
	return ((CUieScan*)lpvData)->UieScannerThread(NULL);
}

/*
*===========================================================================
** Synopsis:    VOID UieScannerThread(UIECOM *pCom);
*     Input:    pCom - address of scanner status
*
** Return:      nothing
*
** Description: This function write scanner data in the ring buffer.
*===========================================================================
*/
UINT CUieScan::UieScannerThread(VOID *pvData)
{
    SHORT       sResult;
    USHORT      usTimeout;
    UCHAR       auchBuffer[UIE_MAX_SCANNER_LEN];
	UIECOM*		pCom   = &m_infoStatus;
	BOOL		bFirst = TRUE;

	// initialize
    pCom->usSem  = PifCreateSem(1);			/* exclusive semaphore   */
    pCom->usSync = PifCreateSem(0);			/* synchronous semaphore */

    while (m_bDoMyJob)
	{
	PROTOCOL    Protocol = {0};

    /* --- open serial port for scanner --- */
    Protocol.fPip             = pCom->fPip;
    Protocol.ulComBaud = pCom->usBaud;
    Protocol.uchComByteFormat = pCom->uchByte;
    Protocol.uchComTextFormat = pCom->uchText;

    pCom->hsHandle = PifOpenCom(pCom->usPort, &Protocol);

	if (bFirst)
	{
		bFirst    = FALSE;
		m_bResult = (pCom->hsHandle >= 0) ? TRUE : FALSE;

		// issue a signal to become ready
		::SetEvent(m_hDone);				// I'm ready to go !

		if (! m_bResult)
			break;							// exit ...

		// set up powered RS232
		m_bPoweredRS232 = FALSE;
		SetPoweredRS232(pCom->hsHandle, TRUE);
	}

    if (pCom->hsHandle < 0) {               /* open error            */
        PifSleep(UIE_SCANNER_OPEN_ERROR);
        continue;                           /* retry ...             */
    }

    /* --- check device status from user --- */
    if (m_fchUieDevice & UIE_ENA_SCANNER) { /* enable scanner        */
        UieScannerEnable();
    } else {                                /* disable scanner       */
        UieScannerDisable();
    }

    /* --- check scale status --- */
    PifRequestSem(pCom->usSem);
    if (pCom->chPhase != UIE_SCANNER_SCALE_NONE &&
        pCom->chPhase != UIE_SCANNER_SCALE_RES) {
        pCom->chPhase = 0;
        pCom->sResult = UIE_SCALE_ERROR;    /* by power failure */
    }
    PifReleaseSem(pCom->usSem);

    /* --- set receive timer --- */
    usTimeout = UIE_SCANNER_READ;
    if (PifControlCom(pCom->hsHandle, PIF_COM_SET_TIME, &usTimeout) != PIF_OK) {
        PifCloseCom(pCom->hsHandle);        /* close port            */
        continue;                           /* re-open port          */
    }

    while (m_bDoMyJob)
	{
        /* --- check scanner control request status --- */
        UieScannerRequestStatus(pCom);

        /* --- check scale control --- */
        if (pCom->chPhase == 0) {           /* fire scale function       */
            PifRequestSem(pCom->usSem);
            pCom->chPhase = UIE_SCANNER_SCALE_RES; /* set response phase */
            PifReleaseSem(pCom->usSync);    /* wake-up user thread       */
            PifReleaseSem(pCom->usSem);
        } else if (pCom->chPhase > 0) {     /* execute scale function    */
            pCom->chPhase--;                /* decrement retry counter   */
        } else if (pCom->chPhase != UIE_SCANNER_SCALE_NONE) {
            /* --- re-check scale request --- */
            PifRequestSem(pCom->usSem);
            if (pCom->chPhase == UIE_SCANNER_SCALE_REQ) {
                pCom->chPhase = UIE_SCANNER_SCALE_EXEC; /* set counter   */
            }
            PifReleaseSem(pCom->usSem);
            sResult = UieScannerScaleRequest(pCom);
        }

#if defined (ORIGINAL_7448)
        /* --- receive scanner data --- */

        sResult = PifReadCom(pCom->hsHandle,
                             auchBuffer,
                             sizeof(auchBuffer));
#else
        /* --- Fix a glitch (06/04/2001)
            PifReadCom will return to caller, even if the scanned data is
            remained in RS232 buffer. The reason why this is that read timer
            is expired before the whole scanned data have been read.  In this
            situation, PifReadCom returns only some bytes of the scanned data.
            These bytes of data will be treated as "Invalid Barcode".
            To avoid this failure, read 1 byte from RS232 until ETX found.
        --- */

        BOOL    IsEtxFound;
		IsEtxFound = FALSE;
		UCHAR i;	//defined here so it can be used after this for loop

        for ( i = 0; i < UIE_MAX_SCANNER_LEN; i++ )
        {
            /* --- read 1 byte from RS232 and store it to saved area --- */
            sResult = PifReadCom( pCom->hsHandle, &auchBuffer[ i ], sizeof( UCHAR ));
            if ( sResult != sizeof( UCHAR ))
            {
                /* --- read operation failed... --- */
                if (( sResult == PIF_ERROR_COM_TIMEOUT ) && ( 0 < i ))
                {
                    /* --- get read bytes to pass it to encoding function --- */
                    sResult = i;
                }

                /* --- Exit read operation because of read error --- */
                break;
            }

            /* --- I have read a byte of scanned data! --- */
            if ( ! IsEtxFound )
            {
                if ( auchBuffer[ i ] != UIE_SCANNER_ETX )
                {
                    /* --- The data is not ETX, read next one --- */
                    continue;
                }
                else
                {
                    /* --- ETX is found! --- */
                    IsEtxFound = TRUE;
                    if ( pCom->sType == UIE_SCANNER_78SSBCC )
                    {
                        /* ---  Read BCC data following ETX --- */
                        continue;
                    }
                }
            }

            /* --- I have read a barcode data completely! --- */
            break;
        }

        /* --- get no.of bytes read and pass it to encoding function --- */
        sResult = ( IsEtxFound && ( sResult == sizeof( UCHAR ))) ? i + 1 : sResult;
#endif
        if ( sResult < 0 )
        {
            switch (sResult) {
            case PIF_ERROR_COM_TIMEOUT:     /* timeout               */
                continue;
            case PIF_ERROR_COM_POWER_FAILURE: /* power down recovery */
                break;
            default:                        /* other error           */
                PifBeep(UIE_ERR_TONE);
                break;
            }
            break;                          /* exit loop             */
        }

        /* --- check read data form scanner --- */
        sResult = UieCheckScannerScale(pCom, auchBuffer, sResult);
    }

	SetPoweredRS232(pCom->hsHandle, FALSE);	// turn off the powered RS232
    PifCloseCom(pCom->hsHandle);            /* close port            */

    }                                       /* endless loop          */

	// finalize
	PifDeleteSem(pCom->usSem);
	PifDeleteSem(pCom->usSync);

	UieRemoveAllScanner();

	::SetEvent(m_hDone);

	PifEndThread();

	return 0;
}


/*
*===========================================================================
** Synopsis:    SHORT UieCheckScannerScale(UIECOM *pCom,
*                                          UCHAR *puchData, SHORT sDataLen)
*     Input:    pCom     - address of scanner status
*               puchData - address of read data from scanner
*               sDataLen - length of read data from scanner
*
** Return:      UIE_SUCCESS - successful
*               UIE_ERROR   - invalid data format
*
** Description: It is judged whether this function is correct the content
*               of the scanner data.
*===========================================================================
*/
SHORT CUieScan::UieCheckScannerScale(UIECOM *pCom, UCHAR *puchData, SHORT sDataLen)
{
    SHORT   sResult, sData;
    USHORT  usLength = 0;
    UCHAR   *puchScan;

	UCHAR   *puchStart;
	UCHAR   *puch;
	USHORT  usCopiedSize;
	USHORT  usRecordLen;

    /* --- scale data ? --- */
    if (*(puchData + 0) == '\x31') {
        sResult = UieScannerScale(pCom, puchData, sDataLen);
        if (sResult == UIE_ERROR) {
            sResult = UieScannerScaleRequest(pCom);
        }
        return (sResult);
    }

    /* --- disable scanner ? --- */
    if (! (m_fchUieDevice & UIE_ENA_SCANNER)) {
        PifBeep(UIE_ERR_TONE);
        return (UIE_ERROR);
    }

#if defined (ORIGINAL_7448)
    /* --- scanner data ? --- */
    if (*puchData == UIE_SCANNER_STX) {     /* hand held scanner      */
        sResult = UieCheck7814(puchData, sDataLen, &puchScan, &usLength);
    } else if (*(puchData + 0) == '\x30' && /* scanner/scale model    */
               *(puchData + 1) == '\x38') { /* tag message format     */
        if (pCom->sType == UIE_SCANNER_78SSBCC &&   /* w/ BCC message */
            UieScannerScaleBCC(puchData, sDataLen) == 0) {
            sData = sDataLen - 3;
        } else {                            /* w/o BCC message        */
            sData = sDataLen - 2;
        }
        sResult = UieCheckNCR(puchData + 2, sData, &puchScan, &usLength);
    } else {                                /* scanner only model    */
        sResult = UieCheckNCR(puchData, sDataLen, &puchScan, &usLength);
    }

    if (sResult == UIE_SUCCESS) {
		UieAddScanner(DEVICEIO_SUCCESS, puchScan, usLength);
    } else {
		UieAddScanner(DEVICEIO_E_FAILURE, NULL, 0);	// for Japan
//		PifBeep(UIE_ERR_TONE);						// for Platform
        PifLog(MODULE_UIE_SCANNER, FAULT_INVALID_DATA);
    }
#else	/* === Fix a glitch (double-scan) for Japan (2000/04/13) ==== */

	/* --- determine scanned data has valid length --- */
	if ( sDataLen < 4 )
	{
		/* --- Code39 requires at least 4 bytes (0x42, 0x31, X1, 0x03) --- */
		UieAddScanner(DEVICEIO_E_FAILURE, NULL, 0);	// for Japan
        PifLog(MODULE_UIE_SCANNER, FAULT_INVALID_DATA);
        return (UIE_ERROR);
	}

	/* --- extract a scanner data from the whole of read data --- */
	usCopiedSize = 0;
	while ( usCopiedSize < sDataLen ) {
		/* --- search for terminator (ETX) of scanned data --- */
		puchStart = puchData + usCopiedSize;
		puch = (UCHAR*)memchr( puchStart, UIE_SCANNER_ETX, sDataLen - usCopiedSize );

		if ( puch != NULL ) {
			/* --- calculate no. of bytes of 1 scan data with ETX --- */
			usRecordLen  = puch - puchStart + 1;

			if (pCom->sType == UIE_SCANNER_78SSBCC) {
				/* --- add 1 byte (BCC message) to length --- */
				usRecordLen++;
			}

			/* --- update total extracted size --- */
			usCopiedSize += usRecordLen;

			if ( *puchStart == UIE_SCANNER_STX ) {
				/* hand held scanner */
				sResult = UieCheck7814(puchStart, usRecordLen, &puchScan, &usLength);
			} else if (( puchStart[0] == '\x30' ) && ( puchStart[1] == '\x38')) {
				/* scanner/scale model */
				/* --- remove first 2 bytes (tag message) from data length --- */
				sData = usRecordLen - 2;
				if ( pCom->sType == UIE_SCANNER_78SSBCC ) {
					/* with BCC message */
					if ( UieScannerScaleBCC(puchStart, usRecordLen) == 0) {
						/* --- remove last 1 byte (BCC) from data length --- */
						sData--;
						sResult = UieCheckNCR(puchStart + 2, sData, &puchScan, &usLength);
					} else {
						sResult = UIE_ERROR;
					}
				} else {
					/* without BCC message        */
					sResult = UieCheckNCR(puchStart + 2, sData, &puchScan, &usLength);
				}
			} else {
			    /* scanner only model    */
				sResult = UieCheckNCR(puchStart, usRecordLen, &puchScan, &usLength);
			}
		} else {
			usCopiedSize = sDataLen;
			sResult = UIE_SCANNER_MISSING_ETX;
			NHPOS_ASSERT_TEXT (puch != NULL, "==ERROR: UIE_SCANNER_ETX character not found in Scanner data.");
		}

		if (sResult == UIE_SUCCESS) {
			UieAddScanner(DEVICEIO_SUCCESS, puchScan, usLength);
		} else {
			UieAddScanner(DEVICEIO_E_FAILURE, NULL, 0);	// for Japan
			PifLog(MODULE_UIE_SCANNER, FAULT_INVALID_DATA);
			PifLog(MODULE_ERROR_NO(MODULE_UIE_SCANNER), (USHORT)abs(sResult));
		}
	}
#endif
    return (sResult);
}


/*
*===========================================================================
** Synopsis:    SHORT UieCheck7814(UCHAR *puchData, SHORT sDataLen,
*                                  UCHAR **puchSca, USHORT *pusScaLen);
*     Input:    puchData - address of read data from scanner
*               sDataLen - length of read data from scanner
*               puchSca  - address of scanner data
*               usScaLen - length of scanner data
*
** Return:      UIE_SUCCESS - successful
*               UIE_ERROR   - invalid data format
*
** Description: It is judged whether this function is correct the content
*               of the scanner data.
*===========================================================================
*/
SHORT CUieScan::UieCheck7814(UCHAR *puchData, SHORT sDataLen,
                   UCHAR **puchSca, USHORT *pusScaLen)
{
    /* --- check STX and ETX character in scanner data --- */
    if (* puchData                 != UIE_SCANNER_STX ||
        *(puchData + sDataLen - 1) != UIE_SCANNER_ETX) {
        return (UIE_ERROR);                 /* error                 */
    }

    switch (*(puchData + 1)) {              /* symbol character      */
    case 'A':                               /* JAN-13, EAN-13, UPC-A */
        if (sDataLen != 1 + 14 + 1) {       /* invalid data length   */
            return (UIE_ERROR);
        }
        break;

    case 'B':                               /* JAN-8, EAN-8          */
        if (sDataLen != 1 + 9 + 1) {        /* invalid data length   */
            return (UIE_ERROR);
        }
        break;

    case 'C':                               /* UPC-E                 */
        if (sDataLen != 1 + 8 + 1) {        /* invalid data length   */
            return (UIE_ERROR);
        }
        break;

    case 'N':                               /* NW-7                  */
    case 'H':                               /* Standard 2 of 5       */
    case 'L':                               /* Code 93               */
    case 'K':                               /* Code 128              */
        if (sDataLen < 1 + 6 + 1) {         /* invalid data length   */
            return (UIE_ERROR);
        }
        break;

    case 'M':                               /* Code 39               */
        if (sDataLen < 1 + 4 + 1) {         /* invalid data length   */
            return (UIE_ERROR);
        }

    case 'I':                               /* Interleaved 2 of 5    */
        if (sDataLen < 1 + 7 + 1) {         /* invalid data length   */
            return (UIE_ERROR);
        }
        break;

	case ']':
		if(*(puchData + 2) == 'e' && *(puchData + 3) == '0'){	//RSS-14
			if(sDataLen < 1 + 16 + 1){
				return (UIE_ERROR);
			}
		}else{
			return (UIE_ERROR);
		}
		break;
    default:                                /* undefined symbol      */
        return (UIE_ERROR);
    }

    /* --- set scanner data without STX and ETX --- */
    *puchSca   = puchData + 1;            /* address of scanner data */
    *pusScaLen = (USHORT)(sDataLen - 2);  /* length of scanner data  */

    return (UIE_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    SHORT UieCheckNCR(UCHAR *puchData, SHORT sDataLen,
*                                 UCHAR **puchSca, USHORT *pusScaLen);
*     Input:    puchData - address of read data from scanner
*               sDataLen - length of read data from scanner
*               puchSca  - address of scanner data
*               usScaLen - length of scanner data
*
** Return:      UIE_SUCCESS - successful
*               UIE_ERROR   - invalid data format
*
** Description: It is judged whether this function is correct the content
*               of the scanner data.
*===========================================================================
*/
SHORT CUieScan::UieCheckNCR(UCHAR *puchData, SHORT sDataLen,
                  UCHAR **puchSca, USHORT *pusScaLen)
{
    /* --- check EXT character in scanner data --- */
    if (*(puchData + sDataLen - 1) != UIE_SCANNER_ETX) {
        return (UIE_SCANNER_MISSING_ETX);
    }

    switch (*puchData) {                    /* symbol character      */
    case 'A':                               /* UPC-A                 */
        if (sDataLen != 13 + 1) {           /* invalid data length   */
            return (UIE_SCANNER_DATALENGTH);
        }
        break;

    case 'E':                               /* UPC-E                 */
        if (sDataLen != 8 + 1) {            /* invalid data length   */
            return (UIE_SCANNER_DATALENGTH);
        }
        break;

    case 'F':
        if (*(puchData + 1) == 'F') {       /* JAN-8, EAN-8          */
            if (sDataLen != 10 + 1) {       /* invalid data length   */
                return (UIE_SCANNER_DATALENGTH);
            }
        } else {                            /* JAN-13, EAN-13        */
            if (sDataLen != 14 + 1) {       /* invalid data length   */
                return (UIE_SCANNER_DATALENGTH);
            }
        }
        break;

    case 'B':
        if (*(puchData + 1) == '1' ||       /* Code 39               */
            *(puchData + 1) == '2' ||       /* Interleaved 2 of 5    */
            *(puchData + 1) == '3') {       /* Code 128              */
            if (sDataLen < 3 + 1) {         /* invalid data length   */
                return (UIE_SCANNER_DATALENGTH);
            }
        }
        break;

	case ']':
		if(*(puchData + 1) == 'e' && *(puchData + 2) == '0'){			// RSS-14/RSS-14 Expanded
			if(sDataLen < 3 + 16 + 1 || sDataLen > 3 + 74 + 1){//3 for prefix 1 for ETX
				return UIE_SCANNER_DATALENGTH;			// invalid data length
			}
		}else{
			return UIE_ERROR;
		}
		break;

    default:                                /* undefined symbol      */
        return (UIE_ERROR);
    }

    /* --- set scanner data without ETX --- */
    *puchSca   = puchData;                /* address of scanner data */
    *pusScaLen = (USHORT)(sDataLen - 1);  /* length of scanner data  */

    return (UIE_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    VOID UieScannerRequestStatus(UIECOM *pCom)
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
VOID CUieScan::UieScannerRequestStatus(UIECOM *pCom)
{
#if	defined(_2336_K016)
	return;
#endif

    SHORT   sResult;
    USHORT  usTimeout = 0;
    static UIESCANFC FARCONST aNotonFile[] =
                {{3, "\x33\x46\x03"},       /* 78xx scanner only          */
                 {3, "\x30\x35\x03"},       /* 78xx scanner/scale w/o BCC */
                 {4, "\x30\x35\x03\x06"},   /* 78xx scanner/scale w/  BCC */
                 {3, "\x33\x46\x03"}};      /* 7852 scanner               */
    static UIESCANFC FARCONST aDisable[] =
                {{3, "\x32\x42\x03"},       /* 78xx scanner only          */
                 {3, "\x30\x36\x03"},       /* 78xx scanner/scale w/o BCC */
                 {4, "\x30\x36\x03\x05"},   /* 78xx scanner/scale w/  BCC */
                 {3, "\x32\x42\x03"}};      /* 7852 scanner               */
    static UIESCANFC FARCONST aEnable[] =
                {{3, "\x32\x30\x03"},       /* 78xx scanner only          */
                 {3, "\x30\x31\x03"},       /* 78xx scanner/scale w/o BCC */
                 {4, "\x30\x31\x03\x02"},   /* 78xx scanner/scale w/  BCC */
                 {3, "\x36\x30\x03"}};      /* 7852 scanner               */

    /* ----- check scanner enable ----- */
    if (pCom->fchRequest & UIE_SCANNER_REQ_ENA) {
        pCom->fchRequest &= ~UIE_SCANNER_REQ_ENA;
        if (pCom->sType <= UIE_SCANNER_UNKNOWN) {
            pCom->sType = UieScannerType(pCom->hsHandle);
        }
        UieScannerCommand(pCom, &aEnable[pCom->sType]);
        usTimeout = UIE_SCANNER_READ;       /* set timer value */
    }

    /* ----- check scanner disable ----- */
    if (pCom->fchRequest & UIE_SCANNER_REQ_DIS) {
        pCom->fchRequest &= ~UIE_SCANNER_REQ_DIS;
        pCom->sType = UieScannerType(pCom->hsHandle);
        UieScannerCommand(pCom, &aDisable[pCom->sType]);
        usTimeout = UIE_SCANNER_MONITOR;    /* set timer value */
    }

    /* ----- check not-on-file disable ----- */
    if (pCom->fchRequest & UIE_SCANNER_REQ_NOF_ENA) {
        pCom->fchRequest &= ~UIE_SCANNER_REQ_NOF_ENA;
        pCom->sType = UieScannerType(pCom->hsHandle);
        UieScannerCommand(pCom, &aNotonFile[pCom->sType]);
        usTimeout = UIE_SCANNER_MONITOR;    /* set timer value */
    }

    if (usTimeout != 0) {
        /* --- set receive timer --- */
        sResult = PifControlCom(pCom->hsHandle, PIF_COM_SET_TIME, &usTimeout);
    }
}


/*
*===========================================================================
** Synopsis:    VOID UieScannerCommand(UIECOM *pCom, UIESCANFC FAR *pCommand)
*     Input:    pCom - address of scanner status
*               pCommand - scanner control command
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
VOID CUieScan::UieScannerCommand(CONST UIECOM *pCom, CONST UIESCANFC *pCommand)
{
    CHAR    ichRetry;
    SHORT   sResult;
    USHORT  usTimeout;
    UCHAR   auchResponse[10];

    /* ----- check scanner type ----- */
    if (pCom->sType <= UIE_SCANNER_UNKNOWN ||   /* unknown scanner type or error */
        pCom->sType == UIE_SCANNER_7814) {      /* scanner command not support   */
        return;
    }

    /* ----- set receive timer ----- */
    usTimeout = UIE_SCANNER_RESPONSE;
    if (PifControlCom((USHORT)pCom->hsHandle, PIF_COM_SET_TIME, &usTimeout) != PIF_OK) {
        return;                                 /* power failure ? */
    }

    /* ----- send scanner control command ----- */
    for (ichRetry = 3; ichRetry >= 0; ichRetry--) {
        sResult = PifWriteCom((USHORT)pCom->hsHandle, pCommand->puchCode, pCommand->sLength);
        if (sResult == PIF_ERROR_COM_POWER_FAILURE) {
            return;
        } else if (sResult == pCommand->sLength) {
#if defined (ORIGINAL_7448)
            /* ----- receive response command ----- */
            sResult = PifReadCom((USHORT)pCom->hsHandle,
                                 auchResponse,
                                 sizeof(auchResponse));
            return;
#else       
            /* --- Fix a glitch (06/04/2001)
                Some type of scanner will send response data to caller when
                enable or disable commands are received.  To avoild reading
                the response data as "Barcode", the response should be remove
                from RS232 buffer. 
                If scanner sends response data, read 1 byte from RS232 until
                ETX is found. Otherwise, read operation is exited with TIMEOUT.
            --- */

            BOOL    IsEtxFound;
			IsEtxFound = FALSE;

            for ( UCHAR i = 0 ; i < sizeof( auchResponse ); i++ )
            {
                /* --- read 1 byte from RS232 and store it --- */
                sResult = PifReadCom( pCom->hsHandle, &auchResponse[ i ], sizeof( UCHAR ));

                if ( sResult != sizeof( UCHAR ))
                {
                    /* --- No response, and exit this function --- */
                    break;
                }

                /* --- I have read a byte of scanned data! --- */
                if ( ! IsEtxFound )
                {
                    if ( auchResponse[ i ] != UIE_SCANNER_ETX )
                    {
                        /* --- The data is not ETX, read next one --- */
                        continue;
                    }
                    else
                    {
                        /* --- ETX is found! --- */
                        IsEtxFound = TRUE;
                        if ( pCom->sType == UIE_SCANNER_78SSBCC )
                        {
                            /* ---  Read BCC data following ETX --- */
                            continue;
                        }
                    }
                }
    
                /* --- I have read a response data completely! --- */
                break;
            }

            /* --- exit the function to caller --- */
			return;
#endif
        }
    }
}


/*
*===========================================================================
** Synopsis:    SHORT UieScannerType(SHORT hsHandle)
*     Input:    hsHandle - communication handle
*
** Return:      0  >= - scanner type
*                       0 - 7870/7875/7880 scanner only or 7890
*                       1 - 7870/7875/7880 scanner/scale
*                       2 - 7852 scanner
*                       3 - 7814 hand held scanner
*               0 <   - error
*                       UIE_SCANNER_UNKNOWN - unknown type
*
** Description: 
*===========================================================================
*/
SHORT CUieScan::UieScannerType(SHORT hsHandle)
{
    SHORT   sResult;
    SHORT   isType;
    USHORT  usTimeout;
    CHAR    ichRetry;
    UCHAR   auchResponse[10];
    static UCHAR FARCONST auch7852EM[] =
                "ENDUNITTEST\x03";          /* 7852 enable command */
    static UIESCANFC FARCONST aStatus[] =
                {{3, "\x33\x36\x03"    },   /* 78xx scanner only          */
                 {3, "\x30\x33\x03"    },   /* 78xx scanner/scale w/o BCC */
                 {4, "\x30\x33\x03\x00"},   /* 78xx scanner/scale w/  BCC */
                 {3, "\x46\x36\x03"    }};  /* 7852 scanner               */

    /* ----- set receive timer ----- */
    usTimeout = UIE_SCANNER_RESPONSE;
    if (PifControlCom(hsHandle, PIF_COM_SET_TIME, &usTimeout) != PIF_OK) {
        return (PIF_ERROR_COM_POWER_FAILURE);
    }

    for (isType = 0; isType < 4; isType++) {
        /* ----- send status request command ----- */
        for (ichRetry = 1; ichRetry >= 0; ichRetry--) {
            if (isType == UIE_SCANNER_7852) {                       /* 7852 */
                /* ----- send scanner enable message command ----- */
                PifWriteCom((USHORT)hsHandle, auch7852EM, sizeof(auch7852EM) - 1);                /* w/o NULL */
            }
            sResult = PifWriteCom((USHORT)hsHandle, aStatus[isType].puchCode, aStatus[isType].sLength);
            if (sResult == aStatus[isType].sLength) {               /* send successful */
                ;
            } else if (sResult == PIF_ERROR_COM_POWER_FAILURE) {    /* power down recovery */
                return (sResult);
            } else if (ichRetry == 0) {                             /* send error */
                PifLog(MODULE_UIE_SCANNER, FAULT_BAD_ENVIRONMENT);
                return (UIE_SCANNER_UNKNOWN);
            }

#if defined (ORIGINAL_7448)
            /* ----- receive response command ----- */
            if ((sResult = PifReadCom((USHORT)hsHandle,
                                      auchResponse,
                                      sizeof(auchResponse))) >= 3) {
                return (isType);
            } else if (sResult == PIF_ERROR_COM_POWER_FAILURE) {
                return (sResult);
            }
#else
            /* --- Fix a glitch (06/04/2001)
                If scanner receives the valid status request, it will respond
                the status to caller. To avoild reading the status data as
                "Barcode", the status should be remove from RS232 buffer.
                If scanner sends status data, read 1 byte from RS232 until
                ETX is found. Otherwise, read operation is exited with TIMEOUT.
            --- */

            BOOL    IsEtxFound;
			IsEtxFound = FALSE;

            for ( UCHAR i = 0; i < sizeof( auchResponse ); i++ )
            {
                /* --- read 1 byte from RS232 and store it --- */
                sResult = PifReadCom( hsHandle, &auchResponse[ i ], sizeof( UCHAR ));
                if ( sResult != sizeof( UCHAR ))
                {
                    /* --- Read operation is failed... --- */
                    if (( sResult == PIF_ERROR_COM_TIMEOUT ) && ( 0 < i ))
                    {
                        /* --- Get no. of read bytes --- */
                        sResult = i;
                    }

                    /* --- Exit read operation because of error --- */
                    break;
                }

                /* --- I have read a byte of status data! --- */
                if ( ! IsEtxFound )
                {
                    if ( auchResponse[ i ] != UIE_SCANNER_ETX )
                    {
                        /* --- The data is not ETX, read next one --- */
                        continue;
                    }
                    else
                    {
                        /* --- ETX is found! --- */
                        IsEtxFound = TRUE;

                        if ( isType == UIE_SCANNER_78SSBCC )
                        {
                            /* ---  Read BCC data following ETX --- */
                            continue;
                        }
                    }
                }

                /* --- I have read a status data completely! --- */
                break;
            }

            /* --- If some status is read, returns scanner type --- */
            if ( 0 < sResult )
            {
			    return ( isType );
            }
#endif
        }
    }

    return (UIE_SCANNER_7814);
}


/*
*===========================================================================
** Synopsis:    SHORT UieScannerScale(UIECOM *pCom, UCHAR *puchData,
*                                     SHORT sLength)
*     Input:    pCom     - address of scanner status
*               puchData - address of receive data
*               sLength  - length of receive data
*
** Return:      UIE_SUCCESS    -
*               UIE_ERROR      -
*               UIE_ERROR_NEXT - not monitor response
*
** Description: 
*===========================================================================
*/
SHORT CUieScan::UieScannerScale(UIECOM *pCom, UCHAR *puchData, SHORT sLength)
{
    SHORT   sIndex;
    UCHAR   uchData;
    ULONG   ulData = 0;

    if (*(puchData + 1) != '\x34') {        /* not monitor response    */
        return (UIE_ERROR_NEXT);
    }

	if (sLength < 4) {						/* length error            */
		return (UIE_ERROR);
	}

    /* --- check BCC --- */
	if (((sLength == 5) ||					/* status with BCC         */
		 (sLength == 9  && *(puchData + sLength - 2) == UIE_SCANNER_ETX) ||	/* 4digits w/ BCC */
		 (sLength == 10)) &&				/* 5digits with BCC        */
		(UieScannerScaleBCC(puchData, sLength) != 0)) {	/* invalid BCC */
        return (UIE_ERROR);
	}

    if (sLength == 4 || sLength == 5) {     /* without weight data     */
        switch (*(puchData + 2)) {
        case '\x31':                        /* scale unstable          */
            pCom->sResult = UIE_SCALE_MOTION;
            break;
        case '\x32':                        /* scale over capacity     */
            pCom->sResult = UIE_SCALE_OVER_CAPACITY;
            break;
        case '\x33':                        /* scale zero weight       */
            /* --- bug fix, Jul-04-97 --- */
            PifRequestSem(pCom->usSem);
            if (pCom->chPhase >= 0) {       /* execute scale function  */
                pCom->chPhase       = 0;
                pCom->pData->ulData = 0U;
                pCom->sResult       = UIE_SUCCESS;
            }
            PifReleaseSem(pCom->usSem);
            return (UIE_SUCCESS);
        case '\x35':                        /* scale under zero weight */
            pCom->sResult = UIE_SCALE_UNDER_CAPACITY;
            break;
        default:
            pCom->sResult = UIE_SCALE_ERROR;
        }
        return (UIE_ERROR);
    }

    if ((sLength != 8 && sLength != 9 && sLength != 10) ||	/* invalid data length */
        *(puchData + 2) != '\x34') {        /* not weight data     */
        return (UIE_ERROR);
    }

    for (sIndex = 3; sIndex < 8; sIndex++) {
        uchData = *(puchData + sIndex);
		if (uchData == UIE_SCANNER_ETX && sIndex >= 7) {
			break;
		}
		if (uchData < '0' || '9' < uchData) {
            return (UIE_ERROR);
        }
        ulData = (ulData * 10) + (uchData - 0x30);
    }

    PifRequestSem(pCom->usSem);
    if (pCom->chPhase >= 0) {               /* execute scale function */
        pCom->chPhase       = 0;
        pCom->pData->ulData = ulData;
        pCom->sResult       = UIE_SUCCESS;
    }
    PifReleaseSem(pCom->usSem);

    return (UIE_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    SHORT UieScannerScaleRequest(UIECOM *pCom)
*     Input:    pCom - address of scanner status
*
** Return:      UIE_SUCCESS -
*               UIE_ERROR   -
*
** Description: 
*===========================================================================
*/
SHORT CUieScan::UieScannerScaleRequest(UIECOM *pCom)
{
    CHAR    ichRetry;
    SHORT   sResult, sIndex, sStatus = FALSE;
    USHORT  usTimeout;
    UCHAR   auchResponse[16];
    static UIESCANFC FARCONST aStatus[] =
                {{3, "\x31\x33\x03"},       /* w/o BCC */
                 {4, "\x31\x33\x03\x01"}};  /* w/  BCC */
    static UIESCANFC FARCONST aMonitor[] =
                {{3, "\x31\x34\x03"},       /* w/o BCC */
                 {4, "\x31\x34\x03\x06"}};  /* w/  BCC */

    if (pCom->chPhase <= 0) {               /* do not request */
        return (UIE_ERROR);                 /* exit ... */
    }

    /* --- check scanner/scale model --- */
    switch (pCom->sType) {
    case UIE_SCANNER_78SS:      sIndex = 0; break;
    case UIE_SCANNER_78SSBCC:   sIndex = 1; break;
    default:
        return (UIE_ERROR);                 /* exit ... */
    }

    if (pCom->pData->uchUnit == 0) {        /* unknown scale uint */
        /* --- set receive timer --- */
        usTimeout = UIE_SCANNER_RESPONSE;
        if (PifControlCom((USHORT)pCom->hsHandle, PIF_COM_SET_TIME, &usTimeout) != PIF_OK) {
            return (UIE_ERROR);             /* power failure ? */
        }

        /* --- send scale status command --- */
        for (ichRetry = 3; ichRetry >= 0; ichRetry--) {
            sResult = PifWriteCom((USHORT)pCom->hsHandle, aStatus[sIndex].puchCode, aStatus[sIndex].sLength);
            if (sResult == PIF_ERROR_COM_POWER_FAILURE) {
                return (UIE_ERROR);         /* exit ... */
            } else if (sResult != aStatus[sIndex].sLength) {
                continue;                   /* retry ... */
            }

            /* --- receive response command --- */
            sResult = PifReadCom((USHORT)pCom->hsHandle, auchResponse, sizeof(auchResponse));
            if (sResult == PIF_ERROR_COM_POWER_FAILURE) {
                return (UIE_ERROR);         /* exit ... */
            } else if (sResult == 9) {      /* w/  BCC response */
                if (UieScannerScaleBCC(auchResponse, sResult) != 0) {
                    continue;               /* retry ... */
                }
            } else if (sResult != 8) {      /* w/o BCC response */
                continue;                   /* retry ... */
            }

            /* --- check scale response --- */
            if (auchResponse[0] != '\x31' ||/* not scale response  */
                auchResponse[1] != '\x33') {/* not status response */
                continue;                   /* retry ...           */
            }

            if (auchResponse[2] == '\x30') {
                pCom->pData->uchUnit = UIE_SCALE_UNIT_LB;
                pCom->pData->uchDec  = 2;
            } else if (auchResponse[2] == '\x31') {
                pCom->pData->uchUnit = UIE_SCALE_UNIT_KG;
                pCom->pData->uchDec  = 3;
            } else {                        /* invalid unit */
                continue;                   /* retry ...    */
            }
            break;
        }
    }

    /* --- set receive timer --- */
    usTimeout = UIE_SCANNER_READ;
    if (PifControlCom((USHORT)pCom->hsHandle, PIF_COM_SET_TIME, &usTimeout) != PIF_OK) {
        return (UIE_ERROR);                 /* power failure ? */
    }

    /* --- re-check scale unit --- */
    if (pCom->pData->uchUnit == 0) {        /* unknown scale uint */
        return (UIE_ERROR);                 /* exit ... */
    }

    /* --- send scale monitor command --- */
    for (ichRetry = 3; ichRetry >= 0; ichRetry--) {
        sResult = PifWriteCom((USHORT)pCom->hsHandle, aMonitor[sIndex].puchCode, aMonitor[sIndex].sLength);
        if (sResult == PIF_ERROR_COM_POWER_FAILURE) {
            return (UIE_ERROR);             /* exit ... */
        } else if (sResult == aMonitor[sIndex].sLength) {
            return (UIE_SUCCESS);
        }
    }

    return (UIE_ERROR);
}


/*
*===========================================================================
** Synopsis:    UCHAR UieScannerScaleBCC(UCHAR *puchData, SHORT sLength)
*     Input:    puchData - address of data
*               sLength  - length of data
*
** Return:      caluclation result of BCC
*
** Description: 
*===========================================================================
*/
UCHAR CUieScan::UieScannerScaleBCC(UCHAR *puchData, SHORT sLength)
{
    UCHAR   uchBCC = 0;
    SHORT   sIndex;

    for (sIndex = 0; sIndex < sLength; sIndex++) {
        uchBCC ^= *(puchData + sIndex);
    }

    return (uchBCC);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function: Set powered RS232
//
// Prototype:	BOOL	CUieScan::SetPoweredRS232(BOOL fState);
//
// Inputs:		BOOL	fState;			- enable or disable
//
// Outputs:		BOOL	fSuccess;		- success or failed
//
/////////////////////////////////////////////////////////////////////////////

BOOL CUieScan::SetPoweredRS232(USHORT usPort, BOOL fState)
{
	BOOL	fResult = TRUE;

//#if	defined(_2336_K016)
	if ((! fState) && (! m_bPoweredRS232))
	{
		// we do not change the powered rs232 state.
		return fResult;
	}

	HANDLE	hHandle = NULL;// = PifGetComRowHandle(usPort);

	if (hHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if (fState)
	{
		DWORD	dwModemStat;

		fResult = ::GetCommModemStatus(hHandle, &dwModemStat);
		if (fResult && ! (dwModemStat & MS_RING_ON))
		{
			m_bPoweredRS232 = TRUE;			// changed Powered RS232 state
		}
	}

	SERIAL_SET_POWER	Power;
	DWORD				dwByteReturned;

	Power.dwMode = (fState) ? SERIAL_POWER_ENABLE : SERIAL_POWER_DISABLE;

	fResult = ::DeviceIoControl(
					hHandle,
					IOCTL_SERIAL_SET_POWER,
					&Power,
					sizeof(Power),
					NULL,
					0,
					&dwByteReturned,
					NULL);
//#endif

	return fResult;
}


/* ====================================== */
/* ========== End of UieScan.C ========== */
/* ====================================== */
