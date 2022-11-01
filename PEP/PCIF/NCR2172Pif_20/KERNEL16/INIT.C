/*hhhh
*****************************************************************************
**
**  Title:      Device Independent Initialization Routine
**
**  File Name:  init.c
**
**  Category:   Device Independent I/O, Kernel
**              2170 XINU Operating System
**
**  Abstruct:   This module provides the following function:
**
**                SHORT SYSCALL XinuInit(SHORT sDevice)
**
**                sDevice       Device number
**
**                Return        Value returned by underlying device
**                              intialization routine
**
**              The operating system calls XinuInit once at system startup
**              for every device configured into the device switch table.
**              Normally, user process do not invoke XinuInit directly.
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
 *  init  --  initialize a device
 *------------------------------------------------------------------------
 */
SHORT SYSCALL XinuInit(SHORT sDevice)
{
	struct	devsw	*devptr;

	if (isbaddev(sDevice) )
		return(SYSERR);
	devptr = &devtab[sDevice];
        return( (*devptr->dvinit)(devptr) );
}

