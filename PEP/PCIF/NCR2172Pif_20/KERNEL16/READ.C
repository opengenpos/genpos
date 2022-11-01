/*hhhh
*****************************************************************************
**
**  Title:      Device Independent Read Routine
**
**  File Name:  read.c
**
**  Category:   Device Independent I/O, Kernel
**              2170 XINU Operating System
**
**  Abstruct:   This module provides the following function:
**
**                SHORT SYSCALL XinuRead(SHORT sDevice, VOID *pchBuff,
**                                       SHORT sBytes)
**
**                sDevice       Device number
**                pchBuff       Pointer to read buffer
**                sCount        Number of bytes to be read
**
**                Return = number of characters read if successful
**                       = SYSERR : incorrect device or cannot read
**
**              The XinuRead read up to sCount bytes from I/O device given by
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
**
*****************************************************************************
hhhh*/

#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  read  --  read one or more bytes from a device
 *------------------------------------------------------------------------
 */
SHORT SYSCALL XinuRead(SHORT sDevice, VOID *pchBuff, SHORT sBytes)
{
	struct	devsw	*devptr;

	if ( isbaddev(sDevice) )
		return(SYSERR);
	devptr = &devtab[sDevice];
	return(	(*devptr->dvread)(devptr,pchBuff,sBytes) );
}

