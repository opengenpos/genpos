
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <FrameworkIO.h>
#include    <DeviceIOPrinter.h>
#include    <log.h>
#include    <plu.h>
#include    <paraequ.h>
#include    <para.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <storage.h>
#include    <csgcs.h>
#include    <prtprty.h>
#include    <trans.h>
#include    <rfl.h>
#include	<eept.h>
#define __BLNOTIFY_C__
#include    <bl.h>
#include    <csttl.h>
#include    <csop.h>
#include    <report.h>

static TCHAR  *pTokenDelimitersTag = _T("<> =\t\n\r");

typedef 	struct {
		TCHAR  *tcsLabel;
		USHORT  usOffset;
} LabelListItem;

// Remove legacy Rel 2.2.0 GenPOS work done for Amtrak as it handled
// card holder data and was an PCI-DSS In Scope application.
// With OpenGenPOS we are now eliminating all legacy source code and
// proprietary source code. With GenPOS Rel 2.3.0 we were an Out Of Scope
// application relying on the Datacap software components for electronic payment..
// Removed int BuiltInDataQueryStoreForward (TCHAR *tcBuffer, DataQueryObject *pDataQueryObject)
// Removed int BuiltInDataQueryPreauthBatch (TCHAR *tcBuffer, DataQueryObject *pDataQueryObject)
//     03/30/2021  Richard Chambers

static int BuiltInDataQueryGuestChecks (TCHAR *tcBuffer, DataQueryObject *pDataQueryObject)
{
	EEPTF_HEADER StoreForwardData;

	TCHAR *tcsTokenName;
	int i = 0;

	tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
	pDataQueryObject->sContentType = BL_DATAQUERY_CONTENT_TEXT;
	if (tcsTokenName && _tcscmp (tcsTokenName, _T("xml")) == 0) {
		pDataQueryObject->sContentType = BL_DATAQUERY_CONTENT_XML;
	}

	if (pDataQueryObject->ulBufferSize > 255 && pDataQueryObject->ptcsBuffer != 0) {
		int jLength = 0, i;
		UCHAR ucStoringTable [] = {0, PIF_STORE_AND_FORWARD_ONE_ON, PIF_STORE_AND_FORWARD_TWO_ON};

		if (pDataQueryObject->sContentType == BL_DATAQUERY_CONTENT_XML) {
			int iEnabled = ((ParaStoreForwardFlagAndStatus() & PIF_STORE_AND_FORWARD_ENABLED) != 0) ? 1 : 0;
			jLength += _stprintf (pDataQueryObject->ptcsBuffer, _T("<guestcheck>\r<enabled>%d</enabled>\r"), iEnabled);
		}

		for (i = 1; i <= 2; i++) {
			CliStoreForwardRead(i, &StoreForwardData);
			if (pDataQueryObject->sContentType == BL_DATAQUERY_CONTENT_XML) {
				int iStoring = ((ParaStoreForwardFlagAndStatus() & ucStoringTable[i]) != 0) ? 1 : 0;

				jLength += _stprintf (pDataQueryObject->ptcsBuffer + jLength, 
					_T("<file>\r<slot>%d</slot>\r<storing>%d</storing>\r<ulTransStored>%d</ulTransStored>\r<ulFailedTrans>%d</ulFailedTrans>\r<ulErrorTrans>%d</ulErrorTrans>\r</file>\r"),
					i, iStoring, StoreForwardData.ulTransStored, StoreForwardData.ulFailedTrans, StoreForwardData.ulErrorTrans);
			} else {
				jLength += _stprintf (pDataQueryObject->ptcsBuffer + jLength,
					_T("File %d: ulTransStored = %d, ulFailedTrans = %d, ulErrorTrans = %d\r"),
					i, StoreForwardData.ulTransStored, StoreForwardData.ulFailedTrans, StoreForwardData.ulErrorTrans);
			}
		}
		if (pDataQueryObject->sContentType == BL_DATAQUERY_CONTENT_XML) {
			jLength += _stprintf (pDataQueryObject->ptcsBuffer + jLength, _T("</guestcheck>\r"));
		}
		pDataQueryObject->nLines = 2;
	}

	return 0;
}

static int BuiltInDataQueryAc23Report (UCHAR uchMinorClass, TCHAR *tcBuffer, DataQueryObject *pDataQueryObject)
{
	USHORT   usTranMnemonics[300] = {0};
	LONG     lTenderAmounts[300] = {0};
	LONG     lCounts[300] = {0};

	TCHAR *tcsTokenName;
	int i = 0, iDetailOutput = 0;

	tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
	pDataQueryObject->sContentType = BL_DATAQUERY_CONTENT_TEXT;
	if (tcsTokenName && _tcscmp (tcsTokenName, _T("xml")) == 0) {
		pDataQueryObject->sContentType = BL_DATAQUERY_CONTENT_XML;
		tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
	} else if (tcsTokenName && _tcscmp (tcsTokenName, _T("json")) == 0) {
		pDataQueryObject->sContentType = BL_DATAQUERY_CONTENT_JSON;
		tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
	}

//	if (tcsTokenName && _tcscmp (tcsTokenName, _T("details")) == 0) {
//		iDetailOutput = 1;
//	}

	if (pDataQueryObject->ulBufferSize > 255 && pDataQueryObject->ptcsBuffer != 0) {
		FILE  *fpFile = NULL;
		int   jLength = 0;

		if (pDataQueryObject->sContentType == BL_DATAQUERY_CONTENT_XML) {
			jLength += _stprintf (pDataQueryObject->ptcsBuffer, _T("<ac23dailysav>\r"));
		} else if (pDataQueryObject->sContentType == BL_DATAQUERY_CONTENT_JSON) {
			jLength += _stprintf (pDataQueryObject->ptcsBuffer, _T("{ \"ac23dailysav\" : "));
		}

		fpFile = ItemOpenHistorialReportsFolder(RPTREGFIN_FOLDER_QUERY, CLASS_TTLREGFIN, uchMinorClass, RPT_ALL_READ, 0, 0, 0);
		ItemGenerateAc23Report (CLASS_TTLREGFIN, uchMinorClass, RPT_ALL_READ, fpFile, 0);
		RptDescriptionClear();     // clear the no longer needed description without closing any files
		pDataQueryObject->fpFile = fpFile;

		if (pDataQueryObject->sContentType == BL_DATAQUERY_CONTENT_XML) {
			jLength += _stprintf (pDataQueryObject->ptcsBuffer + jLength, _T("</ac23dailysav>\r"));
		} else if (pDataQueryObject->sContentType == BL_DATAQUERY_CONTENT_JSON) {
			jLength += _stprintf (pDataQueryObject->ptcsBuffer + jLength, _T("}"));
		}

		pDataQueryObject->nLines = 2;
	}

	return 0;
}

static int BuiltInDataQueryAc21Report (UCHAR uchMinorClass, TCHAR *tcBuffer, DataQueryObject *pDataQueryObject)
{

	TCHAR *tcsTokenName;
	int i = 0, iDetailOutput = 0;

	tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
	pDataQueryObject->sContentType = BL_DATAQUERY_CONTENT_TEXT;
	if (tcsTokenName && _tcscmp (tcsTokenName, _T("xml")) == 0) {
		pDataQueryObject->sContentType = BL_DATAQUERY_CONTENT_XML;
		tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
	} else if (tcsTokenName && _tcscmp (tcsTokenName, _T("json")) == 0) {
		pDataQueryObject->sContentType = BL_DATAQUERY_CONTENT_JSON;
		tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
	}

//	if (tcsTokenName && _tcscmp (tcsTokenName, _T("details")) == 0) {
//		iDetailOutput = 1;
//	}

	if (pDataQueryObject->ulBufferSize > 255 && pDataQueryObject->ptcsBuffer != 0) {
		FILE  *fpFile = NULL;
		int   jLength = 0;

		if (pDataQueryObject->sContentType == BL_DATAQUERY_CONTENT_XML) {
			jLength += _stprintf (pDataQueryObject->ptcsBuffer, _T("<ac23dailysav>\r"));
		} else if (pDataQueryObject->sContentType == BL_DATAQUERY_CONTENT_JSON) {
			jLength += _stprintf (pDataQueryObject->ptcsBuffer, _T("{ \"ac23dailysav\" : "));
		}

		// open the file for 
		fpFile = ItemOpenHistorialReportsFolder(RPTREGFIN_FOLDER_QUERY, CLASS_TTLCASHIER, uchMinorClass, RPT_ALL_READ, 0, 0, 0);
		ItemGenerateAc21Report (CLASS_TTLCASHIER, uchMinorClass, RPT_ALL_READ, fpFile, 0L);
		RptDescriptionClear();     // clear the no longer needed description without closing any files
		pDataQueryObject->fpFile = fpFile;

		if (pDataQueryObject->sContentType == BL_DATAQUERY_CONTENT_XML) {
			jLength += _stprintf (pDataQueryObject->ptcsBuffer + jLength, _T("</ac23dailysav>\r"));
		} else if (pDataQueryObject->sContentType == BL_DATAQUERY_CONTENT_JSON) {
			jLength += _stprintf (pDataQueryObject->ptcsBuffer + jLength, _T("}"));
		}

		pDataQueryObject->nLines = 2;
	}

	return 0;
}

int DataQueryStartParsing (TCHAR *tcBuffer, DataQueryObject *pDataQueryObject)
{
	TCHAR *ptcBuffer = tcBuffer;
	LabelListItem myList [] = {
		{_T("@storeforward"),     1},
		{_T("@guestchecklist"),   2},
		{_T("@preauthbatch"),     3},
		{_T("@ac23dailycur"),     4},      // CLASS_TTLCURDAY
		{_T("@ac23dailysav"),     5},      // CLASS_TTLSAVDAY
		{_T("@ac23ptdcur"),       6},      // CLASS_TTLCURPTD
		{_T("@ac23ptdsav"),       7},      // CLASS_TTLSAVPTD
		{_T("@ac21dailycur"),     8},
		{_T("@ac21dailysav"),     9},
		{_T("@ac21ptdcur"),      10},
		{_T("@ac21ptdsav"),      11},
		{0,                       0}
	};
	int  myReportType[] = {
		CLASS_TTLCURDAY,           // acXXdailycur
		CLASS_TTLSAVDAY,           // acXXdailysav
		CLASS_TTLCURPTD,           // acXXptdcur
		CLASS_TTLSAVPTD            // acXXptdsav
	};

	TCHAR *tcsTokenName;
	int i = 0;

	tcsTokenName = _tcstok (tcBuffer, pTokenDelimitersTag);

	if (tcsTokenName != NULL && *tcsTokenName != 0) {
		int ttlType;

		for (i = 0; myList[i].tcsLabel; i++) {
			if (_tcscmp (myList[i].tcsLabel, tcsTokenName) == 0) {
				break;
			}
		}

		switch (myList[i].usOffset) {
			case 1:
//				BuiltInDataQueryStoreForward (tcBuffer, pDataQueryObject);   // removed Store and Forward for OpenGenPOS  03/30/2021  Richard Chambers
				break;

			case 3:
//				BuiltInDataQueryPreauthBatch (tcBuffer, pDataQueryObject);   // removed Amtrak Preauth for OpenGenPOS  03/30/2021  Richard Chambers
				break;

			case 4:
			case 5:
			case 6:
			case 7:
				ttlType = myList[i].usOffset - 4;
				BuiltInDataQueryAc23Report (myReportType[ttlType], tcBuffer, pDataQueryObject);
				pDataQueryObject->sContentType = BL_DATAQUERY_CONTENT_HTML;
				break;

			case  8:
			case  9:
			case 10:
			case 11:
				ttlType = myList[i].usOffset - 8;
				BuiltInDataQueryAc21Report (myReportType[ttlType], tcBuffer, pDataQueryObject);
				pDataQueryObject->sContentType = BL_DATAQUERY_CONTENT_HTML;
				break;

			default:
				break;
		}
	}

	return 0;
}


__declspec(dllexport)
int BlProcessDataQueryStatement (TCHAR *ptcBuffer, VOID *pObject)
{
	int         iRetStatus = -1;

	if (ptcBuffer != NULL && *ptcBuffer != 0 && pObject != NULL) {
		iRetStatus = DataQueryStartParsing (ptcBuffer, (DataQueryObject *)pObject);
	}

	return iRetStatus;
}
