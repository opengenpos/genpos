// WindowGroup.h: interface for the CWindowGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWGROUP_H__50A94E41_8F1F_4BC4_9086_CB9C14D1C548__INCLUDED_)
#define AFX_WINDOWGROUP_H__50A94E41_8F1F_4BC4_9086_CB9C14D1C548__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*Structure containing all the attributes to be serialized for the WindowGroup class
any new attributes need to be added at the end of the struct so data from 
old layout files does not get corrupted during serialization*/
typedef struct _tagWinGroupAttributes{
	unsigned long signatureStart;
	UINT GroupID;				//group ID
	TCHAR myGroupName[30];      //group name
	TCHAR myGroupDesc[200];		//group description
	UINT  nActiveWnd;			/*specifies the Active Window ID for the Group, which 
								determines which window is initially displayed*/
}WinGroupAttributes;			

class CWindowGroup: public CObject  
{
public:
	CWindowGroup();
	virtual ~CWindowGroup();

	void Serialize( CArchive& ar );

	static UINT uiGlobalGroupID;  //static ID - is incremented every time a new group is added

	WinGroupAttributes groupAttributes;
	CString GroupName;
	CString GroupDesc;

	DECLARE_SERIAL(CWindowGroup)
private:

};

#endif // !defined(AFX_WINDOWGROUP_H__50A94E41_8F1F_4BC4_9086_CB9C14D1C548__INCLUDED_)
