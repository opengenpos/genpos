/* pifpower.c : Defines the entry point for the DLL application. */

#pragma warning( disable : 4201 4214 4514)
#include "stdafx.h"
#include <winioctl.h>

#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "scf.h"
#include "ncrpower.h"
#include "ncrklock.h"
#include "piflocal.h"
// #include "notify.h"
#include "evs.h"

#define POWERLOG    /* for power down debug */
#define SUSPEND  /* for actual suspend process */


VOID THREADENTRY RecoveryThread(VOID);
DWORD GetKeyLock(VOID);
HANDLE PifOpenRecoveryDriver(VOID);

VOID PifInitPowerSem(VOID);
VOID PifSendPowerEvent(USHORT usEvent);
VOID PifWaitForPowerAck(VOID);

UCHAR  FARCONST aszRecoveryThreadName[] = "RECOVERY";

#define PIF_MAX_POWER 8

USHORT              usPifPowerEvent;
PifSemHandle        sPifPowerSem = PIF_SEM_INVALID_HANDLE;
SHORT               fsPifPower = 0;
CRITICAL_SECTION    g_PowerCriticalSection;
SHORT               fsPifDuringPowerDown;
PifSemHandle        sPifPowerSwitchSem = PIF_SEM_INVALID_HANDLE;       /* V1.0.04 */
USHORT              fsPifPowerSwitch;   /* V1.0.04 */
NCRPWR_MODE         PifPowerModeSave;   /* V1.0.04 */
extern CRITICAL_SECTION g_FileCriticalSection;
extern CRITICAL_SECTION g_SioCriticalSection;
extern CRITICAL_SECTION g_NetCriticalSection;

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

typedef struct {
    PifSemHandle sRequestSem;
    PifSemHandle sReleaseSem;
} PIFPOWERTABLE;

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

PIFPOWERTABLE   aPifPowerTable[PIF_MAX_POWER];

HANDLE hPowerHandle = INVALID_HANDLE_VALUE;

#ifdef POWERLOG
VOID   PifPowerLog(TCHAR *pszThread, TCHAR *pwszString);
extern TCHAR wszPifLogPathName[];
#endif
/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifRestart(USHORT usLoadMode)        **
**                                                                  **
**  return:                                                         **
**                                                                  **
**  Description:                                                    **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifRestart(USHORT usLoadMode)
{
    HANDLE          hHandle;
    BOOL            bResult;
    NCRPWR_REBOOT   Reboot;
    USHORT          usFlag;
    DWORD           dwError;
#ifdef DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];
#endif
    hHandle = PifOpenRecoveryDriver();

    if (hHandle != INVALID_HANDLE_VALUE) {
#ifdef DEBUG_PIF_OUTPUT  
        wsprintf(wszDisplay, TEXT("PifRestart is called by %d\r\n"), usLoadMode);
        OutputDebugString(wszDisplay);
#endif
        usFlag = PifDisable();  /* set to critical priority */
        
        PifSaveFarData();
        PifCloseHandle(FALSE);  /* close all file handle */
    
        if (usLoadMode) {
            Reboot.dwRebootFlag = NCRPWR_COLD_REBOOT;
        } else {
            Reboot.dwRebootFlag = NCRPWR_WARM_REBOOT;
        }
        
        bResult = DeviceIoControl(
                hHandle,
                IOCTL_NCRPWR_REBOOT,    // control code
                &Reboot,                // ptr. to input data
                sizeof(Reboot),         // no. of bytes
                NULL,                   // ptr. to output buffer
                0,                      // no. of bytes
                NULL,                  // no. of bytes returned
                NULL);                  // ptr. to overlapped
                
        if (!bResult) {
            dwError = GetLastError();
#ifdef DEBUG_PIF_OUTPUT
            wsprintf(wszDisplay, TEXT("PifRestart Error ,%d\r\n"), dwError);
            OutputDebugString(wszDisplay);
#endif
        }
        PifRestore(usFlag); /* reset priority */
    }   
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHORT PIFENTRY PifTurnOffPower(VOID)               **
**                                                                  **
**  return:                                                         **
**                                                                  **
**  Description:                                                    **     
**                                                                  **
**********************************************************************
fhfh*/
USHORT PIFENTRY PifTurnOffPower(VOID)
{
    return (PifTurnOffPowerEx(TRUE));
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHORT PIFENTRY PifTurnOffPower(VOID)               **
**                                                                  **
**  return:                                                         **
**                                                                  **
**  Description:                                                    **     
**                                                                  **
**********************************************************************
fhfh*/
USHORT PIFENTRY PifTurnOffPowerEx(USHORT usLock)
{
    HANDLE          hHandle;
    BOOL            bResult;
    USHORT          usFlag;
    DWORD           dwError;
    DWORD           dwKeyLock = 2;
#ifdef DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];
#endif
    hHandle = PifOpenRecoveryDriver();

    if (hHandle != INVALID_HANDLE_VALUE) {
#ifdef DEBUG_PIF_OUTPUT        
        wsprintf(wszDisplay, TEXT("PifTurnOff is called\r\n"));
        OutputDebugString(wszDisplay);
#endif
        usFlag = PifDisable();  /* set to critical priority */

        if (usLock) {   /* check lock mode */
            dwKeyLock = GetKeyLock();
        }

        if (dwKeyLock == 2) { /* Lock Mode ? V1.0.04 */
        /* if (dwKeyLock < 4) {  / key lock is lock position ? */
        
            PifSaveFarData();
            PifCloseHandle(FALSE);  /* close all file handle */

            bResult = DeviceIoControl(
                    hHandle,
                    IOCTL_NCRPWR_POWER_OFF, // control code
                    NULL,                   // ptr. to input data
                    0,                      // no. of bytes
                    NULL,                   // ptr. to output buffer
                    0,                      // no. of bytes
                    NULL,                  // no. of bytes returned
                    NULL);                  // ptr. to overlapped
        } else {
            bResult = 0;
        }
     
        PifRestore(usFlag); /* reset priority */
        
        if (bResult == 0) {
            CloseHandle(hHandle);
        
            dwError = GetLastError();
#ifdef DEBUG_PIF_OUTPUT
            wsprintf(wszDisplay, TEXT("PifRestart Error ,%d\r\n"), dwError);
            OutputDebugString(wszDisplay);
#endif
            return FALSE;   /* failed to power off */
        } else {
            return TRUE;        /* successed to power off */
        }
    }
    return FALSE;
}


DWORD GetKeyLock(VOID)
{
    HANDLE  hLock;
    NCRKLOCK_POSITION   Driver;
    BOOL    bResult;
    DWORD   dwBytesRead;
#ifdef DEBUG_PIF_OUTPUT
    DWORD	dwError;
	TCHAR  wszDisplay[128];
#endif
    hLock = CreateFile(
                _T("KLK1:"),
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
                
    if (hLock != INVALID_HANDLE_VALUE) {
        // read a keylock position
        bResult = ReadFile(hLock, &Driver, sizeof(Driver), &dwBytesRead, NULL);
                        
        CloseHandle(hLock);

        // examine status
        if (! bResult || dwBytesRead != sizeof(NCRKLOCK_POSITION))
        {

#ifdef DEBUG_PIF_OUTPUT
            dwError = GetLastError();
            wsprintf(wszDisplay, TEXT("Failed to get Lock Position, %d\r\n"), dwError);
            OutputDebugString(wszDisplay);
#endif
			return (ULONG)-1;
        }

        // make up response data
#ifdef DEBUG_PIF_OUTPUT
        wsprintf(wszDisplay, TEXT("Lock Position is %d\r\n"), Driver.dwPosition);
        OutputDebugString(wszDisplay);
#endif
        return Driver.dwPosition;
    }

#ifdef DEBUG_PIF_OUTPUT
    dwError = GetLastError();
    wsprintf(wszDisplay, TEXT("Failed to get Lock Position, %d\r\n"), dwError);
    OutputDebugString(wszDisplay);
#endif
	return (ULONG)-1;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifStartRecoveryThread(VOID)
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:startup power down recovery monitor thread          **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PifStartRecoveryThread(VOID)
{
	VOID (THREADENTRY *pFunc)(VOID) = RecoveryThread;
    BOOL            bResult;
    NCRPWR_MODE     Mode;
    LONG            lId;
    DWORD           dwError, dwBytes;
#ifdef DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];

    wsprintf(wszDisplay, TEXT("PifStartRecoveryThread is called  ThreadId %x"), GetCurrentThreadId ());
    OutputDebugString(wszDisplay);
#endif
    /* open power management driver */

	//  work around
//    hPowerHandle = PifOpenRecoveryDriver();
	hPowerHandle = INVALID_HANDLE_VALUE;
	// end work around

    dwError = GetLastError();
    fsPifDuringPowerDown = FALSE;
    sPifPowerSem = PifCreateSem(0);
    sPifPowerSwitchSem = PifCreateSem(1);
    PifInitPowerSem();

	//  work around
    fsPifPower = 1;
    PifReleaseSem(sPifPowerSem);
	// end work around

    fsPifPowerSwitch = PIF_POWER_SWITCH_ENABLE; /* V1.0.04 */

    if (hPowerHandle != INVALID_HANDLE_VALUE) {
        /* get previous status */
        bResult = DeviceIoControl(
                hPowerHandle,
                IOCTL_NCRPWR_GET_MODE,   // control code
                NULL,                       // ptr. to output buffer
                0,                          // no. of bytes
                &Mode,                   // ptr. to input data
                sizeof(Mode),            // no. of bytes
                &dwBytes,                       // no. of bytes returned
                NULL);                      // ptr. to overlapped
                
        if (bResult) {
            
            memcpy(&PifPowerModeSave, &Mode, sizeof(PifPowerModeSave)); /* V1.0.04 */

            Mode.fEnablePowerOff = 0;           /* not go to power off automatically */
            Mode.fEnableSuspendOnAcLost = 1;    /* for only AC monitor */
#ifdef SUSPEND
            Mode.fEnableSuspendOnAcLost = 0;    /* not go to suspend automatically */
#endif
            /* set monitor status */

            bResult = DeviceIoControl(
                    hPowerHandle,
                    IOCTL_NCRPWR_SET_MODE,   // control code
                    &Mode,                   // ptr. to input data
                    sizeof(Mode),            // no. of bytes
                    NULL,                       // ptr. to output buffer
                    0,                          // no. of bytes
                    NULL,                       // no. of bytes returned
                    NULL);                      // ptr. to overlapped

            if (bResult) {
                lId = PifBeginThread(pFunc, NULL, 512, PIF_TASK_HIGHEST, (const CHAR*)aszRecoveryThreadName);
            } 
			else {
#ifdef DEBUG_PIF_OUTPUT
                dwError = GetLastError();
                wsprintf(wszDisplay, TEXT("PowerManagement, SETMODE Error, %d\r\n"), dwError);
                OutputDebugString(wszDisplay);
#endif
            }
        } else {
#ifdef DEBUG_PIF_OUTPUT
            dwError = GetLastError();
            wsprintf(wszDisplay, TEXT("PowerManagement, GETMODE Error, %d\r\n"), dwError);
            OutputDebugString(wszDisplay);
#endif
        }
    } else {
#ifdef DEBUG_PIF_OUTPUT
        dwError = GetLastError();
        wsprintf(wszDisplay, TEXT("PowerManagement, CreateFile Error, %d\r\n"), dwError);
        OutputDebugString(wszDisplay);
#endif
    }
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID THREADENTRY RecoveryThread(VOID)
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:power down recovery monitoring thread               **     
**                                                                  **
**********************************************************************
fhfh*/
VOID THREADENTRY RecoveryThread(VOID)
{
    BOOL            bResult;
    NCRPWR_EVENTS   PowerEvents;
    DWORD           dwBytes;
    USHORT          fsPowerSwitch;

#ifdef DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];
	DWORD	dwError;
    wsprintf(wszDisplay, TEXT("RecoveryThread starts successfully  Thread %x"), GetCurrentThreadId ());
    OutputDebugString(wszDisplay);
#endif
    fsPifPower = 1;
    PifReleaseSem(sPifPowerSem);

    while(1) {

        /* wait for power down event */
        bResult = DeviceIoControl(
                hPowerHandle,
                IOCTL_NCRPWR_WAIT_EVENTS,   // control code
                NULL,                       // ptr. to input buffer
                0,                          // no. of bytes
                &PowerEvents,               // ptr. to output data
                sizeof(PowerEvents),        // no. of bytes
                &dwBytes,                   // no. of bytes returned
                NULL);                      // ptr. to overlapped

        if (bResult) {

            if (PowerEvents.dwEvents == NCRPWR_RETURN_FROM_SUSPEND) {
                if (fsPifDuringPowerDown == TRUE) {
                    fsPifDuringPowerDown = FALSE;

                    PifReopenFile();    /* recover all files */
                    LeaveCriticalSection(&g_FileCriticalSection);
                    
                    /* close current opend driver's handle */
                    LeaveCriticalSection(&g_SioCriticalSection);
                    //PifCloseSioHandle(TRUE);

                    LeaveCriticalSection(&g_NetCriticalSection);
                    PifNetCloseHandle(TRUE);       /* disconnect network */

                    PifSendPowerEvent(PIF_RETURN_FROM_SUSPEND);
                    PifWaitForPowerAck();
                
                }
                fsPifDuringPowerDown = FALSE;
            } else if (PowerEvents.dwEvents == NCRPWR_AC_LOST) {

                PifSaveFarData();

                /* go to suspend */
//                if (SysConfig.ausOption[UIE_POWER_OPTION7]) { /* removed at V1.0.04 */

                    if (fsPifDuringPowerDown == FALSE) {
                        PifSendPowerEvent(PIF_AC_LOST);
                        PifWaitForPowerAck();

                        // get sio semaphore
                        PifCloseSioHandle(TRUE);
                        EnterCriticalSection(&g_SioCriticalSection);

                        EnterCriticalSection(&g_NetCriticalSection);
                        //PifNetCloseHandle(TRUE);       /* disconnect network */

 
                        EnterCriticalSection(&g_FileCriticalSection);
                        PifCloseFileHandleExec(FALSE);   /* close all file */

                        fsPifDuringPowerDown = TRUE;

#ifdef SUSPEND
                        bResult = DeviceIoControl(
                                hPowerHandle,
                                IOCTL_NCRPWR_ENTER_SUSPEND, // control code
                                NULL,                   // ptr. to input data
                                0,                      // no. of bytes
                                NULL,                   // ptr. to output buffer
                                0,                      // no. of bytes
                                NULL,                   // no. of bytes returned
                                NULL);                  // ptr. to overlappe
#endif
                    }
                /* removed at V1.0.04 */
//                } else {
//
//                    /* send power down event to plu total */
//                    fsPifDuringPowerDown = FALSE;
//                    PifSendPowerEvent(PIF_SWITCH_PRESSED);
//
//                    /* wait for power down ack from plu total */
//                    PifWaitForPowerAck();
//
//                    fsPifDuringPowerDown = TRUE;
//                    PifCloseHandle(FALSE);          /* unconditional close */
//
//                    /* suspend mode does not supported */
//                    bResult = DeviceIoControl(
//                            hPowerHandle,
//                            IOCTL_NCRPWR_POWER_OFF, // control code
//                            NULL,                   // ptr. to input data
//                            0,                      // no. of bytes
//                            NULL,                   // ptr. to output buffer
//                            0,                      // no. of bytes
//                            NULL,                   // no. of bytes returned
//                            NULL);                  // ptr. to overlapped
//                }
            } else if (PowerEvents.dwEvents == NCRPWR_SWITCH_PRESSED) {

                /* V1.0.04 */
                PifRequestSem(sPifPowerSwitchSem);
                fsPowerSwitch = fsPifPowerSwitch;
                PifReleaseSem(sPifPowerSwitchSem);
                
                if (fsPowerSwitch == PIF_POWER_SWITCH_ENABLE) {  /* V1.0.04 */

                    PifSaveFarData();

                    /* send power down event to plu total */
                    fsPifDuringPowerDown = FALSE;
                    PifSendPowerEvent(PIF_SWITCH_PRESSED);

                    /* wait for power down ack from plu total */
                    PifWaitForPowerAck();

                    fsPifDuringPowerDown = TRUE;
                    PifCloseHandle(FALSE);          /* unconditional close */

                    /* go to power off */
#ifndef XP_PORT    
                    bResult = DeviceIoControl(
                            hPowerHandle,
                            IOCTL_NCRPWR_POWER_OFF, // control code
                            NULL,                   // ptr. to input data
                            0,                      // no. of bytes
                            NULL,                   // ptr. to output buffer
                            0,                      // no. of bytes
                            NULL,                   // no. of bytes returned
                            NULL);                  // ptr. to overlapped
#endif
                }
				else {
                    
                    /* ignore power switch, V1.0.04 */
                    continue;
                }
            } 
			else {
#ifdef DEBUG_PIF_OUTPUT
                dwError = GetLastError();

                wsprintf(wszDisplay, TEXT("Recovery Thread, Unknown event (Error %d)\r\n"), dwError);
                OutputDebugString(wszDisplay);
#endif
                continue;
            }
        } else {

#ifdef DEBUG_PIF_OUTPUT
            dwError = GetLastError();
            wsprintf(wszDisplay, TEXT("Recovery Thread, Failed to get event (Error %d)\r\n"), dwError);
            OutputDebugString(wszDisplay);
#endif
            break;
        }
    }

    if (hPowerHandle != INVALID_HANDLE_VALUE) {
        
        /* V1.0.04 */
        bResult = DeviceIoControl(hPowerHandle, IOCTL_NCRPWR_SET_MODE, &PifPowerModeSave,
                sizeof(PifPowerModeSave), NULL, 0, NULL, NULL);

        CloseHandle(hPowerHandle);
    }

    hPowerHandle = INVALID_HANDLE_VALUE;
#ifdef DEBUG_PIF_OUTPUT
    wsprintf(wszDisplay, TEXT("Recovery Thread, end of thread"));
    OutputDebugString(wszDisplay);
#endif
    PifEndThread();
}

VOID PifStopRecoveryThread(VOID)
{
    if (hPowerHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(hPowerHandle);
    }
    hPowerHandle = INVALID_HANDLE_VALUE;
}

HANDLE PifOpenRecoveryDriver(VOID)
{
    /* open power management driver */
    return CreateFile (TEXT("PWR1:"), /* Pointer to the name of the port */
                        0,
                        /* GENERIC_READ | GENERIC_WRITE, */
                                    /* Access (read-write) mode */
                        FILE_SHARE_READ,/* Share mode */
                        NULL,         /* Pointer to the security attribute */
                        OPEN_EXISTING,/* How to open the serial port */
                        FILE_ATTRIBUTE_NORMAL,  // attributes
                        NULL);        /* Handle to port with attribute */
                                      /* to copy */
}


/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifCreatePowerSem(VOID)
**                                                                  **
**  return:                                                        **
**                                                                  **
**  Description:                                                    **     
**                                                                  **
**********************************************************************
fhfh*/
PifSemHandle PIFENTRY PifCreatePowerSem(VOID)
{
    USHORT        i;

    EnterCriticalSection(&g_PowerCriticalSection);
    
    if (fsPifPower == 0) {
		// Let make sure that as part of startup that the PifStartRecoveryThread()
		// function has initialized the power semaphore before we try to request it.
		while (sPifPowerSem == PIF_SEM_INVALID_HANDLE) {
			PifSleep (50);
		}
        PifRequestSem(sPifPowerSem);
    }

    for (i = 0; i < PIF_MAX_POWER; i++) {
        if (aPifPowerTable[i].sRequestSem == PIF_SEM_INVALID_HANDLE) {
            aPifPowerTable[i].sRequestSem = PifCreateSem(0);
            aPifPowerTable[i].sReleaseSem = PifCreateSem(0);
            break;
        }
    }
    
    if (i >= PIF_MAX_POWER) {
        LeaveCriticalSection(&g_PowerCriticalSection);
        return PIF_SEM_INVALID_HANDLE;
    }
    LeaveCriticalSection(&g_PowerCriticalSection);
    
    return i;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifRequestPowerSem(VOID)
**                                                                  **
**  return:                                                        **
**                                                                  **
**  Description:                                                    **     
**                                                                  **
**********************************************************************
fhfh*/
USHORT PIFENTRY PifRequestPowerSem(USHORT usSem)
{
    PifSemHandle usRequestSem;
    USHORT       usResult;

    EnterCriticalSection(&g_PowerCriticalSection);
    
    usRequestSem = aPifPowerTable[usSem].sRequestSem;
    
    LeaveCriticalSection(&g_PowerCriticalSection);
    
    /* wait for power down event */
    PifRequestSem(usRequestSem);
    
    EnterCriticalSection(&g_PowerCriticalSection);
    
    usResult = usPifPowerEvent;
    
    LeaveCriticalSection(&g_PowerCriticalSection);
    
    return usResult;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifReleasePowerSem(VOID)
**                                                                  **
**  return:                                                        **
**                                                                  **
**  Description:                                                    **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifReleasePowerSem(USHORT usSem)
{
    PifSemHandle usReleaseSem;

    EnterCriticalSection(&g_PowerCriticalSection);
    
    usReleaseSem = aPifPowerTable[usSem].sReleaseSem;

    LeaveCriticalSection(&g_PowerCriticalSection);
    
    /* send to Ack to power down */
    PifReleaseSem(usReleaseSem);
    
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifDeleatePowerSem(VOID)
**                                                                  **
**  return:                                                        **
**                                                                  **
**  Description:                                                    **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifDeletePowerSem(USHORT usSem)
{
    PifSemHandle usRequestSem, usReleaseSem;

    EnterCriticalSection(&g_PowerCriticalSection);
    
    usRequestSem = aPifPowerTable[usSem].sRequestSem;
    PifDeleteSem(usRequestSem);

    usReleaseSem = aPifPowerTable[usSem].sReleaseSem;
    PifDeleteSem(usReleaseSem);
    
    aPifPowerTable[usSem].sRequestSem = PIF_SEM_INVALID_HANDLE;
    LeaveCriticalSection(&g_PowerCriticalSection);
}

VOID PifInitPowerSem(VOID)
{
    USHORT i;
    InitializeCriticalSection(&g_PowerCriticalSection);
    EnterCriticalSection(&g_PowerCriticalSection);
    
    for (i = 0; i < PIF_MAX_POWER; i++) {
        aPifPowerTable[i].sRequestSem = PIF_SEM_INVALID_HANDLE;
        aPifPowerTable[i].sReleaseSem = PIF_SEM_INVALID_HANDLE;
    }
    LeaveCriticalSection(&g_PowerCriticalSection);
}

VOID PifSendPowerEvent(USHORT usEvent)
{
    USHORT i;
    SHORT  sSem;
    
#ifdef DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];
    wsprintf(wszDisplay, TEXT("Send Power Down Event\n"));
    MessageBox(NULL, wszDisplay, TEXT("RECOVER THREAD"), MB_OK);
#endif

    for (i = 0; i < PIF_MAX_POWER; i++) {
        EnterCriticalSection(&g_PowerCriticalSection);
        sSem = aPifPowerTable[i].sRequestSem;
        usPifPowerEvent = usEvent;
        LeaveCriticalSection(&g_PowerCriticalSection);
    
        if ( sSem >= 0) {
            /* send power down event */
            PifReleaseSem(sSem);
        }
    }
}


VOID PifWaitForPowerAck(VOID)
{
    USHORT i;
    SHORT  sSem;
#if DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];
    wsprintf(wszDisplay, TEXT("Wait Power Down ACK\n"));
    MessageBox(NULL, wszDisplay, TEXT("RECOVER THREAD"), MB_OK);
#endif
    
    /* wait for power down ACK */
    for (i = 0; i < PIF_MAX_POWER; i++) {
        EnterCriticalSection(&g_PowerCriticalSection);
        sSem = aPifPowerTable[i].sReleaseSem;
        LeaveCriticalSection(&g_PowerCriticalSection);

        if ( sSem >= 0) {
            PifRequestSem(sSem);
#ifdef DEBUG_PIF_OUTPUT
            wsprintf(wszDisplay, TEXT("ACK Received by %d\n"), sSem);
            MessageBox(NULL, wszDisplay, TEXT("RECOVER THREAD"), MB_OK);
#endif
        }
    }
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifSaveRegistry(VOID)                 **
**                                                                  **
**  return:                                                         **
**                                                                  **
**  Description:                                                    **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   PifSaveRegistry(VOID)
{
    HANDLE          hHandle;
    BOOL            bResult;
    DWORD           dwBytesReturned;
#ifdef DEBUG_PIF_OUTPUT
    DWORD	dwError;
	TCHAR  wszDisplay[128];
#endif
    hHandle = PifOpenRecoveryDriver();

    if (hHandle != INVALID_HANDLE_VALUE) {
        bResult = DeviceIoControl(
                        hHandle,
                        IOCTL_NCRPWR_COPY_REGISTRY,
                        NULL,
                        0,
                        NULL,
                        0,
                        &dwBytesReturned,
                        NULL);
#ifdef DEBUG_PIF_OUTPUT          
        if (!bResult) {
            dwError = GetLastError();
            wsprintf(wszDisplay, TEXT("PifSaveRegistry Error ,%d\r\n"), dwError);
            OutputDebugString(wszDisplay);
        }
#endif
       CloseHandle(hHandle);
    }   
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHORT PIFENTRY PifControlPowerSwitch(USHORT usFunc)  **
**                                                                  **
**  return:                                                         **
**                                                                  **
**  Description:                                                    **     
**                                                                  **
**********************************************************************
fhfh*/
USHORT PIFENTRY PifControlPowerSwitch(USHORT usFunc)
{
    USHORT usPrevFunc;
	
    PifRequestSem(sPifPowerSwitchSem);

    usPrevFunc = fsPifPowerSwitch;
    
    fsPifPowerSwitch = usFunc;

    PifReleaseSem(sPifPowerSwitchSem);
    
    return usPrevFunc;
}

VOID PIFENTRY PifShutDownApp(VOID)
{
	void * NhposMainHwnd = 0;

    PifReleaseSem(sPifPowerSem);

    PifSaveFarData();

    /* send power down event to plu total */
    fsPifDuringPowerDown = FALSE;
    PifSendPowerEvent(PIF_SWITCH_PRESSED);

    /* wait for power down ack from plu total */
    PifWaitForPowerAck();

    fsPifDuringPowerDown = TRUE;

	PifCloseHandle(FALSE);
    
	PifGetWindowHandle(&NhposMainHwnd);
	// there appears to be a problem with doing this orderly shutdown.
	// US Customs has reported a problem with Rel 2.2.1.146 displaying a
	// Windows Error Dialog when GenPOS is shutting down.
	// there also seems to be a deadlock issue of some kind as well as
	// an exception error that can be thrown about a bad memory read address.
	// however it also seems that after rebooting PC, issues went away.
	// readings indicate that WM_CLOSE to the main window.should be used instead of WM_QUIT.
	//    Richard Chambers, May 5, 2015
    // we have since changed the architecture to use an event to drive the shutdown thread
    // from function FrameworkShutdownThread() to do shutdown.
    //    Richard Chambers, May 8, 2015
	if (NhposMainHwnd)
		PostMessage(NhposMainHwnd, WM_APP_SHUTDOWN_MSG, 0, 0);
}


#ifdef POWERLOG
VOID   PifPowerLog(TCHAR *pszThread, TCHAR *pwszString)
{
    TCHAR wszFilePathName[MAX_PATH];
    DWORD dwReturn, dwNumberOfBytes;
    TCHAR  wszLog[64+1];
    CHAR    szLog[64+1];
    DATE_TIME DateTime;
    HANDLE  hHandle;
    
    if (SysConfig.ausOption[UIE_TEST_OPTION8] != UIE_TEST_PASS) {  // PifPowerLog() Debug option
        return;
    }

    wsprintf(wszFilePathName, TEXT("%s\\%s"), wszPifLogPathName, TEXT("POWER.LOG"));

    hHandle = CreateFile(wszFilePathName,
        GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        0);

    if (hHandle != INVALID_HANDLE_VALUE) {
        /* read log file header */
        dwReturn = SetFilePointer(hHandle, 0, NULL, FILE_END);

        wsprintf(wszLog, TEXT("%S,%s\n"), pszThread, pwszString);
        wcstombs(szLog, wszLog, sizeof(szLog));

        WriteFile(hHandle, szLog, (DWORD)strlen(szLog), &dwNumberOfBytes, NULL);

        PifGetDateTime(&DateTime);
        wsprintf(wszLog, TEXT("%02d/%02d/%02d,%02d:%02d:%02d,\n"), 
            DateTime.usMonth, DateTime.usMDay, DateTime.usYear%100, DateTime.usHour, DateTime.usMin, DateTime.usSec);
        wcstombs(szLog, wszLog, sizeof(szLog));
        WriteFile(hHandle, szLog, (DWORD)strlen(szLog), &dwNumberOfBytes, NULL);
        
        CloseHandle(hHandle);
    }
}
#endif
/* end of pifpower.c */
