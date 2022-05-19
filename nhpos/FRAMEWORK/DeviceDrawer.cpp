/*************************************************************************
 *
 * DeviceDrawer.cpp
 *
 * $Workfile:: DeviceDrawer.cpp                                          $
 *
 * $Revision::                                                           $
 *
 * $Archive::                                                            $
 *
 * $Date::                                                               $
 *
 *************************************************************************
 *
 * Copyright (c) NCR Corporation 2000.  All rights reserved.
 *
 *************************************************************************
 *
 * $History:: DeviceDrawer.cpp                                           $
 *
 ************************************************************************/

#include "stdafx.h"
#include "Framework.h"
#include "DeviceIOCashDrawer.h"
#include "DeviceEngine.h"
#include "Device.h"
#include "DeviceDrawer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// Local Definition
/////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:    Construction
//
// Prototype:   VOID    CDeviceDrawer::CDeviceDrawer();
//
// Inputs:      nothing
//
// Outputs:     nothing
//
/////////////////////////////////////////////////////////////////////////////

CDeviceDrawer::CDeviceDrawer(CDeviceEngine* pEngine)
    : CDevice(pEngine)
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    Construction
//
// Prototype:   VOID    CDeviceDrawer::~CDeviceDrawer();
//
// Inputs:      nothing
//
// Outputs:     nothing
//
/////////////////////////////////////////////////////////////////////////////

CDeviceDrawer::~CDeviceDrawer()
{
    Close();
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    Open a device
//
// Prototype:   BOOL    CDeviceDrawer::Open(lpszDeviceName, lpszDllName);
//
// Inputs:      LPCTSTR lpszDeviceName; -
//              LPCTSTR lpszDllName;    -
//
// Outputs:     BOOL    bResult;        -
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceDrawer::Open(LPCTSTR lpszDeviceName, LPCTSTR lpszDllName)
{
    BOOL    bResult;

    bResult = CDevice::Open(lpszDeviceName, lpszDllName);

    // examine status

    if (! bResult)
    {
        return FALSE;                       // exit ...
    }

    // create event signals (auto reset & non-signaled initially)

    m_hDone = ::CreateEvent(NULL, FALSE, FALSE, NULL);

    // create a device thread

    m_pThread = AfxBeginThread(DoDeviceThread, this);

    // set his priority higher a little bit

    m_pThread->SetThreadPriority(THREAD_PRIORITY_HIGHEST);

    // do not delete automatically, I'll take care !

    m_pThread->m_bAutoDelete = FALSE;

    // wait for a done signal

    ::WaitForSingleObject(m_hDone, INFINITE);
    ::ResetEvent(m_hDone);

    // exit ...

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    Close the device
//
// Prototype:   BOOL    CDeviceDrawer::Close();
//
// Inputs:      nothing
//
// Outputs:     BOOL    bResult;        -
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceDrawer::Close()
{
    DWORD   dwResult;
    DWORD   dwMilliseconds;

    // not open ?

    if (! m_bOpened)
    {
        return FALSE;                       // exit ...
    }

    // mark as end of the thread

    m_bDoMyJob = FALSE;

    // close the device

    dwResult = m_pfnClose(m_hDevice);

    // examine status

    if (dwResult != DEVICEIO_SUCCESS)
    {
        return FALSE;                       // exit ...
    }

    // wait the thread to the end

#if !defined(_WIN32_WCE_EMULATION)
    dwMilliseconds = INFINITE;
#else
    // The thread terminate event does not notify correctly
    // on Windows CE 2.0 emulation environment.
    // Change the time-out interval value.
    dwMilliseconds = 100;
#endif
    ::WaitForSingleObject(m_hDone, INFINITE);
    ::WaitForSingleObject(m_pThread->m_hThread, dwMilliseconds);

    ::CloseHandle(m_hDone);

    // unload the device DLL

    ::FreeLibrary(m_hDll);

    // mark as closed

    m_bOpened = FALSE;

    // exit ...

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   BOOL    CDeviceDrawer::Enable();
//
// Inputs:      nothing
//
// Outputs:     BOOL    bResult;        -
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceDrawer::Enable(BOOL bState)
{
    // not open ?

    if (! m_bOpened)
    {
        return FALSE;                       // exit ...
    }

    // exit ...

    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   BOOL    CDeviceDrawer::OpenDrawer();
//
// Inputs:      nothing
//
// Outputs:     BOOL    bResult;        -
//
// Note: 2000.4.18. Added code for update status
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceDrawer::OpenDrawer(DWORD dwNumber)
{
    DWORD                   dwResult;
    DWORD                   dwBytesRead;
    DEVICEIO_DRAWER_OPEN    Drawer;

    // not open ?

    if (! m_bOpened)
    {
        return FALSE;                       // exit ...
    }

    // make up parameter

    Drawer.dwNumber = dwNumber;

    // invoke 

    dwResult = m_pfnIoControl(
                    m_hDevice,
                    DEVICEIO_CTL_DRAWER_OPEN,
                    &Drawer,
                    sizeof(Drawer),
                    NULL,
                    0,
                    &dwBytesRead);

    // examine status

    if (dwResult != DEVICEIO_SUCCESS)
    {
        return FALSE;                       // exit ...
    }

    // Update status

    if (dwNumber == 1)
    {
        m_dsPrevious.dwDrawerA = DEVICEIO_DRAWER_BEING_OPENED;
    }
    else if (dwNumber == 2)
    {
        m_dsPrevious.dwDrawerB = DEVICEIO_DRAWER_BEING_OPENED;
    }

    // exit ...

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   BOOL    CDeviceDrawer::Status();
//
// Inputs:      nothing
//
// Outputs:     BOOL    bResult;        -
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceDrawer::GetStatus(PDEVICEIO_DRAWER_STATUS pStatus)
{
    // not open ?

    if (! m_bOpened)
    {
        return FALSE;                       // exit ...
    }

    // make up parameter

    pStatus->dwDrawerA = m_dsPrevious.dwDrawerA;
    pStatus->dwDrawerB = m_dsPrevious.dwDrawerB;

    // exit ...

    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// 

/////////////////////////////////////////////////////////////////////////////
//
// Function:    Entry point of device thread. (static function)
//
// Prototype:   UINT        CDeviceDrawer::DoDeviceThread(lpvData);
//
// Inputs:      LPVOID      lpvData;
//
// Outputs:     UINT        nResult;    - status
//
/////////////////////////////////////////////////////////////////////////////

UINT CDeviceDrawer::DoDeviceThread(LPVOID lpvData)
{
    return ((CDeviceDrawer*)lpvData)->DeviceThread();
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    Input Device Engine thread body
//
// Prototype:   UINT        CDeviceDrawer::DeviceThread();
//
// Inputs:      nothing
//
// Outputs:     UINT        nResult;    - status
//
/////////////////////////////////////////////////////////////////////////////

UINT CDeviceDrawer::DeviceThread()
{
    BOOL                    bResult;
    DWORD                   dwResult;
    DWORD                   dwBytesRead;
    DWORD                   dwWait;

    // initialize

    m_bDoMyJob             = TRUE;
    dwWait                 = 300;           // msec
    m_dsPrevious.dwDrawerA = (DWORD)(-1);   // assume unknown number
    m_dsPrevious.dwDrawerB = (DWORD)(-1);   // assume unknown number

    // issue a signal to become ready

    ::SetEvent(m_hDone);                    // I'm ready to go !

    // do my job

    while (m_bDoMyJob)                      // do my job forever
    {
        memset(&m_dsCurrent, '\0', sizeof(m_dsCurrent));

        dwResult = m_pfnIoControl(
                        m_hDevice,
                        DEVICEIO_CTL_DRAWER_GET_STATUS,
                        NULL,
                        0,
                        &m_dsCurrent,
                        sizeof(m_dsCurrent),
                        &dwBytesRead);

        // is changed a keylock position ?

        if (dwResult == DEVICEIO_SUCCESS &&
            (m_dsCurrent.dwDrawerA != m_dsPrevious.dwDrawerA) )
			
        {
            bResult = m_pEngine->AddData(
                        SCF_TYPE_DRAWER1,
                        &m_dsCurrent,
                        dwBytesRead);

            m_dsPrevious = m_dsCurrent;
        }

		
        if (dwResult == DEVICEIO_SUCCESS &&
            (m_dsCurrent.dwDrawerB != m_dsPrevious.dwDrawerB ))
			
        {
            bResult = m_pEngine->AddData(
                        SCF_TYPE_DRAWER2,
                        &m_dsCurrent,
                        dwBytesRead);

            m_dsPrevious = m_dsCurrent;
        }
        ::Sleep(dwWait);
    }

    // issue a signal to becom end of thread

    ::SetEvent(m_hDone);

    // exit ...

    return 0;
}
