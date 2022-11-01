/*hhhh
*****************************************************************************
**
**  Title:      Error Entry Service Routine
**
**  File Name:  ioerr.c
**
**  Category:   Device Independent I/O, Kernel
**              2170 XINU Operating System
**
**  Abstruct:   This module provides the following function:
**
**                SHORT ioerr(VOID)
**
**                Return = SYSERR
**
**              The ioerr simply returns SYSERR whenever it is called.
**              It is used to fill the error entry in devtab.
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

/*------------------------------------------------------------------------
 *  ioerr  --  return an error (used for "error" entries in devtab)
 *------------------------------------------------------------------------
 */
SHORT ioerr(VOID)
{
	return(SYSERR);
}

