#pragma once

#include "StdAfx.h"
#include "Evs.h"
#include "DeviceEngine.h"

class DeviceEngineThread : public CWinThread {
private:
	CDeviceEngine deviceEngine;
public:
	DECLARE_DYNCREATE(DeviceEngineThread)

	DECLARE_MESSAGE_MAP()

	int ExitInstance();
	BOOL InitInstance();
	BOOL PreTranslateMessage(PMSG msg);

	void OnDeviceEngineIOCTL(WPARAM wParam, LPARAM lParam);
};

