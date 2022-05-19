// CnAdoXP.cpp: implementation of the CnAdoXP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CnAdoXP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


int	CnAdoXPRec::s_nObjCnt =0;						// static object counter

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CnAdoXPRec::CnAdoXPRec(){
	CreateObject();
}

CnAdoXPRec::~CnAdoXPRec(){
	DestroyObject();
}

BOOL	CnAdoXPRec::CreateObject(){
	m_pRecordSet = NULL;
	m_bOpened = FALSE;
	CursorTypeEnum CursorType = adOpenUnspecified;
	LockTypeEnum LockType = adLockOptimistic;

	if(s_nObjCnt == 0){
		if (FAILED(m_hr = CoInitialize(NULL))) {
			char xBuff[256];
			TRACE3("%S(%d): Failed CoInitialize HRESULT = 0x%x\n", __FILE__, __LINE__, m_hr);
			sprintf (xBuff, "Failed CoInitialize HRESULT = 0x%x\n", m_hr);
			NHPOS_ASSERT_TEXT(0, xBuff);
			DestroyObject();
			return FALSE;
		}
	}

	s_nObjCnt++;

	m_hr = m_pConnection.CreateInstance(__uuidof(Connection));    // create the ADO connection object
	if (FAILED(m_hr)) {
		char xBuff[256];
		TRACE3("%S(%d): Failed m_pConnection.CreateInstance HRESULT = 0x%x\n", __FILE__, __LINE__, m_hr);
		sprintf (xBuff, "Failed m_pConnection.CreateInstance HRESULT = 0x%x\n", m_hr);
		NHPOS_ASSERT_TEXT(0, xBuff);
	}

	m_hr = m_pRecordSet.CreateInstance(__uuidof(Recordset));      // create the ADO recordset object
	if (FAILED(m_hr)) {
		char xBuff[256];
		TRACE3("%S(%d): Failed m_pRecordSet.CreateInstance HRESULT = 0x%x\n", __FILE__, __LINE__, m_hr);
		sprintf (xBuff, "Failed m_pRecordSet.CreateInstance HRESULT = 0x%x\n", m_hr);
		NHPOS_ASSERT_TEXT(0, xBuff);
	}
	return TRUE;
}


VOID	CnAdoXPRec::DestroyObject(){
    /* --- BEGIN: determine whether CoInitializeEx() failed or not --- */
    if (0 < s_nObjCnt) {
	    s_nObjCnt--;
    }
	/*  s_nObjCnt--; */
    /* --- END:   determine whether CoInitializeEx() failed or not --- */

	if(m_pRecordSet != NULL){
        /* --- BEGIN: discard allocated resource by Record Set --- */
        //m_pRecordSet->Close();
        /* --- END:   discard allocated resource by Record Set --- */
	//	m_pRecordSet->Release();

		m_pRecordSet.Detach();
		//instead of setting this variable to NULL, we use the Instance's own Detach process to do this.	
	}

	if(m_pConnection != NULL)
	{
		m_pConnection.Detach();
	}


	if(s_nObjCnt == 0)
		CoUninitialize();
}