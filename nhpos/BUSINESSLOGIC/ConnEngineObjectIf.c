#include	<windows.h>
#include	<tchar.h>
#include	<memory.h>
#include	<stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <plu.h>
#include    <paraequ.h>
#include    <para.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <storage.h>
#include    <csgcs.h>
#include    <csop.h>
#include    <prtprty.h>
#include	<ConnEngineObjectIf.h>
#include	<ConnEngine.h>
#include    <trans.h>
#include    <rfl.h>
#include    <csetk.h>
#include    <eept.h>
#include    "csstbfcc.h"
#include    "csstbopr.h"
#include    "uie.h"
#include    "pifmain.h"
#include    "bl.h"


#define PUTSTRUCTMEMBER(s,p,m,f) {\
	int iLen=0;\
	CHAR *pcLabel="" #m "";\
	iLen=_stprintf(s,f,pcLabel,(p)->m,pcLabel);\
	s+=iLen;\
	}

#define PUTSTRINGLABEL(s,p,m,f) {\
	int iLen=0;\
	CHAR *pcLabel="" #m "";\
	iLen=_stprintf(s,f,pcLabel,(p),pcLabel);\
	s+=iLen;\
	}

#if defined(DCURRENCY_LONGLONG)
static TCHAR  *pFormatDcurrency = _T("<%S>%lld</%S>\r");
#else
static TCHAR  *pFormatDcurrency = _T("<%S>%ld</%S>\r");
#endif
static TCHAR  *pFormatNumeric = _T("<%S>%d</%S>\r");
static TCHAR  *pFormatNumericHex = _T("<%S>x%x</%S>\r");
static TCHAR  *pFormatString = _T("<%S>%s</%S>\r");

static TCHAR  *pTokenDelimitersTag = _T("<> =\t\n\r");
static TCHAR  *pTokenDelimitersValue = _T("<>");

static CRITICAL_SECTION    g_ConnEngineCriticalSection;

static USHORT   ConnEngineObjectEchoElectronicJournal = 0;


SHORT ConnEngineObjectEchoElectronicJournalSetting (USHORT usConnEngineObjectEchoElectronicJournalFlag)
{
	if (usConnEngineObjectEchoElectronicJournalFlag) {
		ConnEngineObjectEchoElectronicJournal = 1;
	} else {
		ConnEngineObjectEchoElectronicJournal = 0;
	}

	return 0;
}

SHORT ConnEngineInitCriticalRegion (USHORT usConnEngineObjectEchoElectronicJournalFlag)
{
	if (usConnEngineObjectEchoElectronicJournalFlag) {
		ConnEngineObjectEchoElectronicJournal = 1;
	}

	if (ConnEngineObjectEchoElectronicJournal == 1) {
		SHORT      shFileHandle;
		TCHAR      *szConnEngineElectronicJournal = _T("TOTALEJG");

		NHPOS_NONASSERT_TEXT("-- TOTALEJG is turned on.");
		shFileHandle = PifOpenFile (szConnEngineElectronicJournal, "nw");
		if (shFileHandle >= 0)
			PifCloseFile (shFileHandle);
	} else {
		NHPOS_NONASSERT_TEXT("-- TOTALEJG is turned off.");
	}

    InitializeCriticalSection(&g_ConnEngineCriticalSection);

	return 0;
}

SHORT ConnEngineEnterCriticalRegion ()
{
	EnterCriticalSection(&g_ConnEngineCriticalSection);

	return 0;
}

SHORT ConnEngineLeaveCriticalRegion ()
{
	LeaveCriticalSection(&g_ConnEngineCriticalSection);

	return 0;
}

TCHAR *ConnEngineAddInfoToOutput (TCHAR *ptcsBuffer)
{
	int   nLength;

	{
		PARASPCCO        ParaSpcCo;

		ParaSpcCo.uchMajorClass = CLASS_PARASPCCO;
		ParaSpcCo.uchAddress = SPCO_CONSEC_ADR;
		ParaRead(&ParaSpcCo);

		nLength = _stprintf (ptcsBuffer, _T("<usConsNo>%d</usConsNo>\r"), ParaSpcCo.usCounter);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	{
		PARASTOREGNO   StRegNoRcvBuff;

        StRegNoRcvBuff.uchMajorClass = CLASS_PARASTOREGNO;    /* get store/ reg No. */
        StRegNoRcvBuff.uchAddress = SREG_NO_ADR;
        ParaRead(&StRegNoRcvBuff);
        
        nLength = _stprintf (ptcsBuffer, _T("<ulStoreregNo>\r<usStoreNo>%d</usStoreNo>\r<usRegNo>%d</usRegNo>\r</ulStoreregNo>\r"), StRegNoRcvBuff.usStoreNo, StRegNoRcvBuff.usRegisterNo);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	{
		DATE_TIME DateTime;          /* for TOD read */

		PifGetDateTime(&DateTime);
		nLength = _stprintf (ptcsBuffer, _T("<datetime>%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d(%s)</datetime>\r"),
			DateTime.usYear, DateTime.usMonth, DateTime.usMDay, DateTime.usHour, DateTime.usMin, DateTime.usSec, DateTime.wszTimeZoneName);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	return ptcsBuffer;
}

// EJ file state message generated for each XML message if Connection Engine Interface EJ enabled, guarded by g_ConnEngineCriticalSection
// any function accessing this should be in the calling tree from 
static TCHAR   ConnEngineEjFileStateXml[2048];  // EJ file state message generated for each XML message if Connection Engine Interface EJ enabled

#define CONNENGINESTATESNAPSHOT_DATAREAD    0x00000001

typedef struct {
	ULONG  ulFlagsAndState;          // various flags and state used to manage this structure.
	ULONG  ulFileSizeLow;            // file position high long of file position in Connection Engine Interface XML Electronic Journal
	ULONG  ulFileSizeHigh;           // file position high long of file position in Connection Engine Interface XML Electronic Journal
	ULONG  ulConsecutiveNumber;      // Connection Engine Interface consecutive number of last XML message sent
	ULONG  ulGuestCheckCount;        // current count of number of guest checks transmitted
	ULONG  ulGuestCheckCountError;   // current count of number of guest check retrieval errors
	ULONG  ulCashierAction;          // current count of number of cashier action messages transmitted
	ULONG  ulEmployeeChange;         // current count of number of employee change messages transmitted
	ULONG  ulStoreForwardAction;     // current count of number of store forward messages transmitted
	ULONG  ulPreauthCaptureAction;   // current count of number of preauth capture cashier action messages transmitted
	ULONG  ulCashierSignIn;          // current count of number of sign-in cashier action messages transmitted
	ULONG  ulCashierSignOut;         // current count of number of sign-out cashier action messages transmitted
	ULONG  ulCashierCancelTran;      // current count of number of cancel-tran cashier action messages transmitted
	ULONG  ulCashierNoSale;          // current count of number of no-sale cashier action messages transmitted
	ULONG  ulCashierPickup;          // current count of number of cancel-tran cashier action messages transmitted
	ULONG  ulCashierLoan;            // current count of number of cancel-tran cashier action messages transmitted
	ULONG  ulCashierPaidOut;         // current count of number of cancel-tran cashier action messages transmitted
	ULONG  ulCashierReceiveAccount;  // current count of number of cancel-tran cashier action messages transmitted
	ULONG  ulCashierOther;           // current count of number of other cashier action messages transmitted
	DATE_TIME  DateTime;             // date time stamp of when last XML message sent
	USHORT usGCNumber;               // guest check number of last transaction processed
	USHORT usConsNo;                 // consecutive number of last transaction processed
} ConnEngineStateSnapShot;

static ConnEngineStateSnapShot ConnEngineStateSnapShotData = {0};

static TCHAR  szConnEngineStateSnapShotFile[] = _T("TOTALCES");

static int ConnEngineStateSnapShotRead (ConnEngineStateSnapShot *pSnapShot)
{
	SHORT  shFileHandle = 0;

	memset (pSnapShot, 0, sizeof(ConnEngineStateSnapShot));
#if 0
	// this source for snapshot of XML EJ is currently not enabled
	// however we are saving these changes for later.
	shFileHandle = PifOpenFile (szConnEngineStateSnapShotFile, "towr");
	if (shFileHandle < 0) {
		shFileHandle = PifOpenFile (szConnEngineStateSnapShotFile, "tnwr");
	}
	if (shFileHandle >= 0) {
		ULONG  ulActualBytesRead;
		PifReadFile (shFileHandle, pSnapShot, sizeof(ConnEngineStateSnapShot), &ulActualBytesRead);
		PifCloseFile (shFileHandle);
	}
	pSnapShot->ulFlagsAndState |= CONNENGINESTATESNAPSHOT_DATAREAD;
#endif
	return ((shFileHandle >= 0) ? 0 : -1);
}

static int ConnEngineStateSnapShotWrite (ConnEngineStateSnapShot *pSnapShot)
{
	SHORT  shFileHandle = -1;

	if (pSnapShot->ulFlagsAndState & CONNENGINESTATESNAPSHOT_DATAREAD) {
		pSnapShot->ulFlagsAndState &= ~CONNENGINESTATESNAPSHOT_DATAREAD;

		shFileHandle = PifOpenFile (szConnEngineStateSnapShotFile, "towr");
		if (shFileHandle < 0) {
			shFileHandle = PifOpenFile (szConnEngineStateSnapShotFile, "tnwr");
		}
		if (shFileHandle >= 0) {
			PifWriteFile (shFileHandle, pSnapShot, sizeof(ConnEngineStateSnapShot));
			PifCloseFile (shFileHandle);
		}
	}

	return ((shFileHandle >= 0) ? 0 : -1);
}

static struct {
	USHORT   usTchar;
	TCHAR    *ptcsTchar;
} ConnEngineSpecialCharactersArray[] = {
	{_T('&'), _T("&amp;")},
	{_T('<'), _T("&lt;")},
	{_T('>'), _T("&gt;")},
	{_T('"'), _T("&quot;")},
	{0, 0}
};

TCHAR *ConnEngineTransposeSpecialCharacters (TCHAR *ptcsOutput, TCHAR *ptcsInput)
{
	TCHAR *ptcsOutputSave = ptcsOutput;

	while (*ptcsInput) {
		int i;

		for (i = 0; ConnEngineSpecialCharactersArray[i].ptcsTchar; i++) {
			if (*ptcsInput == ConnEngineSpecialCharactersArray[i].usTchar) {
				TCHAR *pSpcl = ConnEngineSpecialCharactersArray[i].ptcsTchar;
				while (*pSpcl) {
					*ptcsOutput++ = *pSpcl++;
				}
				ptcsInput++;
				break;
			}
		}
		if (! ConnEngineSpecialCharactersArray[i].ptcsTchar) {
			*ptcsOutput++ = *ptcsInput++;
		}
	}

	return ptcsOutputSave;
}

TCHAR *ConnEngineTransposeSpecialStrings (TCHAR *ptcsOutput, USHORT usElementSize, TCHAR *ptcsInput)
{
	USHORT usSize = usElementSize/sizeof(TCHAR);
	TCHAR *ptcsOutputSave = ptcsOutput;

	while (*ptcsInput && usSize > 0) {
		if (*ptcsInput != _T('&')) {
			usSize--;
			*ptcsOutput++ = *ptcsInput++;
		} else {
			int i;

			ptcsInput++;   // increment past the ampersand to start with the 
			for (i = 0; ConnEngineSpecialCharactersArray[i].ptcsTchar; i++) {
				// if the second character matches then check the rest of the string
				if (*ptcsInput == ConnEngineSpecialCharactersArray[i].ptcsTchar[1]) {
					TCHAR *pSpcl = ConnEngineSpecialCharactersArray[i].ptcsTchar + 1;
					TCHAR *ptcsInputTemp = ptcsInput;

					while (*pSpcl && *ptcsInputTemp && *ptcsInputTemp == *pSpcl) { ptcsInputTemp++; pSpcl++;}

					if (*pSpcl == 0) {
						usSize--;
						*ptcsOutput++ = ConnEngineSpecialCharactersArray[i].usTchar;
						ptcsInput = ptcsInputTemp;
						break;
					}
				}
			}
		}
	}

	return ptcsOutputSave;
}

TCHAR  *ConnEngineObjectMnemonicString (TCHAR *tcsOutputString, TCHAR *tcsInputString, int nBytes)
{
	int  iLoop, jLoop;
	int  nChars = nBytes/sizeof(TCHAR);

	for (iLoop = 0, jLoop = 0; tcsInputString[iLoop] && iLoop < nChars; iLoop++) {
		if (tcsInputString[iLoop] >= _T(' ')) {
			int i;

			for (i = 0; ConnEngineSpecialCharactersArray[i].ptcsTchar; i++) {
				if (tcsInputString[iLoop] == ConnEngineSpecialCharactersArray[i].usTchar) {
					TCHAR *pSpcl = ConnEngineSpecialCharactersArray[i].ptcsTchar;
					while (*pSpcl) {
						tcsOutputString[jLoop++] = *pSpcl++;
					}
					break;
				}
			}
			if (ConnEngineSpecialCharactersArray[i].ptcsTchar == 0)
				tcsOutputString[jLoop++] = tcsInputString[iLoop];
		}
	}
	tcsOutputString[jLoop] = 0;
	return tcsOutputString;
}

TCHAR  *ConnEngineObjectMnemonicStringUchar (TCHAR *tcsOutputString, UCHAR *uchInputString, int nBytes)
{
	int  iLoop, jLoop;
	int  nChars = nBytes;

	for (iLoop = 0, jLoop = 0; uchInputString[iLoop] && iLoop < nChars; iLoop++) {
		if (uchInputString[iLoop] >= _T(' ')) {
			int i;

			for (i = 0; ConnEngineSpecialCharactersArray[i].ptcsTchar; i++) {
				if (uchInputString[iLoop] == ConnEngineSpecialCharactersArray[i].usTchar) {
					TCHAR *pSpcl = ConnEngineSpecialCharactersArray[i].ptcsTchar;
					while (*pSpcl) {
						tcsOutputString[jLoop++] = *pSpcl++;
					}
					break;
				}
			}
			if (ConnEngineSpecialCharactersArray[i].ptcsTchar == 0)
				tcsOutputString[jLoop++] = uchInputString[iLoop];
		}
	}
	tcsOutputString[jLoop] = 0;
	return tcsOutputString;
}

int ConnEngineSendMasterConnEngineInterface (USHORT hConnEngineSocket, LPCTSTR tcBuffer, LONG tcBufferSize)
{
	int    iRetStatus = 1;
	SHORT  sRetStatus = 0;
	SHORT  sMasterStatus = CstCheckMasterRoleAssummed ();

	if (sMasterStatus == STUB_NOT_MASTER) {
		// this is a Satellite Terminal so we need to send our
		// Connection Engine Interface data to the Master Terminal
		if (tcBufferSize < TCHARSTRINGARRAYSIZE) {
			sRetStatus = CliOpConnEngine (tcBuffer, tcBufferSize);
		} else {
			ULONG  ulActualPosition;
			SHORT  sFileHandle;
			TCHAR  szTempFile[] = _T("CONSEND1");

			sFileHandle = PifOpenFile (szTempFile, "tnwr");
			if (sFileHandle < 0) {
				PifDeleteFile (szTempFile);
				sFileHandle = PifOpenFile (szTempFile, "tnwr");
			}
			PifSeekFile (sFileHandle, 0, &ulActualPosition);
			PifWriteFile (sFileHandle, tcBuffer, tcBufferSize * sizeof(TCHAR));
			PifSeekFile (sFileHandle, 0, &ulActualPosition);
			sRetStatus = CliOpConnEngineFH (sFileHandle, (USHORT)(tcBufferSize * sizeof(TCHAR)));
			PifCloseFile (sFileHandle);
			PifDeleteFile (szTempFile);
		}
	} else if (sMasterStatus == STUB_SELF) {
		// this is a Master Terminal so we will 
		iRetStatus = fnConnEngineSendMessage (hConnEngineSocket, tcBuffer, tcBufferSize);
	} else {
		char xBuff[128];

		sprintf (xBuff, "ConnEngineSendMasterConnEngineInterface(): sMasterStatus = %d", sMasterStatus);
		NHPOS_ASSERT_TEXT((sMasterStatus == STUB_SELF), xBuff);
	}

	return iRetStatus;
}

/*
	Send a message out the Connecton Engine Interface and also save the message sent in the
	Connection Engine Electronic Journal file.

	The Connection Engine Electronic Journal file is deleted as a part of doing an EJ clear
	operation through Supervisor Menu or PCIF.  See function EJClear() which contains the
	source to delete the file.
**/
static  TCHAR  ConnEngineSendBuffer[256000];

int  ConnEngineObjectSendMessage (USHORT usCopyToEj, USHORT hConnEngineSocket, LPCTSTR tcBuffer, LONG tcBufferSize)
{
	ULONG      ulFileSizeLow = 0, ulFileSizeHigh = 0, ulActualPosition = 0;
	ULONG      ConnEngineConsecutiveNumber;
	TCHAR      *szConnEngineElectronicJournal = _T("TOTALEJG");
	DATE_TIME  DateTime;          /* for TOD read */

	// Set the Sign-out Date/Time information
	PifGetDateTime(&DateTime);

	ConnEngineConsecutiveNumber = ParaIncrementConnEngineConsecNumber();

	ConnEngineEjFileStateXml[0] = 0;  // ensure 
	if (usCopyToEj && ConnEngineObjectEchoElectronicJournal) {
		TCHAR      tcsBuff[128];
		TCHAR      tcsDateBuff[24], tcsTimeBuff[24];
		SHORT      shFileHandle;

		shFileHandle = PifOpenFile (szConnEngineElectronicJournal, "w");

		if (shFileHandle >= 0) {

			ulFileSizeLow = PifGetFileSize(shFileHandle, &ulFileSizeHigh);
			PifSeekFile(shFileHandle, ulFileSizeLow, &ulActualPosition);

			_stprintf_s (tcsDateBuff, sizeof(tcsDateBuff)/sizeof(tcsDateBuff[0]), _T("%2.2d/%2.2d/%4.4d"), DateTime.usMonth, DateTime.usMDay, DateTime.usYear);
			_stprintf_s (tcsTimeBuff, sizeof(tcsTimeBuff)/sizeof(tcsTimeBuff[0]), _T("%2.2d:%2.2d:%2.2d"), DateTime.usHour, DateTime.usMin, DateTime.usSec);

			// this uses same XML tags as does ejfileinfo tag
			_stprintf (ConnEngineEjFileStateXml, _T("<ejfilestate>\r<ulFilePositionHigh>%d</ulFilePositionHigh>\r<ulFilePositionLow>%d</ulFilePositionLow>\r<consecnumber>%d</consecnumber>\r</ejfilestate>\r"), ulFileSizeHigh, ulFileSizeLow, ConnEngineConsecutiveNumber);

			_stprintf_s (tcsBuff, sizeof(tcsBuff)/sizeof(tcsBuff[0]), _T("<ej_entry_start  consecnumber=\"%d\" date=\"%s\" time=\"%s\">\n"), ConnEngineConsecutiveNumber, tcsDateBuff, tcsTimeBuff);
			PifWriteFile (shFileHandle, tcsBuff, _tcslen(tcsBuff) * sizeof(tcsBuff[0]));

			PifWriteFile (shFileHandle, tcBuffer, tcBufferSize * sizeof(tcBuffer[0]));

			_stprintf_s (tcsBuff, sizeof(tcsBuff)/sizeof(tcsBuff[0]), _T("</ej_entry_start>\n"));
			PifWriteFile (shFileHandle, tcsBuff, _tcslen(tcsBuff) * sizeof(tcsBuff[0]));
			PifCloseFile (shFileHandle);    // Close the Connection Engine Electronic Journal
		}
	}

	if (ConnEngineStateSnapShotData.ulFlagsAndState & CONNENGINESTATESNAPSHOT_DATAREAD) {
		ConnEngineStateSnapShotData.ulFileSizeLow = ulFileSizeLow;
		ConnEngineStateSnapShotData.ulFileSizeHigh = ulFileSizeHigh;
		ConnEngineStateSnapShotData.ulConsecutiveNumber = ConnEngineConsecutiveNumber;
		ConnEngineStateSnapShotData.DateTime = DateTime;
		ConnEngineStateSnapShotWrite (&ConnEngineStateSnapShotData);
	}

	return ConnEngineSendMasterConnEngineInterface (hConnEngineSocket, tcBuffer, tcBufferSize);
}

int  ConnEngineObjectSendFile (USHORT usTerminalNo, USHORT usNoBytes)
{
	int    nLength;
	ULONG  ulActualBytesRead = 0;
	SHORT  sRetStatus;
	TCHAR  *ptcsBuffer;

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return 0;
	}

	ConnEngineEnterCriticalRegion ();

	ptcsBuffer = ConnEngineSendBuffer;
	nLength = _stprintf (ptcsBuffer, _T("<CONNENGINE>\r<terminalnumber>%d</terminalnumber>\r"), usTerminalNo);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	PifSeekFile (hsSerTmpFile, SERTMPFILE_DATASTART, &ulActualBytesRead);
	PifReadFile (hsSerTmpFile, ptcsBuffer, usNoBytes, &ulActualBytesRead);
	ptcsBuffer += (ulActualBytesRead)/sizeof(TCHAR);

	nLength = _stprintf (ptcsBuffer, _T("</CONNENGINE>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	sRetStatus = ConnEngineSendMasterConnEngineInterface (0, ConnEngineSendBuffer, ptcsBuffer - ConnEngineSendBuffer);

	ConnEngineLeaveCriticalRegion ();

	return sRetStatus;
}

/****************************************************************
*  Description:  Reading from the temporary transaction storage file created
*                by function TrnSaveGC() in Trans/trnbody.c we are going to
*                transform the transaction data into a form that can be sent
*                to some other application that is connected to us via the
*                ConnEngineMfc dll and its Transport Engine functionality.
**/

SHORT ConnEngineSendGuestCheckList (GCF_STATUS_STATE *pRcvBuffer, USHORT usRcvActualCount, GCF_STATUS_HEADER *pGcf_FileHed, SHORT gcnRequest)
{
	TCHAR      *ptcsBuffer;
	int        nLength;
	USHORT     usCount;
	USHORT     usSequenceNo;

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return 0;
	}

	ConnEngineEnterCriticalRegion ();

	ptcsBuffer = ConnEngineSendBuffer;
	nLength = _stprintf (ptcsBuffer, _T("<GUESTCHECK_LIST>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	usSequenceNo = 1;
	nLength = _stprintf (ptcsBuffer, _T("<sequenceno>%d</sequenceno>\r<usRcvActualCount>%d</usRcvActualCount>\r"), usSequenceNo, usRcvActualCount);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	if (pGcf_FileHed) {
		nLength = _stprintf (ptcsBuffer, _T("<usCurGcn>%d</usCurGcn>\r"), pGcf_FileHed->usStartGCN);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	usSequenceNo++;

	if (gcnRequest < 0) {
		// summary request only
		usRcvActualCount = 0;
	}

	for (usCount = 0; usCount < usRcvActualCount; usCount++, pRcvBuffer++) {
		if (ptcsBuffer + 1000 > ConnEngineSendBuffer + sizeof(ConnEngineSendBuffer)/sizeof(ConnEngineSendBuffer[0])) {
			// If we are approaching the maximum size of the output buffer then lets send this message and
			// start a new message continuing the list of guest checks.
			nLength = _stprintf (ptcsBuffer, _T("</GUESTCHECK_LIST>\r"));
			if (nLength >= 0)
				ptcsBuffer += nLength;

			ConnEngineObjectSendMessage (0, 0, ConnEngineSendBuffer, ptcsBuffer - ConnEngineSendBuffer);

			PifSleep(50);      // Give up the processor to allow the socket I/O to be performed.  Otherwise only first message will be seen

			ptcsBuffer = ConnEngineSendBuffer;
			nLength = _stprintf (ptcsBuffer, _T("<GUESTCHECK_LIST>\r"));
			if (nLength >= 0)
				ptcsBuffer += nLength;

			nLength = _stprintf (ptcsBuffer, _T("<sequenceno>%d</sequenceno>\r<usRcvActualCount>%d</usRcvActualCount>\r"), usSequenceNo, usRcvActualCount);
			usSequenceNo++;
			if (nLength >= 0)
				ptcsBuffer += nLength;

			if (pGcf_FileHed) {
				nLength = _stprintf (ptcsBuffer, _T("<usCurGcn>%d</usCurGcn>\r"), pGcf_FileHed->usStartGCN);
				if (nLength >= 0)
					ptcsBuffer += nLength;
			}

		}

		nLength = _stprintf (ptcsBuffer, _T("<check>\r<gcnum>%d</gcnum>\r"), pRcvBuffer->usGCNO);
		if (nLength >= 0)
			ptcsBuffer += nLength;

		nLength = _stprintf (ptcsBuffer, _T("<fbContFlag>"));
		if (nLength >= 0)
			ptcsBuffer += nLength;

		// indicate whether the guest check has been paid out or not
		if ((pRcvBuffer->fbContFlag & GCF_PAYMENT_TRAN_MASK) != 0) {
			nLength = _stprintf (ptcsBuffer, _T("PO,"));
			if (nLength >= 0)
				ptcsBuffer += nLength;
		}
		else {
			nLength = _stprintf (ptcsBuffer, _T("UP,"));
			if (nLength >= 0)
				ptcsBuffer += nLength;
		}

		// Indicate whether the guest check is locked or not
		if ((pRcvBuffer->fbContFlag & GCF_READ_FLAG_MASK) != 0) {
			nLength = _stprintf (ptcsBuffer, _T("LK,"));
			if (nLength >= 0)
				ptcsBuffer += nLength;
		}
		else {
			nLength = _stprintf (ptcsBuffer, _T("UL,"));
			if (nLength >= 0)
				ptcsBuffer += nLength;
		}

		// Indicate whether this is on a drive thru queue or is a counter check
		if ((pRcvBuffer->fbContFlag & GCF_DRIVE_THROUGH_MASK) != 0) {
			nLength = _stprintf (ptcsBuffer, _T("DT,"));
			if (nLength >= 0)
				ptcsBuffer += nLength;
		}
		else {
			nLength = _stprintf (ptcsBuffer, _T("CN,"));
			if (nLength >= 0)
				ptcsBuffer += nLength;
		}

		if (*ptcsBuffer == _T(',')) ptcsBuffer--;   // if trailing comma then remove it as unneeded

		nLength = _stprintf (ptcsBuffer, _T("</fbContFlag>\r"));
		if (nLength >= 0)
			ptcsBuffer += nLength;

		// provide the transaction date and time for when the transaction was actually done.
		nLength = _stprintf (ptcsBuffer, _T("<trandatetime>%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d(%s)</trandatetime>\r"),
			pRcvBuffer->uchGcfYear + 2000, pRcvBuffer->uchGcfMonth, pRcvBuffer->uchGcfDay, pRcvBuffer->uchGcfHour,
			pRcvBuffer->uchGcfMinute, pRcvBuffer->uchGcfSecond, _T(" "));
		if (nLength >= 0)
			ptcsBuffer += nLength;

		nLength = _stprintf (ptcsBuffer, _T("</check>\r"));
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	nLength = _stprintf (ptcsBuffer, _T("</GUESTCHECK_LIST>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	ConnEngineObjectSendMessage (0, 0, ConnEngineSendBuffer, ptcsBuffer - ConnEngineSendBuffer);

	ConnEngineLeaveCriticalRegion ();
	return 0;
}

static SHORT ConnEngineSendTransactionItemCheck (ITEMSALES *pItemSales, SHORT hsFileHandle, ULONG ulItemsAreaOffset, TRANSTORAGEDISCNON  *pItemDiscNon)
{
	SHORT                 sbCheckPassed = 0;

	if (pItemDiscNon)
		memset (pItemDiscNon, 0, sizeof(TRANSTORAGEDISCNON));

	sbCheckPassed = ((pItemSales->fbReduceStatus & REDUCE_ITEM) == 0); // the item has not been marked as reduce due to Condiment Edit, etc.

	if (sbCheckPassed && pItemSales->usItemOffset != 0 && ((pItemSales->fbModifier & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) != VOID_MODIFIER)) {
		// this is a non-void item and there is an associated item such as void to modify this item
#if 0
		if (pItemSales->uchMinorClass == CLASS_PLUITEMDISC || pItemSales->uchMinorClass == CLASS_DEPTITEMDISC
			) {
			// if this is a discount
			sbCheckPassed = 0;
		} else {
#else
		{
#endif
			ULONG                 ulActualBytesRead;
			UCHAR                 achBuff[sizeof(UCHAR)+sizeof(TRANSTORAGESALESNON) + sizeof(USHORT)];
			TRANSTORAGESALESNON   *pSalesNon = (TRANSTORAGESALESNON *) (achBuff + 1);

			memset (&achBuff, 0, sizeof(achBuff));

			PifSeekFile (hsFileHandle, pItemSales->usItemOffset + ulItemsAreaOffset, &ulActualBytesRead);
			PifReadFile(hsFileHandle, achBuff, sizeof(achBuff), &ulActualBytesRead);
			pItemSales->lQTY += pSalesNon->lQTY;
			pItemSales->lProduct += pSalesNon->lProduct;
			if (pItemSales->uchMinorClass == CLASS_PLUITEMDISC || pItemSales->uchMinorClass == CLASS_DEPTITEMDISC) {
				USHORT usDiscOffset = RflGetStorageItemLen (achBuff, sizeof(achBuff));

				PifSeekFile (hsFileHandle, pItemSales->usItemOffset + ulItemsAreaOffset + usDiscOffset + 1, &ulActualBytesRead);
				PifReadFile(hsFileHandle, pItemDiscNon, sizeof(TRANSTORAGEDISCNON), &ulActualBytesRead);
			}
		}
	}

	sbCheckPassed = ( sbCheckPassed && ((pItemSales->fbModifier & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) != VOID_MODIFIER) );

	sbCheckPassed = ( sbCheckPassed && (pItemSales->lQTY != 0) );
	return sbCheckPassed;
}

static int ConnEngineSendTransactionTender (ITEMTENDER * pItemTender, TCHAR *ptcsBuffer)
{
	TCHAR  *ptcsBufferTemp = ptcsBuffer;
	int     nLength;
	TCHAR   tcsMnemonicTempBuff[256];

	if (pItemTender->uchMinorClass == CLASS_TEND_TIPS_RETURN) {
		nLength = _stprintf (ptcsBuffer, _T("<tipsreturn>\r"));
		if (nLength >= 0)
			ptcsBuffer += nLength;
	} else {
		nLength = _stprintf (ptcsBuffer, _T("<tender>\r"));
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	nLength = _stprintf (ptcsBuffer, _T("<newItem>%d</newItem>\r"), ((pItemTender->fbModifier & RETURNS_ORIGINAL) ? 0 : 1));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	PUTSTRUCTMEMBER(ptcsBuffer,pItemTender,uchMajorClass,pFormatNumeric);
	PUTSTRUCTMEMBER(ptcsBuffer,pItemTender,uchMinorClass,pFormatNumeric);
	PUTSTRUCTMEMBER(ptcsBuffer,pItemTender,usCheckTenderId,pFormatNumeric);
	PUTSTRUCTMEMBER(ptcsBuffer,pItemTender,lTenderAmount,pFormatDcurrency);
	PUTSTRUCTMEMBER(ptcsBuffer,pItemTender,lForeignAmount,pFormatDcurrency);
	PUTSTRUCTMEMBER(ptcsBuffer,pItemTender,lBalanceDue,pFormatDcurrency);
	PUTSTRUCTMEMBER(ptcsBuffer,pItemTender,lGratuity,pFormatDcurrency);
	PUTSTRUCTMEMBER(ptcsBuffer,pItemTender,lChange,pFormatDcurrency);
	PUTSTRUCTMEMBER(ptcsBuffer,pItemTender,usReasonCode,pFormatNumeric);
	PUTSTRUCTMEMBER(ptcsBuffer,pItemTender,fbModifier,pFormatNumericHex);

	nLength = _stprintf (ptcsBuffer, _T("<tenderdate>%2.2d%2.2d%2.2d%2.2d%2.2d%2.2d</tenderdate>\r"), (pItemTender->uchTenderYear % 100), pItemTender->uchTenderMonth, pItemTender->uchTenderDay,
		pItemTender->uchTenderHour, pItemTender->uchTenderMinute, pItemTender->uchTenderSecond);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	if (pItemTender->usReasonCode != 0) {
		OPMNEMONICFILE MnemonicFile;

		MnemonicFile.usMnemonicFileId = OPMNEMONICFILE_ID_REASON;
		MnemonicFile.usMnemonicAddress = pItemTender->usReasonCode;
		OpMnemonicsFileIndRead(&MnemonicFile, 0);
		ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, MnemonicFile.aszMnemonicValue, sizeof(MnemonicFile.aszMnemonicValue));
		PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszReasonMnemonic,pFormatString);
	}
	if ((pItemTender->fbModifier & EPTTEND_SET_MODIF) == 0 && pItemTender->aszNumber[0]) {
		if ((pItemTender->fbModifier & RETURNS_ORIGINAL) == 0) {
			RflDecryptByteString ((UCHAR *)&(pItemTender->aszNumber[0]), sizeof(pItemTender->aszNumber));
		}
		ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pItemTender->aszNumber, NUM_NUMBER * sizeof(TCHAR));
		PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszNumber_0,pFormatString);
	}
	ConnEngineObjectMnemonicStringUchar (tcsMnemonicTempBuff, pItemTender->authcode.auchAuthCode, sizeof(pItemTender->authcode.auchAuthCode));
	PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,tchAuthCode,pFormatString);
	ConnEngineObjectMnemonicStringUchar (tcsMnemonicTempBuff, pItemTender->refno.auchReferncNo, sizeof(pItemTender->refno.auchReferncNo));
	PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,tchRefNum,pFormatString);
	ConnEngineObjectMnemonicStringUchar (tcsMnemonicTempBuff, pItemTender->invno.auchInvoiceNo, sizeof(pItemTender->invno.auchInvoiceNo));
	PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,tchInvoiceNum,pFormatString);
	ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pItemTender->aszPostTransNo, sizeof(pItemTender->aszPostTransNo));
	PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszPostTransNo,pFormatString);

	ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pItemTender->ITEMTENDER_CARDTYPE, sizeof(pItemTender->ITEMTENDER_CARDTYPE));
	PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,tchCardType,pFormatString);        // see also TRN_DSI_CARDTYPE and EEPTRSPDATA.aszCardLabel

	nLength = _stprintf (ptcsBuffer, _T("<eptresponse>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	if (pItemTender->fbModifier & APPROVED_EPT_MODIF) {
		_tcscpy (tcsMnemonicTempBuff, _T("Approved"));
	}
	else if (pItemTender->fbModifier & STORED_EPT_MODIF) {
		_tcscpy (tcsMnemonicTempBuff, _T("Stored"));
	}
	else if (pItemTender->fbModifier & DECLINED_EPT_MODIF) {
		_tcscpy (tcsMnemonicTempBuff, _T("Declined"));
	} else {
		_tcscpy (tcsMnemonicTempBuff, _T("Unknown"));
	}
	PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,auchStatus,pFormatString);

	// put in the Electronic Payment Interface response text if it exists.
	if (pItemTender->aszCPMsgText[0][0]) {
		TCHAR  tcsBuffer[48];
		_tcsncpy (tcsBuffer, pItemTender->aszCPMsgText[0], 40);
		tcsBuffer[40] = 0;
		PUTSTRINGLABEL(ptcsBuffer,tcsBuffer,line1,pFormatString);
	}
	if (pItemTender->aszCPMsgText[1][0]) {
		TCHAR  tcsBuffer[48];
		_tcsncpy (tcsBuffer, pItemTender->aszCPMsgText[1], 40);
		tcsBuffer[40] = 0;
		PUTSTRINGLABEL(ptcsBuffer,tcsBuffer,line2,pFormatString);
	}
	if (pItemTender->aszCPMsgText[2][0]) {
		TCHAR  tcsBuffer[48];
		_tcsncpy (tcsBuffer, pItemTender->aszCPMsgText[2], 40);
		tcsBuffer[40] = 0;
		PUTSTRINGLABEL(ptcsBuffer,tcsBuffer,line3,pFormatString);
	}
	if (pItemTender->aszCPMsgText[3][0]) {
		TCHAR  tcsBuffer[48];
		_tcsncpy (tcsBuffer, pItemTender->aszCPMsgText[3], 40);
		tcsBuffer[40] = 0;
		PUTSTRINGLABEL(ptcsBuffer,tcsBuffer,line4,pFormatString);
	}
	if (pItemTender->aszCPMsgText[4][0]) {
		TCHAR  tcsBuffer[48];
		_tcsncpy (tcsBuffer, pItemTender->aszCPMsgText[4], 40);
		tcsBuffer[40] = 0;
		PUTSTRINGLABEL(ptcsBuffer,tcsBuffer,line5,pFormatString);
	}
	if (pItemTender->aszCPMsgText[5][0]) {
		TCHAR  tcsBuffer[48];
		_tcsncpy (tcsBuffer, pItemTender->aszCPMsgText[5], 40);
		tcsBuffer[40] = 0;
		PUTSTRINGLABEL(ptcsBuffer,tcsBuffer,line6,pFormatString);
	}
	if (pItemTender->aszCPMsgText[6][0]) {
		TCHAR  tcsBuffer[48];
		_tcsncpy (tcsBuffer, pItemTender->aszCPMsgText[6], 40);
		tcsBuffer[40] = 0;
		PUTSTRINGLABEL(ptcsBuffer,tcsBuffer,line7,pFormatString);
	}
	if (pItemTender->aszCPMsgText[7][0]) {
		TCHAR  tcsBuffer[48];
		_tcsncpy (tcsBuffer, pItemTender->aszCPMsgText[7], 40);
		tcsBuffer[40] = 0;
		PUTSTRINGLABEL(ptcsBuffer,tcsBuffer,line8,pFormatString);
	}
	// This last item in the response text message contains special text to identify
	// the card with the card type and the card account number masked.
	if (pItemTender->aszCPMsgText[NUM_CPRSPCO_CARDLABEL][0]) {
		TCHAR  tcsBuffer[48];
		_tcsncpy (tcsBuffer, pItemTender->aszCPMsgText[NUM_CPRSPCO_CARDLABEL], 40);
		tcsBuffer[40] = 0;
		PUTSTRINGLABEL(ptcsBuffer,tcsBuffer,cardlabel,pFormatString);
	}

	if (pItemTender->srno.auchRecordNumber[0]) {
		nLength = _stprintf (ptcsBuffer, _T("<auchRecordNumber>%S</auchRecordNumber>\r"), pItemTender->srno.auchRecordNumber);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	nLength = _stprintf (ptcsBuffer, _T("</eptresponse>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

#if 1
	{
		ITEMTENDERCHARGETIPS *pItemTenderChargeTips = &ITEMTENDER_CHARGETIPS(pItemTender);
		if (pItemTender->lGratuity || (pItemTenderChargeTips->uchSignature == 0xff && pItemTenderChargeTips->lGratuity)) {

			// if there is a tip then we need to provide the chargetips information
			nLength = _stprintf (ptcsBuffer, _T("<chargetips>\r"));
			if (nLength >= 0)
				ptcsBuffer += nLength;

			PUTSTRUCTMEMBER(ptcsBuffer,pItemTenderChargeTips,uchMinorClass,pFormatNumeric);
			PUTSTRUCTMEMBER(ptcsBuffer,pItemTenderChargeTips,uchMinorTenderClass,pFormatNumeric);
			PUTSTRUCTMEMBER(ptcsBuffer,pItemTenderChargeTips,lGratuity,pFormatDcurrency);

			nLength = _stprintf (ptcsBuffer, _T("</chargetips>\r"));
			if (nLength >= 0)
				ptcsBuffer += nLength;
		}
	}
#endif

	if (pItemTender->uchMinorClass == CLASS_TEND_TIPS_RETURN) {
		nLength = _stprintf (ptcsBuffer, _T("</tipsreturn>\r"));
		if (nLength >= 0)
			ptcsBuffer += nLength;
	} else {
		nLength = _stprintf (ptcsBuffer, _T("</tender>\r"));
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	return (ptcsBuffer - ptcsBufferTemp);
}

static SHORT ConnEngineSendTransactionItem (TCHAR **ptcsBufferAdr, PRTIDX *pIdxFile, UCHAR *auchTranStorageWork, ULONG ulActualBytesRead, SHORT hsFileHandle, ULONG ulItemsAreaOffset)
{
	TCHAR         tcsMnemonicTempBuff[256];
	int           nLength;
    UCHAR         *puchWorkBuffer;
	TCHAR         *ptcsBuffer = *ptcsBufferAdr;
	CHAR          achBuff[sizeof(ITEMDATASIZEUNION)];
	USHORT        usReadLen = 0;
	PRTIDX        IdxFileTemp;
	ITEMSALES    *pTranItem, *pItemSales;
    TRANSTORAGESALESNON   *pTranNon;
	struct {
		TCHAR auchPLUNo [NUM_PLU_LEN + 1];
	} tempItemSales;
	struct {
		UCHAR  uchMinorClass;
		TCHAR  *aszName;
	} MyDiscountList[] = {
			{CLASS_UIREGDISC1, _T("CLASS_UIREGDISC1")},        // regular discount #1 */
			{CLASS_UIREGDISC2, _T("CLASS_UIREGDISC2")},        // regular discount #2 */
			{CLASS_UIREGDISC3, _T("CLASS_UIREGDISC3")},        // regular discount #3 */
			{CLASS_UIREGDISC4, _T("CLASS_UIREGDISC4")},        // regular discount #4 */
			{CLASS_UIREGDISC5, _T("CLASS_UIREGDISC5")},        // regular discount #5 */
			{CLASS_UIREGDISC6, _T("CLASS_UIREGDISC6")},        // regular discount #6 */
			{CLASS_UICHARGETIP, _T("CLASS_UICHARGETIP")},      // charge tip */
			{CLASS_UICHARGETIP2, _T("CLASS_UICHARGETIP2")},    // charge tip, V3.3 */
			{CLASS_UICHARGETIP3, _T("CLASS_UICHARGETIP3")},    // charge tip, V3.3 */
			{CLASS_AUTOCHARGETIP, _T("CLASS_AUTOCHARGETIP")},      // auto charge tip, V3.3 */
			{CLASS_AUTOCHARGETIP2, _T("CLASS_AUTOCHARGETIP2")},    // auto charge tip, V3.3 */
			{CLASS_AUTOCHARGETIP3, _T("CLASS_AUTOCHARGETIP3")},    // auto charge tip, V3.3 */
			{0, 0}
		};
	TRANSTORAGEDISCNON  ItemDiscNon;

	pTranItem = pItemSales = ( ITEMSALES * ) ( achBuff);

    puchWorkBuffer = &auchTranStorageWork[ 0 ];
	pTranNon = ( TRANSTORAGESALESNON * ) ( puchWorkBuffer + sizeof( UCHAR ));

	switch (pTranNon->uchMajorClass) {
		case CLASS_ITEMSALES:

			/* decompress itemize data */
			memset (&achBuff, 0, sizeof(achBuff));
			usReadLen = RflGetStorageItem (achBuff, puchWorkBuffer, RFL_WITH_MNEM);

			if (pIdxFile) {
				IdxFileTemp = *pIdxFile;
			} else {
				memset (&IdxFileTemp, 0, sizeof (IdxFileTemp));
				IdxFileTemp.lProduct = pItemSales->lProduct;
				IdxFileTemp.lQTY = pItemSales->lQTY;
				IdxFileTemp.uchMinorClass = pItemSales->uchMinorClass;
			}
			if (ConnEngineSendTransactionItemCheck(pItemSales, hsFileHandle, ulItemsAreaOffset, &ItemDiscNon)) {
				USHORT fbModifier_ItemSales = pItemSales->fbModifier;   // save this for use by item discount since we reuse the buffer.

				nLength = _stprintf (ptcsBuffer, _T("<item>\r"));
				if (nLength >= 0)
					ptcsBuffer += nLength;

				if (pItemSales->fbModifier & VOID_MODIFIER) {
					nLength = _stprintf (ptcsBuffer, _T("<modifier>0x%x</modifier>\r"), (pItemSales->fbModifier & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)));
					if (nLength >= 0)
						ptcsBuffer += nLength;
				}

				nLength = _stprintf (ptcsBuffer, _T("<newItem>%d</newItem>\r"), ((fbModifier_ItemSales & RETURNS_ORIGINAL) ? 0 : 1));
				if (nLength >= 0)
					ptcsBuffer += nLength;

				// We print out what is in the index because the index may be unconsolidated
				// and there may be multiple index entries pointing to the same item information
				// since the item information is consolidated.
				memset (&tempItemSales, 0, sizeof(tempItemSales));
				PUTSTRUCTMEMBER(ptcsBuffer,pItemSales,uchMajorClass,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemSales,uchMinorClass,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemSales,usUniqueID,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemSales,uchSeatNo,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemSales,uchHourlyOffset,pFormatNumeric);
				RflConvertPLU (tempItemSales.auchPLUNo, pItemSales->auchPLUNo);
				PUTSTRUCTMEMBER(ptcsBuffer,&tempItemSales,auchPLUNo,pFormatString);
				ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pItemSales->aszMnemonic, sizeof(pItemSales->aszMnemonic));
				PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszMnemonic,pFormatString);
				if (pItemSales->aszNumber[0][0]) {
					ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pItemSales->aszNumber[0], NUM_NUMBER * sizeof(TCHAR));
					PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszNumber_0,pFormatString);
				}
				if (pItemSales->aszNumber[1][0]) {
					ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pItemSales->aszNumber[1], NUM_NUMBER * sizeof(TCHAR));
					PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszNumber_1,pFormatString);
				}
				if (pItemSales->aszNumber[2][0]) {
					ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pItemSales->aszNumber[2], NUM_NUMBER * sizeof(TCHAR));
					PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszNumber_2,pFormatString);
				}
				if (pItemSales->aszNumber[3][0]) {
					ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pItemSales->aszNumber[3], NUM_NUMBER * sizeof(TCHAR));
					PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszNumber_3,pFormatString);
				}
				if (pItemSales->aszNumber[4][0]) {
					ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pItemSales->aszNumber[4], NUM_NUMBER * sizeof(TCHAR));
					PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszNumber_4,pFormatString);
				}
				PUTSTRUCTMEMBER(ptcsBuffer,pItemSales,usDeptNo,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemSales,uchAdjective,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemSales,usReasonCode,pFormatNumeric);
				if (pItemSales->usReasonCode != 0) {
					OPMNEMONICFILE MnemonicFile;

					MnemonicFile.usMnemonicFileId = OPMNEMONICFILE_ID_REASON;
					MnemonicFile.usMnemonicAddress = pItemSales->usReasonCode;
					OpMnemonicsFileIndRead(&MnemonicFile, 0);
					ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, MnemonicFile.aszMnemonicValue, sizeof(MnemonicFile.aszMnemonicValue));
					PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszReasonMnemonic,pFormatString);
				}
				PUTSTRUCTMEMBER(ptcsBuffer,pItemSales,lProduct,pFormatDcurrency);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemSales,lQTY,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemSales,lUnitPrice, pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemSales,sCouponQTY,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemSales,fsLabelStatus,pFormatNumericHex);
				PUTSTRINGLABEL(ptcsBuffer,pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0],auchStatus_00,pFormatNumericHex);
				PUTSTRINGLABEL(ptcsBuffer,pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1],auchStatus_01,pFormatNumericHex);
				PUTSTRINGLABEL(ptcsBuffer,pItemSales->ControlCode.usBonusIndex,usBonusIndex,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemSales,fbModifier,pFormatNumericHex);

				if (pTranItem->uchCondNo > 0) {
					int i;

					for (i = 0; i < pTranItem->uchCondNo; i++) {
						nLength = _stprintf (ptcsBuffer, _T("<condiment>\r"));
						if (nLength >= 0)
							ptcsBuffer += nLength;

						pTranItem->Condiment[i].auchPLUNo[13] = 0;
						PUTSTRUCTMEMBER(ptcsBuffer,&(pTranItem->Condiment[i]),auchPLUNo,pFormatString);
						ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pTranItem->Condiment[i].aszMnemonic, sizeof(pTranItem->Condiment[i].aszMnemonic));
						PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszMnemonic,pFormatString);
						PUTSTRUCTMEMBER(ptcsBuffer,&(pTranItem->Condiment[i]),lUnitPrice,pFormatNumeric);
						PUTSTRUCTMEMBER(ptcsBuffer,&(pTranItem->Condiment[i]),uchAdjective,pFormatNumeric);
						PUTSTRUCTMEMBER(ptcsBuffer,&(pTranItem->Condiment[i]),usDeptNo,pFormatNumeric);
						PUTSTRUCTMEMBER(ptcsBuffer,&(pTranItem->Condiment[i]),uchCondColorPaletteCode,pFormatNumericHex);
						nLength = _stprintf (ptcsBuffer, _T("</condiment>\r"));
						if (nLength >= 0)
							ptcsBuffer += nLength;
					}
				}

				if (pTranItem->uchMinorClass == CLASS_PLUITEMDISC || pTranItem->uchMinorClass == CLASS_DEPTITEMDISC) {
					ITEMDISC  *pItemDisc;

					/* decompress itemize data */
					memset (&achBuff, 0, sizeof(achBuff));
					usReadLen = RflGetStorageItem (achBuff, puchWorkBuffer+usReadLen, RFL_WITH_MNEM);

					pItemDisc = (ITEMDISC *) (achBuff);
					pItemDisc->lAmount += ItemDiscNon.lAmount;
					pItemDisc->lQTY -= ItemDiscNon.lQTY;

					if (usReadLen > 0) {
						int iDiscNo = pItemDisc->uchMinorClass;

						switch(pItemDisc->uchMinorClass) {
							case CLASS_ITEMDISC1:       /* Item Discount */
								iDiscNo = pItemDisc->uchDiscountNo;
								break;
							default:
								break;
						}

						nLength = _stprintf (ptcsBuffer, _T("<discount>\r"));
						if (nLength >= 0)
							ptcsBuffer += nLength;

						nLength = _stprintf (ptcsBuffer, _T("<newItem>%d</newItem>\r"), ((fbModifier_ItemSales & RETURNS_ORIGINAL) ? 0 : 1));
						if (nLength >= 0)
							ptcsBuffer += nLength;

						PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,uchMajorClass,pFormatNumeric);
						PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,uchMinorClass,pFormatNumeric);
						PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,uchDiscountNo,pFormatNumeric);
						PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,uchHourlyOffset,pFormatNumeric);
						RflConvertPLU (tempItemSales.auchPLUNo, pItemDisc->auchPLUNo);
						PUTSTRUCTMEMBER(ptcsBuffer,&tempItemSales,auchPLUNo,pFormatString);
						ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pItemDisc->aszMnemonic, sizeof(pItemDisc->aszMnemonic));
						PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszMnemonic,pFormatString);
						PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,lAmount,pFormatDcurrency);
						PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,lQTY,pFormatNumeric);
						PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,uchRate,pFormatNumeric);
						PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,usReasonCode,pFormatNumeric);
						if (pItemDisc->usReasonCode != 0) {
							OPMNEMONICFILE MnemonicFile;

							MnemonicFile.usMnemonicFileId = OPMNEMONICFILE_ID_REASON;
							MnemonicFile.usMnemonicAddress = pItemDisc->usReasonCode;
							OpMnemonicsFileIndRead(&MnemonicFile, 0);
							ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, MnemonicFile.aszMnemonicValue, sizeof(MnemonicFile.aszMnemonicValue));
							PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszReasonMnemonic,pFormatString);
						}
						PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,uchSeatNo,pFormatNumeric);

						nLength = _stprintf (ptcsBuffer, _T("</discount>\r"));
						if (nLength >= 0)
							ptcsBuffer += nLength;
					}
				}

				nLength = _stprintf (ptcsBuffer, _T("</item>\r"));
				if (nLength >= 0)
					ptcsBuffer += nLength;
			}
			break;

		case CLASS_ITEMTOTAL:
			{
				ITEMTOTAL      *pItemTotal;

				nLength = _stprintf (ptcsBuffer, _T("<total>\r"));
				if (nLength >= 0)
					ptcsBuffer += nLength;

				/* decompress itemize data */
				memset (&achBuff, 0, sizeof(achBuff));
				usReadLen = RflGetStorageItem (achBuff, puchWorkBuffer, RFL_WITH_MNEM);

				pItemTotal = ( ITEMTOTAL * ) ( achBuff);

				nLength = _stprintf (ptcsBuffer, _T("<newItem>%d</newItem>\r"), ((pItemTotal->fbModifier & RETURNS_ORIGINAL) ? 0 : 1));
				if (nLength >= 0)
					ptcsBuffer += nLength;

				PUTSTRUCTMEMBER(ptcsBuffer,pItemTotal,uchMajorClass,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemTotal,uchMinorClass,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemTotal,lMI,pFormatDcurrency);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemTotal,lService,pFormatDcurrency);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemTotal,lTax,pFormatDcurrency);
				if (pItemTotal->aszNumber[0]) {
					ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pItemTotal->aszNumber, NUM_NUMBER * sizeof(TCHAR));
					PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszNumber_0,pFormatString);
				}

				nLength = _stprintf (ptcsBuffer, _T("</total>\r"));
				if (nLength >= 0)
					ptcsBuffer += nLength;
			}
			break;

		case CLASS_ITEMTENDER:
			{
				ITEMTENDER     *pItemTender;
				/* decompress itemize data */
				memset (&achBuff, 0, sizeof(achBuff));
				usReadLen = RflGetStorageItem (achBuff, puchWorkBuffer, RFL_WITH_MNEM);

				pItemTender = ( ITEMTENDER * ) ( achBuff);
				nLength = ConnEngineSendTransactionTender (pItemTender, ptcsBuffer);
				if (nLength >= 0)
					ptcsBuffer += nLength;
			}
			break;

		case CLASS_ITEMCOUPON:
			{
				ITEMCOUPON  *pItemCoupon;

				nLength = _stprintf (ptcsBuffer, _T("<coupon>\r"));
				if (nLength >= 0)
					ptcsBuffer += nLength;

				/* decompress itemize data */
				memset (&achBuff, 0, sizeof(achBuff));
				usReadLen = RflGetStorageItem (achBuff, puchWorkBuffer, RFL_WITH_MNEM);

				pItemCoupon = (ITEMCOUPON *) (achBuff);

				nLength = _stprintf (ptcsBuffer, _T("<newItem>%d</newItem>\r"), ((pItemCoupon->fbModifier & RETURNS_ORIGINAL) ? 0 : 1));
				if (nLength >= 0)
					ptcsBuffer += nLength;

				PUTSTRUCTMEMBER(ptcsBuffer,pItemCoupon,uchMajorClass,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemCoupon,uchMinorClass,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemCoupon,uchCouponNo,pFormatNumeric);
				ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pItemCoupon->aszMnemonic, sizeof(pItemCoupon->aszMnemonic));
				PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszMnemonic,pFormatString);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemCoupon,lAmount,pFormatDcurrency);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemCoupon,lDoubleAmount,pFormatDcurrency);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemCoupon,usReasonCode,pFormatNumeric);
				if (pItemCoupon->usReasonCode != 0) {
					OPMNEMONICFILE MnemonicFile;

					MnemonicFile.usMnemonicFileId = OPMNEMONICFILE_ID_REASON;
					MnemonicFile.usMnemonicAddress = pItemCoupon->usReasonCode;
					OpMnemonicsFileIndRead(&MnemonicFile, 0);
					ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, MnemonicFile.aszMnemonicValue, sizeof(MnemonicFile.aszMnemonicValue));
					PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszReasonMnemonic,pFormatString);
				}
				PUTSTRUCTMEMBER(ptcsBuffer,pItemCoupon,uchHourlyOffset,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemCoupon,usBonusIndex,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemCoupon,fbModifier,pFormatNumericHex);
				PUTSTRINGLABEL(ptcsBuffer,pItemCoupon->fbStatus[0],fbStatus_0,pFormatNumericHex);
				PUTSTRINGLABEL(ptcsBuffer,pItemCoupon->fbStatus[1],fbStatus_1,pFormatNumericHex);

				nLength = _stprintf (ptcsBuffer, _T("</coupon>\r"));
				if (nLength >= 0)
					ptcsBuffer += nLength;
			}
			break;

		case CLASS_ITEMDISC:
			{
				ITEMDISC        *pItemDisc;
				int  i;

				/* decompress itemize data */
				memset (&achBuff, 0, sizeof(achBuff));
				usReadLen = RflGetStorageItem (achBuff, puchWorkBuffer, RFL_WITH_MNEM);

				pItemDisc = (ITEMDISC *) (achBuff);

				if (pIdxFile) {
					IdxFileTemp = *pIdxFile;
				} else {
					memset (&IdxFileTemp, 0, sizeof (IdxFileTemp));
					IdxFileTemp.uchMinorClass = pItemDisc->uchMinorClass;
				}

				for (i = 0; MyDiscountList[i].aszName != 0; i++) {
					if (MyDiscountList[i].uchMinorClass == IdxFileTemp.uchMinorClass)
						break;
				}

				if (MyDiscountList[i].aszName != 0) {
					nLength = _stprintf (ptcsBuffer, _T("<discount>\r"));
					if (nLength >= 0)
						ptcsBuffer += nLength;

					nLength = _stprintf (ptcsBuffer, _T("<newItem>%d</newItem>\r"), ((pItemDisc->fbDiscModifier & RETURNS_ORIGINAL) ? 0 : 1));
					if (nLength >= 0)
						ptcsBuffer += nLength;

					PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,uchMajorClass,pFormatNumeric);
					PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,uchMinorClass,pFormatNumeric);
					PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,uchDiscountNo,pFormatNumeric);
					PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,uchHourlyOffset,pFormatNumeric);
					if (pItemDisc->auchPLUNo[0]) {
						RflConvertPLU (tempItemSales.auchPLUNo, pItemDisc->auchPLUNo);
						PUTSTRUCTMEMBER(ptcsBuffer,&tempItemSales,auchPLUNo,pFormatString);
					}
					ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pItemDisc->aszMnemonic, sizeof(pItemDisc->aszMnemonic));
					PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszMnemonic,pFormatString);
					PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,lAmount,pFormatDcurrency);
					PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,lQTY,pFormatNumeric);
					PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,uchRate,pFormatNumeric);
					PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,usReasonCode,pFormatNumeric);
					if (pItemDisc->usReasonCode != 0) {
						OPMNEMONICFILE MnemonicFile;

						MnemonicFile.usMnemonicFileId = OPMNEMONICFILE_ID_REASON;
						MnemonicFile.usMnemonicAddress = pItemDisc->usReasonCode;
						OpMnemonicsFileIndRead(&MnemonicFile, 0);
						ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, MnemonicFile.aszMnemonicValue, sizeof(MnemonicFile.aszMnemonicValue));
						PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,aszReasonMnemonic,pFormatString);
					}
					PUTSTRUCTMEMBER(ptcsBuffer,pItemDisc,uchSeatNo,pFormatNumeric);
					PUTSTRINGLABEL(ptcsBuffer,pItemDisc->auchDiscStatus[0],auchStatus_00,pFormatNumericHex);
					PUTSTRINGLABEL(ptcsBuffer,pItemDisc->auchDiscStatus[1],auchStatus_01,pFormatNumericHex);
					PUTSTRINGLABEL(ptcsBuffer,pItemDisc->auchDiscStatus[2],auchStatus_02,pFormatNumericHex);
					PUTSTRINGLABEL(ptcsBuffer,pItemDisc->fbDiscModifier,fbModifier,pFormatNumericHex);

					nLength = _stprintf (ptcsBuffer, _T("</discount>\r"));
					if (nLength >= 0)
						ptcsBuffer += nLength;
				}
			}
			break;

		case CLASS_ITEMAFFECTION:
			{
				ITEMAFFECTION  *pItemAffect;

				/* decompress itemize data */
				memset (&achBuff, 0, sizeof(achBuff));
				usReadLen = RflGetStorageItem (achBuff, puchWorkBuffer, RFL_WITH_MNEM);

				pItemAffect = (ITEMAFFECTION *) (achBuff);

				nLength = _stprintf (ptcsBuffer, _T("<affection>\r"));
				if (nLength >= 0)
					ptcsBuffer += nLength;

				nLength = _stprintf (ptcsBuffer, _T("<newItem>%d</newItem>\r"), ((pItemAffect->fbModifier & RETURNS_ORIGINAL) ? 0 : 1));
				if (nLength >= 0)
					ptcsBuffer += nLength;

				PUTSTRUCTMEMBER(ptcsBuffer,pItemAffect,uchMajorClass,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemAffect,uchMinorClass,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemAffect,lAmount,pFormatDcurrency);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemAffect,lCancel,pFormatDcurrency);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemAffect,ulId,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemAffect,fbModifier,pFormatNumericHex);
				PUTSTRUCTMEMBER(ptcsBuffer,pItemAffect,uchOffset,pFormatNumeric);

				nLength = _stprintf (ptcsBuffer, _T("</affection>\r"));
				if (nLength >= 0)
					ptcsBuffer += nLength;
			}
			break;

		case CLASS_ITEMTRANSOPEN:
			{
				ITEMTRANSOPEN   *pTransOpen;

				/* decompress itemize data */
				memset (&achBuff, 0, sizeof(achBuff));
				usReadLen = RflGetStorageItem (achBuff, puchWorkBuffer, RFL_WITH_MNEM);

				pTransOpen = (ITEMTRANSOPEN *) (achBuff);

				nLength = _stprintf (ptcsBuffer, _T("<tranopen>\r"));
				if (nLength >= 0)
					ptcsBuffer += nLength;

				nLength = _stprintf (ptcsBuffer, _T("<newItem>%d</newItem>\r"), ((pTransOpen->fbModifier & RETURNS_ORIGINAL) ? 0 : 1));
				if (nLength >= 0)
					ptcsBuffer += nLength;

				PUTSTRUCTMEMBER(ptcsBuffer,pTransOpen,uchMajorClass,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pTransOpen,uchMinorClass,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pTransOpen,ulCashierID,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pTransOpen,usGuestNo,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pTransOpen,usTableNo,pFormatNumeric);
				PUTSTRUCTMEMBER(ptcsBuffer,pTransOpen,uchHourlyOffset,pFormatNumeric);

				nLength = _stprintf (ptcsBuffer, _T("</tranopen>\r"));
				if (nLength >= 0)
					ptcsBuffer += nLength;
			}
			break;

		case CLASS_ITEMTRANSCLOSE:
			break;

		default:
			break;
	}

	*ptcsBufferAdr = ptcsBuffer;

	return 0;
}

SHORT ConnEngineSendTransactionFH (BOOL bRecordEj, SHORT sGcfStatus, GCNUM usGCNumber, SHORT hsFileHandle, ULONG ulStartPoint, ULONG ulSizeIn)
{
	TCHAR          *ptcsBuffer;
	int            nLength;
	ULONG          ulSize = 0, ulItemsAreaOffset = 0;
	ULONG          ulActualBytesRead = 0;
	TrnVliOffset   usTranConsStoVli = 0;
	USHORT         usItemNumber;
	PRTIDXHDR      IdxFileInfo;
	PRTIDX         IdxFile;
    UCHAR                  auchTranStorageWork[ TRN_TEMPBUFF_SIZE ];
    UCHAR                 *puchWorkBuffer;
	TRANGCFQUAL           WorkGcfQual;
    TRANSTORAGESALESNON   *pTranNon;
	struct {
		UCHAR  uchMinorClass;
		TCHAR  *aszName;
	} MyDiscountList[] = {
			{CLASS_UIREGDISC1, _T("CLASS_UIREGDISC1")},        // regular discount #1 */
			{CLASS_UIREGDISC2, _T("CLASS_UIREGDISC2")},        // regular discount #2 */
			{CLASS_UIREGDISC3, _T("CLASS_UIREGDISC3")},        // regular discount #3 */
			{CLASS_UIREGDISC4, _T("CLASS_UIREGDISC4")},        // regular discount #4 */
			{CLASS_UIREGDISC5, _T("CLASS_UIREGDISC5")},        // regular discount #5 */
			{CLASS_UIREGDISC6, _T("CLASS_UIREGDISC6")},        // regular discount #6 */
			{CLASS_UICHARGETIP, _T("CLASS_UICHARGETIP")},      // charge tip */
			{CLASS_UICHARGETIP2, _T("CLASS_UICHARGETIP2")},    // charge tip, V3.3 */
			{CLASS_UICHARGETIP3, _T("CLASS_UICHARGETIP3")},    // charge tip, V3.3 */
			{CLASS_AUTOCHARGETIP, _T("CLASS_AUTOCHARGETIP")},      // auto charge tip, V3.3 */
			{CLASS_AUTOCHARGETIP2, _T("CLASS_AUTOCHARGETIP2")},    // auto charge tip, V3.3 */
			{CLASS_AUTOCHARGETIP3, _T("CLASS_AUTOCHARGETIP3")},    // auto charge tip, V3.3 */
			{0, 0}
		};

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return 0;
	}

	ConnEngineEnterCriticalRegion ();

	ulSize = ulStartPoint;
	// Determine if we need to adjust the offset within the transaction file or not.
	// If the transaction file is on this terminal, we are the Master and transaction is
	// from the Master then we do not need to adjust the offset to access the transaction
	// data area of the transaction file.
	// However if this is a transaction file transfered from a Satellite Terminal then
	// the transaction file contains not only the leading two USHORT variables, it also
	// contains the request header information so we have to have an offset that accomodates
	// that additional data.  We do this by checking the passed in ulStartPoint to see if it
	// is greater than the standard two USHORT variables.
	if (ulStartPoint > sizeof(USHORT) + sizeof(USHORT))
		ulItemsAreaOffset = ulSize - (sizeof(USHORT) + sizeof(USHORT));


	// we expect this file to have a Consolidated Storage layout.
	// The header part of this file is a TRANCONSSTORAGEHEADER struct.
	PifSeekFile (hsFileHandle, ulSize, &ulActualBytesRead);
    PifReadFile(hsFileHandle, &WorkGcfQual, sizeof(WorkGcfQual), &ulActualBytesRead);
	ulSize += sizeof(TRANGCFQUAL);
	ulSize += sizeof(TRANITEMIZERS);
	if (ulSize < ulSizeIn) {
		PifSeekFile (hsFileHandle, ulSize, &ulActualBytesRead);
		PifReadFile(hsFileHandle, &usTranConsStoVli, sizeof(usTranConsStoVli), &ulActualBytesRead);
		ulSize += sizeof(usTranConsStoVli);
		ulSize += usTranConsStoVli;
	}

    puchWorkBuffer = &auchTranStorageWork[ 0 ];
	pTranNon = ( TRANSTORAGESALESNON * ) ( puchWorkBuffer + sizeof( UCHAR ));

	if (sGcfStatus < 0) {
		TCHAR  tcsReference[128], tcsPrefix[256];
		TCHAR  *tcsErrorString = _T("Error retrieving Guest Check.");
		ULONG  ulErrorCode = 1;

		if (sGcfStatus == GCF_NOT_IN) {
			tcsErrorString = _T("GCF_NOT_IN: Guest Check not found.");
		} else if (sGcfStatus == GCF_LOCKED) {
			tcsErrorString = _T("GCF_LOCKED: Guest Check found but locked.");
		} else if (sGcfStatus == GCF_ALL_LOCKED) {
			tcsErrorString = _T("GCF_ALL_LOCKED: Guest Check found but locked.");
		} else if (sGcfStatus == GCF_NOT_MASTER) {
			tcsErrorString = _T("GCF_NOT_MASTER: Guest Check stored on Master only.");
		}
		ulErrorCode = abs(sGcfStatus);
		_stprintf (tcsReference, _T("gcnum %d, sStatus %d"), usGCNumber, sGcfStatus);

		// provide the transaction date and time for when the transaction was actually done.
		_stprintf (tcsPrefix, _T("<gcnum>%d</gcnum>\r<creationtime>%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d(%s)</creationtime>\r"), usGCNumber,
			WorkGcfQual.DateTimeStamp.usYear, WorkGcfQual.DateTimeStamp.usMonth, WorkGcfQual.DateTimeStamp.usMDay, WorkGcfQual.DateTimeStamp.usHour,
			WorkGcfQual.DateTimeStamp.usMin, WorkGcfQual.DateTimeStamp.usSec, WorkGcfQual.DateTimeStamp.wszTimeZoneName);
		ConnEngineSendResponseWithErrorLocal(_T("GUESTCHECK_RTRV"), tcsPrefix, _T("Failed"), tcsReference, ulErrorCode, tcsErrorString);

		if (ConnEngineStateSnapShotData.ulFlagsAndState & CONNENGINESTATESNAPSHOT_DATAREAD) {
			ConnEngineStateSnapShotRead (&ConnEngineStateSnapShotData);
			ConnEngineStateSnapShotData.ulGuestCheckCountError++;
			ConnEngineStateSnapShotWrite (&ConnEngineStateSnapShotData);
		}

		ConnEngineLeaveCriticalRegion ();
		return 0;
	}

	ptcsBuffer = ConnEngineSendBuffer;
	nLength = _stprintf (ptcsBuffer, _T("<ITEMSALES>\r<gcnum>%d</gcnum>\r"), usGCNumber);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	{
		TCHAR  tchUniqueIdentifier[32];

		// Since these values are coming from the memory resident data base, they also need to be processed
		// in function ObjectMakerTRANSACTION() and function CliGusLoadFromConnectionEngine() so that the
		// memory resident data base is updated with these global flags.
		memset (tchUniqueIdentifier, 0, sizeof(tchUniqueIdentifier));
		UifConvertUniqueIdReturningString (tchUniqueIdentifier, WorkGcfQual.uchUniqueIdentifier);

		nLength = _stprintf (ptcsBuffer, _T("<uchUniqueIdentifier>%s</uchUniqueIdentifier>\r"), tchUniqueIdentifier);
		if (nLength >= 0)
			ptcsBuffer += nLength;

		nLength = _stprintf (ptcsBuffer, _T("<fsGCFStatus>0x%x</fsGCFStatus>\r<fsGCFStatus2>0x%x</fsGCFStatus2>\r"), WorkGcfQual.fsGCFStatus, WorkGcfQual.fsGCFStatus2);
		if (nLength >= 0)
			ptcsBuffer += nLength;

		if (WorkGcfQual.fsGCFStatus & GCFQUAL_TRETURN) {
			// if this is a return then we want to include the unique identifier of the transaction that
			// the return is based upon.
			UifConvertUniqueIdReturningString (tchUniqueIdentifier, WorkGcfQual.uchUniqueIdentifierReturn);
			nLength = _stprintf (ptcsBuffer, _T("<uchUniqueIdentifierReturn>%s</uchUniqueIdentifierReturn>\r"), tchUniqueIdentifier);
			if (nLength >= 0)
				ptcsBuffer += nLength;
			nLength = _stprintf (ptcsBuffer, _T("<usReturnType>%d</usReturnType>\r"), (WorkGcfQual.usReturnType &0x00ff));  // remove any flags that may be there
			if (nLength >= 0)
				ptcsBuffer += nLength;
			nLength = _stprintf (ptcsBuffer, _T("<usReasonCode>%d</usReasonCode>\r"), WorkGcfQual.usReasonCode);
			if (nLength >= 0)
				ptcsBuffer += nLength;
		}
	}
	{
		int i;
		UCHAR ucStoringTable [] = {0, PIF_STORE_AND_FORWARD_ONE_ON, PIF_STORE_AND_FORWARD_TWO_ON};
		int iEnabled = ((ParaStoreForwardFlagAndStatus() & PIF_STORE_AND_FORWARD_ENABLED) != 0) ? 1 : 0;

		nLength = _stprintf (ptcsBuffer, _T("<storeforward>\r<enabled>%d</enabled>\r"), iEnabled);
		if (nLength >= 0)
			ptcsBuffer += nLength;

		for (i = 1; i <= 2; i++) {
			int iStoring = ((ParaStoreForwardFlagAndStatus() & ucStoringTable[i]) != 0) ? 1 : 0;

			nLength = _stprintf (ptcsBuffer, _T("<file>\r<slot>%d</slot>\r<storing>%d</storing>\r</file>\r"), i, iStoring);
			if (nLength >= 0)
				ptcsBuffer += nLength;
		}
		nLength = _stprintf (ptcsBuffer, _T("</storeforward>\r"));
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}


	// The following global values from the memory resident data base are data for transaction storage however
	// these values will be modified if the transaction is read back in as a return.
	PUTSTRUCTMEMBER(ptcsBuffer,&WorkGcfQual,ulCashierID,pFormatNumeric);  // Provide the cashier ID in the itemsales message

	// provide the transaction date and time for when the transaction was actually done.
	{
		nLength = _stprintf (ptcsBuffer, _T("<trandatetime>%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d(%s)</trandatetime>\r"),
			WorkGcfQual.DateTimeStamp.usYear, WorkGcfQual.DateTimeStamp.usMonth, WorkGcfQual.DateTimeStamp.usMDay, WorkGcfQual.DateTimeStamp.usHour,
			WorkGcfQual.DateTimeStamp.usMin, WorkGcfQual.DateTimeStamp.usSec, WorkGcfQual.DateTimeStamp.wszTimeZoneName);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	// If there is index information (usTranConsStoVli is non-zero)
	// then lets read it in as well.
    if ( 0 < usTranConsStoVli ) {

		PifSeekFile (hsFileHandle, ulSize, &ulActualBytesRead);
		PifReadFile(hsFileHandle, &IdxFileInfo, sizeof(PRTIDXHDR), &ulActualBytesRead);
		ulSize += sizeof(PRTIDXHDR);

		// We have copied over the the actual transaction data and now we
		// need to copy over the index file.  The index file provides us the
		// means to find a particular item in the transaction.
		usItemNumber = 0;
		while (usItemNumber < IdxFileInfo.uchNoOfItem) {
			CHAR          achBuff[sizeof(ITEMSALESDISC)];
			USHORT        usReadLen = 0;
			ITEMSALES    *pTranItem = (ITEMSALES *)achBuff;

			//RPH SR# 201
			PifSeekFile (hsFileHandle, ulSize, &ulActualBytesRead);
			PifReadFile(hsFileHandle, &IdxFile, sizeof(PRTIDX), &ulActualBytesRead);

			if (ulActualBytesRead < 1)
				break;

			ulSize += ulActualBytesRead;

			PifSeekFile (hsFileHandle, IdxFile.usItemOffset + ulItemsAreaOffset, &ulActualBytesRead);
			PifReadFile(hsFileHandle, puchWorkBuffer, sizeof(auchTranStorageWork), &ulActualBytesRead);
			ConnEngineSendTransactionItem (&ptcsBuffer, &IdxFile, auchTranStorageWork, ulActualBytesRead, hsFileHandle, ulItemsAreaOffset);
			usItemNumber++;
		}
	}

	// commented out for now. may be needed in future, coordinate with MWS/CWS/Inventory.  Richard Chambers, May 17, 2011
	nLength = _stprintf (ptcsBuffer, _T("<ulStoreregNo>\r<usStoreNo>%d</usStoreNo>\r<usRegNo>%d</usRegNo>\r</ulStoreregNo>\r"), WorkGcfQual.ulStoreregNo/1000, WorkGcfQual.ulStoreregNo % 1000);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	nLength = _stprintf (ptcsBuffer, _T("<usConsNo>%d</usConsNo>\r"), WorkGcfQual.usConsNo);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	{
		DATE_TIME DateTime;          /* for TOD read */

		PifGetDateTime(&DateTime);
		nLength = _stprintf (ptcsBuffer, _T("<datetime>%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d(%s)</datetime>\r"),
			DateTime.usYear, DateTime.usMonth, DateTime.usMDay, DateTime.usHour, DateTime.usMin, DateTime.usSec, DateTime.wszTimeZoneName);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	nLength = _stprintf (ptcsBuffer, _T("</ITEMSALES>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	ConnEngineStateSnapShotRead (&ConnEngineStateSnapShotData);
	ConnEngineStateSnapShotData.ulGuestCheckCount++;
	ConnEngineStateSnapShotData.usGCNumber = usGCNumber;
	ConnEngineStateSnapShotData.usConsNo = WorkGcfQual.usConsNo;

	// There are some guest checks transmissions that should not be recorded in the
	// Connection Engine Interface Electronic Journal.  If we are sending a saved
	// Guest Check that is being retrieved then do not record this in the file.
	ConnEngineObjectSendMessage (bRecordEj, 0, ConnEngineSendBuffer, ptcsBuffer - ConnEngineSendBuffer);  // record in EJ

	ConnEngineLeaveCriticalRegion ();
	return 0;
}


SHORT ConnEngineSendButtonPress (TCHAR *tcsWindowName, TCHAR *tcsButtonName, TCHAR *tcsActionType, int iExtendedFsc, TCHAR *tcsArgument)
{
	TCHAR      tcsBuffer[4096], *ptcsBuffer;
	int        nLength;

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return 0;
	}

	ConnEngineEnterCriticalRegion ();

	ptcsBuffer = tcsBuffer;
	nLength = _stprintf (ptcsBuffer, _T("<BUTTONPRESS>\r<windowname>%s</windowname>\r<buttonname>%s</buttonname>\r"), tcsWindowName, tcsButtonName);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	nLength = _stprintf (ptcsBuffer, _T("<actiontype>%s</actiontype>\r<extendedfsc>%d</extendedfsc>\r<argument>%s</argument>\r"), tcsActionType, iExtendedFsc, tcsArgument);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	nLength = _stprintf (ptcsBuffer, _T("</BUTTONPRESS>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	ConnEngineSendMasterConnEngineInterface (0, tcsBuffer, ptcsBuffer - tcsBuffer);  // button presses do not go to the new Electronic Journal

	ConnEngineLeaveCriticalRegion ();
	return 0;
}

static struct _tagConnEngineTerminalRequest {
	USHORT  usSequenceNumber;
	TCHAR   tcsIdentifier[16];
} ConnEngineTerminalRequestArray[] = {
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0}
};

SHORT ConnEngineStartRequest(USHORT usTerminalNo, TCHAR *tcsTextBuffer)
{
	TCHAR      *ptcsBuffer;
	int        nLength, nTotalLen;
	SHORT      sRetStatus = 0;

	if (usTerminalNo < 1 || usTerminalNo > 16)
		return -1;

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return 0;
	}

	ConnEngineEnterCriticalRegion ();

	ConnEngineTerminalRequestArray[usTerminalNo].usSequenceNumber = 1;

	nTotalLen = 0;

	ptcsBuffer = ConnEngineSendBuffer;
	nLength = _stprintf (ptcsBuffer, _T("<CONNENGINE>\r<terminalnumber>%d</terminalnumber>\r"), usTerminalNo);
	if (nLength >= 0) {
		ptcsBuffer += nLength;
		nTotalLen += nLength;
	}

	_tcscpy (ptcsBuffer, tcsTextBuffer);
	nLength = _tcslen (ptcsBuffer);
	if (nLength >= 0) {
		ptcsBuffer += nLength;
		nTotalLen += nLength;
	}

	nLength = _stprintf (ptcsBuffer, _T("</CONNENGINE>\r"));
	if (nLength >= 0) {
		ptcsBuffer += nLength;
		nTotalLen += nLength;
	}

	sRetStatus = ConnEngineSendMasterConnEngineInterface (0, ConnEngineSendBuffer, nTotalLen);  // button presses do not go to the new Electronic Journal

	ConnEngineLeaveCriticalRegion ();

	if (sRetStatus == 1)
		sRetStatus = 0;
	else
		sRetStatus = LDT_LNKDWN_ADR;
	return sRetStatus;
}

static SHORT ConnEngineSendRequestIndicator = 0;
static LONG  ConnEngineSendRequestErrorCode = 0;

SHORT ConnEngineSendRequestTransactionStatus (int iIndicator, LONG *plErrorCode)
{
	if (iIndicator == 0) {
		// indicate that no one is waiting on a message.
		// we will not change the error code so that the last one
		// can be retrieved at any time until a new session is started.
		ConnEngineSendRequestIndicator = 0;
	} else if (iIndicator == 1) {
		// indicate that we are starting to look for a response to a send request
		// we will indicate that nothing has come in by setting the error code to negative one.
		ConnEngineSendRequestIndicator = 1;
		ConnEngineSendRequestErrorCode = -1;   // indicate that nothing has been received.
	} else if (iIndicator == 2) {
		ConnEngineSendRequestIndicator = 2;
		if (plErrorCode)
			ConnEngineSendRequestErrorCode = *plErrorCode;
	} else {
		// we will return the error code setting if it is requested
		if (plErrorCode)
			*plErrorCode = ConnEngineSendRequestErrorCode;
	}

	return ConnEngineSendRequestIndicator;
}

SHORT ConnEngineSendRequestTransaction (TCHAR *tcsActionType, TCHAR  *uchUniqueIdentifier)
{
	TCHAR      tcsBuffer[4096], *ptcsBuffer;
	int        nLength;
	SHORT      sRetStatus = 0;

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return LDT_LNKDWN_ADR;
	}

	ConnEngineEnterCriticalRegion ();

	ptcsBuffer = tcsBuffer;
	nLength = _stprintf (ptcsBuffer, _T("<TRANSACTIONREQUEST>\r<actiontype>%s</actiontype>\r"), tcsActionType);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	nLength = _stprintf (ptcsBuffer, _T("<uchUniqueIdentifier>%s</uchUniqueIdentifier>\r"), uchUniqueIdentifier);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	ptcsBuffer = ConnEngineAddInfoToOutput (ptcsBuffer);

	nLength = _stprintf (ptcsBuffer, _T("</TRANSACTIONREQUEST>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	sRetStatus = ConnEngineSendMasterConnEngineInterface (0, tcsBuffer, ptcsBuffer - tcsBuffer);  // button presses do not go to the new Electronic Journal

	ConnEngineLeaveCriticalRegion ();

	if (sRetStatus == 1)
		sRetStatus = 0;
	else
		sRetStatus = LDT_LNKDWN_ADR;
	return sRetStatus;
}

SHORT ConnEngineSendDataQueryResult (TCHAR *tcsActionType, TCHAR  *uchDataQueryResult)
{
	TCHAR      tcsBuffer[4096], *ptcsBuffer;
	int        nLength;
	SHORT      sRetStatus = 0;

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return LDT_LNKDWN_ADR;
	}

	ConnEngineEnterCriticalRegion ();

	ptcsBuffer = tcsBuffer;
	nLength = _stprintf (ptcsBuffer, _T("<DATAQUERYRESPONSE>\r<actiontype>%s</actiontype>\r"), tcsActionType);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	while (*uchDataQueryResult) {
		*ptcsBuffer++ = *uchDataQueryResult++;
	}

	nLength = _stprintf (ptcsBuffer, _T("</DATAQUERYRESPONSE>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	sRetStatus = ConnEngineSendMasterConnEngineInterface (0, tcsBuffer, ptcsBuffer - tcsBuffer);  // button presses do not go to the new Electronic Journal

	ConnEngineLeaveCriticalRegion ();

	if (sRetStatus == 1)
		sRetStatus = 0;
	else
		sRetStatus = LDT_LNKDWN_ADR;
	return sRetStatus;
}
/***************************************************************
	Send an Employee data change message out the Connection Engine.

	Employee Data Change messages are used to inform the application as to:
		- Employee Time-in Action event
		- Employee Time-out Action event
		- Employee Job Change Action event
**/

SHORT ConnEngineSendCashierActionWithAddon (TCHAR *tcsActionType, TCHAR *tcsEmployeeId, TCHAR *tcsAddOnString)
{
	TCHAR      tcsBuffer[4096], *ptcsBuffer;
	int        nLength;

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return 0;
	}

	ConnEngineEnterCriticalRegion ();

	ptcsBuffer = tcsBuffer;
	nLength = _stprintf (ptcsBuffer, _T("<CASHIERACTION>\r<tcsActionType>%s</tcsActionType>\r"), tcsActionType);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	if (tcsEmployeeId && tcsEmployeeId[0] != 0) {
		nLength = _stprintf (ptcsBuffer, _T("<ulEmployeeId>%s</ulEmployeeId>\r"), tcsEmployeeId);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

#if 1
	ptcsBuffer = ConnEngineAddInfoToOutput (ptcsBuffer);
#else
	{
		PARASPCCO        ParaSpcCo;

		ParaSpcCo.uchMajorClass = CLASS_PARASPCCO;
		ParaSpcCo.uchAddress = SPCO_CONSEC_ADR;
		CliParaRead(&ParaSpcCo);

		nLength = _stprintf (ptcsBuffer, _T("<usConsNo>%d</usConsNo>\r"), ParaSpcCo.usCounter);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	{
		PARASTOREGNO   StRegNoRcvBuff;

        StRegNoRcvBuff.uchMajorClass = CLASS_PARASTOREGNO;    /* get store/ reg No. */
        StRegNoRcvBuff.uchAddress = SREG_NO_ADR;
        CliParaRead(&StRegNoRcvBuff);
        
        nLength = _stprintf (ptcsBuffer, _T("<ulStoreregNo>\r<usStoreNo>%d</usStoreNo>\r<usRegNo>%d</usRegNo>\r</ulStoreregNo>\r"), StRegNoRcvBuff.usStoreNo, StRegNoRcvBuff.usRegisterNo);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	{
		DATE_TIME DateTime;          /* for TOD read */

		PifGetDateTime(&DateTime);
		nLength = _stprintf (ptcsBuffer, _T("<datetime>%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d(%s)</datetime>\r"),
			DateTime.usYear, DateTime.usMonth, DateTime.usMDay, DateTime.usHour, DateTime.usMin, DateTime.usSec, DateTime.wszTimeZoneName);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}
#endif

	if (tcsAddOnString && tcsAddOnString[0] != 0) {
		nLength = _stprintf (ptcsBuffer, _T("%s\r"), tcsAddOnString);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	nLength = _stprintf (ptcsBuffer, _T("</CASHIERACTION>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	ConnEngineStateSnapShotRead (&ConnEngineStateSnapShotData);
	ConnEngineStateSnapShotData.ulCashierAction++;
	ConnEngineStateSnapShotData.usGCNumber = 0;
	ConnEngineStateSnapShotData.usConsNo = 0;

	if (_tcscmp (tcsActionType, CONNENGINE_ACTIONTYPE_SIGNIN) == 0) {
		ConnEngineStateSnapShotData.ulCashierSignIn++;
	} else if (_tcscmp (tcsActionType, CONNENGINE_ACTIONTYPE_SIGNOUT) == 0) {
		ConnEngineStateSnapShotData.ulCashierSignOut++;
	} else if (_tcscmp (tcsActionType, CONNENGINE_ACTIONTYPE_CANCELTRAN) == 0) {
		ConnEngineStateSnapShotData.ulCashierCancelTran++;
	} else if (_tcscmp (tcsActionType, CONNENGINE_ACTIONTYPE_NOSALE) == 0) {
		ConnEngineStateSnapShotData.ulCashierNoSale++;
	} else if (_tcscmp (tcsActionType, CONNENGINE_ACTIONTYPE_PICKUP) == 0) {
		ConnEngineStateSnapShotData.ulCashierPickup++;
	} else if (_tcscmp (tcsActionType, CONNENGINE_ACTIONTYPE_LOAN) == 0) {
		ConnEngineStateSnapShotData.ulCashierLoan++;
	} else if (_tcscmp (tcsActionType, CONNENGINE_ACTIONTYPE_PAIDOUT) == 0) {
		ConnEngineStateSnapShotData.ulCashierPaidOut++;
	} else if (_tcscmp (tcsActionType, CONNENGINE_ACTIONTYPE_RECEIVEACCOUNT) == 0) {
		ConnEngineStateSnapShotData.ulCashierReceiveAccount++;
	} else {
		ConnEngineStateSnapShotData.ulCashierOther++;
	}

	ConnEngineObjectSendMessage (1, 0, tcsBuffer, ptcsBuffer - tcsBuffer);

	ConnEngineLeaveCriticalRegion ();
	return 0;
}


SHORT ConnEngineSendEmployeeDataChange (TCHAR *tcsActionType, TCHAR *tcsEmployeeId, TCHAR *tcsStatus, ULONG ulStatusFlags)
{
	TCHAR      tcsBuffer[4096], *ptcsBuffer;
	int        nLength;

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return 0;
	}

	ConnEngineEnterCriticalRegion ();

	if (tcsStatus == 0)
		tcsStatus = _T("ok");

	ptcsBuffer = tcsBuffer;
	nLength = _stprintf (ptcsBuffer, _T("<EMPLOYEECHANGE>\r<tcsActionType>%s</tcsActionType>\r<ulEmployeeId>%s</ulEmployeeId>\r<tcsStatus>%s</tcsStatus>\r"), tcsActionType, tcsEmployeeId, tcsStatus);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	if (ulStatusFlags != 0) {
		if ((ulStatusFlags & UIFREGMISC_STATUS_FIRST) != 0) {
			nLength = _stprintf (ptcsBuffer, _T("<firstemployee>1</firstemployee>\r"));
			if (nLength >= 0)
				ptcsBuffer += nLength;
		}
	}

#if 1
	ptcsBuffer = ConnEngineAddInfoToOutput (ptcsBuffer);
#else
	{
		PARASTOREGNO   StRegNoRcvBuff;

        StRegNoRcvBuff.uchMajorClass = CLASS_PARASTOREGNO;    /* get store/ reg No. */
        StRegNoRcvBuff.uchAddress = SREG_NO_ADR;
        CliParaRead(&StRegNoRcvBuff);
        
        nLength = _stprintf (ptcsBuffer, _T("<ulStoreregNo>\r<usStoreNo>%d</usStoreNo>\r<usRegNo>%d</usRegNo>\r</ulStoreregNo>\r"), StRegNoRcvBuff.usStoreNo, StRegNoRcvBuff.usRegisterNo);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	{
		DATE_TIME DateTime;          /* for TOD read */

		PifGetDateTime(&DateTime);
		nLength = _stprintf (ptcsBuffer, _T("<datetime>%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d(%s)</datetime>\r"),
			DateTime.usYear, DateTime.usMonth, DateTime.usMDay, DateTime.usHour, DateTime.usMin, DateTime.usSec, DateTime.wszTimeZoneName);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}
#endif

	nLength = _stprintf (ptcsBuffer, _T("</EMPLOYEECHANGE>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	ConnEngineStateSnapShotRead (&ConnEngineStateSnapShotData);
	ConnEngineStateSnapShotData.ulEmployeeChange++;
	ConnEngineStateSnapShotData.usGCNumber = 0;
	ConnEngineStateSnapShotData.usConsNo = 0;
	ConnEngineObjectSendMessage (1, 0, tcsBuffer, ptcsBuffer - tcsBuffer);

	ConnEngineLeaveCriticalRegion ();
	return 0;
}

SHORT ConnEngineSendEmployeeDataField (TCHAR *tcsActionType, VOID  *EtkDetailsArray, int iCount)
{
	TCHAR        *ptcsBuffer;
	int          nLength;
	ETK_DETAILS  *pEtkDetailsArray = (ETK_DETAILS *)EtkDetailsArray;
	TCHAR        auchEmployeeName[ETK_MAX_NAME_SIZE+1];    /* Employee Name, R3.1 */

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return 0;
	}

	ConnEngineEnterCriticalRegion ();

	ptcsBuffer = ConnEngineSendBuffer;
	nLength = _stprintf (ptcsBuffer, _T("<EMPLOYEECHANGE>\r<tcsActionType>%s</tcsActionType>\r<EmployeeList>\r"), tcsActionType);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	for ( ; iCount > 0; iCount--, pEtkDetailsArray++) {
		nLength = _stprintf (ptcsBuffer, _T("<EmployeeItem>\r"));
		if (nLength >= 0)
			ptcsBuffer += nLength;

		nLength = _stprintf (ptcsBuffer, _T("<ulEmployeeId>%d</ulEmployeeId>\r"), pEtkDetailsArray->ulEmployeeNo);
		if (nLength >= 0)
			ptcsBuffer += nLength;

		_tcsncpy (auchEmployeeName, pEtkDetailsArray->auchEmployeeName, ETK_MAX_NAME_SIZE);
		auchEmployeeName[ETK_MAX_NAME_SIZE] = 0;

		nLength = _stprintf (ptcsBuffer, _T("<tcsMnemonic>%s</tcsMnemonic>\r"), auchEmployeeName);
		if (nLength >= 0)
			ptcsBuffer += nLength;

		nLength = _stprintf (ptcsBuffer, _T("<usJobCode>%d</usJobCode>\r"), pEtkDetailsArray->EtkJob.uchJobCode1);
		if (nLength >= 0)
			ptcsBuffer += nLength;

		nLength = _stprintf (ptcsBuffer, _T("</EmployeeItem>\r"));
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	nLength = _stprintf (ptcsBuffer, _T("</EmployeeList>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

#if 1
	ptcsBuffer = ConnEngineAddInfoToOutput (ptcsBuffer);
#else
	{
		PARASTOREGNO   StRegNoRcvBuff;

        StRegNoRcvBuff.uchMajorClass = CLASS_PARASTOREGNO;    /* get store/ reg No. */
        StRegNoRcvBuff.uchAddress = SREG_NO_ADR;
        CliParaRead(&StRegNoRcvBuff);
        
        nLength = _stprintf (ptcsBuffer, _T("<ulStoreregNo>\r<usStoreNo>%d</usStoreNo>\r<usRegNo>%d</usRegNo>\r</ulStoreregNo>\r"), StRegNoRcvBuff.usStoreNo, StRegNoRcvBuff.usRegisterNo);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	{
		DATE_TIME DateTime;          /* for TOD read */

		PifGetDateTime(&DateTime);
		nLength = _stprintf (ptcsBuffer, _T("<datetime>%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d(%s)</datetime>\r"),
			DateTime.usYear, DateTime.usMonth, DateTime.usMDay, DateTime.usHour, DateTime.usMin, DateTime.usSec, DateTime.wszTimeZoneName);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}
#endif

	nLength = _stprintf (ptcsBuffer, _T("</EMPLOYEECHANGE>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	ConnEngineObjectSendMessage (1, 0, ConnEngineSendBuffer, ptcsBuffer - ConnEngineSendBuffer);

	ConnEngineLeaveCriticalRegion ();
	return 0;
}

SHORT ConnEngineSendCashierActionWithAmount (TCHAR *tcsActionType, TCHAR *tcsEmployeeId, TCHAR *tcsReference, DCURRENCY lAmount, USHORT usVoidIndic)
{
	TCHAR      tcsBuffer[512], *ptcsBuffer;
	int        nLength;

	ptcsBuffer = tcsBuffer;

	if (tcsReference && tcsReference[0] != 0) {
		nLength = _stprintf (ptcsBuffer, _T("<tcsReference>%s</tcsReference>\r"), tcsReference);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

#if defined(DCURRENCY_LONGLONG)
	nLength = _stprintf (ptcsBuffer, _T("<lAmount>%lld</lAmount>\r"), lAmount);
#else
	nLength = _stprintf (ptcsBuffer, _T("<lAmount>%d</lAmount>\r"), lAmount);
#endif
	if (nLength >= 0)
		ptcsBuffer += nLength;

	nLength = _stprintf (ptcsBuffer, _T("<usVoidIndic>%d</usVoidIndic>\r"), usVoidIndic);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	return ConnEngineSendCashierActionWithAddon (tcsActionType, tcsEmployeeId, tcsBuffer);
}

SHORT ConnEngineSendCashierActionWithError (TCHAR *tcsActionType, TCHAR *tcsReference, ULONG ulErrorCode, TCHAR *tcsErrorString)
{
	TCHAR      tcsBuffer[512], *ptcsBuffer;
	int        nLength;

	ptcsBuffer = tcsBuffer;

	if (tcsReference && tcsReference[0] != 0) {
		nLength = _stprintf (ptcsBuffer, _T("<tcsReference>%s</tcsReference>\r"), tcsReference);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	nLength = _stprintf (ptcsBuffer, _T("<errorcode>%d</errorcode>\r"), ulErrorCode);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	if (tcsErrorString && tcsErrorString[0] != 0) {
		nLength = _stprintf (ptcsBuffer, _T("<errortext>%s</errortext>\r"), tcsErrorString);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	return ConnEngineSendCashierActionWithAddon (tcsActionType, 0, tcsBuffer);
}

SHORT ConnEngineSendResponseWithErrorLocal (TCHAR *tcsCommandType, TCHAR *tcsPrefix, TCHAR *tcsActionType, TCHAR *tcsReference, ULONG ulErrorCode, TCHAR *tcsErrorString)
{
	TCHAR      tcsBuffer[1024], *ptcsBuffer;
	int        nLength;

	ptcsBuffer = tcsBuffer;

	if (tcsPrefix && tcsPrefix[0] != 0) {
		nLength = _stprintf (ptcsBuffer, _T("%s\r"), tcsPrefix);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	if (tcsActionType && tcsActionType[0] != 0) {
		nLength = _stprintf (ptcsBuffer, _T("<tcsActionType>%s</tcsActionType>\r"), tcsActionType);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	if (tcsReference && tcsReference[0] != 0) {
		nLength = _stprintf (ptcsBuffer, _T("<tcsReference>%s</tcsReference>\r"), tcsReference);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	nLength = _stprintf (ptcsBuffer, _T("<errorcode>%d</errorcode>\r"), ulErrorCode);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	if (tcsErrorString && tcsErrorString[0] != 0) {
		nLength = _stprintf (ptcsBuffer, _T("<errortext>%s</errortext>\r"), tcsErrorString);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	nLength = _stprintf (ConnEngineSendBuffer, _T("<%s>\r%s</%s>"), tcsCommandType, tcsBuffer, tcsCommandType);

	ConnEngineSendMasterConnEngineInterface (0, ConnEngineSendBuffer, nLength);

	return 0;
}


SHORT ConnEngineSendResponseWithError (TCHAR *tcsCommandType, TCHAR *tcsActionType, TCHAR *tcsReference, ULONG ulErrorCode, TCHAR *tcsErrorString)
{
	SHORT      sRetStatus;

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return 0;
	}

	ConnEngineEnterCriticalRegion ();

	sRetStatus = ConnEngineSendResponseWithErrorLocal(tcsCommandType, 0, tcsActionType, tcsReference, ulErrorCode, tcsErrorString);

	ConnEngineLeaveCriticalRegion ();

	return sRetStatus;
}


SHORT ConnEngineEjEntryNext (ULONG ulFilePosHigh, ULONG ulFilePosLow)
{
	TCHAR      *szConnEngineElectronicJournal = _T("TOTALEJG");
	TCHAR      *szEjEntryStart = _T("<ej_entry_start ");
	TCHAR      *szEjEntryEnd = _T("</ej_entry_start>");
	TCHAR      *ptcsBuffer;
	TCHAR      tcsBuff[1024];
	SHORT      shFileHandle;

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return 0;
	}

	ConnEngineEnterCriticalRegion ();

	ptcsBuffer = ConnEngineSendBuffer;

	shFileHandle = PifOpenFile (szConnEngineElectronicJournal, "r");

	if (shFileHandle >= 0) {
		ULONG  ulActualPosition, ulBytesRead;
		ULONG ulFileSizeHigh, ulFileSizeLow;
		ULONG  ulTagLengthOffset = _tcslen (szEjEntryEnd) * sizeof(TCHAR);
		TCHAR  *tcsEntryStart, *tcsEntryEnd;

		ulFileSizeLow = PifGetFileSize(shFileHandle, &ulFileSizeHigh);

		if (ulFilePosLow < ulFileSizeLow) {
			ulFilePosLow &= 0xfffffffe;   // make sure that the file position is a multiple of 2 since we are doing TCHARs in the file.

			PifSeekFile(shFileHandle, ulFilePosLow, &ulActualPosition);

			memset (tcsBuff, 0, sizeof(tcsBuff));
			PifReadFile (shFileHandle, tcsBuff, ((sizeof(tcsBuff)/sizeof(tcsBuff[0]) - 2) * sizeof(TCHAR)), &ulBytesRead);
			tcsEntryStart = _tcsstr (tcsBuff, szEjEntryStart);
			while (tcsEntryStart == 0 && ulBytesRead > ulTagLengthOffset) {
				ulFilePosLow += ulBytesRead - ulTagLengthOffset;
				PifSeekFile (shFileHandle, ulFilePosLow, &ulActualPosition);
				PifReadFile (shFileHandle, tcsBuff, ((sizeof(tcsBuff)/sizeof(tcsBuff[0]) - 2) * sizeof(TCHAR)), &ulBytesRead);
				tcsEntryStart = _tcsstr (tcsBuff, szEjEntryStart);
			}

			if (tcsEntryStart != 0) {
				int  nLength, sBuffEnd;

				sBuffEnd = (tcsBuff + ulBytesRead/sizeof(tcsBuff[0])) - tcsEntryStart;
				_tcsncpy (ptcsBuffer, tcsEntryStart, sBuffEnd);
				ptcsBuffer += sBuffEnd;
				ulFilePosLow += (tcsEntryStart - tcsBuff) * sizeof(TCHAR);
				PifReadFile (shFileHandle, ptcsBuffer, 80000, &ulBytesRead);
				*(ptcsBuffer + ulBytesRead/2 + 1) = 0;  // ensure there is a zero terminator in the string.
				tcsEntryEnd = _tcsstr (ConnEngineSendBuffer, szEjEntryEnd);

				if (tcsEntryEnd) {
					ptcsBuffer = tcsEntryEnd;
				} else {
					NHPOS_ASSERT_TEXT((tcsEntryEnd), "**ERROR ConnEngineEjEntryNext(): </ej_entry_start> not found.");
					ptcsBuffer += ulBytesRead/2;
				}
				nLength = _stprintf (ptcsBuffer, _T("<ejfileinfo>\r<ulFilePositionHigh>%d</ulFilePositionHigh>\r<ulFilePositionLow>%d</ulFilePositionLow>\r</ejfileinfo>\r"), ulFilePosHigh, ulFilePosLow);
				if (nLength >= 0)
					ptcsBuffer += nLength;
				nLength = _stprintf (ptcsBuffer, _T("%s"), szEjEntryEnd);
				if (nLength >= 0)
					ptcsBuffer += nLength;
				PifCloseFile (shFileHandle);    // Close the Connection Engine Electronic Journal
				ConnEngineSendMasterConnEngineInterface (0, ConnEngineSendBuffer, ptcsBuffer - ConnEngineSendBuffer);
			} else {
				PifCloseFile (shFileHandle);    // Close the Connection Engine Electronic Journal
				ConnEngineSendResponseWithErrorLocal(_T("EJENTRYACTION"), 0, _T("next"), 0, -2, _T("Postion past end of file."));
			}
		} else {
			PifCloseFile (shFileHandle);    // Close the Connection Engine Electronic Journal
			ConnEngineSendResponseWithErrorLocal(_T("EJENTRYACTION"), 0, _T("next"), 0, -2, _T("Postion past end of file."));
		}
	} else {
		ConnEngineSendResponseWithErrorLocal(_T("EJENTRYACTION"), 0, _T("next"), 0, -3, _T("Unable to open file."));
	}

	ConnEngineLeaveCriticalRegion ();
	return 0;
}

SHORT  ConnEngineEjEntryFirst (VOID)
{
	return ConnEngineEjEntryNext (0, 0);
}

SHORT ConnEngineEjEntryClear (ULONG ulFilePosHigh, ULONG ulFilePosLow)
{
	TCHAR      *szConnEngineElectronicJournal = _T("TOTALEJG");
	TCHAR      *szConnEngineTempFile = _T("TEMPEJG");
	TCHAR      *szEjEntryStart = _T("<ej_entry_start ");
	TCHAR      *szEjEntryEnd = _T("</ej_entry_start>");
	TCHAR      *ptcsBuffer;
	TCHAR      tcsBuff[1024];
	SHORT      shFileHandle, shFileHandleTemp;

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return 0;
	}

	ConnEngineEnterCriticalRegion ();

	ptcsBuffer = ConnEngineSendBuffer;

	shFileHandle = PifOpenFile (szConnEngineElectronicJournal, "r");
	shFileHandleTemp = PifOpenFile (szConnEngineTempFile, "nw");
	if (shFileHandleTemp < 0) {
		PifDeleteFile(szConnEngineTempFile);   /* Delete existing temp file and try again */
		shFileHandleTemp = PifOpenFile (szConnEngineTempFile, "nw");
	}

	if (shFileHandle >= 0 && shFileHandleTemp >= 0) {
		ULONG  ulActualPosition, ulBytesRead;
		ULONG ulFileSizeHigh, ulFileSizeLow;
		ULONG  ulTagLengthOffset = _tcslen (szEjEntryEnd) * sizeof(TCHAR);
		TCHAR  *tcsEntryStart;

		ulFileSizeLow = PifGetFileSize(shFileHandle, &ulFileSizeHigh);

		if (ulFilePosLow < ulFileSizeLow) {
			ulFilePosLow &= 0xfffffffe;   // make sure that the file position is a multiple of 2 since we are doing TCHARs in the file.

			PifSeekFile(shFileHandle, ulFilePosLow, &ulActualPosition);

			memset (tcsBuff, 0, sizeof(tcsBuff));
			PifReadFile (shFileHandle, tcsBuff, ((sizeof(tcsBuff)/sizeof(tcsBuff[0]) - 2) * sizeof(TCHAR)), &ulBytesRead);
			tcsEntryStart = _tcsstr (tcsBuff, szEjEntryStart);
			while (tcsEntryStart == 0 && ulBytesRead > ulTagLengthOffset) {
				ulFilePosLow += ulBytesRead - ulTagLengthOffset;
				PifSeekFile (shFileHandle, ulFilePosLow, &ulActualPosition);
				PifReadFile (shFileHandle, tcsBuff, ((sizeof(tcsBuff)/sizeof(tcsBuff[0]) - 2) * sizeof(TCHAR)), &ulBytesRead);
				tcsEntryStart = _tcsstr (tcsBuff, szEjEntryStart);
			}

			if (tcsEntryStart != 0) {
				int  sBuffEnd;

				sBuffEnd = (tcsBuff + ulBytesRead/sizeof(tcsBuff[0])) - tcsEntryStart;
				_tcsncpy (ptcsBuffer, tcsEntryStart, sBuffEnd);
				PifWriteFile (shFileHandleTemp, tcsEntryStart, sBuffEnd * sizeof(TCHAR));
				ulFilePosLow += (tcsEntryStart - tcsBuff) * sizeof(TCHAR);
				do {
					PifReadFile (shFileHandle, tcsBuff, sizeof(tcsBuff), &ulBytesRead);
					PifWriteFile (shFileHandleTemp, tcsBuff, ulBytesRead);
				} while (ulBytesRead > 0);
				// close the two files and indicate that we have closed them.
				PifCloseFile (shFileHandle);
				PifCloseFile (shFileHandleTemp);
				shFileHandleTemp = shFileHandle = -1;
				// now delete the TOTALEJG Connection Engine Interface Electronic Journal file
				// and move/rename the copied content to TOTALEJG.
				PifDeleteFile (szConnEngineElectronicJournal);
				PifMoveFile (szConnEngineTempFile, FALSE, szConnEngineElectronicJournal, FALSE);
				ConnEngineSendResponseWithErrorLocal(_T("EJENTRYACTION"), 0, _T("clear"), 0, 0, _T("Clear successful."));
				{
					char  xBuff[128];
					sprintf (xBuff, "ConnEngineEjEntryClear(): EJ Clear %u", ulFilePosLow);
					NHPOS_NONASSERT_TEXT(xBuff);
				}
			} else {
				// close the two files and indicate that we have closed them.
				PifCloseFile (shFileHandle);
				PifCloseFile (shFileHandleTemp);
				shFileHandleTemp = shFileHandle = -1;
				// now delete the TOTALEJG Connection Engine Interface Electronic Journal file
				PifDeleteFile (szConnEngineElectronicJournal);
				NHPOS_NONASSERT_TEXT("ConnEngineEjEntryClear(): EJ Clear all, file position");
				ConnEngineSendResponseWithErrorLocal(_T("EJENTRYACTION"), 0, _T("clear"), 0, 0, _T("Clear all, file position."));
			}
		} else {
			// close the two files and indicate that we have closed them.
			PifCloseFile (shFileHandle);
			PifCloseFile (shFileHandleTemp);
			shFileHandleTemp = shFileHandle = -1;
			// now delete the TOTALEJG Connection Engine Interface Electronic Journal file
			PifDeleteFile (szConnEngineElectronicJournal);
			NHPOS_NONASSERT_TEXT("ConnEngineEjEntryClear(): EJ Clear all file size.");
			ConnEngineSendResponseWithErrorLocal(_T("EJENTRYACTION"), 0, _T("clear"), 0, 0, _T("Clear all, file size."));
		}
	} else {
		ConnEngineSendResponseWithErrorLocal(_T("EJENTRYACTION"), 0, _T("clear"), 0, -3, _T("Unable to open file."));
	}

	if (shFileHandle >= 0) {
		PifCloseFile (shFileHandle);
	}
	if (shFileHandleTemp >= 0) {
		PifCloseFile (shFileHandleTemp);
	}

	ConnEngineLeaveCriticalRegion ();

	return 0;
}


SHORT ConnEngineEjEntrySearch ()
{
	return 0;
}

SHORT ConnEngineSendStoreAndForwardAction (TCHAR *tcsActionType, ITEMTENDER *pItemTender, ITMSTOREFORWARD *pdata, VOID* resData)
{
	TCHAR         tcsBuffer[4096], *ptcsBuffer;
	int           nLength;
	EEPTRSPDATA*  pResData = (EEPTRSPDATA*)resData;
	TCHAR         tcsMnemonicTempBuff[256];

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return 0;
	}

	ConnEngineEnterCriticalRegion ();

	ptcsBuffer = tcsBuffer;
	nLength = _stprintf (ptcsBuffer, _T("<STOREFORWARDACTION>\r<tcsActionType>%s</tcsActionType>\r"), tcsActionType);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	nLength = _stprintf (ptcsBuffer, _T("<checkData>\r<gcnum>%d</gcnum>\r"), pdata->usGuestNo);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	nLength = _stprintf (ptcsBuffer, _T("<ulEmployeeId>%d</ulEmployeeId>\r"), pdata->ulCashierID);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	nLength = _stprintf (ptcsBuffer, _T("<usConsNo>%d</usConsNo>\r"), pdata->usConsNo);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	{
		TCHAR  tchUniqueIdentifier[32];

		UifConvertUniqueIdReturningString (tchUniqueIdentifier, pdata->uchUniqueIdentifier);
		nLength = _stprintf (ptcsBuffer, _T("<uchUniqueIdentifier>%s</uchUniqueIdentifier>\r"), tchUniqueIdentifier);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	{
		TCHAR  tcsBuffer[48];
		int    i, j;

		for (j = i = 0; i < sizeof(pdata->auchPurchase)/sizeof(pdata->auchPurchase[0]); i++) {
			if (pdata->auchPurchase[i] != '.') {
				tcsBuffer[j] = pdata->auchPurchase[i];
				j++;
			}
		}
		tcsBuffer[j] = 0;
		PUTSTRINGLABEL(ptcsBuffer,tcsBuffer,auchPurchase,pFormatString);

		for (j = i = 0; i < sizeof(pdata->auchGratuity)/sizeof(pdata->auchGratuity[0]); i++) {
			if (pdata->auchGratuity[i] != '.') {
				tcsBuffer[j] = pdata->auchGratuity[i];
				j++;
			}
		}
		tcsBuffer[j] = 0;
		PUTSTRINGLABEL(ptcsBuffer,tcsBuffer,auchGratuity,pFormatString);
	}

	nLength = _stprintf (ptcsBuffer, _T("</checkData>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;


	nLength = ConnEngineSendTransactionTender (pItemTender, ptcsBuffer);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	nLength = _stprintf (ptcsBuffer, _T("<eptstatus>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	switch (pResData->auchEptStatus) {
		case EEPT_FC2_STORED:
		case EEPT_FC2_ACCEPTED:
			_tcscpy (tcsMnemonicTempBuff, _T("Approved"));
			break;

		case EEPT_FC2_REJECTED:
			_tcscpy (tcsMnemonicTempBuff, _T("Declined"));
			break;

		case EEPT_FC2_ERROR:
			_tcscpy (tcsMnemonicTempBuff, _T("Error"));
			break;

		case EEPT_FC2_OVERRIDE:
			_tcscpy (tcsMnemonicTempBuff, _T("Override"));
			break;

		case EEPT_FC2_INVALID:
		default:
			_tcscpy (tcsMnemonicTempBuff, _T("Invalid"));
			break;
	}
	PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,auchStatus,pFormatString);

	ConnEngineObjectMnemonicStringUchar (tcsMnemonicTempBuff, pResData->auchErrorNumber, sizeof(pResData->auchErrorNumber));
	PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,auchErrorNumber,pFormatString);

	ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pResData->auchErrorText, sizeof(pResData->auchErrorText));
	PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,auchErrorText,pFormatString);

	if (pdata->srno.auchRecordNumber[0]) {
		nLength = _stprintf (ptcsBuffer, _T("<auchRecordNumber>%S</auchRecordNumber>\r"), pdata->srno.auchRecordNumber);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	nLength = _stprintf (ptcsBuffer, _T("</eptstatus>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	ptcsBuffer = ConnEngineAddInfoToOutput (ptcsBuffer);

	nLength = _stprintf (ptcsBuffer, _T("</STOREFORWARDACTION>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	ConnEngineStateSnapShotRead (&ConnEngineStateSnapShotData);
	ConnEngineStateSnapShotData.ulStoreForwardAction++;
	ConnEngineStateSnapShotData.usGCNumber = 0;
	ConnEngineStateSnapShotData.usConsNo = 0;

	ConnEngineObjectSendMessage (1, 0, tcsBuffer, ptcsBuffer - tcsBuffer);

	ConnEngineLeaveCriticalRegion ();
	return 0;
}

SHORT ConnEngineSendPreauthCaptureAction (TCHAR *tcsActionType, ITEMTENDER *pItemTender, ITEMTENDERPREAUTH *pdata, VOID* resData)
{
	TCHAR         tcsBuffer[4096], *ptcsBuffer;
	int           nLength;
	EEPTRSPDATA*  pResData = (EEPTRSPDATA*)resData;
	TCHAR         tcsMnemonicTempBuff[256];

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return 0;
	}

	ConnEngineEnterCriticalRegion ();

	ptcsBuffer = tcsBuffer;
	nLength = _stprintf (ptcsBuffer, _T("<CASHIERACTION>\r<tcsActionType>%s</tcsActionType>\r"), tcsActionType);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	nLength = _stprintf (ptcsBuffer, _T("<checkData>\r<gcnum>%d</gcnum>\r"), pdata->usGuestNo);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	nLength = _stprintf (ptcsBuffer, _T("<ulEmployeeId>%d</ulEmployeeId>\r"), pdata->ulCashierID);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	nLength = _stprintf (ptcsBuffer, _T("<usConsNo>%d</usConsNo>\r"), pdata->usConsNo);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	{
		TCHAR  tchUniqueIdentifier[32];

		UifConvertUniqueIdReturningString (tchUniqueIdentifier, pdata->uchUniqueIdentifier);
		nLength = _stprintf (ptcsBuffer, _T("<uchUniqueIdentifier>%s</uchUniqueIdentifier>\r"), tchUniqueIdentifier);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	{
		LONG   lPurchaseAmount = pdata->lTenderAmount;
		TCHAR  tcsBuffer[48];

		lPurchaseAmount -= pdata->lGratuity;    // separate out the gratuity from the tender amount to get the purchase amount
		_stprintf (tcsBuffer, _T("%ld"), lPurchaseAmount);
		PUTSTRINGLABEL(ptcsBuffer,tcsBuffer,auchPurchase,pFormatString);
#if defined(DCURRENCY_LONGLONG)
		_stprintf(tcsBuffer, _T("%lld"), pdata->lGratuity);
#else
		_stprintf (tcsBuffer, _T("%ld"), pdata->lGratuity);
#endif
		PUTSTRINGLABEL(ptcsBuffer,tcsBuffer,auchGratuity,pFormatString);
	}

	nLength = _stprintf (ptcsBuffer, _T("</checkData>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	nLength = ConnEngineSendTransactionTender (pItemTender, ptcsBuffer);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	nLength = _stprintf (ptcsBuffer, _T("<eptstatus>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	switch (pResData->auchEptStatus) {
		case EEPT_FC2_STORED:
		case EEPT_FC2_ACCEPTED:
			_tcscpy (tcsMnemonicTempBuff, _T("Approved"));
			break;

		case EEPT_FC2_REJECTED:
			_tcscpy (tcsMnemonicTempBuff, _T("Declined"));
			break;

		case EEPT_FC2_ERROR:
			_tcscpy (tcsMnemonicTempBuff, _T("Error"));
			break;

		case EEPT_FC2_OVERRIDE:
			_tcscpy (tcsMnemonicTempBuff, _T("Override"));
			break;

		case EEPT_FC2_INVALID:
		default:
			_tcscpy (tcsMnemonicTempBuff, _T("Invalid"));
			break;
	}
	PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,auchStatus,pFormatString);

	ConnEngineObjectMnemonicStringUchar (tcsMnemonicTempBuff, pResData->auchErrorNumber, sizeof(pResData->auchErrorNumber));
	PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,auchErrorNumber,pFormatString);

	ConnEngineObjectMnemonicString (tcsMnemonicTempBuff, pResData->auchErrorText, sizeof(pResData->auchErrorText));
	PUTSTRINGLABEL(ptcsBuffer,tcsMnemonicTempBuff,auchErrorText,pFormatString);

	if (pdata->srno.auchRecordNumber[0]) {
		nLength = _stprintf (ptcsBuffer, _T("<auchRecordNumber>%S</auchRecordNumber>\r"), pdata->srno.auchRecordNumber);
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}

	nLength = _stprintf (ptcsBuffer, _T("</eptstatus>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	ptcsBuffer = ConnEngineAddInfoToOutput (ptcsBuffer);

	nLength = _stprintf (ptcsBuffer, _T("</CASHIERACTION>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	ConnEngineStateSnapShotRead (&ConnEngineStateSnapShotData);
	ConnEngineStateSnapShotData.ulPreauthCaptureAction++;
	ConnEngineStateSnapShotData.usGCNumber = 0;
	ConnEngineStateSnapShotData.usConsNo = 0;

	ConnEngineObjectSendMessage (1, 0, tcsBuffer, ptcsBuffer - tcsBuffer);

	ConnEngineLeaveCriticalRegion ();
	return 0;
}

SHORT ConnEngineSendStateChange (TCHAR *tcsActionType, TCHAR *tcsService, ULONG ulEventId, CONNENGINE_PRINTSPOOLER_DIALOG  *pEventDialog)
{
	TCHAR         tcsBuffer[4096], *ptcsBuffer;
	int           nLength;

	// check Connection Engine Interface status and if not server then do nothing.
	if (fnConnEngineStartStatus (0) == 0) {
		return 0;
	}

	ConnEngineEnterCriticalRegion ();

	ptcsBuffer = tcsBuffer;
	nLength = _stprintf (ptcsBuffer, _T("<STATECHANGE>\r<tcsActionType>%s</tcsActionType>\r<service>%s</service>\r<uleventid>%d</uleventid>\r"), tcsActionType, tcsService, ulEventId);
	if (nLength >= 0)
		ptcsBuffer += nLength;

	if (pEventDialog) {
		nLength = _stprintf (ptcsBuffer, _T("<dialog>\r<uldialogid>%d</uldialogid>\r"), pEventDialog->iDialogId);
		if (nLength >= 0)
			ptcsBuffer += nLength;
		nLength = _stprintf (ptcsBuffer, _T("<title>%s</title>\r<errortext>%s</errortext>\r"), pEventDialog->tcsTitle, pEventDialog->tcsText);
		if (nLength >= 0)
			ptcsBuffer += nLength;
		if (pEventDialog->iNoOptions > 0) {
			int i = 0;
			nLength = _stprintf (ptcsBuffer, _T("<options>\r"));
			if (nLength >= 0)
				ptcsBuffer += nLength;
			for (i = 0; i < pEventDialog->iNoOptions; i++) {
				nLength = _stprintf (ptcsBuffer, _T("<option>\r<id>%d</id>\r<text>%s</text>\r</option>\r"), i+1, pEventDialog->tcsOptions[i]);
				if (nLength >= 0)
					ptcsBuffer += nLength;
			}
			nLength = _stprintf (ptcsBuffer, _T("</options>\r"));
			if (nLength >= 0)
				ptcsBuffer += nLength;
		}
		nLength = _stprintf (ptcsBuffer, _T("</dialog>\r"));
		if (nLength >= 0)
			ptcsBuffer += nLength;
	}
	ptcsBuffer = ConnEngineAddInfoToOutput (ptcsBuffer);

	nLength = _stprintf (ptcsBuffer, _T("</STATECHANGE>\r"));
	if (nLength >= 0)
		ptcsBuffer += nLength;

	ConnEngineStateSnapShotRead (&ConnEngineStateSnapShotData);
	ConnEngineStateSnapShotData.ulStoreForwardAction++;
	ConnEngineStateSnapShotData.usGCNumber = 0;
	ConnEngineStateSnapShotData.usConsNo = 0;

	ConnEngineObjectSendMessage (1, 0, tcsBuffer, ptcsBuffer - tcsBuffer);

	ConnEngineLeaveCriticalRegion ();
	return 0;
}
