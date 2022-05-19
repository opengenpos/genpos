
#pragma once


// CMySourceData command target

class CMySourceData : public COleDataSource
{
	DECLARE_DYNAMIC(CMySourceData)

public:
	CMySourceData();
	virtual ~CMySourceData();

	BOOL StartDragFile (CString FileName);
	BOOL StartDragText (CString TextString);

protected:
	DECLARE_MESSAGE_MAP()
};


