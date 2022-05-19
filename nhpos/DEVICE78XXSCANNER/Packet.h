/*************************************************************************
 *
 * Packet.h
 *
 * $Workfile::
 *
 * $Revision::
 *
 * $Archive::
 *
 * $Date::
 *
 *************************************************************************
 *
 * Copyright (c) NCR Corporation 1998.  All rights reserved.
 *
 *************************************************************************
 *
 * $History::
 *
 ************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// constants definition
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// function interface definition
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPacket class definition
/////////////////////////////////////////////////////////////////////////////

class CPacket
{
private:
	CEvent		m_evtDone;							// signal to synchronize
	
public:
	UINT		m_nCommand;							// command code
	DWORD		m_nStatus;							// execution status
	PVOID		m_pvData;							// ptr. to function data
public:
	CPacket();										// constructor
	CPacket(UINT nCommand, PVOID pvData = NULL);	// constructor
	virtual ~CPacket();								// destructor
public:
	DWORD				Post(DWORD nThreadID);		// post the packet
	VOID				WaitForReturn(VOID);		// wait for returned
	static	CPacket*	GetPacket(VOID);			// get a packet
	VOID				Dispose(VOID);				// dispose the packet
protected:
	enum { WM_PACKET = WM_USER + 500 };
};

/////////////////////////////////////////////////////////////////////////////
