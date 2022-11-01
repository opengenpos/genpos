/*hhhh
*****************************************************************************
**
**  Title:      Device Independent Write Routine
**
**  File Name:  write.c
**
**  Category:   Device Independent I/O, Kernel
**              2170 XINU Operating System
**
**  Abstruct:   This module provides the following function:
**
**                SHORT SYSCALL XinuWrite(SHORT sDevice, CHAR *pchBuff,
**                                        SHORT sBytes)
**
**                sDevice       Device number
**                pchBuff       Pointer to write buffer
**                sCount        Number of bytes to be written
**
**                Return = number of characters written if successful
**                       = SYSERR : incorrect device or cannot write
**
**              The XinuWrite write sBytes bytes to the I/O device given by
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
 *  write  --  write 1 or more bytes to a device
 *------------------------------------------------------------------------
 */
SHORT SYSCALL XinuWrite(SHORT descrp, CHAR *buff, SHORT count)
{
	struct	devsw	*devptr;

	if (isbaddev(descrp) )
		return(SYSERR);
	devptr = &devtab[descrp];
	return(	(*devptr->dvwrite)(devptr,buff,count) );
}

