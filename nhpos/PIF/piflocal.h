/* PifLocal.h */
#include <winsock2.h>

#define PIF_MAX_TASK 64+1
#define PIF_MAX_SEM 128

/* #define PIF_MAX_FILE 64 */
#define PIF_MAX_SIO  (PIF_LEN_PORT * 2)   // max number of serial ports. number of SysConfig.auchComPort[] entries doubled for USB Virtual Serial ports
#define PIF_MAX_NET  96

//#define PIF_MAX_HANDLE PIF_MAX_SIO
/* #define PIF_MAX_HANDLE PIF_MAX_FILE+PIF_MAX_SIO*/

#define PIF_TASK_LEN 8
#define PIF_DEVICE_LEN  64
#define PIF_ERROR_HANDLE_OVERFLOW   -1

#define PIF_WM_USER_SEM 0x1000
#define PIF_WM_USER_MSG 0x1001

/* used with Open Communication */
#define PC_PORT_FLAG        0x100
#define KITCHEN_PORT_FLAG   0x400
#define XON_OFF_PORT_FLAG   KITCHEN_PORT_FLAG
#define SLIP_PORT_FLAG      0x800

#define    HOSTSKEY  TEXT("Comm\\Tcpip\\Hosts")
#define    IPADDRESS TEXT("ipaddr")
#define    ALIASES  TEXT("aliases")
#define    IDENT    TEXT("Ident")
#define    NAME     TEXT("Name")

#define    SYSTEMID_BUILD TEXT("D570-0725-0000 V%d.%02d.%02d (Build %03d)")
#define    SYSTEMID       TEXT(" D570-0725-0000 V%d.%02d.%02d")

#define    VERSION_FILE   TEXT("version.txt")
#define    LOG_FILE       TEXT("PIFLOG")
#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

typedef struct {
	ULONG   ulSignature;     // bit pattern to detect if table entry is still good or not
    SHORT   sPifHandle;      // -1 indicates unused, -2 indicates reserved, otherwise PIF resource handle.
    HANDLE  hWin32Handle;    // the Windows API handle to the Windows resource.
    SOCKET  hWin32Socket;
    BOOL    fCompulsoryReset;
    TCHAR   wszDeviceName[PIF_DEVICE_LEN+1];
    CHAR    chMode;
    USHORT  fsMode;
    SHORT   sCount;
    HANDLE  hMutexHandle;
    XGHEADER xgHeader;
    SHORT   sNetHandle;
    ULONG   ulFilePointer;
} PIFHANDLETABLE;

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

extern CRITICAL_SECTION g_HandleCriticalSection;

//extern PIFHANDLETABLE   aPifHandleTable[PIF_MAX_HANDLE];

extern const TCHAR wszFilePathFlashDisk[];
extern const TCHAR wszFilePathDataBase[];
extern const TCHAR wszFilePathProgram[];

extern SYSCONFIG    SysConfig;
extern SHORT        fsPifDuringPowerDown;

VOID PifInitialize(VOID);
VOID PifTableInitialize(VOID);
VOID PifSubInitAbortSem(VOID);
VOID PifSubHandleTableInit(PIFHANDLETABLE *pPifHandleTable, USHORT usMaxNumber);
USHORT PifSubGetNewId(PIFHANDLETABLE *pPifHandleTable, USHORT usMaxNumber);
USHORT PifSubGetNewIdReserve(PIFHANDLETABLE *pPifHandleTable, USHORT usMaxNumber);
VOID PifSubGetNewIdReserveClear(USHORT usHandle, PIFHANDLETABLE *pPifHandleTable, USHORT usMaxNumber);
VOID PifSnapShotFileTable (TCHAR *pSnapFileName, CHAR *pcSourcePath, int iLineNo, CHAR *aszHeader);
VOID PifSnapShotFileTableFH (SHORT  shFile);
VOID PifSnapShotCommTable (TCHAR *pSnapFileName, CHAR *pcSourcePath, int iLineNo, CHAR *aszHeader);
VOID PifSnapShotCommTableFH (SHORT  shFile);
VOID PifSnapShotSemaphoreTable (SHORT  shSnap);
SHORT PifSubSearchId(USHORT usHandle, PIFHANDLETABLE *pPifHandleTable, USHORT usMaxNumber);
VOID PifSemInitialize();
VOID PifQueueInitialize();
VOID PifTaskTableInit(VOID);
USHORT PifCheckAndCreateDirectory(VOID);
SHORT PifNetStartup(VOID);
VOID PifFinalize(VOID);
VOID PifNetFinalize(VOID);
VOID PifCloseHandle(USHORT usPowerDown);
VOID PifNetCloseHandle(USHORT usPowerDown);
VOID PifDeleteSemAll(VOID);
USHORT PifCheckPowerDown(USHORT usHandle, PIFHANDLETABLE *pPifHandleTable);
/*USHORT PifCheckPowerDown(USHORT usHandle);*/
VOID PifSetDiskNamePath(VOID);

UCHAR  PifSimGetKeyLock(VOID);
SHORT  PifSimGetWaiter(VOID);
USHORT PifSimOpenDrawer(USHORT usDrawerId);
USHORT PifSimDrawerStatus(USHORT usDrawerId);
SHORT  PifSimOpenMsr(VOID);
SHORT  PifSimReadMsr(MSR_BUFFER FAR *pMsrBuffer);
VOID   PifSimCloseMsr(VOID);
VOID   PifSimBeep(USHORT usMsec);
VOID   PifSimPlayStanza(CONST STANZA FAR *pStanza);
VOID   PifSimToneVolume(USHORT usVolume);
VOID   PifSimPlayStanza2(USHORT usFile, USHORT usLoop);

VOID   PifSimDisplayString(USHORT usDispId, USHORT usRow,
                                 USHORT usColumn, CONST UCHAR FAR *puchString,
                                 USHORT usChars, UCHAR uchAttr);
VOID   PifSimDisplayAttr(USHORT usDispId, USHORT usRow, USHORT usColumn,
                               CONST UCHAR FAR *puchString, USHORT usChars);
VOID   PifSimLightDescr(USHORT usDispId, USHORT usDescrId, UCHAR uchAttr);

VOID PifStartRecoveryThread(VOID);
VOID PifStopRecoveryThread(VOID);
VOID THREADENTRY RecoveryThread(VOID);
VOID PifSetSysConfig(VOID);

VOID PifOpenSpeaker(VOID);
VOID PifCloseSpeaker(VOID);
VOID PifReopenSpeaker(VOID);
VOID PifReopenFile(VOID);
VOID PifCloseFileHandle(USHORT usPowerDown);
VOID PifCloseFileHandleExec(USHORT usPowerDown);
VOID PifCloseSioHandle(USHORT usPowerDown);
SHORT PifCheckInitialReset();
VOID   PifSaveRegistry(VOID);
VOID PifDeleteChkFile(VOID);    /* delete *.chk files (01-24-2001) */

SHORT  PifControlComInternal(USHORT usPort, USHORT usFunc, ULONG  *pulArg);
SHORT  PifNetControlInternal (USHORT usNet, USHORT usFunc, ULONG  *pulArg);
