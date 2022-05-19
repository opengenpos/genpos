
#include <stdio.h>
#include <memory.h>

#include "DeviceSigCapStorage.h"
#include "ecr.h"
#include "pif.h"
#include "FrameworkIO.h"

static TCHAR CONST szTtlSignatureFileName[] = _T("TOTALSIG");   /* Signature storage File Name */

CSignatureCaptureStorage::CSignatureCaptureStorage () : m_hsFileHandle(-1), m_IndexCurrentBlob(0xFFFFFFFF)
{
	memset (&m_Header, 0, sizeof(m_Header));
}

CSignatureCaptureStorage::~CSignatureCaptureStorage ()
{
	if (m_hsFileHandle >= 0) {
		PifCloseFile (m_hsFileHandle);
		m_hsFileHandle = -1;
	}
}

int CSignatureCaptureStorage::InitFile (LPCTSTR pFileName, ULONG ulBlobCount, ULONG ulBlobSize)
{
	int     iRetStatus = 0;
	SHORT   sStatus = 0;

	sStatus = PifOpenFile (pFileName, "nw");
	if (sStatus < 0) {
		sStatus = PifOpenFile (pFileName, "ow");
	}

	if (sStatus < 0) {
		iRetStatus = sStatus;
	} else {
		CDevicePersistantStorageHeader  Header = {0};
		ULONG     ulActualPosition;

		Header.ulBlobCountMax = ulBlobCount;
		Header.ulBlobSize = ulBlobSize;
		Header.ulIndexBlobFirst = 0;
		Header.ulIndexBlobLast = 0;
		_tcscpy (Header.tcsIdentBlobFile, _T("SigCap 2.2.1"));

		PifSeekFile (sStatus, 0, &ulActualPosition);
		PifWriteFile (sStatus, &Header, sizeof(Header));
		PifCloseFile (sStatus);
	}

	return iRetStatus;
}

int CSignatureCaptureStorage::InitSignatureCaptureFile (int iMaxBlobCount)
{
	return InitFile (szTtlSignatureFileName, iMaxBlobCount, sizeof(FRAMEWORK_IO_SIGCAP_BLOB));
}

int CSignatureCaptureStorage::OpenSignatureCaptureFile ()
{
	int     iRetStatus = 0;

	iRetStatus = OpenFile (szTtlSignatureFileName);
	if (iRetStatus == PIF_ERROR_FILE_NOT_FOUND) {
		InitSignatureCaptureFile(2000);
		iRetStatus = OpenFile(szTtlSignatureFileName);
	}

	return iRetStatus;
}

int CSignatureCaptureStorage::OpenFile (LPCTSTR pFileName)
{
	int     iRetStatus = 0;

	if (m_hsFileHandle >= 0) {
		PifCloseFile (m_hsFileHandle);
		m_hsFileHandle = -1;
	}

	iRetStatus = m_hsFileHandle = PifOpenFile (pFileName, "owr");

	if (iRetStatus >= 0) {
		iRetStatus = 0;
		ReadHeader (&m_Header);
	}

	return iRetStatus;
}

int CSignatureCaptureStorage::CloseFile (void)
{
	int     iRetStatus = 0;

	if (m_hsFileHandle < 0) {
		iRetStatus = m_hsFileHandle;
	} else {
		WriteHeader (&m_Header);
		PifCloseFile (m_hsFileHandle);
		m_hsFileHandle = -1;
	}

	return iRetStatus;
}


int CSignatureCaptureStorage::ReadHeader (CDevicePersistantStorageHeader *pHeader)
{
	int     iRetStatus = 0;

	memset (pHeader, 0, sizeof(CDevicePersistantStorageHeader));

	if (m_hsFileHandle < 0) {
		iRetStatus = m_hsFileHandle;
	} else {
		ULONG     ulActualPosition;
		ULONG     ulBytesRead;

		PifSeekFile (m_hsFileHandle, 0, &ulActualPosition);
		PifReadFile (m_hsFileHandle, pHeader, sizeof(CDevicePersistantStorageHeader), &ulBytesRead);
	}
	return iRetStatus;
}

int CSignatureCaptureStorage::WriteHeader (CDevicePersistantStorageHeader *pHeader)
{
	int     iRetStatus = 0;

	if (m_hsFileHandle >= 0) {
		ULONG     ulActualPosition;

		PifSeekFile (m_hsFileHandle, 0, &ulActualPosition);
		PifWriteFile (m_hsFileHandle, pHeader, sizeof(CDevicePersistantStorageHeader));
	}
	return iRetStatus;
}

int CSignatureCaptureStorage::ReadFirstBlob (void *pBlobBuf, int iMaxBufSizeBytes)
{
	int     iRetStatus = -1;

	if (m_hsFileHandle >= 0) {
		ULONG     ulActualPosition;
		ULONG     ulPosition;
		ULONG     ulBytesRead;

		ReadHeader (&m_Header);

		if (m_Header.ulIndexBlobFirst != m_Header.ulIndexBlobLast) {
			// the file is not empty and has something in it.
			m_IndexCurrentBlob = m_Header.ulIndexBlobFirst;
			ulPosition = m_IndexCurrentBlob * m_Header.ulBlobSize + sizeof(m_Header);

			PifSeekFile (m_hsFileHandle, ulPosition, &ulActualPosition);
			PifReadFile (m_hsFileHandle, pBlobBuf, iMaxBufSizeBytes, &ulBytesRead);
			iRetStatus = 0;
		} else {
			m_IndexCurrentBlob = 0xFFFFFFFF;
		}
	}
	return iRetStatus;
}

int CSignatureCaptureStorage::ReadNextBlob (void *pBlobBuf, int iMaxBufSizeBytes)
{
	int     iRetStatus = -1;

	if (m_IndexCurrentBlob < 0xFFFFFFFF) {
		m_IndexCurrentBlob++;   // increment to the next blob

		if (m_IndexCurrentBlob >= m_Header.ulBlobCountMax)
			m_IndexCurrentBlob = 0;

		if (m_IndexCurrentBlob == m_Header.ulIndexBlobLast) {
			// we have reached the end of the list of blobs in the file
			m_IndexCurrentBlob = 0xFFFFFFFF;
		} else {
			ULONG     ulActualPosition;
			ULONG     ulPosition;
			ULONG     ulBytesRead;

			ulPosition = m_IndexCurrentBlob * m_Header.ulBlobSize + sizeof(m_Header);

			PifSeekFile (m_hsFileHandle, ulPosition, &ulActualPosition);
			PifReadFile (m_hsFileHandle, pBlobBuf, iMaxBufSizeBytes, &ulBytesRead);
			iRetStatus = 0;
		}
	}
	return iRetStatus;
}

int CSignatureCaptureStorage::ReadCurrBlob (void *pBlobBuf, int iMaxBufSizeBytes)
{
	int     iRetStatus = -1;

	if (m_IndexCurrentBlob < 0xFFFFFFFF) {
		if (m_IndexCurrentBlob == m_Header.ulIndexBlobLast) {
			// we have reached the end of the list of blobs in the file
		} else {
			ULONG     ulActualPosition;
			ULONG     ulPosition;
			ULONG     ulBytesRead;

			ulPosition = m_IndexCurrentBlob * m_Header.ulBlobSize + sizeof(m_Header);

			PifSeekFile (m_hsFileHandle, ulPosition, &ulActualPosition);
			PifReadFile (m_hsFileHandle, pBlobBuf, iMaxBufSizeBytes, &ulBytesRead);
			iRetStatus = 0;
		}
	}
	return iRetStatus;
}

int CSignatureCaptureStorage::ReadIndexedBlob (void *pBlobBuf, int iMaxBufSizeBytes, ULONG ulBlobIndex)
{
	int     iRetStatus = -1;

	if (ulBlobIndex < 0xFFFFFFFF) {
		do {
			if (m_Header.ulIndexBlobFirst == m_Header.ulIndexBlobLast) {
				// file is empty and there is nothing to read so just stop
				break;
			} else if (m_Header.ulIndexBlobFirst < m_Header.ulIndexBlobLast) {
				// we have not wrapped around in the file so this is pretty normal test and read
				if (ulBlobIndex < m_Header.ulIndexBlobFirst)
					break;   // index is out of range so forget it
				if (ulBlobIndex >= m_Header.ulIndexBlobLast)
					break;   // index is out of range so forget it
			} else if (m_Header.ulIndexBlobFirst > m_Header.ulIndexBlobLast) {
				// we have wrapped in the file such that the index to the last is actually lower than index to first
				if (ulBlobIndex < m_Header.ulIndexBlobFirst && ulBlobIndex >= m_Header.ulIndexBlobLast)
					break;   // index is out of range so forget it
				if (ulBlobIndex >= m_Header.ulBlobCountMax)
					break;   // index is out of range so forget it
			}
			{
				ULONG     ulActualPosition;
				ULONG     ulPosition;
				ULONG     ulBytesRead;

				ulPosition = ulBlobIndex * m_Header.ulBlobSize + sizeof(m_Header);

				PifSeekFile (m_hsFileHandle, ulPosition, &ulActualPosition);
				PifReadFile (m_hsFileHandle, pBlobBuf, iMaxBufSizeBytes, &ulBytesRead);
				iRetStatus = 0;
			}
		} while (0);  // we only want to do this convience loop once.  This construct allows us to use breaks in the decision tree code
	}
	return iRetStatus;
}


int CSignatureCaptureStorage::DeleteCurrBlob (void *pBlobBuf, int iMaxBufSizeBytes)
{
	int     iRetStatus = -1;

	if (m_IndexCurrentBlob < 0xFFFFFFFF) {
		if (m_Header.ulIndexBlobFirst != m_Header.ulIndexBlobLast) {
			// the file is not empty so there is something to read and something to delete
			if (pBlobBuf) {
				ReadCurrBlob (pBlobBuf, iMaxBufSizeBytes);
			}

			if (m_IndexCurrentBlob == m_Header.ulIndexBlobFirst) {
				m_IndexCurrentBlob++;
				m_IndexCurrentBlob %= m_Header.ulBlobCountMax;
				m_Header.ulIndexBlobFirst = m_IndexCurrentBlob;
				WriteHeader (&m_Header);
				iRetStatus = 0;
			} else if (m_IndexCurrentBlob == m_Header.ulIndexBlobLast) {
				if (m_IndexCurrentBlob == 0)
					m_IndexCurrentBlob = m_Header.ulBlobCountMax + 1;
				m_IndexCurrentBlob--;
				m_Header.ulIndexBlobLast = m_IndexCurrentBlob;
				WriteHeader (&m_Header);
				iRetStatus = 0;
			}
		}
	}
	return iRetStatus;
}
int CSignatureCaptureStorage::ReadLastBlob (void *pBlobBuf, int iMaxBufSizeBytes)
{
	int     iRetStatus = -1;

	if (m_hsFileHandle >= 0) {
		ULONG     ulActualPosition;
		ULONG     ulPosition;
		ULONG     ulBytesRead;

		if (m_Header.ulIndexBlobFirst != m_Header.ulIndexBlobLast) {
			// the file is not empty and has something in it.
			m_IndexCurrentBlob = m_Header.ulIndexBlobLast;
			if (m_IndexCurrentBlob == 0)
				m_IndexCurrentBlob = m_Header.ulBlobCountMax + 1;

			m_IndexCurrentBlob--;

			ulPosition = m_IndexCurrentBlob * m_Header.ulBlobSize + sizeof(m_Header);

			PifSeekFile (m_hsFileHandle, ulPosition, &ulActualPosition);
			PifReadFile (m_hsFileHandle, pBlobBuf, iMaxBufSizeBytes, &ulBytesRead);
			iRetStatus = 0;
		} else {
			m_IndexCurrentBlob = 0xFFFFFFFF;
		}
	}

	return iRetStatus;
}

int CSignatureCaptureStorage::ReadPrevBlob (void *pBlobBuf, int iMaxBufSizeBytes)
{
	int     iRetStatus = -1;

	if (m_IndexCurrentBlob < 0xFFFFFFFF) {
		if (m_IndexCurrentBlob == m_Header.ulIndexBlobFirst) {
			// we have reached the end of the list of blobs in the file
			m_IndexCurrentBlob = 0xFFFFFFFF;
			return -1;
		}

		if (m_hsFileHandle >= 0) {
			ULONG     ulActualPosition;
			ULONG     ulPosition;
			ULONG     ulBytesRead;

			if (m_IndexCurrentBlob == 0)
				m_IndexCurrentBlob = m_Header.ulBlobCountMax;

			m_IndexCurrentBlob--;   // decrement to the previous blob

			ulPosition = m_IndexCurrentBlob * m_Header.ulBlobSize + sizeof(m_Header);

			PifSeekFile (m_hsFileHandle, ulPosition, &ulActualPosition);
			PifReadFile (m_hsFileHandle, pBlobBuf, iMaxBufSizeBytes, &ulBytesRead);
		}
	}
	return iRetStatus;
}

int CSignatureCaptureStorage::UpdateCurrBlob (void *pBlobBuf, int iNumBytes)
{
	int     iRetStatus = -1;
	ULONG     ulActualPosition;
	ULONG     ulPosition;

	if (m_hsFileHandle >= 0 && m_IndexCurrentBlob < 0xFFFFFFFF) {
		ulPosition = m_IndexCurrentBlob * m_Header.ulBlobSize + sizeof(m_Header);

		PifSeekFile (m_hsFileHandle, ulPosition, &ulActualPosition);
		PifWriteFile (m_hsFileHandle, pBlobBuf, iNumBytes);
		iRetStatus = 0;
	}

	return iRetStatus;
}

int CSignatureCaptureStorage::AppendLastBlob (void *pBlobBuf, int iNumBytes)
{
	int     iRetStatus = -1;

	if (m_hsFileHandle >= 0) {
		ULONG     ulActualPosition;
		ULONG     ulPosition;

		iRetStatus = 0;
		if (iNumBytes > m_Header.ulBlobSize) {
			iNumBytes = m_Header.ulBlobSize;
			iRetStatus = 1;
		}

		m_IndexCurrentBlob = m_Header.ulIndexBlobLast;
		ulPosition = m_IndexCurrentBlob * m_Header.ulBlobSize + sizeof(m_Header);

		m_Header.ulIndexBlobLast++;
		m_Header.ulIndexBlobLast %= m_Header.ulBlobCountMax;

		if (m_Header.ulIndexBlobLast == m_Header.ulIndexBlobFirst) {
			// file is full so we need to indicate we are full and
			// to start increment over write count.
			m_Header.ulOperationalFlags |= m_Header.ulFlagStoreFullOverWriting;

			m_Header.ulCountOverwrittenBlobs++;

			m_Header.ulIndexBlobFirst++;
			m_Header.ulIndexBlobFirst %= m_Header.ulBlobCountMax;
		}
		if (m_Header.ulCountOverwrittenBlobs == 1) {
			char  xBuff[128];
			sprintf(xBuff, "Blob OverWritten = %d, first %d, last %d", m_Header.ulCountOverwrittenBlobs, m_Header.ulIndexBlobFirst, m_Header.ulIndexBlobLast);
			NHPOS_NONASSERT_TEXT(xBuff);
		}

		PifSeekFile (m_hsFileHandle, ulPosition, &ulActualPosition);
		PifWriteFile (m_hsFileHandle, pBlobBuf, iNumBytes);
		WriteHeader (&m_Header);
	}
	return iRetStatus;
}