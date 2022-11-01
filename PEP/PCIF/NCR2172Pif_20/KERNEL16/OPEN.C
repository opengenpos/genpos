/*hhhh
*****************************************************************************
**
**  Title:      Device Independent Open Routine
**
**  File Name:  open.c
**
**  Category:   Device Independent I/O, Kernel
**              2170 XINU Operating System
**
**  Abstruct:   This module provides the following function:
**
**                SHORT SYSCALL XinuOpen(SHORT sDevice, ...)
**
**                sDevice       Device number
**
**                Return >=0      : Device Handle
**                       = SYSERR : incorrect device or cannot be opened
**
**              The XinuOpen establish connection with the device given
**              sDevice.
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
 *  open  --  open a connection to a device/file (arg1 & arg2 optional)
 *------------------------------------------------------------------------
 */
SHORT SYSCALL XinuOpen(SHORT descrp, ...)
{
	struct	devsw	*devptr;

	if ( isbaddev(descrp) )
		return(SYSERR);
	devptr = &devtab[descrp];
#if	WIN32
	return(	(*devptr->dvopen)(devptr,
                              *((char **)((long *)&descrp+1)),
                              *((char **)((long *)&descrp+1)+1)) );
#else
	return(	(*devptr->dvopen)(devptr,
                              *((char **)(&descrp+1)),
                              *((char **)(&descrp+1)+1)) );
#endif
}

