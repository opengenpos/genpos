
#include "stdafx.h"

#include "ScfParameters.h"


CScfParameters::CScfParameters (LPCTSTR aszCvsString)
{
	if (aszCvsString) {
		Convert2CAPS (aszCvsString);
	}
	else {
		baud = SCF_CAPS_BAUD_9600;
		parity = SCF_CAPS_PARITY_NONE;
		charbits = SCF_CAPS_CHAR_8;
		stopbits = SCF_CAPS_STOP_1;
		flowcontrol = SCF_CAPS_HANDSHAKE_NONE;
	}
}

CScfParameters::~CScfParameters ()
{
}

void CScfParameters::Convert2CAPS (LPCTSTR aszCvsString)
{
		CString csTemp (aszCvsString);
		CString csTemp2;
		int nCount, nCountP,nCountD;


		csTemp.TrimLeft ();
		csTemp.TrimRight ();

		nCount = csTemp.Find (_T(","));
		nCountP = csTemp.Find (_T("."));
		if (nCount < 0)
			return;

		// We first check to see if the first item is a standard baud rate or is an IP address
		// If standard baud rate, then convert as if it is a COMM device.
		// If its an IP address, then convert as if it is an Ethernet device
		if (nCountP < 0 || nCountP > nCount) {
			csBaud = csTemp.Left (nCount);
			csTemp.Delete (0, nCount);
			csTemp.TrimLeft (_T(','));
			csTemp.TrimLeft ();
			ScfConvertString2Bit(SCF_DATANAME_BAUD, &baud, csBaud);

			nCount = csTemp.Find (_T(","));
			ASSERT(nCount >= 0);
			if (nCount < 0)
				return;
			csCharBits = csTemp.Left (nCount);
			csTemp.Delete (0, nCount);
			csTemp.TrimLeft (_T(','));
			csTemp.TrimLeft ();
			ScfConvertString2Bit(SCF_DATANAME_CHARBIT, &charbits, csCharBits);

			nCount = csTemp.Find (_T(","));
			ASSERT(nCount >= 0);
			if (nCount < 0)
				return;
			csParity = csTemp.Left (nCount);
			csTemp.Delete (0, nCount);
			csTemp.TrimLeft (_T(','));
			csTemp.TrimLeft ();
			ScfConvertString2Bit(SCF_DATANAME_PARITY, &parity, csParity);

			nCount = csTemp.Find (_T(","));
			ASSERT(nCount >= 0);
			if (nCount < 0)
				return;
			csStopBits = csTemp.Left (nCount);
			csTemp.Delete (0, nCount);
			csTemp.TrimLeft (_T(','));
			csTemp.TrimLeft ();
			ScfConvertString2Bit(SCF_DATANAME_STOPBIT, &stopbits, csStopBits);

			csFlowControl = csTemp;
			ScfConvertString2Bit(SCF_DATANAME_HANDSHAKE, &flowcontrol, csFlowControl);
		}
		else if (nCountP >= 0) {
			csIPAddr = csTemp.Left (nCount);
			csTemp.Delete (0, nCount);
			if( !((csTemp.GetAt(0) == ',') && (csTemp.GetAt(1) == ',')))
			{
				csTemp.TrimLeft (_T(','));
				csTemp.TrimLeft ();
			}
			
			//try to get the next ip address if it exist
			nCount = csTemp.Find (_T(","));
			nCountD = csTemp.Find (_T("."));
			//if exist, add it to IP address 2
			if (nCount > 0 && nCount > nCountD)
			{
				csIPAddr2 = csTemp.Left (nCount);
				csTemp.Delete (0, nCount);
			}

			//Port Number
			if((csTemp.GetAt(0) == ',') && (csTemp.GetAt(1) == ','))
			{
				csTemp.TrimLeft (_T(','));
				csTemp.TrimLeft ();
			}
			else
			{
				csTemp.TrimLeft (_T(','));
				csTemp.TrimLeft ();
				nCount = csTemp.Find (_T(","));
				if(nCount<0)

				{
					csPortNo = csTemp;
				}
				else{
					csPortNo = csTemp.Left (nCount);
					csTemp.Delete (0, nCount);
					csTemp.TrimLeft (_T(','));
					csTemp.TrimLeft ();
				}
			}
			
			
			nCount = csTemp.Find (_T(","));
			csIPAddr3 = csTemp.Left (nCount);
			csTemp.Delete (0, nCount);
			csTemp.TrimLeft (_T(','));
			csTemp.TrimLeft ();

			csIPAddr4 = csTemp.Left (nCount);

		}
}