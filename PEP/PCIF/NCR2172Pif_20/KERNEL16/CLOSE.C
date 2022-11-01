/*hhhh
*****************************************************************************
**
**  Title:      Device Independent Close Routine
**
**  File Name:  close.c
**
**  Category:   Device Independent I/O, Kernel
**              2170 XINU Operating System
**
**  Abstruct:   This module provides the following function:
**
**                SHORT SYSCALL XinuClose(SHORT sDevice)
**
**                sDevice       Device number
**
**                Return = OK     : No error
**                       = SYSERR : incorrect device or cannot be closed
**
**              The XinuClose disconnect I/O from the device given by
**              sDevice.
**
*****************************************************************************
*****************************************************************************
**
**  Modification History:
**
**  Ver.    Date        Name        Description
**  00.00.00     Mar 31,88   D.Comer     Original PC-XINU
**  00.00.01     Apr 10,92   Y.Nozawa    Applied coding convension of 2170
**
*****************************************************************************
hhhh*/
#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  close  --  close a device
 *------------------------------------------------------------------------
 */
SHORT SYSCALL XinuClose(SHORT sDevice)
{
	struct	devsw	*devptr;

	if (isbaddev(sDevice) )
		return(SYSERR);
	devptr = &devtab[sDevice];
	return( (*devptr->dvclose)(devptr));
}

