#pragma once

#include <windows.h>
#include <wincrypt.h>

#define CRYPTO_KEYCODE_HASH_VALUE  "12345678901234567890"

class CCrypto
{
public:
	CCrypto(void);
	virtual ~CCrypto(void);

	int     RflConstructEncryptByteStringClass (CHAR *szPassword);
	VOID    RflDestructEncryptByteStringClass (VOID);
	int     RflRehashEncryptDecrypt (CHAR *szPassword);
	UCHAR * RflEncryptByteString (UCHAR *bsString, int nByteCount, int nBuffLen);
	UCHAR * RflDecryptByteString (UCHAR *bsString, int nByteCount, int nBuffLen);
	UCHAR * RflEncryptSerialNoString (UCHAR *bsSerialNumber, UCHAR bsString[48]);
	UCHAR * RflDecryptSerialNoString (UCHAR *bsSerialNumber, UCHAR bsString[48]);

private:
	DWORD      m_dwLastError;
	HCRYPTPROV hCryptProv; 
	HCRYPTKEY  hKey; 
	HCRYPTKEY  hXchgKey; 
	HCRYPTHASH hHash; 

	PBYTE      pbKeyBlob; 
	DWORD      dwKeyBlobLen;

	UCHAR      ucPasswordSaveAndCheck[48];

};
