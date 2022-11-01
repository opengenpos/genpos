/////////////////////////////////////////////////////////////////////////////
// info.h : header file
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

#ifndef __INFO_H__
#define __INFO_H__


#define	_FREE			(-1)
#pragma pack(push, 1)
typedef struct _INFO_FILE
{
	short			m_nNumber;
	CString		m_strName;
	CString		m_strOption;
} INFO_FILE;

typedef struct _INFO_DEVICE
{
	short		m_nNumber;
	CString		m_strName;
	short		m_nWidth;
	CString		m_strOption;
} INFO_DEVICE;

typedef struct _INFO_FORMAT
{
	short		m_nNumber;
	CString		m_strName;
} INFO_FORMAT;

typedef struct _INFO_COND
{
	short		m_nNumber;
	CString		m_strName;
} INFO_COND;

typedef struct _CFG_OPEGROUP
{
	short	m_nIndex;
	short	m_nCount;
	short	m_nOperation;
	short	m_nGroup;
	CString	m_strName;
} CFG_OPEGROUP;
#pragma pack(pop)
/////////////////////////////////////////////////////////////////////////////

class CInfoField : public CObject
{
protected:
	DECLARE_DYNAMIC(CInfoField)

// Attributes
public:
	int			m_nMajor;
	int			m_nMinor;
	CString		m_strName;

// Operations
public:
			void		FieldInsertString(CListBox* pLb);
	virtual void    	ItemInsertString(CListBox* pLb, CStringList* strList = NULL) = 0;
	static	CInfoField*	GetField(int nFile, int nOperation, int nGroup,
								 int nField, int* pnBias = NULL);
	static	int			GetMajorIndex(int nFile, int nOperation, int nGroup,
									  int nMajor);
protected:
};


/////////////////////////////////////////////////////////////////////////////

#define	LOOP_NUM_COND		5

class CInfoLoop : public CInfoField
{
protected:
	DECLARE_DYNAMIC(CInfoLoop)
#pragma pack(push,1)
	typedef struct _LOOP_COND
	{
		short	m_nNumber;
		CString	m_strName;
	} LOOP_COND;
#pragma pack(pop)
public:
	CInfoLoop(CString& strData);

// Attributes
public:
	LOOP_COND	Condition[LOOP_NUM_COND];

// Operations
public:
	virtual void    	ItemInsertString(CListBox* pLb, CStringList* pstrList = NULL);
	static	CInfoLoop*	GetField(int nFile, int nGroup, int nField,
								 int* pnBias = NULL);
	static	int			GetMajorIndex(int nFile, int nGroup, int nMajor);
protected:
			int			ReadMajor(CString& strData);
			int			ReadMinor(CString& strData);
			int			ReadCondition(CString& strData, int nIndex);
};


/////////////////////////////////////////////////////////////////////////////

#define	PRINT_NUM_ITEM		3
#define	PRINT_F_RIGHT		0x0001
#define	PRINT_F_LEFT		0x0004
#define	PRINT_F_WIDE		0x0008
#define	PRINT_F_WIDTH		0x0010
#define	PRINT_F_FORMAT		0x0040
#define	PRINT_F_ACCUM		0x0080

class CInfoPrint : public CInfoField
{
protected:
	DECLARE_DYNAMIC(CInfoPrint)
#pragma pack(push, 1)
	typedef struct _PRINT_ITEM
	{
		short	m_nNumber;
		short	m_nFormat;
		short	m_nWidth;
	} PRINT_ITEM;
#pragma pack(pop)
public:
	CInfoPrint(CString& strData);

// Attributes
public:
	int			m_nMinorCount;
	int			m_nFlags;
	PRINT_ITEM	Item[PRINT_NUM_ITEM];

// Operations
public:
	virtual void    	ItemInsertString(CListBox* pLb, CStringList* pstrList = NULL);
	static	CInfoPrint*	GetField(int nFile, int nGroup, int nField,
								 int* pnBias = NULL);
	static	int			GetMajorIndex(int nFile, int nGroup, int nMajor);
protected:
			int			ReadMajor(CString& strData);
			int			ReadMinor(CString& strData);
			int			ReadFlags(CString& strData);
			int			ReadItem(CString& strData, int nIndex);
};


/////////////////////////////////////////////////////////////////////////////

#define	MATHE_NUM_COND		5

class CInfoMathe : public CInfoField
{
protected:
	DECLARE_DYNAMIC(CInfoMathe)
#pragma pack(push, 1)
	typedef struct _MATHE_COND
	{
		short	m_nNumber;
		CString	m_strName;
	} MATHE_COND;
#pragma pack(pop)
public:
	CInfoMathe(CString& strData);

// Attributes
public:
	int			m_nMinorCount;
	MATHE_COND	Condition[MATHE_NUM_COND];

// Operations
public:
	virtual void    	ItemInsertString(CListBox* pLb, CStringList* pstrList = NULL);
	static	CInfoMathe*	GetField(int nFile, int nGroup, int nField,
								 int* pnBias = NULL);
	static	int			GetMajorIndex(int nFile, int nGroup, int nMajor);
protected:
			int			ReadMajor(CString& strData);
			int			ReadMinor(CString& strData);
			int			ReadCondition(CString& strData, int nIndex);
};


/////////////////////////////////////////////////////////////////////////////

#define	CFG_NUM_OPERATION	3
#define	CFG_INDEX_LOOP		0
#define	CFG_INDEX_PRINT		1
#define	CFG_INDEX_MATHE		2
#define	CFG_NUM_GROUP		5
#define	CFG_INDEX_KEY		0
#define	CFG_INDEX_TOTAL		1
#define	CFG_INDEX_PARAM		2
#define	CFG_INDEX_MNEMO		3
#define	CFG_INDEX_ACCUM		4

class CInfoConfig : public CObject
{
protected:
	DECLARE_DYNAMIC(CInfoConfig)
public:
	~CInfoConfig();

public:
protected:
	CPtrList	m_OpeGroup;
	CObList		m_Field;

// Operations
public:
	CFG_OPEGROUP*	GetGroup(int nOperation, int nGroup);
	int				GetGroupIndex(int nOperation, int nGroup);
	const wchar_t *	GetGroupName(int nOperation, int nGroup);
	POSITION		GetHeadGroup()
						{ return m_OpeGroup.GetHeadPosition(); }
	CFG_OPEGROUP*	GetNextGroup(POSITION& pos, int nOperation);
	POSITION		FieldFindIndex(int nIndex)
						{ return m_Field.FindIndex(nIndex); }
	CInfoField*		FieldGetNext(POSITION& pos)
						{ return (CInfoField*)m_Field.GetNext(pos); }
	BOOL			ReadConfig(INFO_FILE* pFile);
protected:
	void			ReadGroup(INFO_FILE* pFile, int nOperation, int* nIndex);
	const wchar_t *	GetOperation(int nIndex);
};


/////////////////////////////////////////////////////////////////////////////

#define	INFO_NUM_FILE		11
#define	INFO_DEVICE_LCD		5			// caution !!

class CInfo : public CObject
{
protected:
	DECLARE_DYNAMIC(CInfo)
public:
	CInfo();

// Attributes
public:
	CPtrList	m_File;
	CPtrList	m_Device;
	int			m_nEod[INFO_NUM_FILE];
	int			m_nPtd[INFO_NUM_FILE];
	CPtrList	m_Format;
	CObList		m_Config;
protected:
	CPtrList	m_Condition;

// Operations
public:
	INFO_FILE*		GetFile(int nFile);
	CString			GetFileName(int nIndex);
	INFO_DEVICE*	GetDevice(int nDevice);
	CInfoConfig*	GetConfig(int nFile);
	const wchar_t *	GetItem(int nGroup);
	INFO_FORMAT*	GetFormat(int nNumber);
	POSITION		CondGetHeadPosition()
						{ return m_Condition.GetHeadPosition(); }
	POSITION		CondFindIndex(int nIndex)
						{ return m_Condition.FindIndex(nIndex); }
	INFO_COND*		CondGetNext(POSITION& pos)
						{ return (INFO_COND*)m_Condition.GetNext(pos); }
	INFO_COND*		CondGetAt(POSITION& pos)
						{ return (INFO_COND*)m_Condition.GetAt(pos); }
protected:
	void 		ReadFile();
	void 		DeleteFile();
	int			AnalyzeData(CString& strData, INFO_FILE* pFile);
	void 		ReadDevice();
	void 		DeleteDevice();
	int			AnalyzeData(CString& strData, INFO_DEVICE* pDevice);
	void		ReadFormat();
	void		DeleteFormat();
	int			AnalyzeData(CString& strData, INFO_FORMAT* pFormat);
	void		ReadCondition();
	void		DeleteCondition();
	int			AnalyzeData(CString& strData, INFO_COND* pCond);
	void		ReadConfig();
	void		DeleteConfig();
	void		ReadEod();
	void		ReadPtd();

// Implementation
public:
	virtual	~CInfo();
private:
	
#ifdef _DEBUG
    void Dump(CDumpContext& dc) const;
#endif
};


#endif
