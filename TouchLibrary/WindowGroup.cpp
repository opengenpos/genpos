// WindowGroup.cpp: implementation of the CWindowGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WindowGroup.h"

IMPLEMENT_SERIAL( CWindowGroup, CObject, VERSIONABLE_SCHEMA | 1 )

UINT CWindowGroup::uiGlobalGroupID = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWindowGroup::CWindowGroup()
{
	//initialize attributes
	groupAttributes.signatureStart = 0xABCDEF87;
	groupAttributes.GroupID = 0;
	GroupDesc = _T("");
	GroupName = _T("");
	groupAttributes.nActiveWnd = 0;
}

CWindowGroup::~CWindowGroup()
{

}

void CWindowGroup::Serialize( CArchive& ar )
{
	UCHAR* pBuff = (UCHAR*) &groupAttributes;
	CObject::Serialize( ar );

	if( ar.IsStoring() ){
		//copy CString to TCHARs for serialization
		lstrcpy(groupAttributes.myGroupName ,GroupName);
		lstrcpy(groupAttributes.myGroupDesc ,GroupDesc);

		//verify teh start signature to check against data corruption
		ASSERT(groupAttributes.signatureStart = 0xABCDEF87);
		//store the global group id so when we open this file, we start giving new groups the appropriate id's
		ar << uiGlobalGroupID;
		//serialize the attributes of this class
		ar<<sizeof(_tagWinGroupAttributes);
		for(int i = 0; i < sizeof(_tagWinGroupAttributes); i++){
			ar<<pBuff[i];
		}
	}
	else{
		//load the global group id so when we open this file, we start giving new groups the appropriate id's
		ar >> uiGlobalGroupID;
		ULONG mySize;
		memset(&groupAttributes,0,sizeof(_tagWinGroupAttributes));
		ar>>mySize;

		// Because the file that is being loaded in may be from a
		// different version of Layout Manager, we must be careful
		// about just reading in the information from the file.
		// If the object stored is larger than the object in the
		// version of Layout Manager being used then we will
		// overwrite memory and cause application failure.
		ULONG  xMySize = mySize;
		UCHAR  ucTemp;

		if (xMySize > sizeof (groupAttributes)) {
			xMySize = sizeof (groupAttributes);
		}
		UINT y = 0;
		for(y = 0; y < xMySize;y++){
			ar>>pBuff[y];
		}
		for(; y < mySize;y++){
			ar>>ucTemp;
		}

		//verify teh start signature to check against data corruption
		ASSERT(groupAttributes.signatureStart = 0xABCDEF87);
		
		//copy TCHARs to CStrings
		GroupName = groupAttributes.myGroupName;
		GroupDesc = groupAttributes.myGroupDesc;
	 }
}
