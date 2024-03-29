// httpserver.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "afxconv.h"
#include "ServerProc.h"
#include "httpserver.h"

#include <pif.h>
#include <appllog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INIT_OLESYSTEM   HRESULT xOleResult; xOleResult = ::OleInitialize(NULL);\
	ASSERT (xOleResult != OLE_E_WRONGCOMPOBJ);\
	ASSERT (xOleResult != RPC_E_CHANGED_MODE);\
	ASSERT (xOleResult != E_UNEXPECTED);\
	ASSERT (xOleResult != E_OUTOFMEMORY);\
	ASSERT (xOleResult != E_INVALIDARG);\
	ASSERT((xOleResult == S_OK) || (xOleResult == S_FALSE));

#define UNINIT_OLESYSTEM   	::OleUninitialize();

// this is required to build the wrappers for the msxml3 dll
// notice MSXML3 lives in the MSXML2 namespace (gotta love it)
//
// See also the following postings on StackOverflow concerning MSXML and MSXML versions.
//    https://stackoverflow.com/questions/1075891/what-became-of-msxml-4-0
//    https://stackoverflow.com/questions/951804/which-version-of-msxml-should-i-use
//
#import <msxml3.dll>


//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CHttpserverApp

BEGIN_MESSAGE_MAP(CHttpserverApp, CWinApp)
	//{{AFX_MSG_MAP(CHttpserverApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHttpserverApp construction

CHttpserverApp::CHttpserverApp() :
	m_nBytesCount(0),
	fpFileStream(NULL)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHttpserverApp object

CHttpserverApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHttpserverApp initialization

BOOL CHttpserverApp::InitInstance()
{

//	INIT_OLESYSTEM ;

	AfxEnableControlContainer();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	pceConnectSem = new CSemaphore(0, 1, _T("HttpSem"), NULL);

	return TRUE;
}

CHttpserverApp * CHttpserverApp::GetServerObject()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	return &theApp;
}

#define BL_DATAQUERY_CONTENT_UNKNOWN   0
#define BL_DATAQUERY_CONTENT_TEXT      1
#define BL_DATAQUERY_CONTENT_XML       2
#define BL_DATAQUERY_CONTENT_JSON      3
#define BL_DATAQUERY_CONTENT_HTML      4
#define BL_DATAQUERY_CONTENT_JPEG      5

int CHttpserverApp::csIssueRequest ()
{
	struct _tgArray
	{
		TCHAR *tcFileExt;
		char  *cContentType;
	} tgArray [] = {
		{ _T("html"), "text/html"},
		{ _T("txt"),  "text/txt"},
		{ _T("js"),   "text/js"},
		{ _T("json"), "application/json"},
		{ _T("xml"),  "text/xml"},
		{ 0, 0}
	};

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	char *myTextHeaderNoFile = "HTTP/1.0 404 File Not Found\r\n";
	char *myTextHeaderOk = "HTTP/1.0 200 OK\r\n";
	char *myTextLineCache = "Pragma: nocache\r\nCacheControl: no-cache\r\n";
	char *myTextLineHtml = "Content-Type: text/html\r\n";
	char *myTextLineText = "Content-Type: text/txt\r\n";
	char *myTextLineImageJpeg = "Content-Type: img/jpg\r\n";
	char *myTextLineImageGif = "Content-Type: img/gif\r\n";
	char *myTextLineJson = "Content-Type: application/json\r\n";
	char *myTextLineXml = "Content-Type: application/xml\r\n";
	char *myTextLineServer = "Server: NCR General POS Terminal Server/2.2.0 Beta\r\n";
	char *myTextLineConnection = "Connection: close\r\n";
	char *myTextLineLast = "\r\n";
	char *myTextLineOpen = "<html><head><title>NCR General POS Test Page</title></head><body>";
	char *myTextLineDummy = "<h1>NCR General POS</h1><h2>Hello World</h2>";
	char *myTextLineGet = "<h1>NCR General POS</h1><h2>Hello World</h2><H3>Get Request</h3>";
	char *myTextLinePost = "<h1>NCR General POS</h1><h2>Hello World</h2><H3>Post Request</h3>";
	char *myTextLineClose = "</body></html>";
	char *myTextLineFileNotFound = "<html><head><title>404 File not found</title></head><body><h1>404 File not found</h1</body></html>";
	CString csFileName;

	if (m_RequestType == RequestGet)
	{
		if (_tcsstr (m_csGetFileName, _T(".sys")) == 0 &&
			_tcsstr (m_csGetFileName, _T(".xml")) == 0 &&
			_tcsstr (m_csGetFileName, _T(".json")) == 0)
		{
			CHAR tcBuffer[4096];

			csFileName.Format (_T("C:\\FlashDisk\\NCR\\Saratoga\\Web\\%s"), m_csGetFileName);   // should be STD_FOLDER_WEBFOLDER
			csFileName.Replace (_T("/"), _T("\\"));
			csFileName.Replace (_T("\\.."), _T("\\"));
			csFileName.Replace (_T("\\\\"), _T("\\"));
			CFile myFile;
			if (myFile.Open (csFileName, CFile::modeRead | CFile::shareDenyWrite))
			{
				casInUse.Send ((UCHAR *)myTextHeaderOk, strlen(myTextHeaderOk));
				casInUse.Send ((UCHAR *)myTextLineCache, strlen(myTextLineCache));
				casInUse.Send ((UCHAR *)myTextLineServer, strlen(myTextLineServer));
				casInUse.Send ((UCHAR *)myTextLineLast, strlen(myTextLineLast));
				DWORD  dwRead = 0;

				do
				{
					dwRead = myFile.Read(tcBuffer, sizeof(tcBuffer));
					casInUse.Send ((UCHAR *)tcBuffer, dwRead);
					Sleep(0);
				}
				while (dwRead > 0);
				myFile.Close ();
			}
			else
			{
				casInUse.Send ((UCHAR *)myTextHeaderNoFile, strlen(myTextHeaderNoFile));
				casInUse.Send ((UCHAR *)myTextLineServer, strlen(myTextLineServer));
				casInUse.Send ((UCHAR *)myTextLineLast, strlen(myTextLineLast));
			}
		}
		else if (m_nBytesCount > 0)
		{
			casInUse.Send ((UCHAR *)myTextHeaderOk, strlen(myTextHeaderOk));
			casInUse.Send ((UCHAR *)myTextLineCache, strlen(myTextLineCache));
			switch (sContentType) {
				case BL_DATAQUERY_CONTENT_TEXT:
					casInUse.Send ((UCHAR *)myTextLineText, strlen(myTextLineText));
					break;
				case BL_DATAQUERY_CONTENT_HTML:
					casInUse.Send ((UCHAR *)myTextLineHtml, strlen(myTextLineHtml));
					break;
				case BL_DATAQUERY_CONTENT_XML:
					casInUse.Send ((UCHAR *)myTextLineXml, strlen(myTextLineXml));
					break;
				case BL_DATAQUERY_CONTENT_JSON:
					casInUse.Send ((UCHAR *)myTextLineJson, strlen(myTextLineJson));
					break;
				case BL_DATAQUERY_CONTENT_JPEG:
					casInUse.Send ((UCHAR *)myTextLineImageJpeg, strlen(myTextLineImageJpeg));
					break;
				case BL_DATAQUERY_CONTENT_UNKNOWN:
				default:
					if (_tcsstr (m_csGetFileName, _T(".json")) != 0) {
						casInUse.Send ((UCHAR *)myTextLineJson, strlen(myTextLineJson));
					}
					break;
			}

			casInUse.Send ((UCHAR *)myTextLineServer, strlen(myTextLineServer));
			casInUse.Send ((UCHAR *)myTextLineLast, strlen(myTextLineLast));
			casInUse.Send ((UCHAR *)m_ucharBuffer, m_nBytesCount);
		}
		else if (fpFileStream)
		{
			CHAR tcBuffer[4096];

			casInUse.Send ((UCHAR *)myTextHeaderOk, strlen(myTextHeaderOk));
			casInUse.Send ((UCHAR *)myTextLineCache, strlen(myTextLineCache));
			switch (sContentType) {
				case BL_DATAQUERY_CONTENT_TEXT:
					casInUse.Send ((UCHAR *)myTextLineText, strlen(myTextLineText));
					break;
				case BL_DATAQUERY_CONTENT_HTML:
					casInUse.Send ((UCHAR *)myTextLineHtml, strlen(myTextLineHtml));
					break;
				case BL_DATAQUERY_CONTENT_XML:
					casInUse.Send ((UCHAR *)myTextLineXml, strlen(myTextLineXml));
					break;
				case BL_DATAQUERY_CONTENT_JSON:
					casInUse.Send ((UCHAR *)myTextLineJson, strlen(myTextLineJson));
					break;
				case BL_DATAQUERY_CONTENT_JPEG:
					casInUse.Send ((UCHAR *)myTextLineImageJpeg, strlen(myTextLineImageJpeg));
					break;
				case BL_DATAQUERY_CONTENT_UNKNOWN:
				default:
					if (_tcsstr (m_csGetFileName, _T(".json")) != 0) {
						casInUse.Send ((UCHAR *)myTextLineJson, strlen(myTextLineJson));
					} else if (_tcsstr (m_csGetFileName, _T(".xml")) != 0) {
						casInUse.Send ((UCHAR *)myTextLineXml, strlen(myTextLineXml));
					} else if (_tcsstr (m_csGetFileName, _T(".html")) != 0) {
						casInUse.Send ((UCHAR *)myTextLineHtml, strlen(myTextLineHtml));
					}
					break;
			}
			casInUse.Send ((UCHAR *)myTextLineServer, strlen(myTextLineServer));

			casInUse.Send ((UCHAR *)myTextLineLast, strlen(myTextLineLast));

			fseek(fpFileStream, 0, SEEK_END);
			long lSize = ftell(fpFileStream);
			fseek(fpFileStream, 0, SEEK_SET);
			DWORD  dwRead = 0;
			do
			{
				long lCount = sizeof(tcBuffer);
				if (lSize < sizeof(tcBuffer)) lCount = lSize;
				dwRead = fread (tcBuffer, 1, lCount, fpFileStream);
				casInUse.Send ((UCHAR *)tcBuffer, dwRead);
				Sleep(0);
				lSize -= dwRead;
			}
			while (dwRead > 0);
			fclose (fpFileStream);
			fpFileStream = NULL;
		}
		else
		{
			casInUse.Send ((UCHAR *)myTextHeaderNoFile, strlen(myTextHeaderNoFile));
			casInUse.Send ((UCHAR *)myTextLineServer, strlen(myTextLineServer));
			casInUse.Send ((UCHAR *)myTextLineLast, strlen(myTextLineLast));
			casInUse.Send ((UCHAR *)myTextLineFileNotFound, strlen(myTextLineFileNotFound));
		}
	}
	else if (m_RequestType == RequestPost)
	{
		if (m_nBytesCount > 0)
		{
			casInUse.Send ((UCHAR *)myTextHeaderOk, strlen(myTextHeaderOk));
			casInUse.Send ((UCHAR *)myTextLineCache, strlen(myTextLineCache));
			casInUse.Send ((UCHAR *)myTextLineServer, strlen(myTextLineServer));
			if (m_csGetContentType[0]) {
				casInUse.Send ((UCHAR *)m_csGetContentType, strlen(m_csGetContentType));
			}
			casInUse.Send ((UCHAR *)myTextLineLast, strlen(myTextLineLast));
			casInUse.Send ((UCHAR *)m_ucharBuffer, m_nBytesCount);
		}
		else
		{
			casInUse.Send ((UCHAR *)myTextHeaderNoFile, strlen(myTextHeaderNoFile));
			casInUse.Send ((UCHAR *)myTextLineServer, strlen(myTextLineServer));
			casInUse.Send ((UCHAR *)myTextLineLast, strlen(myTextLineLast));
		}
	}
	else
	{
		casInUse.Send ((UCHAR *)myTextLineOpen, strlen(myTextLineOpen)) ;
		casInUse.Send ((UCHAR *)myTextLineDummy, strlen(myTextLineDummy)) ;
		casInUse.Send ((UCHAR *)myTextLineClose, strlen(myTextLineClose)) ;
	}

	casInUse.ShutDown (SD_BOTH);
	casInUse.Close ();
	return 0;
}

static int ProcessEncodedString (UCHAR *sDest, UCHAR *sSource, int nSourceLength)
{
	UCHAR * sSourceSave = sSource;
	UCHAR * sDestSave = sDest;

	if (nSourceLength < 0)
		nSourceLength = 4096;

	if (sDest == 0 || sSource == 0 || nSourceLength == 0)
		return 0;

	while (*sSource && *sSource != ' ' && sSource - sSourceSave < nSourceLength)
	{
		if (*sSource == '%')
		{
			int charValue = 0;
			sSource++;
			if ((*sSource & 0xf0) == 0x30)
			{
				charValue = (*sSource & 0xf);
			}
			else if ((*sSource & 0xf0) == 0x60 || (*sSource & 0xf0) == 0x40)
			{
				charValue = (*sSource & 0x7) + 10;
			}
			charValue <<= 4;
			sSource++;
			if ((*sSource & 0xf0) == 0x30)
			{
				charValue += (*sSource & 0xf);
			}
			else if ((*sSource & 0xf0) == 0x60 || (*sSource & 0xf0) == 0x40)
			{
				charValue += (*sSource & 0x7) + 10;
			}
			*sDest++ = charValue;
			sSource++;
		}
		else
		{
			*sDest++ = *sSource++;
		}
	}
	return (sDest - sDestSave);
}

int CHttpserverApp::csReadRequest ()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_nBytesRead = m_nBytesCount = m_nHeaderCount = 0;
	m_ucharBuffer[0] = 0;

	int  iRetryRead = 2;
	while (m_nBytesCount < 1 && iRetryRead > 0)
	{
		Sleep (10);
		m_nBytesCount = casInUse.Receive (m_ucharBuffer, sizeof(m_ucharBuffer));
		iRetryRead--;
	}

	m_csGetFileName[0] = 0;
	m_csGetContentType[0] = 0;
	m_RequestType = RequestUnknown;
	if (m_nBytesCount < 1) {
		return 1;
	}

	// Now we will parse the read in information to get everything sent

	if (m_nBytesCount < sizeof(m_ucharBuffer))
	{
		while (m_nBytesRead > 0 && m_nBytesCount < sizeof(m_ucharBuffer))
		{
			Sleep (10);
			m_nBytesRead = casInUse.Receive (m_ucharBuffer + m_nBytesCount, sizeof(m_ucharBuffer));
			m_nBytesCount += m_nBytesRead;
		}
	}

	// We will first process the request header.
	// The request header uses ANSI character set per the W3C standard
	// and contains a set of directives each of which is terminated by a
	// carriage return and line feed sequence.  The end of the request block
	// is indicated by an empty line with just a carriage return and line feed.
	// While parsing through the header, we will replace the carriage return
	// with a binary zero, the string termination character in C, so that we
	// can process the header information as strings with the standard string
	// library.

	BYTE  *pucharBuffer = m_ucharBuffer;

	m_ucharBuffer[m_nBytesCount] = 0;

	memset(m_ucharBufferHeaderLine, 0, sizeof(m_ucharBufferHeaderLine));
	memset(m_ucharBufferPostKeyword, 0, sizeof(m_ucharBufferPostKeyword));
	memset(m_ucharBufferPostValue, 0, sizeof(m_ucharBufferPostValue));

	m_nHeaderCount = 0;

	m_ucharBufferHeaderLine[m_nHeaderCount] = m_ucharBuffer;
	// First of all, lets parse through the HTTP header information to
	// build a list of the header directives.
	// The end of the HTTP header is indicated with an empty line.
	while (pucharBuffer - m_ucharBuffer < m_nBytesCount)
	{
		if (*pucharBuffer == '\r' && *(pucharBuffer+1) == '\n')
		{
			m_nHeaderCount++;
			*pucharBuffer = 0;
			pucharBuffer += 2;
			if (*pucharBuffer == '\r' && *(pucharBuffer+1) == '\n')
			{
				// Position pucharBuffer to where the payload in the HTTP
				// message will begin if there is any payload, after the empty line.
				pucharBuffer += 2;
				break;
			}
			m_ucharBufferHeaderLine[m_nHeaderCount] = pucharBuffer;
		}
		else
		{
			pucharBuffer++;
		}
	}

	m_ucharBufferPayLoad = pucharBuffer;

	if (memcmp (m_ucharBuffer, "GET", 3) == 0)
	{
		UCHAR  aszBuffer[sizeof(m_csGetFileName)/sizeof(m_csGetFileName[0])];

		int ijk = ProcessEncodedString (aszBuffer, (m_ucharBuffer + 4), sizeof(aszBuffer));

		m_csGetFileName[ijk] = 0;
		ijk--;
		for ( ; ijk >= 0; ijk--)
		{
			m_csGetFileName [ijk] = aszBuffer[ijk];
		}

		m_RequestType = RequestGet;
	}
	else if (memcmp (m_ucharBuffer, "PUT", 3) == 0)
	{
		UCHAR  aszBuffer[sizeof(m_csGetFileName)/sizeof(m_csGetFileName[0])];

		int ijk = ProcessEncodedString (aszBuffer, (m_ucharBuffer + 4), sizeof(aszBuffer));

		m_csGetFileName[ijk] = 0;
		ijk--;
		for ( ; ijk >= 0; ijk--)
		{
			m_csGetFileName [ijk] = aszBuffer[ijk];
		}

		m_RequestType = RequestPut;
	}
	else if (memcmp (m_ucharBuffer, "POST", 4) == 0)
	{
		UCHAR  aszBuffer[sizeof(m_csGetFileName)/sizeof(m_csGetFileName[0])];

		int ijk = ProcessEncodedString (aszBuffer, (m_ucharBuffer + 5), sizeof(aszBuffer));

		m_csGetFileName[ijk] = 0;
		ijk--;
		for ( ; ijk >= 0; ijk--)
		{
			m_csGetFileName [ijk] = aszBuffer[ijk];
		}

		if (_tcsstr(m_csGetFileName, _T(".xml")) == 0)
		{
			// Next, lets go through the payload of the HTTP
			// request and lets build a list of the keyword=value pairs.
			// We will do this only if it is not an XML request.
			int  nPostCount = 0;
			pucharBuffer = m_ucharBufferPayLoad;
			m_ucharBufferPostKeyword[nPostCount] = pucharBuffer;
			while (pucharBuffer - m_ucharBuffer < m_nBytesCount)
			{
				if (*pucharBuffer == '\r' && *(pucharBuffer+1) == '\n')
				{
					nPostCount++;
					*pucharBuffer = 0;
					pucharBuffer++;
					m_ucharBufferPostKeyword[nPostCount] = pucharBuffer + 1;
				}
				if (*pucharBuffer == '=')
				{
					*pucharBuffer = 0;
					pucharBuffer++;
					m_ucharBufferPostValue[nPostCount] = pucharBuffer;
				}
				pucharBuffer++;
			}

			// Now we will go through the keyword=value pairs and we
			// will process the pairs to translate %xx values into characters.
			for ( nPostCount--; nPostCount >= 0; nPostCount--)
			{
				ijk = ProcessEncodedString (m_ucharBufferPostKeyword[nPostCount], m_ucharBufferPostKeyword[nPostCount], -1);
				if (m_ucharBufferPostKeyword[nPostCount] != 0)
					m_ucharBufferPostKeyword[nPostCount][ijk] = 0;
				ijk = ProcessEncodedString (m_ucharBufferPostValue[nPostCount], m_ucharBufferPostValue[nPostCount], -1);
				if (m_ucharBufferPostValue[nPostCount] != 0)
					m_ucharBufferPostValue[nPostCount][ijk] = 0;
			}
		}
		m_RequestType = RequestPost;
	}

	return 0;
}



__declspec(dllexport) int  StartThread ()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHttpserverApp * myhttp = CHttpserverApp::GetServerObject();

	Sleep (10);
	if (!myhttp->casAccept.Create (8080))
	{
		ASSERT(0);
		DWORD iRet = GetLastError();
	}

	if (! myhttp->casAccept.Listen ())
	{
		ASSERT(0);
		DWORD iRet = GetLastError();
	}

	return 0;
}

__declspec(dllexport) int  StopThread ()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHttpserverApp * myhttp = CHttpserverApp::GetServerObject();

	myhttp->casAccept.ShutDown ();

	return 0;
}

__declspec(dllexport) int  IssueRequest (BYTE *pUserBuffer, int nBufSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHttpserverApp * myhttp = CHttpserverApp::GetServerObject();

	myhttp->m_nBytesCount = 0;
	if (pUserBuffer && nBufSize > 0)
	{
		myhttp->m_nBytesCount = (nBufSize <= sizeof(myhttp->m_ucharBuffer)) ? nBufSize : sizeof(myhttp->m_ucharBuffer);
		memcpy (myhttp->m_ucharBuffer, pUserBuffer, myhttp->m_nBytesCount);
	}
	myhttp->csIssueRequest ();

	return 0;
}

__declspec(dllexport) int  IssueFilePointerRequest (FILE *fpFile, SHORT  sContentType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHttpserverApp * myhttp = CHttpserverApp::GetServerObject();

	myhttp->m_nBytesCount = 0;
	myhttp->fpFileStream = fpFile;
	myhttp->sContentType = sContentType;
	myhttp->csIssueRequest ();

	return 0;
}

__declspec(dllexport) int  ProvideCallBack ()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return 0;
}

__declspec(dllexport) int  ProvideWinHandle ()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return 0;
}

__declspec(dllexport) int  WaitOnConnection ()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHttpserverApp * myhttp = CHttpserverApp::GetServerObject();

	myhttp->casAccept.Accept(myhttp->casInUse);

	return 0;
}

__declspec(dllexport) int  ReadRequest (ServerProcInterface *pMyProcInterface)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHttpserverApp * myhttp = CHttpserverApp::GetServerObject();

	myhttp->csReadRequest ();

	pMyProcInterface->m_ucharBufferPayLoad = myhttp->m_ucharBufferPayLoad;
	pMyProcInterface->m_ucharBuffer = myhttp->m_ucharBuffer;
	pMyProcInterface->m_nBytesCount = myhttp->m_nBytesCount;
	pMyProcInterface->m_nRequestType = myhttp->m_RequestType;
	pMyProcInterface->m_tcharFileName = myhttp->m_csGetFileName;
	pMyProcInterface->m_ucharBufferHeaderLine = myhttp->m_ucharBufferHeaderLine;
	pMyProcInterface->m_ucharBufferPostLineKeyword = myhttp->m_ucharBufferPostKeyword;
	pMyProcInterface->m_ucharBufferPostLineValue = myhttp->m_ucharBufferPostValue;
	return 0;
}

__declspec(dllexport) int  ParsePayLoadXML (ServerProcInterface *pMyProcInterface)
{
	TCHAR   tcReqBuffer[4096];
	TCHAR   tcRespBuffer[4096];
	TCHAR   *pReqBuff = tcReqBuffer, *pRespBuffer = tcRespBuffer;
	int     nRespBuffSize = sizeof(tcRespBuffer);
	int     nLen = 0;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHttpserverApp * myhttp = CHttpserverApp::GetServerObject();

	if (pMyProcInterface->m_tcharReqBuffer != 0)
		pReqBuff = pMyProcInterface->m_tcharReqBuffer;

	if (pMyProcInterface->m_tcharRespBuffer != 0)
	{
		pRespBuffer = pMyProcInterface->m_tcharRespBuffer;
		nRespBuffSize = pMyProcInterface->m_nRespBufferSize;
	}

	if (pMyProcInterface->m_tcharDtdFileName) {
		FILE *pFile = _tfopen (pMyProcInterface->m_tcharDtdFileName, _T("r"));
		char  *pTemp = (char *)tcRespBuffer;
		if (pFile) {
			nLen = fread (tcRespBuffer, 1, sizeof(tcRespBuffer), pFile);
			fclose (pFile);
			for (int i = 0; i < nLen; i++)
			{
				pReqBuff[i] = pTemp [i];
			}
		}
		else
		{
		}
	}

	for (int i = 0, j = nLen; i < myhttp->m_nBytesCount; i++, j++)
	{
		pReqBuff[j] = myhttp->m_ucharBufferPayLoad [i];
	}

	myhttp->InterpretXML (pReqBuff, pRespBuffer, nRespBuffSize, 0);

	return 0;
}


BOOL CHttpserverApp::InterpretXML(const TCHAR* tchXML, VOID* pXEPTResData, USHORT usSize, SHORT sType)
{
	TCHAR * resData = (TCHAR *)pXEPTResData;
//	CString strXml(tchXML);
	CString error;
	USES_CONVERSION;
	CString csAccount;
	CString csExp;
	CString csApp;
	CString csRef, csBalance, csCardType;
	int		iWritePt=0;

	try
	{
//		_bstr_t strXML (tchXML);
		MSXML2::IXMLDOMDocumentPtr pDOMDoc(__uuidof(MSXML2::DOMDocument));
		// Load into the XML parser the XML document as a string of
		// characters in the buffer.
		VARIANT_BOOL varResult = pDOMDoc->loadXML(tchXML);
		if (VARIANT_FALSE == varResult)
		{
			MSXML2::IXMLDOMParseErrorPtr pParseError = pDOMDoc->GetparseError();
			long dwError = pParseError->GeterrorCode();
			_bstr_t bstrReason = pParseError->Getreason();
			long num = pParseError->Getline();
			CString strReason = W2T(bstrReason);
			error.Format(_T("httpserver XML Parse Error at line#:%d\r\n%s"),num,strReason) ;
//			AfxMessageBox(error);
			PifLog(MODULE_HTTPSERVER, LOG_EVENT_HTTP_XML_PARSE_ERROR);
			return FALSE;
		}
		try
		{
#if 1
			memset (resData, 0, usSize);
			MSXML2::IXMLDOMNodeListPtr pNodeList = pDOMDoc->selectNodes("//*");
			MSXML2::IXMLDOMNodePtr pNode = 0;
			pHttpXmlTableItem   pTable = (pHttpXmlTableItem) pXEPTResData;
			int njjj = ((sizeof(HttpXmlTableItem) * pNodeList->length)/sizeof(TCHAR) + sizeof(TCHAR));
			TCHAR *pRespData = resData + ((sizeof(HttpXmlTableItem) * pNodeList->length)/sizeof(TCHAR) + sizeof(TCHAR));
			for (int jkl = 1; jkl < pNodeList->length; jkl++)
			{
				BSTR  myName;
				pNode = pNodeList->Getitem (jkl);
				pNode->get_nodeName (&myName);
				_tcscpy(pRespData, W2T(myName));
				pTable->tcKeyword = pRespData;
				pRespData += _tcslen(pRespData) + 1;
				_tcscpy(pRespData, W2T(pNode->Gettext()));
				pTable->tcValue = pRespData;
				pRespData += _tcslen(pRespData) + 1;
				*pRespData = _T('\0');
				pTable++;
			}
#else
			MSXML2::IXMLDOMNodePtr pNode = pDOMDoc->selectSingleNode("//name");
			if(pNode)
			{
				_tcscpy(resData, W2T(pNode->Gettext()));
			}
#endif
#if 0
			pNode = pDOMDoc->selectSingleNode("//ResponseOrigin");
			if(pNode)
			{
				strcpy((char*)resData->auchResOrigin, W2A(pNode->Gettext()));
			}
#endif
		}
		catch(...)
		{
			TRACE(_T("Caught MSXML2 Exception: CHttpserverApp::InterpretXML Error in selectSingleNode"));
			AfxMessageBox(_T("Error with selectSingleNode"));
			return FALSE;
		}
	}
	catch(_com_error &e)
	{
		TRACE(_T("Caught MSXML2 Exception: CHttpserverApp::InterpretXML"));
		AfxMessageBox(e.ErrorMessage());
		return FALSE;
	}
	catch(...)
	{
		TRACE(_T("Caught MSXML2 Exception: CHttpserverApp::InterpretXML"));
		AfxMessageBox(_T("MSXML2 Load Failure"));
		return FALSE;
	}
	return TRUE;
}

