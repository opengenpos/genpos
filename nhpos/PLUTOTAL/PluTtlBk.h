// PluTtlBk.h: interface for the CnPluTotalBackUp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUTTLBK_H__E37E9763_35F1_11D4_BF0B_00A0C961E76F__INCLUDED_)
#define AFX_PLUTTLBK_H__E37E9763_35F1_11D4_BF0B_00A0C961E76F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "PluTtlD.h"
#include "PluTtlRg.h"

#ifdef XP_PORT  //building for Win2000/XP
#include "CnAdoXP.h"
#else  //building for CE
#include "CnAdoCe.h"
#endif

#include "PluTtlDb.h"
#include "tbc\\tbc.h"

#define PLUTOTAL_BK_MAX_FILE_NUM    8

class CnPluTotalBackUp
{
protected:
    CString     m_strWorkPath;
    CString     m_strBkupPath;

    CStringList m_strlstBackupFileList;
    CStringList m_strlstRestoreFileList;
    int         m_nFileCnt;
    TCHAR   *   m_pBackupFileTbl[PLUTOTAL_BK_MAX_FILE_NUM + 1];
    TCHAR   *   m_pRestoreFileTbl[PLUTOTAL_BK_MAX_FILE_NUM + 1];

public:
    CnPluTotalBackUp();
    virtual ~CnPluTotalBackUp();

    VOID    SetFileName(LPCTSTR szFileName);
    VOID    SetBkupPath(LPCTSTR szBkup);
    VOID    SetWorkPath(LPCTSTR szWork);

    ULONG   Initialize();
    ULONG   StartCopy(const BOOL nBackupMode = TRUE);
    ULONG   GetStatus();

protected:
    int         CreateParams(void);
    VOID        ClearParams(void);
    VOID        ClearFileList(void) {
                    m_strlstBackupFileList.RemoveAll();
                    m_strlstRestoreFileList.RemoveAll();
                    m_nFileCnt = 0;
                };

};

#endif // !defined(AFX_PLUTTLBK_H__E37E9763_35F1_11D4_BF0B_00A0C961E76F__INCLUDED_)
