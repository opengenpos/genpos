#pragma once

#include "StdAfx.h"

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

struct CDevicePersistantStorageHeader
{
public:
	static const ULONG   ulFlagAllowOverWrite = 0x00000001;
	static const ULONG   ulFlagStoreFullOverWriting = 0x10000000;

	ULONG  ulBlobSize;
	ULONG  ulBlobCountMax;
	ULONG  ulIndexBlobFirst;
	ULONG  ulIndexBlobLast;
	ULONG  ulCountOverwrittenBlobs;
	ULONG  ulOperationalFlags;
	TCHAR  tcsIdentBlobFile[24];
};

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

#define PERSISTANTSTOR_FILE_NOT_FOUND      PIF_ERROR_FILE_NOT_FOUND       

class CDevicePersistantStorage
{
public:
	virtual int InitFile (LPCTSTR pFileName, ULONG ulBlobCount, ULONG ulBlobSize) = 0;
	virtual int OpenFile (LPCTSTR pFileName) = 0;
	virtual int CloseFile (void) = 0;
	virtual int ReadHeader (CDevicePersistantStorageHeader *pHeader) = 0;
	virtual int WriteHeader (CDevicePersistantStorageHeader *pHeader) = 0;
	virtual int ReadFirstBlob (void *pBlobBuf, int iMaxBufSizeBytes) = 0;
	virtual int ReadNextBlob (void *pBlobBuff, int iMaxBufSizeBytes) = 0;
	virtual int ReadLastBlob (void *pBlobBuf, int iMaxBufSizeBytes) = 0;
	virtual int ReadPrevBlob (void *pBlobBuf, int iMaxBufSizeBytes) = 0;
	virtual int ReadCurrBlob (void *pBlobBuf, int iMaxBufSizeBytes) = 0;
	virtual int ReadIndexedBlob (void *pBlobBuf, int iMaxBufSizeBytes, ULONG ulBlobIndex) = 0;
	virtual int UpdateCurrBlob (void *pBlobBuf, int iNumBytes) = 0;
	virtual int DeleteCurrBlob (void *pBlobBuf = 0, int iNumBytes = 0) = 0;
	virtual int AppendLastBlob (void *pBlobBuf, int iNumBytes) = 0;
};

class CSignatureCaptureStorage : CDevicePersistantStorage
{
	friend class FormatReaderDialog_TOTALSIG;
public:
	CSignatureCaptureStorage ();
	~CSignatureCaptureStorage ();

	int InitFile (LPCTSTR pFileName, ULONG ulBlobCount, ULONG ulBlobSize);
	int OpenFile (LPCTSTR pFileName);
	int CloseFile (void);
	int ReadHeader (CDevicePersistantStorageHeader *pHeader);
	int WriteHeader (CDevicePersistantStorageHeader *pHeader);
	int ReadFirstBlob (void *pBlobBuf, int iMaxBufSizeBytes);
	int ReadNextBlob (void *pBlobBuff, int iMaxBufSizeBytes);
	int ReadLastBlob (void *pBlobBuf, int iMaxBufSizeBytes);
	int ReadPrevBlob (void *pBlobBuf, int iMaxBufSizeBytes);
	int ReadCurrBlob (void *pBlobBuf, int iMaxBufSizeBytes);
	int ReadIndexedBlob (void *pBlobBuf, int iMaxBufSizeBytes, ULONG ulBlobIndex = 0xFFFFFFFF);
	int UpdateCurrBlob (void *pBlobBuf, int iNumBytes);
	int DeleteCurrBlob (void *pBlobBuf = 0, int iNumBytes = 0);
	int AppendLastBlob (void *pBlobBuf, int iNumBytes);

	int OpenSignatureCaptureFile ();
	int InitSignatureCaptureFile (int iMaxBlobCount = 2000);

protected:
	CDevicePersistantStorageHeader m_Header;
	ULONG   m_IndexCurrentBlob;
	SHORT   m_hsFileHandle;
};