#include "DeviceEngineThread.h"

IMPLEMENT_DYNCREATE(DeviceEngineThread,CWinThread)

	BEGIN_MESSAGE_MAP(DeviceEngineThread, CWinThread)
		ON_THREAD_MESSAGE(WM_APP_DEVICEENGINE_IOCTL,OnDeviceEngineIOCTL)
	END_MESSAGE_MAP()

BOOL DeviceEngineThread::InitInstance(){
	deviceEngine.Open();

#ifdef DEVENG_REPLACEMENT
	deviceEngine.m_myAppForMessages = this;
#endif
	
	return TRUE;
}

int DeviceEngineThread::ExitInstance(){
	deviceEngine.Close();
	return CWinThread::ExitInstance();
}

BOOL DeviceEngineThread::PreTranslateMessage(PMSG msg){
	if(deviceEngine.PreTranslateMessage(msg))
		return TRUE;
	return CWinThread::PreTranslateMessage(msg);
}

void DeviceEngineThread::OnDeviceEngineIOCTL(WPARAM wParam, LPARAM lParam){
	deviceEngine.HandleIOCTL(wParam,lParam);
}