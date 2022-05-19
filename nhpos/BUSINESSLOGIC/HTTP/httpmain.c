/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : HTTP module, Main Functions Source File                        
* Category    : HTTP module, US Hospitality Model
* Program Name: httpmain.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*       HttpStartUp();               Server start up (main loop)
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
*          :           :                                    
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
#include	<tchar.h>
#include    <memory.h>
#include    <string.h>
#include	<stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    "httpmain.h"
#include    <uie.h>
#define __BLNOTIFY_C__
#include    <bl.h>
#include    <pifmain.h>

#include <appllog.h>
#include <paraequ.h>
#include <para.h>
#include <regstrct.h>
#include <transact.h>
#include <trans.h>
#include <csstbgcf.h>
#include <report.h>

#if 0
// following discussion concerns the XML framework we are using
// The heading of the received XML should look something like the following
<?xml version="1.0" encoding="UTF8" standalone="no"?>
<!DOCTYPE guestcheck [
  <!ELEMENT guestcheck (number,name,john1,john2)>
  <!ELEMENT number (#PCDATA)>
  <!ELEMENT name (#PCDATA)>
  <!ELEMENT john1 (#PCDATA)>
  <!ELEMENT john2 (#PCDATA)>
]>

// or it may look something like the following
<?xml version="1.0" encoding="UTF8" standalone="no"?>
<!DOCTYPE guestcheck SYSTEM "/FlashDisk/NCR/Saratoga/Web/gcf.dtd">


// The above examples are for standard XML files.  What we are contemplating
// is that the sender of an XML file will not specify the XML header information
// but will instead specify a filename in the POST or the GET that will indicate
// a default XML DTD that will be read in.  Basically what we are considering is
// that the NeighborhoodPOS would prepend to an input text additional text
// that looks like the first option above.
<guestcheck>
<number>33</number>
<name>Franklins grocery</name>
<john1>John1 grocery</john1>
<john2>John2 grocery</john2>;
</guestcheck>

// So that the input that is actually parsed would like like the following
<?xml version="1.0" encoding="UTF8" standalone="no"?>
<!DOCTYPE guestcheck [
  <!ELEMENT guestcheck (number,name,john1,john2)>
  <!ELEMENT number (#PCDATA)>
  <!ELEMENT name (#PCDATA)>
  <!ELEMENT john1 (#PCDATA)>
  <!ELEMENT john2 (#PCDATA)>
]>
<guestcheck>
<number>33</number>
<name>Franklins grocery</name>
<john1>John1 grocery</john1>
<john2>John2 grocery</john2>;
</guestcheck>

#endif

PifSemHandle semHttpServer = PIF_SEM_INVALID_HANDLE;
ServerProcInterface HttpThreadMyInterface;

static unsigned char    myXbuffer [8192];
static unsigned char    *pmyXbuffer = 0;
static TCHAR            myFileNameGcfXml[] = _T("C:/FlashDisk/NCR/Saratoga/Web/gcfxmlheader.txt");

static USHORT RptDisplayConfigurationLine (TCHAR *tsLineData)
{
	for ( ; *tsLineData != 0 && pmyXbuffer < myXbuffer + sizeof (myXbuffer) - 10; )
	{
		*pmyXbuffer++ = *tsLineData++;
	}
	*pmyXbuffer++ = '\r';
	*pmyXbuffer++ = '\n';
	*pmyXbuffer = 0;
	return 1;
}

/*
*===========================================================================
** Synopsis:    VOID    SerStartUp(VOID);
*
** Return:      none.
*
** Description: Http Server Module Start Up Function (Main Loop).
		This is the main loop for the http server functionality in BusinessLogic.
		This thread is started by PifMain ().
*===========================================================================
*/

static void HttpServerRequestGcf (ServerProcInterface *pMyProcInterface)
{

	GCNUM  usGCorderNo = 0;
	SHORT  sReturn = 0;
	pHttpXmlTableItem   pTable = (pHttpXmlTableItem) pMyProcInterface->m_tcharRespBuffer;

	sReturn = CliGusRetrieveFirstQueue(GCF_DELIVERY_QUEUE1, GCF_STORE_PAYMENT, &usGCorderNo);

	myXbuffer[0] = 0;

	sprintf (myXbuffer, "<?xml version=\"1.1\"?><xmlmessage>%s</xmlmessage>", pMyProcInterface->m_ucharBufferPayLoad);

}

static void HttpServerRequestXml (ServerProcInterface *pMyProcInterface)
{

	GCNUM  usGCorderNo = 0;
	SHORT  sReturn = 0;
	pHttpXmlTableItem   pTable = (pHttpXmlTableItem) pMyProcInterface->m_tcharRespBuffer;
	AllObjects  myObject;

	BlProcessConnEngineMessageAndTag(&(pMyProcInterface->m_tcharReqBuffer), &myObject);

	myXbuffer[0] = 0;

	sprintf (myXbuffer, "<?xml version=\"1.1\"?><gcf number=\"%d\"><status value=\"%d\" /></gcf>",
		usGCorderNo, sReturn);

}

static TCHAR   tcReqBuffer[64000];
static TCHAR   tcRespBuffer[64000];

VOID THREADENTRY HttpStartUp(VOID)
{
    USHORT  usForEver = 1;

	if (HttpThreadMyInterface.m_pfnStartThread == 0)
	{
		PifLog (MODULE_HTTPSERVER, LOG_EVENT_HTTP_BAD_FUNC_POINTER);
		PifEndThread();
		return;
	}

	HttpThreadMyInterface.m_pfnStartThread ();
    while(usForEver) {
		HttpThreadMyInterface.m_pfnWaitOnConnection ();
		HttpThreadMyInterface.m_pfnReadRequest (&HttpThreadMyInterface);
		if (HttpThreadMyInterface.m_nRequestType == SERVERPROC_REQUEST_POST)
		{
			if (_tcscmp (HttpThreadMyInterface.m_tcharFileName, _T("/gcf.xml")) == 0)
			{
				HttpThreadMyInterface.m_tcharReqBuffer = tcReqBuffer;
				HttpThreadMyInterface.m_nReqBufferSize = sizeof(tcReqBuffer);
				HttpThreadMyInterface.m_tcharRespBuffer = tcRespBuffer;
				HttpThreadMyInterface.m_nRespBufferSize = sizeof(tcRespBuffer);
				HttpThreadMyInterface.m_tcharDtdFileName = myFileNameGcfXml;

				HttpThreadMyInterface.m_pfnParsePayLoad (&HttpThreadMyInterface);
				HttpServerRequestGcf (&HttpThreadMyInterface);
				HttpThreadMyInterface.m_pfnIssueRequest (myXbuffer, strlen(myXbuffer));

				HttpThreadMyInterface.m_tcharDtdFileName = 0;
				HttpThreadMyInterface.m_tcharReqBuffer = 0;
				HttpThreadMyInterface.m_nReqBufferSize = 0;
				HttpThreadMyInterface.m_tcharRespBuffer = 0;
				HttpThreadMyInterface.m_nRespBufferSize = 0;
			}
			else if (_tcscmp (HttpThreadMyInterface.m_tcharFileName, _T("/xmlquery.xml")) == 0) {
				int iLoop;

				HttpThreadMyInterface.m_tcharReqBuffer = tcReqBuffer;
				HttpThreadMyInterface.m_nReqBufferSize = sizeof(tcReqBuffer);
				HttpThreadMyInterface.m_tcharRespBuffer = tcRespBuffer;
				HttpThreadMyInterface.m_nRespBufferSize = sizeof(tcRespBuffer);
				HttpThreadMyInterface.m_tcharDtdFileName = myFileNameGcfXml;

				for (iLoop = 0; iLoop < sizeof(tcReqBuffer)/sizeof(tcReqBuffer[0]); iLoop++) {
					tcReqBuffer[iLoop] = HttpThreadMyInterface.m_ucharBufferPayLoad[iLoop];
					if (HttpThreadMyInterface.m_ucharBufferPayLoad[iLoop] == 0) break;
				}
				HttpServerRequestXml (&HttpThreadMyInterface);
				HttpThreadMyInterface.m_pfnIssueRequest (myXbuffer, strlen(myXbuffer));

				HttpThreadMyInterface.m_tcharDtdFileName = 0;
				HttpThreadMyInterface.m_tcharReqBuffer = 0;
				HttpThreadMyInterface.m_nReqBufferSize = 0;
				HttpThreadMyInterface.m_tcharRespBuffer = 0;
				HttpThreadMyInterface.m_nRespBufferSize = 0;
			}
			else
			{
				HttpThreadMyInterface.m_pfnIssueRequest (0, 0);
			}
		}
		else if (HttpThreadMyInterface.m_nRequestType == SERVERPROC_REQUEST_GET)
		{
			TCHAR *tcsArgList;
			tcsArgList = _tcschr (HttpThreadMyInterface.m_tcharFileName, _T('?'));

			if (tcsArgList) {
				*tcsArgList = 0;
				tcsArgList++;
			}

			if (_tcscmp (HttpThreadMyInterface.m_tcharFileName, _T("/systemstatus.sys")) == 0)
			{
				pmyXbuffer = myXbuffer + strlen(myXbuffer);
				RptTerminalConfiguration(RptDisplayConfigurationLine);
				HttpThreadMyInterface.m_pfnIssueRequest (myXbuffer, strlen(myXbuffer));
			}
			else if (_tcscmp (HttpThreadMyInterface.m_tcharFileName, _T("/databasequery.sys")) == 0 ||
				_tcscmp (HttpThreadMyInterface.m_tcharFileName, _T("/databasequery.json")) == 0)
			{
				ULONG ulLoop;
				DataQueryObject DataQuery = {0};
				TCHAR  tcsQuery[128];
				TCHAR  tcsBuffer[4000];

				DataQuery.ptcsBuffer = tcsBuffer;
				tcsQuery[0] = tcsBuffer[0] = myXbuffer[0] = 0;
				DataQuery.ulBufferSize = sizeof(tcsBuffer)/sizeof(tcsBuffer[0]);

				if (tcsArgList && *tcsArgList) {
					tcsQuery [0] = _T('@');
					_tcscpy (tcsQuery+1, tcsArgList);
				}
				BlProcessDataQueryStatement (tcsQuery, &DataQuery);
				if (DataQuery.fpFile) {
					HttpThreadMyInterface.m_pfnIssueFilePointerRequest (DataQuery.fpFile, DataQuery.sContentType);
					fclose (DataQuery.fpFile);
					DataQuery.fpFile = 0;
				} else {
					pmyXbuffer = myXbuffer + strlen(myXbuffer);
					for (ulLoop = 0; ulLoop < DataQuery.ulBufferSize - 1 && tcsBuffer[ulLoop]; ulLoop++) {
						*pmyXbuffer++ = tcsBuffer[ulLoop];
					}
					*pmyXbuffer = 0;
					HttpThreadMyInterface.m_pfnIssueRequest (myXbuffer, strlen(myXbuffer));
				}
			}
			else
			{
				HttpThreadMyInterface.m_pfnIssueRequest (0, 0);
			}
		}
		else
		{
			HttpThreadMyInterface.m_pfnIssueRequest (0, 0);
		}
		PifSleep (0);
    }
	HttpThreadMyInterface.m_pfnStopThread ();
    PifEndThread();
}


