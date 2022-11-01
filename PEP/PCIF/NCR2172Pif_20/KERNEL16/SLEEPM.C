/*hhhh
**************************************************************************
**                                                                      **
**  Title:      Go to Sleep for a Specified Seconds                     **
**                                                                      **
**  File Name:  sleepm.c                                                **
**                                                                      **
**  Category:   Real-Time Clock Management, Kernel                      **
**              2170 XINU Operating System                              **
**                                                                      **
**  Abstruct:   This module contains,                                   **
**                                                                      **
**                SHORT SYSCALL XinuSleepm(USHORT usMSecs)              **
**                                                                      **
**                  usSecs       Number of mili-seconds to sleep for    **
**                                                                      **
**                  Return = OK if successful                           **
**                         = SYSERR if sSecs < 0                        **
**                                                                      **
**              The XinuSleep causes the current process to delay for a **
**              specified seconds.                                      **
**                                                                      **
**************************************************************************
**************************************************************************
**                                                                      **
**  Modification History:                                               **
**                                                                      **
**  Ver.      Date        Name        Description                       **
**  00.00.00  Aug/18/92   T.Kojima    Initial                           **
**            Jun/22/98   O.Nakada    Win32                             **
**                                                                      **
**************************************************************************
hhhh*/

#include <windows.h>
#include "xinu.h" 

SHORT SYSCALL XinuSleepm(USHORT usMSecs)
{
#if	WIN32
	Sleep((DWORD)usMSecs);

	return(OK);
#else
    DWORD   wCurrentTime, wStartTime, wSleepTime;
    
    if (usMSecs != 0) {

        /* get the current time   */
        wStartTime = GetCurrentTime();

        /* sleep    */
        for(;;) {

            /* get the current time */
            wCurrentTime = GetCurrentTime();

            wSleepTime = wCurrentTime - wStartTime;

            /* sleep for the specified time */
            if (wSleepTime > (DWORD)usMSecs) break;

            /* loop */
        }
    }

	return(OK);
#endif
}
