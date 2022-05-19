// DisplayBitMap.cpp : implementation file
//

#include "stdafx.h"
#include "Framework.h"
#include "DisplayBitMap.h"


// CDisplayBitMap dialog

IMPLEMENT_DYNAMIC(CDisplayBitMap, CDialog)

CDisplayBitMap::CDisplayBitMap(CWnd* pParent /*=NULL*/)
	: CDialog(CDisplayBitMap::IDD, pParent)
{

}

CDisplayBitMap::~CDisplayBitMap()
{
}

void CDisplayBitMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (pDX->m_bSaveAndValidate == 0) {
		CStatic  *pBitmapDisplay;

		pBitmapDisplay = (CStatic  *)GetDlgItem (IDC_BITMAPAREA);
		pBitmapDisplay->SetBitmap (myBitMap);
	}
}


BEGIN_MESSAGE_MAP(CDisplayBitMap, CDialog)
END_MESSAGE_MAP()


// CDisplayBitMap message handlers
