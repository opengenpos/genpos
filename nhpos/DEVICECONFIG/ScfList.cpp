
#include "stdafx.h"

#include "ScfList.h"


CScfList::CScfList ()
{
}

CScfList::~CScfList ()
{
	CScfInterface *myScf;
	while (! myScfList.IsEmpty ()) {
		myScf = myScfList.RemoveHead ();
		if (myScf) 
			delete myScf;
	}
}