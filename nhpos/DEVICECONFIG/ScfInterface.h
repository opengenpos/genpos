
#if !defined(SCFINTERFACE_H_INCLUDED)
#define SCFINTERFACE_H_INCLUDED

#include <scf.h>

class CScfInterface {
public:
	CScfInterface ();
	~CScfInterface ();

	USHORT PifGetLanConfig(TCHAR *pachDevice, UCHAR *ucIP, USHORT *usPort, USHORT *usStoreForward);
	DWORD PifGetParameter(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName,
                                LPDWORD lpDataType, LPVOID lpData, DWORD dwNumberOfBytesToRead,
                                LPDWORD lpNumberOfBytesRead);

	USHORT PifSetLanConfig(TCHAR *pachDevice, UCHAR *ucIP, USHORT usPort);

	CString csDeviceName;
	CString csDeviceDLL;
	CString csDeviceActiveType;
	CString csDeviceInterface;
	CString csDeviceSettings;
	CString csDeviceOptions;
};

#endif