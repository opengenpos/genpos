/*************************************************************************
 *
 * Packet.cpp
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
 *
 ************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// Include header files
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"								// standard AFX header file
#include "Packet.h"								// my header file

/////////////////////////////////////////////////////////////////////////////
// Debugging control
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Local Definition
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Function Prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//						CPacket handlers								   //
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Constructor for CPacket class
//
// Prototype:	CPacket::CPacket();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

CPacket::CPacket()
{
	m_nCommand = 0;								// command code
	m_nStatus  = 0;								// execution status
	m_pvData   = NULL;							// ptr. to request data
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Constructor for CPacket class
//
// Prototype:	void		CPacket::CPacket(nCommand, pvData);
//
// Inputs:		UINT		nCommand;		- command code
//				PVOID		pvData;			- ptr. to a function data
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

CPacket::CPacket(UINT nCommand, PVOID pvData)
{
	m_nCommand = nCommand;							// command code
	m_nStatus  = 0;									// execution status
	m_pvData   = pvData;							// ptr. to request data
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Destructor for CPacket class
//
// Prototype:	CPacket::~Cpacket()
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

CPacket::~CPacket()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Post a packet message
//
// Prototype:	DWORD	CPacket::Post(nThreadID);
//
// Inputs:		DWORD	nThreadID;		- thread ID to post the packet
//
// Outputs:		DOWRD	dwStatus;		- function status
//
/////////////////////////////////////////////////////////////////////////////

DWORD	CPacket::Post(DWORD nThreadID)
{
	DWORD	dwStatus = 0;

	// post the message to the thread

	if (! ::PostThreadMessage(nThreadID, WM_PACKET, (WPARAM)(this), NULL))
	{
		dwStatus = ::GetLastError();
		ASSERT(FALSE);
	}

	// exit ...
	return dwStatus;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Wait for the packet returned
//
// Prototype:	VOID	CPacket::WaitForReturn(VOID);
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

VOID	CPacket::WaitForReturn(VOID)
{
		VERIFY(m_evtDone.Lock());
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Get a packet delivered
//
// Prototype:	CPacket*	CPacket::GetPacket(VOID);
//
// Inputs:		nothing
//
// Outputs:		CPacket*	pPacket;		- ptr. to a packet message
//
/////////////////////////////////////////////////////////////////////////////

CPacket* CPacket::GetPacket(VOID)
{
	BOOL		fFound;
	MSG			msg;
	CPacket *	pPacket;

	// initialize

    pPacket = NULL;                             // assume no message found

	// get a message

	while (fFound = ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
		// is packet message ?

		if (msg.message == WM_PACKET)
		{
			pPacket = (CPacket *)(msg.wParam);
			break;
		}

		// else, is timer notification ?

		else if (msg.message == WM_TIMER)
		{
			TRACE(_T("PeekMessage(WM_TIMER)\n"));
		}
	}

	// exit ...
	return pPacket;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	Dispose the packet message
//
// Prototype:	VOID		CPacket::Dispose(VOID);
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

VOID	CPacket::Dispose(VOID)
{
	VERIFY(m_evtDone.SetEvent());
	
}

/////////////////////////// End Of Program //////////////////////////////////
