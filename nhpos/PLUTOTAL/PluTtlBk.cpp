// PluTtlBk.cpp: implementation of the CnPluTotalBackUp class.
//
//
// 2000.07.06 V1.0.0.2 m.teraki Tbc specification changed
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PluTotal.h"
#include "PluTtlBk.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// constructor
CnPluTotalBackUp::CnPluTotalBackUp()
{
    ClearParams();
}

// destructor
CnPluTotalBackUp::~CnPluTotalBackUp()
{
    //ClearParams();
    ClearFileList();
}

// clear all parameters
VOID    CnPluTotalBackUp::ClearParams(void)
{
    memset(m_pBackupFileTbl,0,sizeof(TCHAR *) * (PLUTOTAL_BK_MAX_FILE_NUM + 1));
    memset(m_pRestoreFileTbl,0,sizeof(TCHAR *) * (PLUTOTAL_BK_MAX_FILE_NUM + 1));
    m_nFileCnt = 0;
    ClearFileList();
}

// set path used to place backup file
VOID    CnPluTotalBackUp::SetBkupPath(LPCTSTR szBkup)   /* added teraki (2000.07.06) */
{
    // update member
    m_strBkupPath = szBkup;

    // add trailing backslash if not specified.
    if (m_strBkupPath.Right(1) != TEXT('\\')) {
        m_strBkupPath += TEXT('\\');
    }
}

// set path used to working directory
VOID    CnPluTotalBackUp::SetWorkPath(LPCTSTR szWork)
{
    // update member
    m_strWorkPath = szWork;

    // add trailing backslash if not specified.
    if(m_strWorkPath.Right(1) != TEXT("\\"))
        m_strWorkPath += TEXT("\\");
}

// set filename to be performed backup/restore
VOID    CnPluTotalBackUp::SetFileName(LPCTSTR szFileName)
{
    CString strB, strR;

    strB = m_strWorkPath + szFileName;
    strR = m_strBkupPath + szFileName;

    m_strlstBackupFileList.AddTail(strB);
    m_strlstRestoreFileList.AddTail(strR);
}

/*
*============================================================================
**Synopsis:     int     CnPluTotalBackUp::_MakeCommandParams(void)
*    Input:     Nothing
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      >0 : parameter length
*               0  : error
** Description  
*
*============================================================================
*/
int     CnPluTotalBackUp::CreateParams(void)
{
    int nPos;
    POSITION pos;

    m_nFileCnt = m_strlstBackupFileList.GetCount(); // ### ADD Saratoga (060700)
    if(m_nFileCnt <= 0 || (PLUTOTAL_BK_MAX_FILE_NUM < m_nFileCnt)){
        m_nFileCnt = 0;
        return  0;
    }

    // create cmd table for backup
    nPos = 0;
    pos = m_strlstBackupFileList.GetHeadPosition();
    while (pos != NULL){

        // backup table
        m_pBackupFileTbl[nPos] = (TCHAR *)((LPCTSTR)(m_strlstBackupFileList.GetAt(pos)));

        m_strlstBackupFileList.GetNext(pos);
        nPos++;
    }
    m_pBackupFileTbl[nPos] = (TCHAR *)((LPCTSTR)m_strBkupPath);

    // create cmd table for restore
    nPos = 0;
    pos = m_strlstRestoreFileList.GetHeadPosition();
    while (pos != NULL){

        // restore table
        m_pRestoreFileTbl[nPos] = (TCHAR *)((LPCTSTR)(m_strlstRestoreFileList.GetAt(pos)));

        m_strlstRestoreFileList.GetNext(pos);
        nPos++;
    }
    m_pRestoreFileTbl[nPos] = (TCHAR *)((LPCTSTR)m_strWorkPath);

    return  m_nFileCnt;
}

/*
*============================================================================
**Synopsis:     ULONG   CnPluTotalBackUp::Initialize(void)
*               ULONG   CnPluTotalBackUp::StartCopy(const BOOL bBackupMode)
*               ULONG   CnPluTotalBackUp::GetStatus(void)
*
*    Input:     bBackupMode         - T/F : backup/restore
*
*   Output:     Nothing
*
*    InOut:     Nothing
*
** Return:      TBC status (see TBC documents)
*
** Description  TBC functions.
*
*============================================================================
*/
ULONG   CnPluTotalBackUp::Initialize(void)
{
    TBC_CMD     cmd;
    cmd.dwCmdId = TBC_CMDID_INIT;
    cmd.dwResult = 0;
    cmd.dwLen = 0;
    cmd.pvData = NULL;

    TbcCommand(&cmd);

    if (cmd.dwResult == TBC_RSLT_COMPLETE) {
        return PLUTOTAL_SUCCESS;
    }
    return  PLUTOTAL_E_FAILURE;
}


ULONG   CnPluTotalBackUp::StartCopy(const BOOL nBackupMode)
{
    DWORD dwRet;

    if (m_nFileCnt == 0)
        CreateParams();

    TBC_CMD     cmd;
    cmd.dwCmdId = TBC_CMDID_STARTCOPY;
    cmd.dwResult = 0;

    cmd.dwLen = (m_nFileCnt + 1) * sizeof(TCHAR *);
    if (cmd.dwLen <= 0)
        return  PLUTOTAL_E_FAILURE;

    if(nBackupMode)
        cmd.pvData = (VOID *)m_pBackupFileTbl;
    else
        cmd.pvData = (VOID *)m_pRestoreFileTbl;

    TbcCommand(&cmd);

    switch (cmd.dwResult) {
    case TBC_RSLT_ACC_CLEAR:
        dwRet = PLUTOTAL_BK_ACCEPTED_CLEARLY;
        break;
    case TBC_RSLT_ACC_LOWSTOR:
        dwRet = PLUTOTAL_BK_ACCEPTED_LOWSTORAGE;
        break;
    case TBC_RSLT_DNY_BUSY:
        dwRet = PLUTOTAL_BK_DENIED_BUSY;
        break;
    case TBC_RSLT_DNY_NOHEAP:
        dwRet = PLUTOTAL_BK_DENIED_NOHEAP;
        break;
    case TBC_RSLT_DNY_NOSTOR:
        dwRet = PLUTOTAL_BK_DENIED_NOSTORAGE;
        break;
    default:
        dwRet = PLUTOTAL_E_FAILURE;
    }

    return  dwRet;
}

ULONG   CnPluTotalBackUp::GetStatus()
{
    DWORD dwRet;

    TBC_CMD     cmd;
    cmd.dwCmdId = TBC_CMDID_GETSTATUS;
    cmd.dwResult = 0;
    cmd.dwLen = 0;
    cmd.pvData = NULL;

    TbcCommand(&cmd);

    switch (cmd.dwResult) {
    case TBC_RSLT_COMPLETE:
        dwRet = PLUTOTAL_BK_STATUS_READY;
        break;

	case TBC_RSLT_DNY_BUSY:
    case TBC_RSLT_BUSY:
        dwRet = PLUTOTAL_BK_STATUS_BUSY;
        break;

    default:
		{
			char  xBuff[128];

			sprintf (xBuff, "==NOTE: CnPluTotalBackUp::GetStatus() TbcCommand %d, 0x%x", cmd.dwResult, cmd.dwResult);
			NHPOS_ASSERT_TEXT((cmd.dwResult == TBC_RSLT_COMPLETE), xBuff);
		}
        dwRet = PLUTOTAL_E_FAILURE;
        break;
    }

    return dwRet;
}

