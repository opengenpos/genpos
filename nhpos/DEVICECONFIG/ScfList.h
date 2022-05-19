
#if !defined(SCFLIST_H_INCLUDED)
#define SCFLIST_H_INCLUDED

#include "ScfInterface.h"

class CScfList {
public:
	CScfList ();
	~CScfList ();

	CList <CScfInterface *, CScfInterface *> myScfList;

typedef CMap < CString, LPCTSTR, CArray <CString> *, CArray <CString> * > DevListMap;

};


#endif