/////////////////////////////////////////////////////////////////////////////
// info.cpp : implementation file
//
// Copyright (C) 1996 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date       Version  Author       Comments
// =========  =======  ===========  ========================================
// Mar-01-96           O.Nakada     initial
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "progrpt.h"

#include "info.h"

static	wchar_t szGlobalSection[] = L"Programmable Report";

/////////////////////////////////////////////////////////////////////////////
// CInfoField

IMPLEMENT_DYNAMIC(CInfoField, CObject)

/////////////////////////////////////////////////////////////////////////////
// CInfoField operations

void CInfoField::FieldInsertString(CListBox* pLb)
{
	pLb->InsertString(-1, m_strName);
}

CInfoField* CInfoField::GetField(int nFile, int nOperation, int nGroup,
	int nField, int* pnBias /*=NULL*/)
{
	CInfoConfig* pConfig = ptheInfo->GetConfig(nFile);
	if (pConfig == NULL)
		return NULL;
	CFG_OPEGROUP* pOpeGroup = pConfig->GetGroup(nOperation, nGroup);
	if (pOpeGroup == NULL)
		return NULL;
	POSITION pos = pConfig->FieldFindIndex(pOpeGroup->m_nIndex + nField);
	if (pos == NULL)
		return NULL;

	if (pnBias != NULL)
		*pnBias = pOpeGroup->m_nGroup - nGroup;

	return pConfig->FieldGetNext(pos);
}

int CInfoField::GetMajorIndex(int nFile, int nOperation, int nGroup, int nMajor)
{
	CInfoConfig* pConfig = ptheInfo->GetConfig(nFile);
	if (pConfig == NULL)
		return -1;
	CFG_OPEGROUP* pOpeGroup = pConfig->GetGroup(nOperation, nGroup);
	if (pOpeGroup == NULL)
		return -1;

	int nIndex = 0;
	POSITION pos = pConfig->FieldFindIndex(pOpeGroup->m_nIndex);
	while (pos != NULL)
	{
		CInfoField* pField = pConfig->FieldGetNext(pos);
		if (pField->m_nMajor == nMajor)
			return nIndex;
		else
			nIndex++;
	}

	return -1;
}


/////////////////////////////////////////////////////////////////////////////
// CInfoLoop

IMPLEMENT_DYNAMIC(CInfoLoop, CInfoField)

/////////////////////////////////////////////////////////////////////////////
// CInfoLoop construction/destruction

CInfoLoop::CInfoLoop(CString &strData)
{
	int nIndex = 0;
	int nPos = ReadMajor(strData);
	if (nPos == -1)
	{
		TRACE(L"warning: Invalid format to Loop Operation\n");
		ASSERT(FALSE);
		return;
	}

	strData = strData.Mid(nPos);
	if ((nPos = ReadMinor(strData)) == -1)
	{
		TRACE(L"warning: Invalid format to Loop Operation\n");
		ASSERT(FALSE);
		return;
	}

	while (nPos > 0)
	{
		strData = strData.Mid(nPos);
		nPos = ReadCondition(strData, nIndex++);
	}
	while (nIndex < LOOP_NUM_COND)
		Condition[nIndex++].m_nNumber = _FREE;	// end mark
}

/////////////////////////////////////////////////////////////////////////////
// CInfoLoop operations

void CInfoLoop::ItemInsertString(CListBox* pLb, CStringList* pstrList /*=NULL*/)
{
	for (int nCount = 0; nCount < LOOP_NUM_COND; nCount++)
	{
		if (Condition[nCount].m_strName.IsEmpty())
			return;
		pLb->InsertString(-1, Condition[nCount].m_strName);
	}
}

CInfoLoop* CInfoLoop::GetField(int nFile, int nGroup, int nField,
	int* pnBias /*=NULL*/)
{
	return (CInfoLoop*)
		CInfoField::GetField(nFile, CFG_INDEX_LOOP, nGroup, nField, pnBias);
}

int CInfoLoop::GetMajorIndex(int nFile, int nGroup, int nMajor)
{
	return CInfoField::GetMajorIndex(nFile, CFG_INDEX_LOOP, nGroup, nMajor);
}

/////////////////////////////////////////////////////////////////////////////
// CInfoLoop implementations

int CInfoLoop::ReadMajor(CString& strData)
{
	int nPos1 = strData.Find(L':');
	if (nPos1 == -1)
	{
		TRACE(L"warning: CInfoLoop::ReadMajor\n");
		return -1;
	}

	m_nMajor = _wtoi(strData.Left(nPos1));

	int nPos2 = strData.Find(L',');
	if (nPos2 == -1)
	{
		TRACE(L"warning: CInfoLoop::ReadMajor\n");
		return -1;
	}

	CString strIndex = strData.Mid(nPos1 + 1, nPos2 - nPos1 - 1);
	m_strName = AfxGetApp()->GetProfileString(L"Mnemonics", L"Address" + strIndex);
	
	return nPos2 + 1;
}

int CInfoLoop::ReadMinor(CString& strData)
{
	int nPos1 = strData.Find(L',');
	if (nPos1 == -1)
	{
		TRACE(L"warning: CInfoLoop::ReadLevel\n");
		return -1;
	}

	m_nMinor = _wtoi(strData.Left(nPos1));

	return nPos1 + 1;
}

int CInfoLoop::ReadCondition(CString& strData, int nIndex)
{
	int nPos1 = strData.Find(L':');
	if (nPos1 == -1)
	{
		TRACE(L"warning: CInfoLoop::ReadCondition\n");
		return -1;
	}

	Condition[nIndex].m_nNumber = _wtoi(strData.Left(nPos1));

	BOOL bNull = FALSE;
	int nPos2 = strData.Find(L'.');
	if (nPos2 == -1)
	{
		nPos2 = strData.GetLength();
		bNull = TRUE;
	}
	
	CString strIndex = strData.Mid(nPos1 + 1, nPos2 - nPos1 - 1);

	Condition[nIndex].m_strName =
		AfxGetApp()->GetProfileString(L"Mnemonics", L"Address" + strIndex);

	if (bNull)
		return 0;
	else
		return nPos2 + 1;
}


/////////////////////////////////////////////////////////////////////////////
// CInfoPrint

IMPLEMENT_DYNAMIC(CInfoPrint, CInfoField)

/////////////////////////////////////////////////////////////////////////////
// CInfoPrint construction/destruction

CInfoPrint::CInfoPrint(CString& strData)
{
	int nIndex = 0;
	int nPos = ReadMajor(strData);
	if (nPos == -1)
	{
		TRACE(L"warning: Invalid format to Print Operation\n");
		ASSERT(FALSE);
		return;
	}

	strData = strData.Mid(nPos);
	if ((nPos = ReadMinor(strData)) == -1)
	{
		TRACE(L"warning: Invalid format to Print Operation\n");
		ASSERT(FALSE);
		return;
	}

	strData = strData.Mid(nPos);
	if ((nPos = ReadFlags(strData)) == -1)
	{
		TRACE(L"warning: Invalid format to Print Operation\n");
		ASSERT(FALSE);
		return;
	}

	while (nPos > 0)
	{
		strData = strData.Mid(nPos);
		nPos = ReadItem(strData, nIndex++);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CInfoPrint operations

void CInfoPrint::ItemInsertString(CListBox* pLb, CStringList* pstrList /*=NULL*/)
{
	if (m_nMinorCount <= 1)
		return;

	POSITION pos = NULL;
	if (pstrList != NULL)
		pos = pstrList->GetHeadPosition();
	
	for (int nCount = 0; nCount < m_nMinorCount; nCount++)
	{
		wchar_t szData[16];
		_itow(m_nMinor + nCount, szData, 10);
		CString strData(szData);
		if (pos != NULL)
		{
			strData += L":";
			strData += pstrList->GetNext(pos);
		}
		pLb->InsertString(-1, strData);
	}
}

CInfoPrint* CInfoPrint::GetField(int nFile, int nGroup, int nField,
	int* pnBias /*=NULL*/)
{
	return (CInfoPrint*)
		CInfoField::GetField(nFile, CFG_INDEX_PRINT, nGroup, nField, pnBias);
}

int CInfoPrint::GetMajorIndex(int nFile, int nGroup, int nMajor)
{
	return CInfoField::GetMajorIndex(nFile, CFG_INDEX_PRINT, nGroup, nMajor);
}

/////////////////////////////////////////////////////////////////////////////
// CInfoPrint implementations

int CInfoPrint::ReadMajor(CString& strData)
{
	int nPos1 = strData.Find(L':');
	if (nPos1 == -1)
	{
		TRACE(L"warning: CInfoPrint::ReadMajor\n");
		return -1;
	}

	m_nMajor = _wtoi(strData.Left(nPos1));

	int nPos2 = strData.Find(L',');
	if (nPos2 == -1)
	{
		TRACE(L"warning: CInfoPrint::ReadMajor\n");
		return -1;
	}

	CString strIndex = strData.Mid(nPos1 + 1, nPos2 - nPos1 - 1);
	m_strName = AfxGetApp()->GetProfileString(L"Mnemonics", L"Address" + strIndex);
	
	return nPos2 + 1;
}

int CInfoPrint::ReadMinor(CString& strData)
{
	int nPos2;

	int nPos1 = strData.Find(L'-');
	if (nPos1 == -1)
	{
		nPos2 = strData.Find(L',');

		m_nMinor      = _wtoi(strData.Left(nPos2));
		m_nMinorCount = 1;
	}
	else
	{	
		nPos2 = strData.Find(L',');
		if (nPos2 == -1)
		{
			TRACE(L"warning: CInfoPrint::ReadMinor\n");
			return -1;
		}

		m_nMinor      = _wtoi(strData.Left(nPos1));
		m_nMinorCount = _wtoi(strData.Mid(nPos1 + 1, nPos2 - nPos1 - 1)) - m_nMinor + 1;
	}

	return nPos2 + 1;
}

int CInfoPrint::ReadFlags(CString& strData)
{
	int nPos1 = strData.Find(L',');
	if (nPos1 == -1)
	{
		TRACE(L"warning: CInfoPrint::ReadMajor\n");
		return -1;
	}

	m_nFlags = _wtoi(strData.Left(nPos1));

	return nPos1 + 1;
}

int CInfoPrint::ReadItem(CString& strData, int nIndex)
{
	int nPos1 = strData.Find(L':');
	if (nPos1 == -1)
	{
		TRACE(L"warning: CInfoPrint::ReadItem\n");
		return -1;
	}

	Item[nIndex].m_nNumber = _wtoi(strData.Left(nPos1));

	int nPos2 = (strData.Left(nPos1 + 1)).Find(L':');
	if (nPos2 == -1)
	{
		TRACE(L"warning: CInfoPrint::ReadItem\n");
		return -1;
	}

	Item[nIndex].m_nFormat = _wtoi(strData.Mid(nPos1 + 1, nPos2));

	BOOL bNull = FALSE;
	int nPos3 = strData.Find(L'.');
	if (nPos3 == -1)
	{
		nPos3 = strData.GetLength();
		bNull = TRUE;
	}

	nPos2 = nPos1 + 1 + nPos2 + 1;
	Item[nIndex].m_nWidth = _wtoi(strData.Mid(nPos2, nPos3 - nPos2));

	if (! bNull)
		return nPos3 + 1;

	// end of index
	Item[nIndex + 1].m_nNumber = _FREE;
	Item[nIndex + 1].m_nFormat = _FREE;
	Item[nIndex + 1].m_nWidth  = _FREE;

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CInfoMathe

IMPLEMENT_DYNAMIC(CInfoMathe, CInfoField)

/////////////////////////////////////////////////////////////////////////////
// CInfoLoop construction/destruction

CInfoMathe::CInfoMathe(CString &strData)
{
	int nIndex = 0;
	int nPos = ReadMajor(strData);
	if (nPos == -1)
	{
		TRACE(L"warning: Invalid format to Mathematics Operation\n");
		ASSERT(FALSE);
		return;
	}

	strData = strData.Mid(nPos);
	if ((nPos = ReadMinor(strData)) == -1)
	{
		TRACE(L"warning: Invalid format to Mathematics Operation\n");
		ASSERT(FALSE);
		return;
	}

	while (nPos > 0)
	{
		strData = strData.Mid(nPos);
		nPos = ReadCondition(strData, nIndex++);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CInfoMathe operations

void CInfoMathe::ItemInsertString(CListBox* pLb, CStringList* pstrList /*=NULL*/)
{
	for (int nCount = 0; nCount < m_nMinorCount; nCount++)
	{
		wchar_t szItem[7];
		_itow(m_nMinor + nCount, szItem, 10);
		pLb->InsertString(-1, szItem);
	}
}

CInfoMathe* CInfoMathe::GetField(int nFile, int nGroup, int nField,
	int* pnBias /*=NULL*/)
{
	return (CInfoMathe*)
		CInfoField::GetField(nFile, CFG_INDEX_MATHE, nGroup, nField, pnBias);
}

int CInfoMathe::GetMajorIndex(int nFile, int nGroup, int nMajor)
{
	return CInfoField::GetMajorIndex(nFile, CFG_INDEX_MATHE, nGroup, nMajor);
}

/////////////////////////////////////////////////////////////////////////////
// CInfoMathe implementations

int CInfoMathe::ReadMajor(CString& strData)
{
	int nPos1 = strData.Find(L':');
	if (nPos1 == -1)
	{
		TRACE(L"warning: CInfoMathe::ReadMajor\n");
		return -1;
	}

	m_nMajor = _wtoi(strData.Left(nPos1));

	int nPos2 = strData.Find(L',');
	if (nPos2 == -1)
	{
		TRACE(L"warning: CInfoMathe::ReadMajor\n");
		return -1;
	}

	CString strIndex = strData.Mid(nPos1 + 1, nPos2 - nPos1 - 1);
	m_strName = AfxGetApp()->GetProfileString(L"Mnemonics", L"Address" + strIndex);
	
	return nPos2 + 1;
}

int CInfoMathe::ReadMinor(CString& strData)
{
	int nPos2;

	int nPos1 = strData.Find(L'-');
	if (nPos1 == -1)
	{
		nPos2 = strData.Find(L',');

		m_nMinor      = _wtoi(strData.Left(nPos2));
		m_nMinorCount = 1;
	}
	else
	{	
		nPos2 = strData.Find(',');
		if (nPos2 == -1)
		{
			TRACE(L"warning: CInfoMathe::ReadMinor\n");
			return -1;
		}

		m_nMinor      = _wtoi(strData.Left(nPos1));
		m_nMinorCount = _wtoi(strData.Mid(nPos1 + 1, nPos2 - nPos1 - 1));
	}

	return nPos2 + 1;
}

int CInfoMathe::ReadCondition(CString& strData, int nIndex)
{
	int nPos1 = strData.Find(L':');
	if (nPos1 == -1)
	{
		TRACE(L"warning: CInfoMathe::ReadCondition\n");
		return -1;
	}

	Condition[nIndex].m_nNumber = _wtoi(strData.Left(nPos1));

	BOOL bNull = FALSE;
	int nPos2 = strData.Find(L'.');
	if (nPos2 == -1)
	{
		nPos2 = strData.GetLength();
		bNull = TRUE;
	}
	
	CString strIndex = strData.Mid(nPos1 + 1, nPos2 - nPos1 - 1);

	Condition[nIndex].m_strName = AfxGetApp()->GetProfileString(L"Mnemonics", L"Address" + strIndex);

	if (bNull)
		return 0;
	else
		return nPos2 + 1;
}


/////////////////////////////////////////////////////////////////////////////
// CInfoConfig

IMPLEMENT_DYNAMIC(CInfoConfig, CObject)

/////////////////////////////////////////////////////////////////////////////
// CInfoConfig construction/destruction

CInfoConfig::~CInfoConfig()
{
	POSITION pos = m_OpeGroup.GetHeadPosition();
	while (pos != NULL)
	{
		CFG_OPEGROUP* pOpeGroup = (CFG_OPEGROUP*)m_OpeGroup.GetNext(pos);
		delete pOpeGroup;
	}
	m_OpeGroup.RemoveAll();

	pos = m_Field.GetHeadPosition();
	while (pos != NULL)
	{
		CInfoField* pField = (CInfoField*)m_Field.GetNext(pos);
		delete pField;
	}
	m_Field.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
// CInfoConfig operations

CFG_OPEGROUP* CInfoConfig::GetGroup(int nOperation, int nGroup)
{
	int nIndex = 0;
	POSITION pos = m_OpeGroup.GetHeadPosition();
	while (pos != NULL)
	{
		CFG_OPEGROUP* pOpeGroup = (CFG_OPEGROUP*)m_OpeGroup.GetNext(pos);
		ASSERT(pOpeGroup != NULL);
		if (pOpeGroup->m_nOperation != nOperation)
			continue;
		if (nIndex == nGroup)
			return pOpeGroup;
		else
			nIndex++;
	}
	
	return NULL;
}

int CInfoConfig::GetGroupIndex(int nOperation, int nGroup)
{
	int nIndex = 0;
	POSITION pos = m_OpeGroup.GetHeadPosition();
	while (pos != NULL)
	{
		CFG_OPEGROUP* pOpeGroup = (CFG_OPEGROUP*)m_OpeGroup.GetNext(pos);
		ASSERT(pOpeGroup != NULL);
		if (pOpeGroup->m_nOperation != nOperation)
			continue;
		if (pOpeGroup->m_nGroup == nGroup - 1)	// zero based
			return nIndex;
		else
			nIndex++;
	}
	
	return -1;
}

const wchar_t * CInfoConfig::GetGroupName(int nOperation, int nGroup)
{
	CFG_OPEGROUP* pOpeGroup = GetGroup(nOperation, nGroup);
	if (pOpeGroup != NULL)
		return pOpeGroup->m_strName;
	else
		return NULL;
}

CFG_OPEGROUP* CInfoConfig::GetNextGroup(POSITION& pos, int nOperation)
{
	while (pos != NULL)
	{
		CFG_OPEGROUP* pOpeGroup = (CFG_OPEGROUP*)m_OpeGroup.GetNext(pos);
		if (pOpeGroup->m_nOperation == nOperation)
			return pOpeGroup;
	}

	return NULL;
}

BOOL CInfoConfig::ReadConfig(INFO_FILE* pFile)
{
	int nIndex = 0;

	ReadGroup(pFile, CFG_INDEX_LOOP, &nIndex);
	ReadGroup(pFile, CFG_INDEX_PRINT, &nIndex);
	ReadGroup(pFile, CFG_INDEX_MATHE, &nIndex);

	return (nIndex > 0) ? TRUE : FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CInfoConfig implementations

void CInfoConfig::ReadGroup(INFO_FILE* pFile, int nOperation, int* pnIndex)
{
	static wchar_t szGroup0[] = L"Group";
	static wchar_t szGroup1[] = L"Group%d";

	for (int nCount = 0; nCount < CFG_NUM_GROUP; nCount++)
	{
		wchar_t szGroup[32];
		wsprintf(szGroup, szGroup1, nCount + 1);
		CString strGroup = AfxGetApp()->GetProfileString(szGroup0, szGroup);
		if (strGroup.IsEmpty())
			continue;

		int nTopIndex = *pnIndex;

		for (int nItem = 1; nItem < 100; nItem++)
		{
			wchar_t szEntry[32];
			const wchar_t * pszOpe = GetOperation(nOperation);
			wsprintf(szEntry, L"%s%s%d", pszOpe, strGroup, nItem);
			CString strData = AfxGetApp()->GetProfileString(pFile->m_strName, szEntry);
			if (strData.IsEmpty())
				break;

			switch (nOperation)
			{
			case CFG_INDEX_LOOP:
				m_Field.AddTail(new CInfoLoop(strData));
				(*pnIndex)++;
				break;
			case CFG_INDEX_PRINT:
				m_Field.AddTail(new CInfoPrint(strData));
				(*pnIndex)++;
				break;
			case CFG_INDEX_MATHE:
				m_Field.AddTail(new CInfoMathe(strData));
				(*pnIndex)++;
				break;
			default:
				ASSERT(FALSE);
			}
		}

		if (nTopIndex != *pnIndex)
		{
			CFG_OPEGROUP* pConfig = new CFG_OPEGROUP;
			pConfig->m_nIndex     = nTopIndex;
			pConfig->m_nCount     = *pnIndex - nTopIndex;
			pConfig->m_nOperation = nOperation;
			pConfig->m_nGroup     = nCount;
			pConfig->m_strName    = strGroup;
			m_OpeGroup.AddTail(pConfig);
		}
	}
}

static wchar_t szOpe0[] = L"Loop";
static wchar_t szOpe1[] = L"Print";
static wchar_t szOpe2[] = L"Mathe";

const wchar_t * CInfoConfig::GetOperation(int nIndex)
{
	switch (nIndex)
	{
	case CFG_INDEX_LOOP:
		return szOpe0;
	case CFG_INDEX_PRINT:
		return szOpe1;
	case CFG_INDEX_MATHE:
		return szOpe2;
	default:
		return NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CInfo

IMPLEMENT_DYNAMIC(CInfo, CObject)

/////////////////////////////////////////////////////////////////////////////
// CInfo construction/destruction

CInfo::CInfo()
{
	ReadFile();
	ReadDevice();
	ReadFormat();
	ReadCondition();
	ReadConfig();
	ReadEod();
	ReadPtd();
}

CInfo::~CInfo()
{
	DeleteConfig();
	DeleteCondition();
	DeleteFormat();
	DeleteDevice();
	DeleteFile();
}

/////////////////////////////////////////////////////////////////////////////
// CInfo diagnostics

#ifdef _DEBUG
void CInfo::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfo implementations

INFO_FILE* CInfo::GetFile(int nFile)
{
	POSITION pos = m_File.GetHeadPosition();
	while (pos != NULL)
	{
		INFO_FILE* pFile = (INFO_FILE*)m_File.GetNext(pos);
		if (pFile->m_nNumber == nFile)
			return pFile;
	}
	
	return NULL;
}

CString CInfo::GetFileName(int nIndex)
{
	POSITION pos = m_File.FindIndex(nIndex);
	INFO_FILE* pFile = (INFO_FILE*)ptheInfo->m_File.GetAt(pos);
	return pFile->m_strName;
}

void CInfo::ReadFile()
{
	static wchar_t szSection[] = L"File";
	static wchar_t szEntry0[] = L"File%d";
	wchar_t szEntry[32];

	for (int nCount = 1; nCount < 50; nCount++)
	{
		wsprintf(szEntry, szEntry0, nCount);
		CString strData = AfxGetApp()->GetProfileString(szSection, szEntry);
		if (strData.IsEmpty())
			break;

		INFO_FILE* pFile = new INFO_FILE;
		if (AnalyzeData(strData, pFile) == 0)
			m_File.AddTail(pFile);
		else
			delete pFile;
	}
}

void CInfo::DeleteFile()
{
	POSITION pos = m_File.GetHeadPosition();
	while (pos != NULL)
	{
		INFO_FILE* pFile = (INFO_FILE*)m_File.GetNext(pos);
		delete pFile;
	}

	m_File.RemoveAll();
}

int CInfo::AnalyzeData(CString& strData, INFO_FILE* pFile)
{
	int	nPos1 = strData.Find(L':');
	if (nPos1 == -1)						// ':' not found
	{
		TRACE0("warning: CInfo::AnalyzeData\n");
		ASSERT(FALSE);
		return -1;
	}
	
	int	nPos2 = strData.Find(L',');
	if (nPos2 == -1)						// option not found
	{
		pFile->m_nNumber = _wtoi(strData.Left(nPos1));
		pFile->m_strName = strData.Mid(nPos1 + 1);
		pFile->m_strOption.Empty();
	}
	else
	{
		pFile->m_nNumber = _wtoi(strData.Left(nPos1));
		pFile->m_strName = strData.Mid(nPos1 + 1, nPos2 - nPos1 - 1);
		pFile->m_strOption = strData.Mid(nPos2 + 1);
	}

	return 0;
}

INFO_DEVICE* CInfo::GetDevice(int nDevice)
{
	POSITION pos = m_Device.GetHeadPosition();
	while (pos != NULL)
	{
		INFO_DEVICE* pDevice = (INFO_DEVICE*)m_Device.GetNext(pos);
		if (pDevice->m_nNumber == nDevice)
			return pDevice;
	}
	
	return NULL;
}

void CInfo::ReadDevice()
{
	static wchar_t szSection[] = L"Device";
	static wchar_t szEntry0[] = L"Device%d";
	wchar_t szEntry[32];

	for (int nCount = 1; nCount < 50; nCount++)
	{
		wsprintf(szEntry, szEntry0, nCount);
		CString strData = AfxGetApp()->GetProfileString(szSection, szEntry);
		if (strData.IsEmpty())
			break;

		INFO_DEVICE* pDevice = new INFO_DEVICE;
		if (AnalyzeData(strData, pDevice) == 0)
			m_Device.AddTail(pDevice);
		else
			delete pDevice;
	}
}

void CInfo::DeleteDevice()
{
	POSITION pos = m_Device.GetHeadPosition();
	while (pos != NULL)
	{
		INFO_DEVICE* pDevice = (INFO_DEVICE*)m_Device.GetNext(pos);
		delete pDevice;
	}

	m_Device.RemoveAll();
}

int CInfo::AnalyzeData(CString& strData, INFO_DEVICE* pDevice)
{
	int	nPos1 = strData.Find(L':');
	if (nPos1 == -1)						// ':' not found
	{
		TRACE0("warning: CInfo::AnalyzeData\n");
		ASSERT(FALSE);
		return -1;
	}
	
	int	nPos2 = strData.Find(L',');
	if (nPos2 == -1)						// ',' not found
	{
		TRACE0("warning: CInfo::AnalyzeData\n");
		ASSERT(FALSE);
		return -1;
	}
	
	int	nPos3 = strData.ReverseFind(L',');
	if (nPos2 == nPos3)						// option not found
	{
		pDevice->m_nNumber = _wtoi(strData.Left(nPos1));
		pDevice->m_strName = strData.Mid(nPos1 + 1, nPos2 - nPos1 - 1);
		pDevice->m_nWidth  = _wtoi(strData.Mid(nPos2 + 1));
		pDevice->m_strOption.Empty();
	}
	else
	{
		pDevice->m_nNumber   = _wtoi(strData.Left(nPos1));
		pDevice->m_strName   = strData.Mid(nPos1 + 1, nPos2 - nPos1 - 1);
		pDevice->m_nWidth    = _wtoi(strData.Mid(nPos2 + 1, nPos3 - nPos2));
		pDevice->m_strOption = strData.Mid(nPos3 + 1);
	}

	return 0;
}

INFO_FORMAT* CInfo::GetFormat(int nNumber)
{
	POSITION pos = ptheInfo->m_Format.GetHeadPosition();
	while (pos != NULL)
	{
		INFO_FORMAT* pFormat = (INFO_FORMAT*)ptheInfo->m_Format.GetNext(pos);
		if (pFormat->m_nNumber == nNumber)
			return pFormat;
	}

	return NULL;
}

void CInfo::ReadFormat()
{
	static wchar_t szSection[] = L"Format";
	static wchar_t szEntry0[]  = L"Format%d";
	wchar_t szEntry[32];

	for (int nCount = 1; nCount < 50; nCount++)
	{
		wsprintf(szEntry, szEntry0, nCount);
		CString strData = AfxGetApp()->GetProfileString(szSection, szEntry);
		if (strData.IsEmpty())
			break;

		INFO_FORMAT* pFormat = new INFO_FORMAT;
		if (AnalyzeData(strData, pFormat) == 0)
			m_Format.AddTail(pFormat);
		else
			delete pFormat;
	}
}

void CInfo::DeleteFormat()
{
	POSITION pos = m_Format.GetHeadPosition();
	while (pos != NULL)
	{
		INFO_FORMAT* pFormat = (INFO_FORMAT*)m_Format.GetNext(pos);
		delete pFormat;
	}

	m_Format.RemoveAll();
}

int CInfo::AnalyzeData(CString& strData, INFO_FORMAT* pFormat)
{
	int	nPos1 = strData.Find(L':');
	if (nPos1 == -1)						// ':' not found
	{
		TRACE0("warning: CInfo::AnalyzeData\n");
		ASSERT(FALSE);
		return -1;
	}

	pFormat->m_nNumber = _wtoi(strData.Left(nPos1));
	pFormat->m_strName = strData.Mid(nPos1 + 1);

	return 0;
}
	
void CInfo::ReadCondition()
{
	static wchar_t szSection[] = L"Condition";
	static wchar_t szEntry0[]  = L"Condition%d";
	wchar_t szEntry[32];

	for (int nCount = 1; nCount < 50; nCount++)
	{
		wsprintf(szEntry, szEntry0, nCount);
		CString strData = AfxGetApp()->GetProfileString(szSection, szEntry);
		if (strData.IsEmpty())
			break;

		INFO_COND* pCond = new INFO_COND;
		if (AnalyzeData(strData, pCond) == 0)
			m_Condition.AddTail(pCond);
		else
			delete pCond;
	}
}

void CInfo::DeleteCondition()
{
	POSITION pos = m_Condition.GetHeadPosition();
	while (pos != NULL)
	{
		INFO_COND* pCond = (INFO_COND*)m_Condition.GetNext(pos);
		delete pCond;
	}

	m_Condition.RemoveAll();
}

int CInfo::AnalyzeData(CString& strData, INFO_COND* pCond)
{
	int	nPos1 = strData.Find(L':');
	if (nPos1 == -1)						// ':' not found
	{
		TRACE0("warning: CInfo::AnalyzeData\n");
		ASSERT(FALSE);
		return -1;
	}

	pCond->m_nNumber = _wtoi(strData.Left(nPos1));
	pCond->m_strName = strData.Mid(nPos1 + 1);

	return 0;
}
	
void CInfo::ReadConfig()
{
	POSITION pos = m_File.GetHeadPosition();
	while (pos != NULL)
	{
		INFO_FILE* pFile = (INFO_FILE*)m_File.GetNext(pos);
		if (! pFile->m_strOption.IsEmpty())
			continue;

		CInfoConfig* pConfig = new CInfoConfig;
		if (pConfig->ReadConfig(pFile))
			m_Config.AddTail(pConfig);
		else
			delete pConfig;
	}
}

void CInfo::DeleteConfig()
{
	POSITION pos = m_Config.GetHeadPosition();
	while (pos != NULL)
	{
		CInfoConfig* pConfig = (CInfoConfig*)m_Config.GetNext(pos);
		delete pConfig;
	}
	m_Config.RemoveAll();
}

CInfoConfig* CInfo::GetConfig(int nFile)
{
	POSITION pos = m_Config.FindIndex(nFile);
	if (pos == NULL)
		return NULL;

	CInfoConfig* pConfig = (CInfoConfig*)m_Config.GetAt(pos);
	if (pConfig == NULL)
		return NULL;

	return pConfig;
}

void CInfo::ReadEod()
{
	static wchar_t szEntry[] = L"EOD";

	CString strData = AfxGetApp()->GetProfileString(szGlobalSection, szEntry);

	for (int nCount = 0; nCount < INFO_NUM_FILE;)
	{
		int	nPos = strData.Find(L',');
		if (nPos != -1)
		{
			m_nEod[nCount++] = _wtoi(strData.Left(nPos));
			strData = strData.Mid(nPos + 1);
		}
		else
		{
			m_nEod[nCount++] = _wtoi(strData);
			if (nCount < INFO_NUM_FILE)
				m_nEod[nCount] = _FREE;
			return;
		}
	}

	ASSERT(FALSE);
}

void CInfo::ReadPtd()
{
	static wchar_t szEntry[]  = L"PTD";

	CString strData = AfxGetApp()->GetProfileString(szGlobalSection, szEntry);

	for (int nCount = 0; nCount < INFO_NUM_FILE; nCount)
	{
		int	nPos = strData.Find(L',');
		if (nPos != -1)
		{
			m_nPtd[nCount++] = _wtoi(strData.Left(nPos));
			strData = strData.Mid(nPos + 1);
		}
		else
		{
			m_nPtd[nCount++] = _wtoi(strData);
			if (nCount < INFO_NUM_FILE)
				m_nPtd[nCount] = _FREE;
			return;
		}
	}

	ASSERT(FALSE);
}

const wchar_t * CInfo::GetItem(int nItem)
{
	static wchar_t szNone[] = L"None";
	static wchar_t szTotal[] = L"Total";
	static wchar_t szCounter[] = L"Counter";

	switch (nItem)
	{
	case 1:
		return szTotal;
	case 2:
		return szCounter;
	default:
		return szNone;
	}
}
