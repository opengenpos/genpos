#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct {            
    UCHAR uchSTX;
    TCHAR auchTID[8];
    TCHAR auchACCode[3];
    TCHAR auchAccount[20];
    TCHAR auchExpireDate[4];
    TCHAR auchAmount[8];
    TCHAR auchTranType[3];
    TCHAR auchProduct[10];
    UCHAR uchETX;
    UCHAR uchLRC;
}MAINTEPT;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
==============================================================================
            D E F I N E    D E C L A R A T I O N s                        
==============================================================================
*/

#define EPT_RETRY_COUNT  5
#define EPT_BUFFER_SIZE  58

#define EPT_SUCCESS      0
#define EPT_FAIL        -1
#define EPT_DATA_ERR    -2

#define EPT_STX        0x02
#define EPT_ETX        0x03
#define EPT_EOT        0x04
#define EPT_ENQ        0x05
#define EPT_ACK        0x06
#define EPT_NAK        0x15

/*
==============================================================================
+                  S T A T I C   R A M s                                     +
==============================================================================
*/

extern MAINTEPT MaintEpt;

extern TCHAR  auchEptBuf[EPT_BUFFER_SIZE];
                                /* buffer */

extern CHAR   chEptPortID;      /* number of port connected ept */

extern SHORT  hsEptPort;        /* port handle */


SHORT   EptSendProcess(MAINTEPT *pData);
SHORT   EptOpenPort(PROTOCOL *pProt, USHORT usSendTimer);
SHORT   EptSendData(MAINTEPT *pData);
SHORT   EptReceiveData(VOID);
SHORT   EptWriteData(VOID *pData, USHORT usDataLen);
SHORT   EptReadCtlChar(UCHAR uchData);
SHORT   EptReadData(UCHAR uchData);
UCHAR   EptXor(VOID *pData, SHORT sLen);


