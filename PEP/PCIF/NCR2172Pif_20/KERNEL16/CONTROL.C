/*hhhh
*****************************************************************************
**
**  Title:      Device Independent Control Routine
**
**  File Name:  control.c
**
**  Category:   Device Independent I/O, Kernel
**              2170 XINU Operating System
**
**  Abstruct:   This module provides the following function:
**
**                SHORT SYSCALL XinuControl(SHORT sDevice, SHORT sFunctions, ...)
**
**                sDevice       Device number
**                sFunction     Device dependent function number
**
**                Return = OK if successful
**                       = SYSERR if incorrect device or cannot read
**
**              The XinuControl sends control information to devices and device
**              drivers, or interrogate their status.
**
*****************************************************************************
*****************************************************************************
**
**  Modification History:
**
**  Ver.      Date        Name        Description
**  00.00.00  Mar 31,88   D.Comer     Original PC-XINU
**  00.00.01  Apr 10,92   Y.Nozawa    Applied coding convension of 2170
**            Jun 22,98   O.Nakada    Win32
**
*****************************************************************************
hhhh*/

#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  control  --  control a device (e.g., set the mode)
 *------------------------------------------------------------------------
 */
SHORT SYSCALL XinuControl(SHORT sDevice, SHORT sFunctions, ...)
{
	struct	devsw	*devptr;

	if (isbaddev(sDevice) )
		return(SYSERR);
	devptr = &devtab[sDevice];
#if	WIN32
	return(	(*devptr->dvcntl)(devptr, sFunctions,
                              *((char **)((long *)&sFunctions+1)),
                              *((char **)((long *)&sFunctions+1)+1)) );
#else
	return(	(*devptr->dvcntl)(devptr, sFunctions,
                              *((char **)(&sFunctions+1)),
                              *((char **)(&sFunctions+1)+1)) );
#endif
}

