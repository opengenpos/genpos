
#include "stdafx.h"

#include "ScfInterface.h"

CScfInterface::CScfInterface ()
{
}

CScfInterface::~CScfInterface ()
{
}

DWORD CScfInterface::PifGetParameter(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName,
                                LPDWORD lpDataType, LPVOID lpData, DWORD dwNumberOfBytesToRead,
                                LPDWORD lpNumberOfBytesRead)
{
    DWORD   dwResult;
    TCHAR   atchBuffer[SCF_MAX_DATA];
    BOOL    bConverted;

    UNREFERENCED_PARAMETER(dwNumberOfBytesToRead);

    bConverted = FALSE;

    dwResult = ScfGetParameter(
        lpszDeviceName,
        lpszDataName,
        lpDataType,
        (LPVOID)atchBuffer,
        sizeof(atchBuffer),
        lpNumberOfBytesRead);

    if (dwResult != SCF_SUCCESS)
    {
        return(dwResult);
    }

    if (*lpDataType == SCF_DATA_STRING)
    {
        bConverted = ScfConvertString2Bit(lpszDataName, (LPDWORD)lpData, atchBuffer);
    }

    if (bConverted)
    {
        *lpDataType = SCF_DATA_BINARY;
        *lpNumberOfBytesRead = sizeof(DWORD);
    }
    else
    {
        memcpy(lpData, (LPVOID)atchBuffer, *lpNumberOfBytesRead);
    }

    return (dwResult);
}



USHORT CScfInterface::PifGetLanConfig(TCHAR *pachDevice, UCHAR *ucIP, USHORT *usPort, USHORT *usStoreForward)
{

    DWORD   dwDataType, dwDummy;
    TCHAR   atchPort[SCF_USER_BUFFER];
	TCHAR   *pNxt = NULL;
	int     i = 0;

    if (PifGetParameter(    pachDevice,
                            _T("Port"),
                            &dwDataType,
                            (LPVOID)atchPort,
                            sizeof(atchPort),
                            &dwDummy) != SCF_SUCCESS)

    {
        return FALSE;
    }
    *usPort = _ttoi (atchPort);

    if (*usPort == 0) {

        return FALSE;
    }

    if (PifGetParameter(    pachDevice,
                            _T("IPaddr"),
                            &dwDataType,
                            (LPVOID)atchPort,
                            sizeof(atchPort),
                            &dwDummy) != SCF_SUCCESS)

    {
        return FALSE;
    }

	// convert the IP address to binary.
	// We assume it is in some kind of nnn.nnn.nnn.nnn format
	// first of all set to all binary zeros
	memset (ucIP, 0, 4);
	pNxt = atchPort - 1;
	for (i = 0; pNxt && i < 4; i++) {
		pNxt++;
		ucIP[i] = _ttoi (pNxt);
		pNxt = _tcschr (pNxt, _T('.'));
	}

    if (usStoreForward) {
		*usStoreForward = 0;    // default is to show the store and forward as disabled.
		if (PifGetParameter(pachDevice,
                            SCF_DATANAME_STOREFORWARD,
                            &dwDataType,
                            (LPVOID)atchPort,
                            sizeof(atchPort),
                            &dwDummy) == SCF_SUCCESS)
		{
		   *usStoreForward = _ttoi (atchPort);
		}
	}

    return TRUE;
}


USHORT CScfInterface::PifSetLanConfig(TCHAR *pachDevice, UCHAR *ucIP, USHORT usPort)
{

    DWORD   dwDummy;
    TCHAR   atchPort[SCF_USER_BUFFER];
	TCHAR   *pNxt = NULL;
	int     i = 0;

    if (usPort == 0) {

        return FALSE;
    }

	_itot (usPort, atchPort, 10);

    if (ScfSetParameter(pachDevice, _T("Port"), SCF_DATA_STRING,
                            (LPVOID)atchPort,
                            _tcslen (atchPort) * sizeof(TCHAR),
                            &dwDummy) != SCF_SUCCESS)
    {
        return FALSE;
    }


	wsprintf (atchPort, _T("%d.%d.%d.%d"), ucIP[0], ucIP[1], ucIP[2], ucIP[3]);
	
    if (ScfSetParameter(pachDevice, _T("IPaddr"), SCF_DATA_STRING,
                            (LPVOID)atchPort,
                            _tcslen (atchPort) * sizeof(TCHAR),
                            &dwDummy) != SCF_SUCCESS)
    {
        return FALSE;
    }

    return TRUE;
}

