
#if !defined(SCFPARAMETERS_H_INCLUDED)

#define SCFPARAMETERS_H_INCLUDED

#define EPT_DIALOG_KEY_MANAGMENT_MAX	2
#define EPT_DIALOG_MAG_READER_MAX		60
#define EPT_DIALOG_CHECK_READER_MAX		60
#define EPT_DIALOG_MAG_READER_MIN		5
#define EPT_DIALOG_CHECK_READER_MIN		5

#include <scf.h>

class CScfParameters {
public:
	CScfParameters (LPCTSTR aszCvsString = 0);
	~CScfParameters ();

	void Convert2CAPS (LPCTSTR aszCvsString);

	DWORD baud;
	DWORD parity;
	DWORD stopbits;
	DWORD charbits;
	DWORD flowcontrol;

	CString csBaud;
	CString csParity;
	CString csStopBits;
	CString csCharBits;
	CString csFlowControl;

	CString csIPAddr;
	CString csIPAddr2;
	CString csPortNo;
	CString	csIPAddr3;
	CString csIPAddr4;
};

#endif