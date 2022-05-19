/* pifsound.c : PIF sound functions */
#pragma warning( disable : 4201 4214 4514)
#include "stdafx.h"
#include <winioctl.h>

#include "ecr.h"
#include "ncrspker.h"
#include "pif.h"
#include "piflocal.h"

HANDLE hSpeakerHandle = INVALID_HANDLE_VALUE;
USHORT usPifVolume;

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifBeep(USHORT usMsec)              **
**              usMsec:    length of sound                        **
**                                                                  **
**  return:     none                                               **
**                                                                  **
**  Description:emulate beep tone by wave file                      **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifBeep(USHORT usMsec)
{
    BOOL            bResult;
    SOUND_NOTE      Sound;
    DWORD           dwBytes;
//    PARATONECTL		Tone;

    if (hSpeakerHandle != INVALID_HANDLE_VALUE) {
        
        if (usMsec == 0) {
            
            /* stop speaker tone */
            
            bResult = DeviceIoControl(
                    hSpeakerHandle,
                    IOCTL_NCRSPEAKER_CANCEL_SOUND,   // control code
                    NULL,                       // ptr. to input data
                    0,                          // no. of bytes
                    NULL,                       // ptr. to output buffer
                    0,                          // no. of bytes
                    &dwBytes,                   // no. of bytes returned
                    NULL);                      // ptr. to overlapped
                    
        } else {
            
            /* stop speaker tone */
            
            bResult = DeviceIoControl(
                    hSpeakerHandle,
                    IOCTL_NCRSPEAKER_CANCEL_SOUND,   // control code
                    NULL,                       // ptr. to input data
                    0,                          // no. of bytes
                    NULL,                       // ptr. to output buffer
                    0,                          // no. of bytes
                    &dwBytes,                   // no. of bytes returned
                    NULL);                      // ptr. to overlapped
                    
            /* play sound */
            
            memset(&Sound, 0, sizeof(Sound));
        
            Sound.dwDuration = usMsec;
            Sound.dwFrequency = 1000; /* like 2170 */
        
            bResult = DeviceIoControl(
                    hSpeakerHandle,
                    IOCTL_NCRSPEAKER_PLAY_SOUND,   // control code
                    &Sound,                   // ptr. to input data
                    sizeof(Sound),            // no. of bytes
                    NULL,                       // ptr. to output buffer
                    0,                          // no. of bytes
                    &dwBytes,                   // no. of bytes returned
                    NULL);                      // ptr. to overlapped


        }
                
    }else{
		//For Windows XP use the Beep
		//if volume is set
		if(usPifVolume != 0){
			Beep(1000, usMsec);
		}
	}

    return;

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifPlayStanza(CONST STANZA FAR *pStanza) **
**              pStanza:    sound data                             **
**                                                                  **
**  return:     none                                               **
**                                                                  **
**  Description:                                                    **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifPlayStanza(CONST STANZA *pStanza)
{
    BOOL            bResult;
    SOUND_NOTE      Sound;
	DWORD           dwBytes, i;

    if (hSpeakerHandle != INVALID_HANDLE_VALUE) {
        if (pStanza == NULL) {
            /* stop speaker tone */
            bResult = DeviceIoControl(
                    hSpeakerHandle,
                    IOCTL_NCRSPEAKER_CANCEL_SOUND,   // control code
                    NULL,                       // ptr. to input data
                    0,                          // no. of bytes
                    NULL,                       // ptr. to output buffer
                    0,                          // no. of bytes
                    &dwBytes,                   // no. of bytes returned
                    NULL);                      // ptr. to overlapped
        } else {
			int  iLoop = -1;
			do {
				/* play sound */
				for (i = 0; i < 30; i++) {
					if (pStanza[i].usFrequency == STANZA_FREQUENCY_LOOP) {
						if (iLoop < 0)
							iLoop = pStanza[i].usDuration;
					}
					if (pStanza[i].usDuration == STANZA_DURATION_END_STANZA || pStanza[i].usFrequency == STANZA_FREQUENCY_LOOP) {
						/* stop speaker tone */
						bResult = DeviceIoControl(
								hSpeakerHandle,
								IOCTL_NCRSPEAKER_CANCEL_SOUND,   // control code
								NULL,                       // ptr. to input data
								0,                          // no. of bytes
								NULL,                       // ptr. to output buffer
								0,                          // no. of bytes
								&dwBytes,                   // no. of bytes returned
								NULL);                      // ptr. to overlapped
						if(pStanza[i].usDuration == STANZA_DURATION_END_STANZA) {
							iLoop = 0;
						}
						break;
					}
            
					/* stop speaker tone */
					bResult = DeviceIoControl(
							hSpeakerHandle,
							IOCTL_NCRSPEAKER_CANCEL_SOUND,   // control code
							NULL,                       // ptr. to input data
							0,                          // no. of bytes
							NULL,                       // ptr. to output buffer
							0,                          // no. of bytes
							&dwBytes,                   // no. of bytes returned
							NULL);                      // ptr. to overlapped

					if (pStanza[i].usFrequency <= STANZA_FREQUENCY_SILENCE) {
						/* stop speaker */
						PifSleep(pStanza[i].usDuration);
					} else if (pStanza[i].usFrequency < STANZA_FREQUENCY_LOOP) {
						Sound.dwDuration = (DWORD)pStanza[i].usDuration;
						Sound.dwFrequency = (DWORD)pStanza[i].usFrequency;
        
						bResult = DeviceIoControl(
								hSpeakerHandle,
								IOCTL_NCRSPEAKER_PLAY_SOUND,   // control code
								&Sound,                   // ptr. to input data
								sizeof(Sound),            // no. of bytes
								NULL,                       // ptr. to output buffer
								0,                          // no. of bytes
								&dwBytes,                   // no. of bytes returned
								NULL);                      // ptr. to overlapped
                
						PifSleep(pStanza[i].usDuration);
					}
				}
				iLoop--;
			} while (iLoop > 0);
        }
    } else{
		//if volume set play
		if(usPifVolume != 0){
			int  iLoop = -1;
			if(pStanza != NULL) {
				do {
					/* play sound */
					for (i = 0; i < 30; i++) {
						if (pStanza[i].usFrequency == STANZA_FREQUENCY_LOOP) {
							if (iLoop < 0)
								iLoop = pStanza[i].usDuration;
						}

						if(pStanza[i].usDuration == STANZA_DURATION_END_STANZA || pStanza[i].usFrequency == STANZA_FREQUENCY_LOOP) {
							if(pStanza[i].usDuration == STANZA_DURATION_END_STANZA) {
								iLoop = 0;
							}
							break;
						}

						if(pStanza[i].usFrequency <= STANZA_FREQUENCY_SILENCE){
							PifSleep(pStanza[i].usDuration);
						} else if (pStanza[i].usFrequency < STANZA_FREQUENCY_LOOP) {
							Beep(pStanza[i].usFrequency, pStanza[i].usDuration);
							PifSleep(pStanza[i].usDuration);
						}
					}
					iLoop--;
				} while (iLoop > 0);
			}
		}
	}
}


/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifToneVolume(USHORT usVolume)        **
**              usVolume:    volume data                             **
**                                                                  **
**  return:     none                                               **
**                                                                  **
**  Description: adjust volume of sound                             **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifToneVolume(USHORT usVolume)
{
    SPEAKER_VOLUME Volume;
    BOOL            bResult;
    DWORD           dwBytes;
    
        
        memset(&Volume, 0, sizeof(Volume));
        if (usVolume == 0) {
            
            Volume.dwVolume = 0;
        } else                       /* '}' -> '} else' V1.0.4 */
        if (usVolume == 1) {
            
            Volume.dwVolume = 5;
            
        } else
        if ((usVolume == 2) ||
            (usVolume == 3)) {
        
            Volume.dwVolume = 10;
            
        } else {
        
            Volume.dwVolume = 15;
            
        }
        
        usPifVolume = usVolume;
    if (hSpeakerHandle != INVALID_HANDLE_VALUE) {
        bResult = DeviceIoControl(
                hSpeakerHandle,
                IOCTL_NCRSPEAKER_ADJUST_VOLUME,   // control code
                &Volume,                      // ptr. to input data
                sizeof(Volume),              // no. of bytes
                NULL,                       // ptr. to output buffer
                0,                          // no. of bytes
                &dwBytes,                   // no. of bytes returned
                NULL);                      // ptr. to overlapped

    }
    return;

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifPlayStanza2(USHORT usFile          **
**                                          USHORT usLoop)          **
**              usFile:    type of wave file                        **
**              usLoop:     whether sound repeatedly
**                                                                  **
**  return:     none                                               **
**                                                                  **
**  Description: sound wave file                                    **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifPlayStanza2(USHORT usFile, USHORT usLoop)
{
    /* not support at actual 2172 */
    
    return;
    
    usFile = usFile;
    usLoop = usLoop;
}


VOID PifOpenSpeaker(VOID)
{

    /* open power management driver */
    hSpeakerHandle = CreateFile (TEXT("SPK1:"), /* Pointer to the name of the port */
                        0,
                        /* GENERIC_READ | GENERIC_WRITE, */
                                    /* Access (read-write) mode */
                        FILE_SHARE_READ,/* Share mode */
                        NULL,         /* Pointer to the security attribute */
                        OPEN_EXISTING,/* How to open the serial port */
                        0,            /* Port attributes */
                        NULL);        /* Handle to port with attribute */
                                      /* to copy */
}

VOID PifCloseSpeaker(VOID)
{
    if (hSpeakerHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(hSpeakerHandle);
    }
}

VOID PifReopenSpeaker(VOID)
{
    if (hSpeakerHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(hSpeakerHandle);
    }

    PifOpenSpeaker();

    /* set speaker volume by previous value */
    PifToneVolume(usPifVolume);
}
/* end of pifsound.c */
